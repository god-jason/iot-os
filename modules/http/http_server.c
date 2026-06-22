/**
 * @file http_server.c
 * @brief HTTP 服务器实现
 *
 * 基于 net_socket 实现的 HTTP/1.1 服务器，支持路由注册和静态文件服务。
 */

#include "http_server.h"
#include "net.h"
#include "platform.h"
#include "iot_list.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define HTTP_SERVER_MAX_HEADER_SIZE 8192
#define HTTP_SERVER_MAX_PATH_SIZE   512
#define HTTP_SERVER_MAX_METHOD_SIZE 16
#define HTTP_SERVER_MAX_QUERY_SIZE  256
#define HTTP_SERVER_BACKLOG         10

struct http_server_route {
    http_server_method_t method;
    char* path;
    http_server_handler_t handler;
    list_head_t list_node;
};

struct http_server_client {
    net_socket_t* sock;
    http_server_t* server;
    char* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    list_head_t list_node;
};

struct http_server {
    net_socket_t* listen_sock;
    uint16_t port;
    LIST_HEAD(routes);
    LIST_HEAD(clients);
    char* static_dir;
    http_server_request_callback_t request_callback;
    void* request_callback_data;
};

static const char* http_server_method_to_string(http_server_method_t method);
static int http_server_parse_request(const char* buf, size_t len, http_server_request_t* req);
static int http_server_build_response(const http_server_response_t* resp, char* buf, size_t buf_len);
static void http_server_client_callback(net_socket_t* sock, net_event_type_t event, void* user_data);
static void http_server_listen_callback(net_socket_t* sock, net_event_type_t event, void* user_data);
static struct http_server_route* http_server_find_route(http_server_t* server,
                                                        const char* method, const char* path);

static const char* http_server_method_to_string(http_server_method_t method) {
    switch (method) {
        case HTTP_SERVER_METHOD_GET:     return "GET";
        case HTTP_SERVER_METHOD_POST:    return "POST";
        case HTTP_SERVER_METHOD_PUT:     return "PUT";
        case HTTP_SERVER_METHOD_DELETE:  return "DELETE";
        case HTTP_SERVER_METHOD_HEAD:    return "HEAD";
        case HTTP_SERVER_METHOD_OPTIONS: return "OPTIONS";
        case HTTP_SERVER_METHOD_ALL:     return "*";
        default:                         return "GET";
    }
}

static http_server_method_t http_server_string_to_method(const char* str) {
    if (!str) return HTTP_SERVER_METHOD_GET;
    if (strcmp(str, "GET") == 0)     return HTTP_SERVER_METHOD_GET;
    if (strcmp(str, "POST") == 0)    return HTTP_SERVER_METHOD_POST;
    if (strcmp(str, "PUT") == 0)     return HTTP_SERVER_METHOD_PUT;
    if (strcmp(str, "DELETE") == 0)  return HTTP_SERVER_METHOD_DELETE;
    if (strcmp(str, "HEAD") == 0)    return HTTP_SERVER_METHOD_HEAD;
    if (strcmp(str, "OPTIONS") == 0) return HTTP_SERVER_METHOD_OPTIONS;
    return HTTP_SERVER_METHOD_GET;
}

static int http_server_parse_request(const char* buf, size_t len, http_server_request_t* req) {
    if (!buf || !req || len < 4) return -1;
    
    memset(req, 0, sizeof(*req));
    
    const char* line_end = strstr(buf, "\r\n");
    if (!line_end) return -1;
    
    size_t line_len = line_end - buf;
    
    char method[HTTP_SERVER_MAX_METHOD_SIZE] = {0};
    char path[HTTP_SERVER_MAX_PATH_SIZE] = {0};
    char query[HTTP_SERVER_MAX_QUERY_SIZE] = {0};
    
    const char* p = buf;
    const char* space1 = strchr(p, ' ');
    if (!space1 || space1 >= line_end) return -1;
    
    size_t method_len = space1 - p;
    if (method_len >= HTTP_SERVER_MAX_METHOD_SIZE) method_len = HTTP_SERVER_MAX_METHOD_SIZE - 1;
    memcpy(method, p, method_len);
    
    p = space1 + 1;
    const char* space2 = strchr(p, ' ');
    if (!space2 || space2 >= line_end) return -1;
    
    size_t path_len = space2 - p;
    const char* query_start = strchr(p, '?');
    
    if (query_start && query_start < space2) {
        size_t pure_path_len = query_start - p;
        if (pure_path_len >= HTTP_SERVER_MAX_PATH_SIZE) pure_path_len = HTTP_SERVER_MAX_PATH_SIZE - 1;
        memcpy(path, p, pure_path_len);
        
        size_t query_len = space2 - query_start - 1;
        if (query_len >= HTTP_SERVER_MAX_QUERY_SIZE) query_len = HTTP_SERVER_MAX_QUERY_SIZE - 1;
        memcpy(query, query_start + 1, query_len);
    } else {
        if (path_len >= HTTP_SERVER_MAX_PATH_SIZE) path_len = HTTP_SERVER_MAX_PATH_SIZE - 1;
        memcpy(path, p, path_len);
    }
    
    req->method = method;
    req->path = path;
    req->query = (query[0] != '\0') ? query : NULL;
    
    const char* header_end = strstr(buf, "\r\n\r\n");
    if (!header_end) return -1;
    
    size_t header_len = header_end - buf;
    req->header = buf;
    req->header_len = header_len;
    
    size_t body_start = header_len + 4;
    if (body_start < len) {
        req->body = buf + body_start;
        req->body_len = len - body_start;
    }
    
    return 0;
}

static int http_server_build_response(const http_server_response_t* resp, char* buf, size_t buf_len) {
    if (!resp || !buf) return -1;
    
    const char* status_text = "OK";
    switch (resp->status_code) {
        case 200: status_text = "OK"; break;
        case 201: status_text = "Created"; break;
        case 204: status_text = "No Content"; break;
        case 301: status_text = "Moved Permanently"; break;
        case 302: status_text = "Found"; break;
        case 400: status_text = "Bad Request"; break;
        case 401: status_text = "Unauthorized"; break;
        case 403: status_text = "Forbidden"; break;
        case 404: status_text = "Not Found"; break;
        case 500: status_text = "Internal Server Error"; break;
        case 503: status_text = "Service Unavailable"; break;
        default: status_text = "OK";
    }
    
    size_t len = snprintf(buf, buf_len, "HTTP/1.1 %d %s\r\n", resp->status_code, status_text);
    
    if (resp->content_type) {
        len += snprintf(buf + len, buf_len - len, "Content-Type: %s\r\n", resp->content_type);
    } else {
        len += snprintf(buf + len, buf_len - len, "Content-Type: text/plain\r\n");
    }
    
    len += snprintf(buf + len, buf_len - len, "Content-Length: %zu\r\n", resp->body_len);
    
    if (resp->headers && resp->headers_len > 0) {
        if (len + resp->headers_len < buf_len) {
            memcpy(buf + len, resp->headers, resp->headers_len);
            len += resp->headers_len;
        } else {
            return -1;
        }
    }
    
    len += snprintf(buf + len, buf_len - len, "Connection: close\r\n");
    len += snprintf(buf + len, buf_len - len, "\r\n");
    
    if (resp->body && resp->body_len > 0) {
        if (len + resp->body_len < buf_len) {
            memcpy(buf + len, resp->body, resp->body_len);
            len += resp->body_len;
        } else {
            return -1;
        }
    }
    
    return len;
}

static struct http_server_route* http_server_find_route(http_server_t* server,
                                                        const char* method, const char* path) {
    if (!server || !method || !path) return NULL;
    
    http_server_method_t req_method = http_server_string_to_method(method);
    
    struct http_server_route* route;
    list_for_each_entry(route, &server->routes, list_node, struct http_server_route) {
        if (route->method == HTTP_SERVER_METHOD_ALL || route->method == req_method) {
            if (strcmp(route->path, path) == 0) {
                return route;
            }
        }
    }
    
    return NULL;
}

static void http_server_send_response(struct http_server_client* client,
                                      const http_server_response_t* resp) {
    if (!client || !client->sock || !resp) return;
    
    char buf[8192];
    int len = http_server_build_response(resp, buf, sizeof(buf));
    if (len > 0) {
        net_socket_send((sock_t)client->sock, buf, len);
    }
}

static void http_server_handle_request(struct http_server_client* client) {
    if (!client || !client->recv_buf || client->recv_len < 4) return;
    
    http_server_request_t req;
    if (http_server_parse_request(client->recv_buf, client->recv_len, &req) != 0) {
        http_server_response_t resp = {0};
        http_server_response_write_head(&resp, 400, "text/plain", NULL);
        http_server_response_end(&resp, "Bad Request", 11);
        
        http_server_send_response(client, &resp);
        http_server_response_free(&resp);
        return;
    }
    
    http_server_response_t resp = {0};
    http_server_response_write_head(&resp, 404, "text/plain", NULL);
    http_server_response_end(&resp, "Not Found", 9);
    
    if (client->server->request_callback) {
        client->server->request_callback(client->server, &req, &resp, 
                                         client->server->request_callback_data);
    } else {
        struct http_server_route* route = http_server_find_route(client->server, req.method, req.path);
        if (route && route->handler) {
            route->handler(&req, &resp);
        }
    }
    
    http_server_send_response(client, &resp);
    http_server_response_free(&resp);
}

static void http_server_client_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    struct http_server_client* client = (struct http_server_client*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_RECV: {
            char buf[4096];
            int len;
            
            while ((len = net_socket_recv((sock_t)sock, buf, sizeof(buf))) > 0) {
                if (client->recv_len + len > client->recv_capacity) {
                    size_t new_cap = client->recv_capacity + HTTP_SERVER_MAX_HEADER_SIZE;
                    char* new_buf = (char*)iot_realloc(client->recv_buf, new_cap);
                    if (!new_buf) return;
                    client->recv_buf = new_buf;
                    client->recv_capacity = new_cap;
                }
                
                memcpy(client->recv_buf + client->recv_len, buf, len);
                client->recv_len += len;
            }
            
            if (client->recv_len > 0) {
                const char* header_end = strstr(client->recv_buf, "\r\n\r\n");
                if (header_end) {
                    http_server_handle_request(client);
                }
            }
            break;
        }
        case NET_EVENT_DISCONNECTED:
        case NET_EVENT_ERROR: {
            if (client->recv_buf) {
                iot_free(client->recv_buf);
            }
            list_del(&client->list_node);
            iot_free(client);
            break;
        }
        default:
            break;
    }
}

static void http_server_listen_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    http_server_t* server = (http_server_t*)user_data;
    if (!server) return;
    
    if (event == NET_EVENT_ACCEPT) {
        struct http_server_client* client = (struct http_server_client*)iot_malloc(sizeof(struct http_server_client));
        if (!client) return;
        
        memset(client, 0, sizeof(*client));
        client->server = server;
        client->recv_capacity = HTTP_SERVER_MAX_HEADER_SIZE;
        client->recv_buf = (char*)iot_malloc(client->recv_capacity);
        if (!client->recv_buf) {
            iot_free(client);
            return;
        }
        list_init(&client->list_node);
        
        sock_t client_sock = net_socket_accept((sock_t)sock, http_server_client_callback, client);
        if (client_sock != INVALID_SOCKET) {
            client->sock = (net_socket_t*)client_sock;
            list_add_tail(&client->list_node, &server->clients);
        } else {
            iot_free(client->recv_buf);
            iot_free(client);
        }
    }
}

http_server_t* http_server_create(void) {
    http_server_t* server = (http_server_t*)iot_malloc(sizeof(http_server_t));
    if (!server) return NULL;
    
    memset(server, 0, sizeof(*server));
    return server;
}

void http_server_destroy(http_server_t* server) {
    if (!server) return;
    
    http_server_stop(server);
    
    struct http_server_route* route;
    struct http_server_route* tmp;
    list_for_each_entry_safe(route, tmp, &server->routes, list_node, struct http_server_route) {
        list_del(&route->list_node);
        if (route->path) iot_free(route->path);
        iot_free(route);
    }
    
    if (server->static_dir) {
        iot_free(server->static_dir);
    }
    
    iot_free(server);
}

int http_server_start(http_server_t* server, uint16_t port) {
    if (!server) return -1;
    
    server->listen_sock = (net_socket_t*)net_socket_create(SOCK_TYPE_STREAM, NULL,
                                                           http_server_listen_callback, server);
    if (!server->listen_sock) return -1;
    
    if (net_socket_bind((sock_t)server->listen_sock, "0.0.0.0", port) != 0) {
        net_socket_close((sock_t)server->listen_sock);
        server->listen_sock = NULL;
        return -1;
    }
    
    if (net_socket_listen((sock_t)server->listen_sock, HTTP_SERVER_BACKLOG) != 0) {
        net_socket_close((sock_t)server->listen_sock);
        server->listen_sock = NULL;
        return -1;
    }
    
    server->port = port;
    return 0;
}

void http_server_stop(http_server_t* server) {
    if (!server) return;
    
    if (server->listen_sock) {
        net_socket_close((sock_t)server->listen_sock);
        server->listen_sock = NULL;
    }
    
    struct http_server_client* client;
    struct http_server_client* tmp;
    list_for_each_entry_safe(client, tmp, &server->clients, list_node, struct http_server_client) {
        if (client->sock) {
            net_socket_close((sock_t)client->sock);
        }
        if (client->recv_buf) {
            iot_free(client->recv_buf);
        }
        list_del(&client->list_node);
        iot_free(client);
    }
}

int http_server_register_handler(http_server_t* server, http_server_method_t method,
                                 const char* path, http_server_handler_t handler) {
    if (!server || !path || !handler) return -1;
    
    struct http_server_route* route = (struct http_server_route*)iot_malloc(sizeof(struct http_server_route));
    if (!route) return -1;
    
    memset(route, 0, sizeof(*route));
    route->method = method;
    route->path = (char*)iot_malloc(strlen(path) + 1);
    if (!route->path) {
        iot_free(route);
        return -1;
    }
    strcpy(route->path, path);
    route->handler = handler;
    list_init(&route->list_node);
    
    list_add_tail(&route->list_node, &server->routes);
    return 0;
}

int http_server_set_static_dir(http_server_t* server, const char* dir) {
    if (!server) return -1;
    
    if (server->static_dir) {
        iot_free(server->static_dir);
    }
    
    if (!dir) {
        server->static_dir = NULL;
        return 0;
    }
    
    server->static_dir = (char*)iot_malloc(strlen(dir) + 1);
    if (!server->static_dir) return -1;
    
    strcpy(server->static_dir, dir);
    return 0;
}

void http_server_set_request_callback(http_server_t* server, 
                                      http_server_request_callback_t callback,
                                      void* user_data) {
    if (!server) return;
    
    server->request_callback = callback;
    server->request_callback_data = user_data;
}

void http_server_response_write_head(http_server_response_t* resp, int status_code, 
                                     const char* content_type, const char* headers) {
    if (!resp) return;
    
    resp->status_code = status_code;
    
    if (resp->content_type) {
        iot_free(resp->content_type);
    }
    
    if (content_type) {
        resp->content_type = (char*)iot_malloc(strlen(content_type) + 1);
        if (resp->content_type) {
            strcpy(resp->content_type, content_type);
        }
    } else {
        resp->content_type = NULL;
    }
    
    if (resp->headers) {
        iot_free(resp->headers);
    }
    
    if (headers) {
        resp->headers_len = strlen(headers);
        resp->headers = (char*)iot_malloc(resp->headers_len + 1);
        if (resp->headers) {
            strcpy(resp->headers, headers);
        }
    } else {
        resp->headers = NULL;
        resp->headers_len = 0;
    }
}

void http_server_response_set_header(http_server_response_t* resp, const char* key, const char* value) {
    if (!resp || !key || !value) return;
    
    size_t new_len = strlen(key) + strlen(value) + 4;
    if (resp->headers) {
        new_len += resp->headers_len;
    }
    
    char* new_headers = (char*)iot_malloc(new_len + 1);
    if (!new_headers) return;
    
    if (resp->headers) {
        strcpy(new_headers, resp->headers);
        strcat(new_headers, key);
        strcat(new_headers, ": ");
        strcat(new_headers, value);
        strcat(new_headers, "\r\n");
        iot_free(resp->headers);
    } else {
        sprintf(new_headers, "%s: %s\r\n", key, value);
    }
    
    resp->headers = new_headers;
    resp->headers_len = strlen(new_headers);
}

void http_server_response_end(http_server_response_t* resp, const char* body, size_t body_len) {
    if (!resp) return;
    
    if (resp->body) {
        iot_free(resp->body);
    }
    
    if (body && body_len > 0) {
        resp->body = (char*)iot_malloc(body_len);
        if (resp->body) {
            memcpy(resp->body, body, body_len);
            resp->body_len = body_len;
        }
    } else {
        resp->body = NULL;
        resp->body_len = 0;
    }
}

void http_server_response_free(http_server_response_t* resp) {
    if (!resp) return;
    
    if (resp->content_type) {
        iot_free(resp->content_type);
        resp->content_type = NULL;
    }
    
    if (resp->headers) {
        iot_free(resp->headers);
        resp->headers = NULL;
    }
    
    if (resp->body) {
        iot_free(resp->body);
        resp->body = NULL;
    }
    
    resp->body_len = 0;
    resp->headers_len = 0;
    resp->status_code = 200;
}
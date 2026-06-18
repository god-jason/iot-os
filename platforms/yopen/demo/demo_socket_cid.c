
#include "yopen_sockets.h"
#include "yopen_os.h"
#include "yopen_debug.h"
#include "yopen_nw.h"
#include "yopen_datacall.h"

#define TEST_SOCKET_SERVER "47.99.125.240"
#define TEST_SOCKET_TCP_PORT 8001
#define TEST_SOCKET_UDP_PORT 8002

#define CU_ASSERT(c) yopen_assert(c, __FILE__, __LINE__)
#define SOCKET_TRACE(fmt, ...) yopen_trace("[SOCKET_CID]"fmt, ##__VA_ARGS__)

static int _socket_buf(int sock, char *buf, int cid, int id)
{
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    
    if (getsockname(sock, (struct sockaddr *)&local_addr, &addr_len) == 0) {
        // 成功获取地址信息
        const char *ip_str = inet_ntoa(local_addr.sin_addr);
        if (ip_str != NULL) {
            sprintf(buf, "cid-%d,cnt-%d,ip-%s", cid, id, ip_str);
            return 0; // 成功
        }
    }
    return -1; // 失败
}

static int testTcp(uint8_t cid)
{
    int ret;
    char data[64];
    uint16_t data_len;
    int i = 0;
    struct hostent *hostent;
    int socket_fd;

    // struct sockaddr_in local4;

    hostent = gethostbyname(TEST_SOCKET_SERVER);
    if(hostent == NULL)
    {
        SOCKET_TRACE("gethostbyname failed");
        return -1;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socket_fd < 0)
    {
        SOCKET_TRACE("socket failed");
        return -1;
    }


    bind_cid(socket_fd, cid);

    // memset(&local4, 0x00, sizeof(struct sockaddr_in));
    // local4.sin_family = AF_INET;
    // local4.sin_port = 1000+cid;

    // ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TEST_SOCKET_TCP_PORT);
    serv_addr.sin_addr = *((struct in_addr *)hostent->h_addr);

    ret = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(ret < 0)
    {
        SOCKET_TRACE("connect failed");
        return -1;
    }

    for(i = 0; i < 10; i++)
    {
        memset(data, 0, sizeof(data));
        ret = _socket_buf(socket_fd, data, cid, i);
        if(ret < 0)
        {
            SOCKET_TRACE("socket_buf failed");
            break;
        }
        data_len = strlen(data) + 1;

        ret = send(socket_fd, data, data_len, 0);

        if(ret != data_len)
        {
            SOCKET_TRACE("send failed, ret = %d", ret);
            break;
        }
        SOCKET_TRACE("send data_len: %d data: [%s]", data_len, data);

        memset(data, 0, sizeof(data));
        ret = recv(socket_fd, data, sizeof(data), 0);
        if(ret > 0)
        {
            SOCKET_TRACE("recv data_len: %d data: [%s]", ret, data);
        }
        else 
        {
            SOCKET_TRACE("recv failed, ret = %d", ret);
            break;
        }
        
        yopen_rtos_task_sleep_ms(2000);
    }
    ret = close(socket_fd);
    CU_ASSERT(ret == 0);
    
    return ret;
}



static int testUdp(uint8_t cid)
{
    int ret;
    char data[64];
    uint16_t data_len;
    int i = 0;
    struct hostent *hostent;
    int socket_fd;

    // struct sockaddr_in local4;

    hostent = gethostbyname(TEST_SOCKET_SERVER);
    if(hostent == NULL)
    {
        SOCKET_TRACE("gethostbyname failed");
        return -1;
    }

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(socket_fd < 0)
    {
        SOCKET_TRACE("socket failed");
        return -1;
    }


    bind_cid(socket_fd, cid);

    // memset(&local4, 0x00, sizeof(struct sockaddr_in));
    // local4.sin_family = AF_INET;
    // local4.sin_port = 1000+cid;

    // ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TEST_SOCKET_UDP_PORT);
    serv_addr.sin_addr = *((struct in_addr *)hostent->h_addr);

    socklen_t addr_len = sizeof(serv_addr);
    
    for(i = 0; i < 10; i++)
    {
        memset(data, 0, sizeof(data));
        ret = _socket_buf(socket_fd, data, cid, i);
        if(ret < 0)
        {
            SOCKET_TRACE("socket_buf failed");
            break;
        }
        data_len = strlen(data) + 1;

        ret = sendto(socket_fd, data, data_len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if(ret != data_len)
        {
            SOCKET_TRACE("send failed, ret = %d", ret);
            break;
        }
        SOCKET_TRACE("send data_len: %d data: [%s]", data_len, data);

        memset(data, 0, sizeof(data));
        ret = recvfrom(socket_fd, data, sizeof(data), 0, (struct sockaddr *)&serv_addr, &addr_len);
        if(ret > 0)
        {
            SOCKET_TRACE("recvfrom ip: %s data_len: %d data: [%s]", inet_ntoa(serv_addr.sin_addr), ret, data);
        }
        else 
        {
            SOCKET_TRACE("recv failed, ret = %d", ret);
            break;
        }
        
        yopen_rtos_task_sleep_ms(2000);
    }
    ret = close(socket_fd);
    CU_ASSERT(ret == 0);
    return ret;
}



void socket_cid_demo_thread(void * arg)
{
    int ret;
    uint8_t cid;
    do{
        if(yopen_network_register_wait(0, 20) == 0)
        {
            SOCKET_TRACE("network reg success");
            break;
        }
    }while(1);

    do{
        ret = yopen_start_data_call(0, 2, YOPEN_DATA_TYPE_IPV4V6, "CTNET", "", "", YOPEN_DATA_AUTH_TYPE_NONE);
        if(ret == 0)
        {
            SOCKET_TRACE("cid 2 active success");
            break;
        }
        yopen_rtos_task_sleep_ms(5000);
    }while(1);
    
    cid = 1;
    while(1)
    {

        SOCKET_TRACE("=================TEST_TCP CID = %d=================", cid);
        testTcp(cid);
        
        SOCKET_TRACE("=================TEST_UDP CID = %d=================", cid);
        testUdp(cid);

        if(cid == 1)
        {
            cid = 2;
        }
        else 
        {
            cid = 1;
        }
    }

    yopen_stop_data_call(0, 2);

    yopen_rtos_task_delete(NULL);
}

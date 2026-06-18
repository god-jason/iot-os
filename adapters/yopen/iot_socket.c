/**
 * @file iot_socket.c
 * @brief YOPEN 平台 Socket 适配器实现文件
 * @details 基于 lwip 库实现 Socket 接口的扩展功能。
 */

#include "iot_socket.h"
#include <string.h>
#include <errno.h>

int iot_socket_select(iot_socket_t *reads, int rcount,
                      iot_socket_t *writes, int wcount,
                      uint32_t timeout_ms)
{
    fd_set readfds, writefds;
    int maxfd = -1;
    struct timeval tv;
    int ret, i;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    /* 添加读socket */
    if (reads && rcount > 0) {
        for (i = 0; i < rcount; i++) {
            if (reads[i] >= 0) {
                FD_SET(reads[i], &readfds);
                if (reads[i] > maxfd) {
                    maxfd = reads[i];
                }
            }
        }
    }

    /* 添加写socket */
    if (writes && wcount > 0) {
        for (i = 0; i < wcount; i++) {
            if (writes[i] >= 0) {
                FD_SET(writes[i], &writefds);
                if (writes[i] > maxfd) {
                    maxfd = writes[i];
                }
            }
        }
    }

    if (maxfd < 0) {
        return IOT_ERR_PARAM;
    }

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = lwip_select(maxfd + 1, &readfds, &writefds, NULL, &tv);
    if (ret < 0) {
        return IOT_ERR;
    }

    /* 更新返回的socket数组 */
    if (reads && rcount > 0) {
        int j = 0;
        for (i = 0; i < rcount; i++) {
            if (reads[i] >= 0 && FD_ISSET(reads[i], &readfds)) {
                reads[j++] = reads[i];
            }
        }
    }

    if (writes && wcount > 0) {
        int j = 0;
        for (i = 0; i < wcount; i++) {
            if (writes[i] >= 0 && FD_ISSET(writes[i], &writefds)) {
                writes[j++] = writes[i];
            }
        }
    }

    return ret;
}

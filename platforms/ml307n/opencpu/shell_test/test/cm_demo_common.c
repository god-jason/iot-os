#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cm_virt_at.h"

int32_t cm_modem_get_pdp_state(uint16_t cid)
{
    int32_t pdp_state = -1;
    uint8_t rsp[20] = {0};
    int32_t rsp_len = 0;
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CGACT?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CGACT") != NULL)
            {
                // 解析 CGACT 信息，数据格式: "+CGACT: <cid>,<state>"
                char *ptr = (char *)rsp;
                while (1)
                {
                    // 跳过以 "+" 开头的部分
                    ptr = strstr(ptr, "+CGACT:");
                    if (ptr == NULL) break; // 没有找到下一组数据，退出循环

                    // 解析字段
                    unsigned short cid_read = 0;
                    int state_read = 0;

                    // 使用 sscanf 读取 cid 和 state
                    if (sscanf(ptr, "+CGACT: %hu,%d", &cid_read, &state_read) == 2)
                    {
                        if (cid == cid_read)
                        {
                            pdp_state = state_read;
                            break; // 找到目标，退出循环
                        }
                    }

                    // 移动到下一个可能的 "+CGACT:" 子串
                    ptr += strlen("+CGACT:");
                }
            }
    }
    return pdp_state;
}

int32_t cm_modem_get_csq(char *rssi, char *ber)
{
    char csq_rsp[30] = {0};
    int32_t rsp_len = 0;
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CSQ\r\n", csq_rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)csq_rsp, "+CSQ") != NULL)
        {
            // 解析 CSQ 信息，数据格式: "+CSQ: <rssi>,<ber>"
            sscanf((char *)csq_rsp, "\r\n+CSQ: %s.u,%s.u", rssi, strlen(rssi), ber, strlen(rssi));
            return 0;
        }
    }
    return -1;
}




#include "stdio.h"
#include "string.h"
#include "yopen_debug.h"
#include "yopen_ril.h"


#define DEMO_RIL_TRACE(fmt, ...) yopen_trace("[RIL] "fmt, ##__VA_ARGS__)

typedef enum {
    RIL_CMD_IND = 0,
    RIL_CMD_AT,
    RIL_CMD_CEREG,
    RIL_CMD_CFUN_0,
    RIL_CMD_CFUN_1,
    RIL_CMD_CGDCONT,
} RIL_CMD_ID;

//USERDATA可以根据需求定义
static struct ril_cmd
{
    void *userData;
    char *cmd;
} s_ril_cmd[5] = 
{
    {(void *)RIL_CMD_AT, "AT\r\n"},
    {(void *)RIL_CMD_CEREG, "AT+CEREG=2\r\n"},
    {(void *)RIL_CMD_CFUN_0, "AT+CFUN=0\r\n"},
    {(void *)RIL_CMD_CFUN_1, "AT+CFUN=1\r\n"},
    {(void *)RIL_CMD_CGDCONT, "AT+CGDCONT?\r\n"}
};

static int32_t _urc_cb(char *line, uint32_t line_len)
{
    DEMO_RIL_TRACE("========== RIL_URC_IND rsp %s ==========", line);
    return line_len;
}

static  int32_t _rsp_cb(char *line, uint32_t line_len, void *user_data)
{
	//DEMO_RIL_TRACE("========== line %s, %d, userdata %x ==========", line, line_len, user_data);

    switch((uint32_t)user_data)
    {   
        //ril 主动URC主动上报
        case RIL_CMD_AT:
        {
            DEMO_RIL_TRACE("========== RIL_CMD_AT rsp %s ==========", line);
            break;
        }
        case RIL_CMD_CEREG:
        {
            DEMO_RIL_TRACE("========== RIL_CMD_CEREG rsp %s ==========", line);
            break;
        }
        case RIL_CMD_CFUN_0:
        {
            DEMO_RIL_TRACE("========== RIL_CMD_CFUN_0 rsp %s ==========", line);
            break;
        }
        case RIL_CMD_CFUN_1:
        {
            DEMO_RIL_TRACE("========== RIL_CMD_CFUN_1 rsp %s ==========", line);
            break;
        }
        //ril 主动URC主动上报
        case RIL_CMD_CGDCONT:
        {
            DEMO_RIL_TRACE("========== RIL_CMD_CGDCONT rsp %s ==========", line);
            break;
        }
    }

	return line_len;
}

void yopen_ril_demo_task(void * arg)
{
    yopen_errcode_ril_e ret;
	
    yopen_rtos_task_sleep_ms(2000);

	DEMO_RIL_TRACE("========== ril demo start ==========");

    ret =  yopen_ril_init(_urc_cb);
    DEMO_RIL_TRACE("========== yopen_ril_init ret %d ==========", ret);

    for (int i=0; i<sizeof(s_ril_cmd)/sizeof(struct ril_cmd); i++)
    {
        DEMO_RIL_TRACE("========== yopen_ril_send_atcmd cmd %s  ==========", s_ril_cmd[i].cmd);

        ret =  yopen_ril_send_atcmd(s_ril_cmd[i].cmd, strlen(s_ril_cmd[i].cmd), _rsp_cb, s_ril_cmd[i].userData);
        yopen_rtos_task_sleep_ms(1000);
	    
    }
    
    yopen_rtos_task_delete(NULL);  // kill itself
}

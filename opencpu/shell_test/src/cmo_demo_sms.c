#ifdef CM_DEMO_SMS_SUPPORT
#include "cm_virt_at.h"
#include "cm_sms.h"
#include <nr_micro_shell.h>

#define cm_demo_printf osPrintf

//发送txt格式短信
void cm_sendmsg_test_txt_7(char *sms_num)
{
    int ret = 0;
    if(strlen(sms_num) != 11)
    {
        osPrintf("sms num invalid\r\n");
        return;
    }
    ret = cm_sms_send_txt("zhongyimozu", sms_num, CM_MSG_MODE_GSM_7, CM_SIM_ID_0);//SIM0卡发送txt格式GSM7编码短信
    cm_demo_printf("cm_sendmsg_test_txt_7,ret = %d\n",ret);
}

//发送txt格式短信
void cm_sendmsg_test_txt_8(char *sms_num)
{
    int ret = 0;
    if(strlen(sms_num) != 11)
    {
        osPrintf("sms num invalid\r\n");
        return;
    }
    ret = cm_sms_send_txt("4E2D79FB6A217EC4", sms_num,  CM_MSG_MODE_GSM_UCS2, CM_SIM_ID_0);//SIM0卡发送txt格式GSM8编码短信
    cm_demo_printf("cm_sendmsg_test_txt_8,ret = %d\n",ret);
}

//发送pdu格式短信
void cm_sendmsg_test_pdu_7(char *pdu_str)
{
    int ret = 0;
    if(strlen(pdu_str) < 11)
    {
        osPrintf("sms num invalid\r\n");
        return;
    }//0881683108200505F021000D91688158010507F90008084E2D79FB6A217EC4

    ret = cm_sms_send_pdu(pdu_str, "22", CM_MSG_MODE_GSM_7, CM_SIM_ID_0);//SIM0卡发送PDU格式GSM7编码短信
    cm_demo_printf("cm_sendmsg_test_pdu_7,ret = %d\n",ret);
    osDelay(15000);
}

static void SHELL_testSms(char argc, char **argv)
{
    if(argc >= 3 && argc <= 4 ) /* help */
    {
        if (0 == strcmp(argv[1], "pdu"))
        {
            cm_sendmsg_test_pdu_7(argv[2]);
        }
        else if (0 == strcmp(argv[1], "txt"))
        {
            if(0 == strcmp(argv[2], "7"))
                cm_sendmsg_test_txt_7(argv[3]);
            else
                cm_sendmsg_test_txt_8(argv[3]);
        }
        else
            osPrintf("sms parameter invalid, txt or pdu\r\n");
    }
    else
        osPrintf("sms parameter num err\r\n");
}
NR_SHELL_CMD_EXPORT(cm_sms, SHELL_testSms);

#endif


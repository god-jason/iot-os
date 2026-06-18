#ifdef CM_DEMO_SMS_SUPPORT
#ifdef OS_USING_SHELL
#include <string.h>
#include "cm_sms.h"

#define cm_demo_printf osPrintf
void sms_send_callback(int32_t result, void *user_data)
{
    cm_demo_printf("sms_send_callback,result[%d], user_data[%s]\n",result, user_data);
}

void cm_setsmsc_test(char *smsc)
{
    int ret = 0;
    ret = cm_sms_set_smsc(smsc, CM_MAIN_SIM);
    cm_demo_printf("cm_setsmsc_test,ret[%d]\n", ret);
}

void cm_getsmsc_test()
{
    int ret = 0;
    char smsc[20] = {0};
    ret = cm_sms_get_smsc(smsc, 20, CM_MAIN_SIM);
    cm_demo_printf("cm_getsmsc_test,ret[%d],smsc[%s]\n", ret, smsc);
}

//发送txt格式短信
void cm_sendmsg_test_txt(cm_msg_mode_e msg_mode, char *sms_num)
{
    if(strlen(sms_num) != 11 || sms_num == NULL)
    {
        cm_demo_printf("sms num invalid\r\n");
        return;
    }
    int ret = 0;
    if(msg_mode == CM_MSG_MODE_GSM_7)
    {
        ret = cm_sms_send_txt_v2("zhongyimozu", sms_num, CM_MSG_MODE_GSM_7, CM_MAIN_SIM, sms_send_callback, "7_txt");
    }
    else if(msg_mode == CM_MSG_MODE_UCS2)
    {
        ret = cm_sms_send_txt_v2("4E2D79FB6A217EC4", sms_num,  CM_MSG_MODE_UCS2, CM_MAIN_SIM, sms_send_callback, "ucs2_txt");
    }
    else if(msg_mode == CM_MSG_MODE_GSM_8)
    {
        ret = cm_sms_send_txt_v2("7A686F6E6779696D6F7A75", sms_num,  CM_MSG_MODE_GSM_8, CM_MAIN_SIM, sms_send_callback, "8_txt");
    }
    cm_demo_printf("cm_sendmsg_test_txt,ret[%d]\n",ret);
}


//发送pdu格式短信
void cm_sendmsg_test_pdu(char *tpdu_len, char *pdu_str)
{
    int ret = 0;
    if(strlen(pdu_str) < 11 || pdu_str == NULL)
    {
        cm_demo_printf("pdu str invalid\r\n");
        return;
    }//tpdu_len=22,pdu_str=0881683108200505F021000D91688158010507F90008084E2D79FB6A217EC4

    ret = cm_sms_send_pdu_v2(pdu_str, tpdu_len, CM_MSG_MODE_GSM_7, CM_MAIN_SIM, sms_send_callback, "pdu");
    cm_demo_printf("cm_sendmsg_test_pdu,ret[%d]\n",ret);
}

static void SHELL_testSms_old(char argc, char **argv)
{
    int ret = -1;
    if(argv == NULL || argc != 4)
    {
        cm_demo_printf("shell cmd wrong\n");
        return;
    }
    if(argv[1] != NULL && 0 == strcmp(argv[1], "send_txt"))
    {
        if(argv[2] != NULL && 0 == strcmp(argv[2], "7"))
        {
            ret = cm_sms_send_txt("zhongyimozu", argv[3], CM_MSG_MODE_GSM_7, CM_MAIN_SIM);
        }
        else if(argv[2] != NULL && 0 == strcmp(argv[2], "ucs2"))
        {
            ret = cm_sms_send_txt("zhongyimozu", argv[3], CM_MSG_MODE_UCS2, CM_MAIN_SIM);
        }
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "send_pdu") && argv[2] != NULL && argv[3] != NULL)
    {
        ret = cm_sms_send_pdu(argv[3], argv[2], CM_MSG_MODE_GSM_7, CM_MAIN_SIM);
    }
    cm_demo_printf("cm_sendmsg_test_pdu,ret[%d]\n",ret);
}

static void SHELL_testSms(char argc, char **argv)
{
    if(argv == NULL)
    {
        cm_demo_printf("shell cmd wrong\n");
        return;
    }
    if(argv[1]!= NULL)
    {
        if(argc == 3 && 0 == strcmp(argv[1], "set_smsc") && argv[2] != NULL)
        {
            cm_setsmsc_test(argv[2]);
            return;
        }
        else if(argc == 2 && 0 == strcmp(argv[1], "get_smsc"))
        {
            cm_getsmsc_test();
            return;
        }
        else if (argc == 4 && 0 == strcmp(argv[1], "send_txt") && argv[2] != NULL && argv[3] != NULL)
        {
            if(0 == strcmp(argv[2], "7"))
            {
                cm_sendmsg_test_txt(CM_MSG_MODE_GSM_7, argv[3]);
                return;
            }
            else if(0 == strcmp(argv[2], "8"))
            {
                cm_sendmsg_test_txt(CM_MSG_MODE_GSM_8, argv[3]);
                return;
            }
            else
            {
                cm_sendmsg_test_txt(CM_MSG_MODE_UCS2, argv[3]);
                return;
            }
        }
        else if (argc == 4 && 0 == strcmp(argv[1], "send_pdu") && argv[2] != NULL && argv[3] != NULL)
        {
            cm_sendmsg_test_pdu(argv[2], argv[3]);
            return;
        }
    }

    osPrintf("Usage:\n");
    osPrintf("  sms set_smsc X                        \n");
    osPrintf("  sms get_smsc                          \n");
    osPrintf("  sms send_txt X(7/8/ucs2) Y(phone_num)   \n");
    osPrintf("  sms send_pdu X(tpdu_len) Y(PDU)       \n");
}
#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(sms, SHELL_testSms);
NR_SHELL_CMD_EXPORT(cm_sms, SHELL_testSms_old);
#endif

#endif


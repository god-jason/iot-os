#include "cm_os.h"
#include "cm_sys.h"
#include "cm_pm.h"
#include "string.h"
#include "cm_ntp.h"
#include "cm_lwm2m.h"
#include "cm_adc.h"
#include "cm_rtc.h"
#include "stdio.h"
#include "nr_micro_shell.h"
#include "cm_common_api.h"

#ifdef CM_DEMO_ONENET_SUPPORT

#define CM_DEMO_ONENET_LOG(fmt, ...) cm_log_printf(0, fmt, ##__VA_ARGS__)

#ifdef OS_USING_SHELL
#define cm_demo_printf(fmt, ...) shell_printf(fmt, ##__VA_ARGS__)
#else
#define cm_demo_printf(fmt, ...) CM_DEMO_ONENET_LOG(fmt, ##__VA_ARGS__)
#endif


#define SECOND_OF_DAY                       (24*60*60)
#define ONENET_TEST_NTP_CFG_PORT            123
#define ONENET_TEST_NTP_CFG_TIMEOUT         6000
#define ONENET_TEST_NTP_CFG_DNS_PRIORITY    1
#define ONENET_TEST_NTP_CFG_SET_RTC         1

typedef enum
{
    APP_LWM2M_ONENET_NO_REG = 0,
    APP_LWM2M_ONENET_REG_SUCCESS,
    APP_LWM2M_ONENET_REG_FAILED,
} app_lwm2m_onenet_state_e;


static const char * g_weekday[] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
static const char g_day_of_mon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

static cm_lwm2m_handle_t g_onenet_dev = NULL;         //OneNET设备
static app_lwm2m_onenet_state_e g_onenet_state = APP_LWM2M_ONENET_NO_REG;

static void cm_sec_to_date(long lSec, cm_tm_t *tTime)
{
    unsigned short i,j,iDay;
    unsigned long lDay;

    lDay = lSec / SECOND_OF_DAY;    
    lSec = lSec % SECOND_OF_DAY;

    i = 1970;
    while(lDay > 365)
    {
        if(((i%4==0)&&(i%100!=0)) || (i%400==0))
        {
            lDay -= 366;
        }
        else
        {
            lDay -= 365;
        }
        i++;
    }
    if((lDay == 365) && !(((i%4==0)&&(i%100!=0)) || (i%400==0)))
    {
       lDay -= 365;
       i++;
    }
    tTime->tm_year = i;   
    for(j=0;j<12;j++)   
    {
        if((j==1) && (((i%4==0)&&(i%100!=0)) || (i%400==0)))
        {
            iDay = 29;
        }
        else
        {
            iDay = g_day_of_mon[j];
        }
        if(lDay >= iDay) lDay -= iDay;
        else break;
  }
    tTime->tm_mon = j+1;
    tTime->tm_mday = lDay+1;
    tTime->tm_hour = ((lSec / 3600))%24;//这里注意，世界时间已经加上北京时间差8，
    tTime->tm_min = (lSec % 3600) / 60;
    tTime->tm_sec = (lSec % 3600) % 60;
}

static uint8_t cm_time_to_weekday(cm_tm_t *t)
{
    uint32_t u32WeekDay = 0;
    uint32_t u32Year = t->tm_year;
    uint8_t u8Month = t->tm_mon;
    uint8_t u8Day = t->tm_mday;
    if(u8Month < 3U)
    {
        /*D = { [(23 x month) / 9] + day + 4 + year + [(year - 1) / 4] - [(year - 1) / 100] + [(year - 1) / 400] } mod 7*/
        u32WeekDay = (((23U * u8Month) / 9U) + u8Day + 4U + u32Year + ((u32Year - 1U) / 4U) - ((u32Year - 1U) / 100U) + ((u32Year - 1U) / 400U)) % 7U;
    }
    else
    {
        /*D = { [(23 x month) / 9] + day + 4 + year + [year / 4] - [year / 100] + [year / 400] - 2 } mod 7*/
        u32WeekDay = (((23U * u8Month) / 9U) + u8Day + 4U + u32Year + (u32Year / 4U) - (u32Year / 100U) + (u32Year / 400U) - 2U ) % 7U;
    }

    if (0U == u32WeekDay)
    {
        u32WeekDay = 7U;
    }

    return (uint8_t)u32WeekDay;
}

static void __app_lwm2m_event_cb(int32_t event, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[event] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]event:%d\r\n", param.handle, (char *)param.cb_param, event);
    }
    switch(event)
    {
        case CM_LWM2M_EVENT_REG_SUCCESS:
        {
            g_onenet_state = APP_LWM2M_ONENET_REG_SUCCESS;
            cm_demo_printf("lwm2m register ok\r\n");
        }
        break;
        case CM_LWM2M_EVENT_REG_FAILED:
        {
            g_onenet_state = APP_LWM2M_ONENET_REG_FAILED;
            cm_demo_printf("lwm2m register fail\r\n");
        }
        break;
        case CM_LWM2M_EVENT_UNREG_DONE:
        {
            g_onenet_state = APP_LWM2M_ONENET_NO_REG;
            cm_demo_printf("lwm2m unregister done\r\n");
        }
        break;
        default:
        break;
    }
}

static void __app_lwm2m_notify_cb(int32_t mid, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[notify] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]mid:%d\r\n", param.handle, (char *)param.cb_param, mid);
    }
}

static void __app_lwm2m_read_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[read] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]mid:%d objid:%d insid:%d resid:%d\r\n", param.handle, (char *)param.cb_param, mid, objid, insid, resid);
    }

    /* 收到平台读操作事件后，发送温度数据至平台 */
    if (3303 == objid)
    {
        int32_t voltage = 0;            //mV
        int32_t temperature = 0;        //0.1摄氏度
        char temp_data[16] = {0};

        /* 使用ADC采集温度传感器的电压 */
        cm_adc_read(CM_ADC_0, &voltage);

        /* 电压转温度（仅为示例，应采用实际温度传感器算法） */
        temperature = (1430 - voltage) * 100 / 43;

        sprintf(temp_data, "%ld.%ld", (temperature / 10), temperature % 10);

        int32_t ret = cm_lwm2m_read_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED, objid, insid, resid, 4, temp_data, strlen(temp_data));

        if (CM_LWM2M_SUCCESS != ret)
        {
            cm_demo_printf("cm_lwm2m_read_rsp() fail, ret is %d\r\n", ret);
            return;
        }
    }
}

static void __app_lwm2m_write_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, int32_t type, int32_t is_over, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[write] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]mid:%d objid:%d insid:%d resid:%d type:%d is_over:%d len:%d data:%s\r\n", param.handle, (char *)param.cb_param, mid, objid, insid, resid, type, is_over, len, data);
    }

    int32_t ret = cm_lwm2m_write_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("cm_lwm2m_write_rsp() fail, ret is %d\r\n", ret);
        return;
    }
}

static void __app_lwm2m_execute_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[execute] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]mid:%d objid:%d insid:%d resid:%d len:%d data:%s\r\n", param.handle, (char *)param.cb_param, mid, objid, insid, resid, len, data);
    }

    int32_t ret = cm_lwm2m_execute_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("cm_lwm2m_execute_rsp() fail, ret is %d\r\n", ret);
        return;
    }
}

static void __app_lwm2m_observe_cb(int32_t mid, int32_t observe, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[observe] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]%d_%d_%d: %d\r\n", param.handle, (char *)param.cb_param, objid, insid, resid, observe);
    }
}

static void __app_lwm2m_discover_cb(int32_t mid, int32_t objid, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[discover] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]discover mid:%d objid:%d \r\n", param.handle, (char *)param.cb_param, mid, objid);
    }
}

static void __app_lwm2m_params_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *parameter, int32_t len, cm_lwm2m_cb_param_t param)
{
    if (param.cb_param == NULL)
    {
        cm_demo_printf("[params] cb_param is NULL\n");
    }
    else
    {
        cm_demo_printf("[%d %s]discover mid:%d objid:%d insid:%d resid:%d len:%d parameter:%s\r\n", param.handle, (char *)param.cb_param, mid, objid, insid, resid, resid, parameter);
    }

    int32_t ret = cm_lwm2m_param_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("cm_lwm2m_param_rsp() fail, ret is %d\r\n", ret);
        return;
    }
}

static void __app_ntp_cb(cm_ntp_event_e event, void *event_param, void *cb_param)
{
    if (event == CM_NTP_EVENT_SYNC_OK)
    {
        cm_demo_printf("[NTP][%s] get net time %s\r\n", cb_param ? (char *)cb_param : "NULL", event_param);
    }
    else if (event == CM_NTP_EVENT_SETTIME_FAIL)
    {
        cm_demo_printf("[NTP][%s] get net time %s OK, but set time fail\r\n", cb_param ? (char *)cb_param : "NULL", event_param);
    }
    else
    {
        cm_demo_printf("[NTP][%s] error\r\n", cb_param ? (char *)cb_param : "NULL");
    }
}

/**
 * OneNET线程
 *
 */
void onenet_demo_appimg_enter(char *param)
{
    /* 等待模组PDP激活 */
    int32_t pdp_time_out = 0;
    int32_t ret = 0;
    int16_t mid = 0;

    while(1)
    {
        if(pdp_time_out > 20)
        {
            cm_demo_printf("network timeout\r\n");
            cm_pm_reboot(0);
        }
        if(cm_modem_get_pdp_state(1) == 1)
        {
            cm_demo_printf("network ready\r\n");
            break;
        }
        osDelay(200);
        pdp_time_out++;
    }

    /* 适当等待（模组PDP激活到能进行数据通信需要适当等待，或者通过PING操作确定模组数据通信是否可用） */
    osDelay(1000);

    /* 获取NTP时间 */
    uint16_t port = ONENET_TEST_NTP_CFG_PORT;
    uint32_t timeout = ONENET_TEST_NTP_CFG_TIMEOUT;
    uint32_t dns_priority = ONENET_TEST_NTP_CFG_DNS_PRIORITY;
    bool set_rtc = ONENET_TEST_NTP_CFG_SET_RTC;

    /* 使用的NTP服务器为公共服务器，服务器同一时间可能出现高并发的授时请求，有一定概率失败 */
    /* 若应用对授时成功率有要求，请使用高可靠性的专用服务器，或失败后重新请求获取时间 */
    cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "cn.ntp.org.cn"); 
    cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port);
    cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout);
    cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns_priority);
    cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &set_rtc);
    cm_ntp_set_cfg(CM_NTP_CFG_CB, __app_ntp_cb);
    cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, "SYNCHTIME");

    if (0 != cm_ntp_sync())
    {
        cm_demo_printf("[NTP]cm_ntp_sync() fail\r\n");
    }

    /* OneNET登陆 */
    cm_lwm2m_cfg_t cfg = {0};
    cfg.platform = CM_LWM2M_ONENET;
    cfg.endpoint.pattern = 2;       //IMEI;IMSI，OneNET平台默认方式，库中自行填充IMEI、IMSI数据
    cfg.endpoint.name = NULL;
    cfg.host = "183.230.40.39";     //onenet server address
    cfg.flag = 3;       //bootstrap服务器，OneNET不支持moniter功能
    cfg.auth_code = NULL;
    cfg.psk = NULL;
    cfg.pskid = NULL;
    cfg.auto_update = 1;    //开启自动更新
    cfg.cb.onRead = __app_lwm2m_read_cb;
    cfg.cb.onWrite = __app_lwm2m_write_cb;
    cfg.cb.onExec = __app_lwm2m_execute_cb;
    cfg.cb.onObserve = __app_lwm2m_observe_cb;
    cfg.cb.onParams = __app_lwm2m_params_cb;
    cfg.cb.onEvent = __app_lwm2m_event_cb;
    cfg.cb.onNotify = __app_lwm2m_notify_cb;
    cfg.cb.onDiscover = __app_lwm2m_discover_cb;
    cfg.cb_param = "OneNET";

    ret = cm_lwm2m_create(&cfg, &g_onenet_dev);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_create() fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    uint8_t instances_3303[1] = {1};
    ret = cm_lwm2m_add_obj(g_onenet_dev, 3303, instances_3303, 1, 0, 0);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_add_obj() obj 3303 fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    uint8_t instances_3306[1] = {1};
    ret = cm_lwm2m_add_obj(g_onenet_dev, 3306, instances_3306, 1, 0, 0);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_add_obj() obj 3306 fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    int32_t resoures_3303[2] = {5700, 5601};
    ret = cm_lwm2m_discover(g_onenet_dev, 3303, resoures_3303, 2);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_discover() obj 3303 fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    int32_t resoures_3306[2] = {5852, 5853};
    ret = cm_lwm2m_discover(g_onenet_dev, 3306, resoures_3306, 2);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_discover() obj 3306 fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    ret = cm_lwm2m_open(g_onenet_dev, 30, 3000);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("[OneNET] cm_lwm2m_open() fail, ret is %d\r\n", ret);
        cm_pm_reboot(0);
    }

    /* 等待OneNET登陆完成 */
    while(1)
    {
        if (APP_LWM2M_ONENET_REG_FAILED == g_onenet_state)
        {
            cm_demo_printf("[OneNET] APP_LWM2M_ONENET_REG_FAILED\r\n");
            cm_pm_reboot(0);
        }
        else if(APP_LWM2M_ONENET_REG_SUCCESS == g_onenet_state)
        {
            cm_demo_printf("[OneNET] APP_LWM2M_ONENET_REG_SUCCESS\r\n");
            break;
        }
        else
        {
            cm_demo_printf("[OneNET] APP_LWM2M_ONENET_NO_REG\r\n");
            osDelay(200);
        }
    }

    int32_t voltage = 0;            //mV
    int32_t temperature = 0;        //0.1摄氏度
    char temp_data[16] = {0};
    cm_tm_t t;

    while(1)
    {
        /* NTP协议规定底层采用UDP，存在一定的失败概率，应用可添加定期更新NTP时间或者失败重试逻辑，例如定时进行NTP请求 */
#if 0
        if (0 != cm_ntp_sync())
        {
            cm_demo_printf("[NTP]cm_ntp_sync() fail\r\n");
        }
#endif

        /* 使用ADC采集温度传感器的电压 */
        cm_adc_read(CM_ADC_0, &voltage);

        /* 电压转温度（仅为示例，应采用实际温度传感器算法） */
        temperature = (1430 - voltage) * 100 / 43;

        sprintf(temp_data, "%ld.%ld", (temperature / 10), temperature % 10);

        /* log打印当前时间和温度 */
        cm_sec_to_date((long)(cm_rtc_get_current_time() + cm_rtc_get_timezone() * 60 * 60), &t);
        cm_demo_printf("Now:%d-%d-%d:%d:%d:%d,%s,Temperature:%s\r\n", t.tm_year, t.tm_mon , t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, g_weekday[cm_time_to_weekday(&t)-1], temp_data);

        /* 定时上报OneNET温度数据 */
        /* 服务器支持的编码格式（即content_type入参）支持情况请与平台确认，本用例开发时OneNET平台支持的格式为TLV */
        ret = cm_lwm2m_notify_packing(g_onenet_dev, 3303, 0, 5700, 4, temp_data, strlen(temp_data), 4);

        if (CM_LWM2M_SUCCESS != ret)
        {
            cm_demo_printf("[OneNET] cm_lwm2m_notify_packing() fail, ret is %d\r\n", ret);
        }

        ret = cm_lwm2m_notify(g_onenet_dev, mid);

        /* 一定时间内，使用同样的mid可能会被OneNET平台认定为非法数据过滤掉（具体使用限制请咨询OneNET平台），故采用每次递增的方式实现 */
        mid++;

        if (CM_LWM2M_SUCCESS != ret)
        {
            cm_demo_printf("[OneNET] cm_lwm2m_notify() fail, ret is %d\r\n", ret);
        }

        osDelay(12000);
    }
}

void cm_test_onenet(int argc, char **argv)
{
    if (argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    if (argv[1] == NULL)
    {
        cm_demo_printf("cm_test_onenet: argv[1] is NULL\n");
        return;
    }
    const char *cmd = argv[1];
    if (strncmp(cmd, "test", 4) == 0)
    {
        cm_demo_printf("onenet test start!!\r\n");
        cm_thread_create("onenet_demo_task", 4096 * 2, osPriorityNormal, (cm_thread_func_t)onenet_demo_appimg_enter);
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\r\n", cmd);
    }
}

NR_SHELL_CMD_EXPORT(onenet, cm_test_onenet);

#endif

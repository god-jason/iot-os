/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __IP4_FLOWCTRL_H__
#define __IP4_FLOWCTRL_H__

#include "lwip/prot/tcp.h"
#include "lwip/prot/udp.h"
#include "psram_layout.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#if 0
#define FPGA_DELAY_RATE    (0) //右移3位，降位1/8

#define FLC_DL_RECV_WIN     (65535 << 1)
#define FLC_UL_SND_WIN     (65535)


#define FLC_AP_SHARE_BUF_LEN DRAM_BASE_LEN_AP_SHARE

#define FLC_RTT_MEAS_INTERVAL     (1000)  //ms
#define FLC_RTT_DEFAULT           (10*1000) //ms
#define FLC_RTT_LAB              (150)//(150)//ms

#define FLC_PRF_LAB_UL           (5*1000 >> 3 >> FPGA_DELAY_RATE) //ms
#define FLC_MEM_LAB_UL           (FLC_PRF_LAB_UL * FLC_RTT_LAB)
#define FLC_PRF_TH_MAX_UL        (FLC_PRF_LAB_UL << 4) //ms

#define FLC_APBUF_USED_RATE_THRESH      (65)  //625*Rtt_Lab/FLC_AP_SHARE_BUF_LEN


#define FLC_UL_SEND_WIN_MIN     (3072)

#define FLC_TCP_OPT_LEN_WS      (3)

#define FCL_TMR_INTERVAL        (100)
#define FCL_SECOND_BW           (1000)

#endif
#define FLC_APBUF_USED_RATE_OFF      (90)

#define FLC_APBUF_USED_RATE_THR_2      (60*1024)
//#define FLC_APBUF_USED_RATE_THR_2      (40)
#define FLC_MODEMBUF_USED_RATE_THR_2   (75)

#define FLC_APBUF_USED_RATE_THR_1      (105*1024)
//#define FLC_APBUF_USED_RATE_THR_1      (70)
#define FLC_MODEMBUF_USED_RATE_THR_1   (85)

#define FLC_APBUF_USED_RATE_THR_OFF      (85)
#define FLC_APBUF_USED_THR_OFF      (160*1024)


#define LWIP_TCP_WS_SHIFT_COUNT_DL (1)
#define LWIP_TCP_WS_SHIFT_COUNT_UL (1)


#define FCL_DIRECTION_DL (0)
#define FCL_DIRECTION_UL (1)


#define FCL_WIN_SIZE_MIN    (0x2800)  //10KB
#define FCL_WIN_SIZE        (0xFFFF)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    FCL_ICMP_TYPE_NONE = 0,       /* 不是 ping 报文 */
    FCL_ICMP_TYPE_IP4_REQUEST, /* IPv4 的 ping 请求报文 */
    FCL_ICMP_TYPE_IP4_REPLY,      /* IPv4 的 ping 响应报文 */
    FCL_ICMP_TYPE_IP6_REQUEST, /* IPv6 的 ping 请求报文 */
    FCL_ICMP_TYPE_IP6_REPLY,      /* IPv6 的 ping 响应报文 */
}FCL_IcmpType;

typedef struct
{
#if 0
    uint8_t        winScaleUl;
    uint8_t        winScaleDl;
    uint8_t        dlFlag;

    //估算RTT
    uint8_t        rttFlag;

    uint32_t        seqNoRtt;
    osTime_t        baseTimeRtt;
    osTime_t        rttMinLast;
    osTime_t        rttMin;

    //估算发送字节数
    //uint32_t        ackNoSyn;
    osTime_t        lastTime;
    uint32_t        lastAckSeqno;
    uint32_t        accountedFor;
    uint32_t        bk;

    //估算带宽
    //uint32_t        bwEstLast;
    uint32_t        bwEst;
#endif
     uint32_t        seqNoRetrans;
}fcl_westwood_t;


typedef struct
{
    uint32_t dl_total;
    uint32_t dl_natin;
    uint32_t dl_ip6in;
    uint32_t dl_eth;
    uint32_t dl_eth_err;
    uint32_t dl_lwip;
    uint32_t dl_drop;

    uint32_t ul_total;
    uint32_t ul_natout;
    uint32_t ul_ps;
    uint32_t ul_ps_ack;
    uint32_t ul_ps_err;

    uint32_t ping_req_from_drv;
    uint32_t ping_req_to_ps;

    uint32_t ping_reply_from_ps;
    uint32_t ping_reply_to_drv;

    uint32_t retrans;
   // uint32_t highPkt;
} lwip_stastic_t;

typedef uint8_t (*IsServiceOn)(void);

/************************************************************************************
 *                                 变量声明
 ************************************************************************************/
extern lwip_stastic_t ip_static_default;

extern uint32_t tcpBwUl;
extern uint32_t tcpRttMin;

extern IsServiceOn isServOn;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

#define NET_FCL_PRINT                      osPrintf

/**
 ************************************************************************************
 * @brief           IP4的TCP协议报文流控.
 *                  如果apsharebuf使用率超过FLC_APBUF_USED_RATE_THR，上行TCP发送窗调整到FLC_TCP_SEND_WIN_MIN
 *                  如果modemsharebuf使用率超过FLC_APBUF_USED_RATE_THR，上行TCP发送窗调整到FLC_TCP_SEND_WIN_MIN
 *                  其他情况，通过apsharebuf剩余空间大小配置上行TCP发送窗
 *
 * @param[in]       tcpHdr             TCP协议头地址
 *
 * @return
 ************************************************************************************
*/
int8_t FCL_Ip4TCPUl(struct tcp_hdr     *tcpHdr);

int8_t FCL_Ip4TCPDl(struct tcp_hdr     *tcpHdr);

/**
 ************************************************************************************
 * @brief           IP4的UDP协议报文流控.
 *                  如果apsharebuf使用率超过FLC_APBUF_USED_RATE_THR，UDP报文直接丢包
 *                  如果modemsharebuf使用率超过FLC_APBUF_USED_RATE_THR，UDP报文直接丢包
 *
 * @param[in]       udpHdr           UDP协议头地址
 *
 * @return          void
 ************************************************************************************
*/
int8_t FCL_UdpFilter(struct pbuf *p);

/**
 ************************************************************************************
 * @brief           流控定时检测函数.
 *                  如果apsharebuf使用率超过FLC_APBUF_USED_RATE_OFF，则通知spi网卡暂停接收
 *
 * @param[in]       udpHdr           UDP协议头地址
 *
 * @return          void
 ************************************************************************************
*/
void fcl_tmr(void *arg);


int8_t FCL_TcpUpdateWinUl(void *fclPriv,struct tcp_hdr *tcpHdr);

int8_t FCL_TcpUpdateWinDl(void *fclPriv,struct tcp_hdr *tcpHdr);

int FCL_isTcpRetrans(void *fclPriv,struct tcp_hdr *tcpHdr);

int8_t FCL_isPingEcho(void *payload);

int8_t FCL_isPingReply(void *payload);

int8_t FCL_TcpFilter(void);

int8_t FCL_ip6Ul(struct pbuf *p);

void FCL_isServOnReg(IsServiceOn callback);

#ifdef __cplusplus
}
#endif
#endif


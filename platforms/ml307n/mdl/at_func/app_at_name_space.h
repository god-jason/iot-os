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

#ifndef __APP_AT_NAME_SPACE_H__
#define __APP_AT_NAME_SPACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#if defined(AT_CMDSET_Q)
/* AT_SOCKET */
#define AT_SOCKET_CFG             "+QICFG"
#define AT_SOCKET_OPEN            "+QIOPEN"
#define AT_SOCKET_CLOSE           "+QICLOSE"
#define AT_SOCKET_STATE           "+QISTATE"
#define AT_SOCKET_SEND            "+QISEND"
#define AT_SOCKET_READ            "+QIRD"
#define AT_SOCKET_SENDEX          "+QISENDEX"
#define AT_SOCKET_WTMD            "+QISWTMD"
#define AT_SOCKET_SDE             "+QISDE"
#define AT_SOCKET_GETERROR        "+QIGETERROR"
#define AT_SOCKET_URC             "+QIURC"
#define AT_SOCKET_CFG_SNDTIMEOUT  "qisend/timeout"

/* AT_MQTT */
#define AT_MQTT_CFG               "+QMTCFG"
#define AT_MQTT_OPEN              "+QMTOPEN"
#define AT_MQTT_CLOSE             "+QMTCLOSE"
#define AT_MQTT_CONN              "+QMTCONN"
#define AT_MQTT_DISC              "+QMTDISC"
#define AT_MQTT_SUB               "+QMTSUB"
#define AT_MQTT_UNS               "+QMTUNS"
#define AT_MQTT_PUBEX             "+QMTPUBEX"
#define AT_MQTT_RECV              "+QMTRECV"
#define AT_MQTT_PING              "+QMTPING"
#define AT_MQTT_DROP              "+QMTDROP"
#define AT_MQTT_CFG_MTPING        "qmtping"


/* AT_HTTP */
#define AT_HTTP_CFG               "+QHTTPCFG"
#define AT_HTTP_URL               "+QHTTPURL"
#define AT_HTTP_GET               "+QHTTPGET"
#define AT_HTTP_GETEX             "+QHTTPGETEX"
#define AT_HTTP_POST              "+QHTTPPOST"
#define AT_HTTP_POSTFILE          "+QHTTPPOSTFILE"
#define AT_HTTP_PUT               "+QHTTPPUT"
#define AT_HTTP_PUTFILE           "+QHTTPPUTFILE"
#define AT_HTTP_READ              "+QHTTPREAD"
#define AT_HTTP_READFILE          "+QHTTPREADFILE"
#define AT_HTTP_CFGEX             "+QHTTPCFGEX"
#define AT_HTTP_SEND              "+QHTTPSEND"
#define AT_HTTP_STOP              "+QHTTPSTOP"
#define AT_HTTP_URC               "+QHTTPURC"

/* AT_SSL */
#define AT_SSL_CFG                "+QSSLCFG"
#define AT_SSL_OPEN               "+QSSLOPEN"
#define AT_SSL_SEND               "+QSSLSEND"
#define AT_SSL_RECV               "+QSSLRECV"
#define AT_SSL_CLOSE              "+QSSLCLOSE"
#define AT_SSL_STATE              "+QSSLSTATE"
#define AT_SSL_URC                "+QSSLURC"

/* AT_VFS */
#define AT_DISKINFO               "+QFLDS"
#define AT_LISTFILE               "+QFLST"
#define AT_DELETEFILE             "+QFDEL"
#define AT_UPLOADFILE             "+QFUPL"
#define AT_DWLOADFILE             "+QFDWL"
#define AT_OPENFILE               "+QFOPEN"
#define AT_READFILE               "+QFREAD"
#define AT_WRITEFILE              "+QFWRITE"
#define AT_SEEKFILE               "+QFSEEK"
#define AT_FILEPOS                "+QFPOSITION"
#define AT_TUCATFILE              "+QFTUCAT"
#define AT_CLOSEFILE              "+QFCLOSE"
#define AT_MOVEFILE               "+QFMOV"
#define AT_FLUSHFILE              "+QFFLUSH"
#define AT_USBWAIT                "+QUSBWAITTIME"
#define AT_CHANGEDIR              "+QFCWD"
#define AT_MAKEDIR                "+QFMKD"
#define AT_CURRDIR                "+QFPWD"

/* AT_FOTA */
#define AT_QFOTADL                "+QFOTADL"

#define AT_X_PING                 "+QPING"
#define AT_X_NTP                  "+QNTP"
#define AT_X_IDNSGIP              "+QIDNSGIP"  //  用域名获取 IP 地址
#define AT_X_IDNSCFG              "+QIDNSCFG"  //  DNS配置
#define AT_X_ICSGP                "+QICSGP"    //  配置Context Profile
#define AT_X_IACT                 "+QIACT"     //  激活Context Profile
#define AT_X_IDEACT               "+QIDEACT"   //  去激活Context Profile
#define AT_X_POWD                 "+QPOWD"
#define AT_X_POWD_RSP             "POWERED DOWN"

#define AT_X_GSN                  "+QGSN"
#define AT_X_CCID                 "+QCCID"
#define AT_X_NWINFO               "+QNWINFO"
#define AT_X_ENG                  "+QENG"
#define AT_X_CELL                 "+QCELL"
#define AT_X_CELLEX               "+QCELLEX"
#define AT_X_CELLINFO             "+QCELLINFO"
#define AT_X_URCCFG               "+QURCCFG"
#define AT_X_SCLK                 "+QSCLK"
#define AT_X_SCLKEX               "+QSCLKEX"
#define AT_X_DSIM                 "+QDSIM"
#define AT_X_LTS                  "+QLTS"

/* AT_TTS */
#define AT_X_TTSETUP              "+QTTSETUP"
#define AT_X_TTS                  "+QTTS"
#define AT_X_DAI                  "+QDAI"
#define AT_X_AUDSW                "+QAUDSW"


#define AT_X_NETDEVCTL            "+QNETEVCTL"
#define AT_X_INDCFG               "+QINDCFG"
#define AT_X_IND                  "+QIND"

#define AT_X_URC_RDY              "RDY"
#define AT_X_URC_MODE             "^MODE"
#endif

#if defined(AT_CMDSET_I)
/* AT_SOCKET */
#define AT_SOCKET_CFG             "+IICFG"
#define AT_SOCKET_OPEN            "+IIOPEN"
#define AT_SOCKET_CLOSE           "+IICLOSE"
#define AT_SOCKET_STATE           "+IISTATE"
#define AT_SOCKET_SEND            "+IISEND"
#define AT_SOCKET_READ            "+IIRD"
#define AT_SOCKET_SENDEX          "+IISENDEX"
#define AT_SOCKET_WTMD            "+IISWTMD"
#define AT_SOCKET_SDE             "+IISDE"
#define AT_SOCKET_GETERROR        "+IIGETERROR"
#define AT_SOCKET_URC             "+IIURC"
#define AT_SOCKET_CFG_SNDTIMEOUT  "iisend/timeout"

/* AT_MQTT */
#define AT_MQTT_CFG               "+IMTCFG"
#define AT_MQTT_OPEN              "+IMTOPEN"
#define AT_MQTT_CLOSE             "+IMTCLOSE"
#define AT_MQTT_CONN              "+IMTCONN"
#define AT_MQTT_DISC              "+IMTDISC"
#define AT_MQTT_SUB               "+IMTSUB"
#define AT_MQTT_UNS               "+IMTUNS"
#define AT_MQTT_PUBEX             "+IMTPUBEX"
#define AT_MQTT_RECV              "+IMTRECV"
#define AT_MQTT_PING              "+IMTPING"
#define AT_MQTT_DROP              "+IMTDROP"
#define AT_MQTT_CFG_MTPING        "imtping"

/* AT_HTTP */
#define AT_HTTP_CFG               "+IHTTPCFG"
#define AT_HTTP_URL               "+IHTTPURL"
#define AT_HTTP_GET               "+IHTTPGET"
#define AT_HTTP_GETEX             "+IHTTPGETEX"
#define AT_HTTP_POST              "+IHTTPPOST"
#define AT_HTTP_POSTFILE          "+IHTTPPOSTFILE"
#define AT_HTTP_PUT               "+IHTTPPUT"
#define AT_HTTP_PUTFILE           "+IHTTPPUTFILE"
#define AT_HTTP_READ              "+IHTTPREAD"
#define AT_HTTP_READFILE          "+IHTTPREADFILE"
#define AT_HTTP_CFGEX             "+IHTTPCFGEX"
#define AT_HTTP_SEND              "+IHTTPSEND"
#define AT_HTTP_STOP              "+IHTTPSTOP"
#define AT_HTTP_URC               "+IHTTPURC"

/* AT_SSL */
#define AT_SSL_CFG                "+ISSLCFG"
#define AT_SSL_OPEN               "+ISSLOPEN"
#define AT_SSL_SEND               "+ISSLSEND"
#define AT_SSL_RECV               "+ISSLRECV"
#define AT_SSL_CLOSE              "+ISSLCLOSE"
#define AT_SSL_STATE              "+ISSLSTATE"
#define AT_SSL_URC                "+ISSLURC"

/* AT_VFS */
#define AT_DISKINFO               "+IFLDS"
#define AT_LISTFILE               "+IFLST"
#define AT_DELETEFILE             "+IFDEL"
#define AT_UPLOADFILE             "+IFUPL"
#define AT_DWLOADFILE             "+IFDWL"
#define AT_OPENFILE               "+IFOPEN"
#define AT_READFILE               "+IFREAD"
#define AT_WRITEFILE              "+IFWRITE"
#define AT_SEEKFILE               "+IFSEEK"
#define AT_FILEPOS                "+IFPOSITION"
#define AT_TUCATFILE              "+IFTUCAT"
#define AT_CLOSEFILE              "+IFCLOSE"
#define AT_MOVEFILE               "+IFMOV"
#define AT_FLUSHFILE              "+IFFLUSH"
#define AT_USBWAIT                "+IUSBWAITTIME"
#define AT_CHANGEDIR              "+IFCWD"
#define AT_MAKEDIR                "+IFMKD"
#define AT_CURRDIR                "+IFPWD"

/* AT_FOTA */
#define AT_QFOTADL                "+IFOTADL"

#define AT_X_PING                 "+IPING"
#define AT_X_NTP                  "+INTP"
#define AT_X_IDNSGIP              "+IIDNSGIP"  //  用域名获取 IP 地址
#define AT_X_IDNSCFG              "+IIDNSCFG"  //  DNS配置
#define AT_X_ICSGP                "+IICSGP"    //  配置Context Profile
#define AT_X_IACT                 "+IIACT"     //  激活Context Profile
#define AT_X_IDEACT               "+IIDEACT"   //  去激活Context Profile

#define AT_X_GSN                  "+QGSN"
#define AT_X_CCID                 "+QCCID"
#define AT_X_NWINFO               "+QNWINFO"
#define AT_X_ENG                  "+QENG"
#define AT_X_CELL                 "+QCELL"
#define AT_X_CELLEX               "+QCELLEX"
#define AT_X_CELLINFO             "+QCELLINFO"
#define AT_X_URCCFG               "+QURCCFG"
#define AT_X_SCLK                 "+QSCLK"
#define AT_X_SCLKEX               "+QSCLKEX"
#define AT_X_DSIM                 "+QDSIM"
#define AT_X_LTS                  "+QLTS"
#define AT_X_POWD                 "+IPOWD"
#define AT_X_POWD_RSP             "POWER OFF"

/* AT_TTS */
#define AT_X_TTSETUP              "+ITTSETUP"
#define AT_X_TTS                  "+ITTS"
#define AT_X_DAI                  "+IDAI"
#define AT_X_AUDSW                "+IAUDSW"

#define AT_X_NETDEVCTL            "+QNETEVCTL"
#define AT_X_INDCFG               "+QINDCFG"
#define AT_X_IND                  "+QIND"

#define AT_X_URC_RDY              "RDY"
#define AT_X_URC_MODE             "^MODE"
#endif

#define AT_RSP_NOCARRIER             "NO CARRIER"  // TCP/TLS 链接异常断开退出透传提示符

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/



#ifdef __cplusplus
}
#endif
#endif


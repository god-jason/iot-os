
/**  @file
  ycom_fota.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef YOPEN_FOTA_H
#define YOPEN_FOTA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup yopen_fota fota功能
 * @{
*/

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

#ifndef bool
#define bool unsigned char
#endif

#define YOPEN_FOTA_ERRCODE_BASE   (YOPEN_COMPONENT_FOTA<<16)
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
/** @brief fota包结果*/
typedef enum
{
    YOPEN_FOTA_FINISHED = 0,   // FOTA 升级操作完成
    YOPEN_FOTA_NOT_EXIST,      // 没有fota更新包，这不是一个错误状态，需要下载fota包更新
    YOPEN_FOTA_UPGRADE_READY,  // fota更新包已经被设置准备状态，等待更新
    YOPEN_FOTA_STATUS_INVALID, // FOTA状态是无效的
    YOPEN_FOTA_PACK_CHECK_ERR, // FOTA包校验错误
} yopen_fota_result_e;
	
/** @brief fota升级结果*/
typedef enum
{
	YOPEN_FOTA_UPGRADE_SUCCESS = 				0,
	YOPEN_FOTA_UPGRADE_FAIL = 					1 | YOPEN_FOTA_ERRCODE_BASE,
	YOPEN_FOTA_UPGRADE_CHECK_FAIL,
	YOPEN_FOTA_UPGRADE_MD5_FAIL,
	YOPEN_FOTA_UPGRADE_MATCH_FAIL,
	YOPEN_FOTA_UPGRADE_NO_FILE_FAIL,
	YOPEN_FOTA_UPGRADE_OPENFILE_FAIL,
	YOPEN_FOTA_UPGRADE_FILESIZE_FAIL,
	YOPEN_FOTA_UPGRADE_LFS_MOUNT_FAIL,
	YOPEN_FOTA_UPGRADE_PARAM_FAIL,
	YOPEN_FOTA_UPGRADE_PROJECT_MATCH_FAIL,
	YOPEN_FOTA_UPGRADE_BASELINE_MATCH_FAIL,
	YOPEN_FOTA_UPGRADE_POINT_NULL_ERR,
	YOPEN_FOTA_UPGRADE_FLAG_SET_ERR,
	YOPEN_FOTA_UPGRADE_EERROR,
	YOPEN_FOTA_UPGRADE_EUNDEF,         /* undefined error */
	YOPEN_FOTA_UPGRADE_EARGS,          /* arguments invalid */
	YOPEN_FOTA_UPGRADE_EBATT,         /* battery low */
	YOPEN_FOTA_UPGRADE_EPAR,          /* parchive invalid */
	YOPEN_FOTA_UPGRADE_EPARSZ,        /* parchive size err */
	YOPEN_FOTA_UPGRADE_EFLQUERY,      /* flash query fail */
	YOPEN_FOTA_UPGRADE_EFLERASE,     /* flash erase fail */
	YOPEN_FOTA_UPGRADE_EFLWRITE,       /* flash write fail */
	YOPEN_FOTA_UPGRADE_EFLREAD,       /* flash read fail */
	YOPEN_FOTA_UPGRADE_EMINIT,      /* memory init fail */
	YOPEN_FOTA_UPGRADE_EMALLOC,     /* memory alloc fail */
	YOPEN_FOTA_UPGRADE_EMFREE,       /* memory free fail */
	YOPEN_FOTA_UPGRADE_EPVER,         /* parver uncompatible */
	YOPEN_FOTA_UPGRADE_EPMETH,       /* pmethod unsupported */
	YOPEN_FOTA_UPGRADE_EFWNIDENT,     /* fw non-identical */
	YOPEN_FOTA_UPGRADE_EPATCH,        /* fw patch fail */
	YOPEN_FOTA_UPGRADE_EUNFOUND,      /* item not found */
	YOPEN_FOTA_UPGRADE_EOVERFLOW,     /* resource ran out */
	YOPEN_FOTA_UPGRADE_EPERM,        /* scenario not permitted */
	YOPEN_FOTA_UPGRADE_ECHKSUM,    /* checksum calc fail*/
	YOPEN_FOTA_UPGRADE_EDCONV,      /* data convertion fail*/
	YOPEN_FOTA_UPGRADE_EVERIFY,  /* verify signature fail*/
	YOPEN_FOTA_UPGRADE_EOTHER
}yopen_errcode_fota_e;

/** @brief fota升级方式*/
typedef enum
{
	YOPEN_FOTA_INTERNAL_LFS   = 0, 
	YOPEN_FOTA_INTERNAL_FLASH = 1,
}yopen_fota_flash_e;

/*===========================================================================
 * function 
 ===========================================================================*/
 
/**
 * @brief  该函数用于校验模块文件系统中存储的升级包信息
 * @param  void			 
 * @return yopen_errcode_fota_e	 fota错误码
 */
extern yopen_errcode_fota_e yopen_fota_image_verify_without_setflag(char* PackFileName,yopen_fota_flash_e flash_mode);


/**
 * @brief  该函数用于自动区分差分和App全量升级包，并校验升级包包信息，并设置升级就绪标志位
 * @param  void
 * @return yopen_errcode_fota_e	 fota错误码
 */
extern yopen_errcode_fota_e yopen_fota_image_verify(char* PackFileName , yopen_fota_flash_e flash_mode);


/**
 * @brief  函数用于写入FOTA升级包数据到指定的fota nvm空间
 * @param  offset			 	 偏移量
 * @param  buf			         存储数据的内存地址。不可为空指针
 * @param  bufLen			     写入长度
 * @return yopen_errcode_fota_e	 fota错误码
 */

extern yopen_errcode_fota_e yopen_fotanvm_write(uint32_t offset, uint8_t *buf, uint32_t bufLen);


/**
 * @brief  函数用于读取指定地址的fota nvm空间数据
 * @param  offset			 	 偏移量
 * @param  buf			         存储数据的内存地址。不可为空指针
 * @param  bufLen			     读取长度
 * @return yopen_errcode_fota_e	 fota错误码
 */


extern yopen_errcode_fota_e yopen_fotanvm_read(uint32_t offset, uint8_t *buf, uint32_t bufLen);


/**
 * @brief  函数用于初始化fota nvm相关配置信息，并擦除fota nvm空间
 * @param  void			 
 * @return yopen_errcode_fota_e	 fota错误码
 */

extern yopen_errcode_fota_e yopen_fotanvm_init(void);


/**
 * @brief  函数用于获取fota nvm大小
 * @param
 * @return uint32_t	 			fota nvm大小
 */

extern uint32_t yopen_get_fotanvm_size(void);



/**
 * @brief  函数用于删除FOTA升级包，同时清除FOTA升级标志位
 * @param  PackFileName			 升级包名
 * @param  clear_flag			 是否清除fota升级标志位,TRUE OR FALSE
 * @return yopen_errcode_fota_e	 fota错误码
 */
extern yopen_errcode_fota_e yopen_fota_clear(char* PackFileName,yopen_fota_flash_e flash_mode,int clear_flag);


/**
 * @brief  该函数用于获取FOTA升级结果
 * @param  p_fota_result		 FOTA升级结果的指针
 * @return yopen_errcode_fota_e	 fota错误码
 */
extern yopen_errcode_fota_e yopen_fota_get_result(yopen_fota_result_e *p_fota_result);


/**
 * @brief  该函数用于重启模块。FOTA升级包校验通过后，需主动调用该函数重启模块。重启后，模块自动进入升级模式，开始自动升级固件
 * @param  reset_mode		 	 重启方式
 * @return yopen_errcode_fota_e	 fota错误码
 */
 extern yopen_errcode_fota_e yopen_fota_power_reset(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif

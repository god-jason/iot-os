/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        ota_user.h
 *
 * @brief       OTA 用户接口定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-10-21   ict team        创建
 ************************************************************************************
 */

#ifndef  _OTA_USER_H_
#define  _OTA_USER_H_
#include <stdint.h>

/**@name OTA 错误码定义
 * @{
 */
#define OTA_ERROR_OK            (00)    ///< 成功
#define OTA_ERROR_GENERAL       (01)    ///< 一般错误
#define OTA_ERROR_NOMEM         (02)    ///< 内存不足
#define OTA_ERROR_UPIMAGE       (03)    ///< 升级固件镜像失败
#define OTA_ERROR_UPFLSYS       (04)    ///< 升级文件失败
#define OTA_ERROR_VERSION       (05)    ///< 升级包版本不匹配

#define OTA_ERROR_BAD_PATCH     (10)    ///< 升级包错误
#define OTA_ERROR_SPLITSIZE     (11)    ///< 分块大小错误
#define OTA_ERROR_MOUNT_FSS     (12)    ///< 文件系统挂载失败
#define OTA_ERROR_IMAGE_ILLEGAL (13)    ///< 非法镜像类型
#define OTA_ERROR_IMAGE_NTEXIST (14)    ///< 打开待升级版本文件失败
#define OTA_ERROR_IMAGE_LOAD    (15)    ///< 待升级版本数据加载失败
#define OTA_ERROR_IMAGE_MD5     (16)    ///< 与待升级版本MD5不匹配
#define OTA_ERROR_NO_SPACE      (17)    ///< 存储空间不足
#define OTA_ERROR_SIGNATURE     (18)    ///< 升级包验签失败
#define OTA_ERROR_READY         (19)    ///< 升级任务准备好
#define OTA_ERROR_MAX           (20)    ///< 错误码最大值
/** @}*/

#define VERLEN                  (40)
/**
 * @brief OTA 升级包头.
 */
typedef struct {
    int8_t      head[4];            ///< 包头标识
    uint8_t     ver[2];             ///< 包版本号
    uint8_t     sver;               ///< 安启版本号
    uint8_t     count;              ///< 待升级的镜像数量
    uint32_t    size;               ///< 包大小, 包括本头
    uint32_t    obufLen;            ///< 升级时老版本数据的缓冲区大小
    uint32_t    nbufLen;            ///< 升级时新版本数据的缓冲区大小
    uint32_t    dbufLen;            ///< 升级时差分数据的缓冲区大小
    char        oldVersion[VERLEN]; ///< 老版本号
    char        newVersion[VERLEN]; ///< 新版本号
}OTA_PackageHeadzone;


/**
 * \brief 升级类型
 */
typedef enum {
    OTA_CONTENT_INVALID = -1,       ///< no content updating
    OTA_CONTENT_OTHERS,             ///< in updating, but not one of the followings
    OTA_CONTENT_FLASH,              ///< flash updating
    OTA_CONTENT_FILE_UPDATE,        ///< file update
    OTA_CONTENT_FILE_REMOVE,        ///< file remove
    OTA_CONTENT_FILE_INSERT,        ///< file new add
}OTA_ContentType;

/**
 * \brief Flash镜像更新
 */
typedef struct {
    uint32_t deviceName;            ///< flash device name
    uint32_t offset;                ///< flash offset
}OTA_FlashContent;

/**
 * \brief 更新内容说明
 */
typedef struct {
    OTA_ContentType type;           ///< content type: update file, remove, insert
    int             size;           ///< changed size
    int             nameSize;       ///< name size
    char            *name;          ///< file name
}OTA_UpdatePackContent;

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief       OTA 模块初始化.
 *
 * @return      错误码
 * @retval      OTA_ERROR_OK    成功;
 *              其他              失败;
 ************************************************************************************
 */
int32_t OTA_Init(void);

/**
 ************************************************************************************
 * @brief       获取升级包大小的最小值.
 *
 * @return      升级包大小的最小值.
 ************************************************************************************
 */
uint32_t OTA_GetPackageMinSize(void);

/**
 ***********************************************************************************************************************
 * @brief       检测升级包是否有效.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 * @param[out]  pHead   存放读取的头信息, 可为NULL.
 *
 * @return      错误码
 * @retval      OTA_ERROR_OK    成功;
 *              <0              失败;
 ***********************************************************************************************************************
 */
int32_t OTA_CheckPackage(const char *path, OTA_PackageHeadzone *pHead);

/**
 ***********************************************************************************************************************
 * @brief       检测升级版本是否匹配.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 *
 * @return      错误码
 * @retval      OTA_ERROR_OK    成功;
 *              <0              失败;
 ***********************************************************************************************************************
 */
int32_t OTA_CheckVersion(const char *path);

/**
 ***********************************************************************************************************************
 * @brief       开始进行OTA升级.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 *
 * @return      操作结果.
 * @retval      OTA_ERROR_OK    成功;
 *              <0              失败;
 ***********************************************************************************************************************
 */
int32_t OTA_StartUpgrade(const char *path);

/**
 ***********************************************************************************************************************
 * @brief       获取上次 OTA 升级的结果.
 *
 * @return      升级结果.
 * @retval      >=0     上次升级结果;
 *              <0      无有效结果;
 ***********************************************************************************************************************
 */
int32_t OTA_GetResult(void);

/**
 ***********************************************************************************************************************
 * @brief       清除上次升级结果.
 *
 * @return      无.
 ***********************************************************************************************************************
 */
void OTA_ClearResult(void);

/**
 ***********************************************************************************************************************
 * @brief       获取升级包内的版本号.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 * @param[out]  oldVer  存放老版本号, 可为NULL.
 * @param[out]  newVer  存放新版本号, 可为NULL.
 *
 * @retval      OTA_ERROR_OK    成功;
 *              <0              失败;
 ***********************************************************************************************************************
 */
int32_t OTA_GetUpdateVersion(const char *path, char oldVer[VERLEN], char newVer[VERLEN]);

/**
 ***********************************************************************************************************************
 * @brief       计算升级时，所需的临时空间大小(文件系统).
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 *
 * @retval      > 0     临时空间大小空间大小，单位: Byte;
 *              < 0     错误码;
 ***********************************************************************************************************************
 */
int32_t OTA_GetUpdateRequiredSpace(const char *path);

/**
 ***********************************************************************************************************************
 * @brief       获取升级包中升级内容列表.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 * @param[out]  list    内容列表, 不可为NULL.
 *
 * @retval      > 0     列表个数;
 *              < 0     错误码;
 ***********************************************************************************************************************
 */
int32_t OTA_GetUpdatePackContent(const char *path, OTA_UpdatePackContent **list);

/**
 ***********************************************************************************************************************
 * @brief       检测升级包签名是否有效.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 *
 * @retval      OTA_ERROR_OK    成功;
 *              <0              失败;
 ***********************************************************************************************************************
 */
int32_t OTA_CheckSignedPackage(const char *path);

/**
 ***********************************************************************************************************************
 * @brief       获取升级包安全启动版本.
 *
 * @param[in]   path    升级包绝对路径, 不可为NULL.
 *
 * @retval      > 0     安全启动版本;
 *              < 0     失败;
 ***********************************************************************************************************************
 */
int32_t OTA_GetSecureVersion(const char *path);

#ifdef __cplusplus
}
#endif

#endif


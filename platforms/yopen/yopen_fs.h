


#ifndef _YOPEN_FS_H_
#define _YOPEN_FS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "yopen_osi_def.h"
#include "yopen_type.h"

/**
 * @defgroup yopen_fs fs功能
 * @{
*/

/*===========================================================================
 *	Macro Definition
 ===========================================================================*/
//#define  YOPEN_LFS_CHECKSUM_ON   (1)
#define  YOPEN_FS_NAME_MAX 255


#define YOPEN_SEEK_SET	0	/* Seek from beginning of file.  */
#define YOPEN_SEEK_CUR	1	/* Seek from current position.  */
#define YOPEN_SEEK_END	2	/* Seek from end of file.  */

#define YOPEN_FS_TYPE_FILE          0x001
#define YOPEN_FS_TYPE_DIR            0x002

#define YOPEN_FILE_UFS_SUFFIX 		("_UFS_QL")
#define YOPEN_FILE_UFS_SUFFIX_LEN  (7)

#define YOPEN_FILE_PRXFIX_UFS 				"UFS:"
#define YOPEN_FILE_PRXFIX_UFS_LEN		   	(4)
#define YOPEN_FILE_PRXFIX_EFS 				"EFS:"
#define YOPEN_FILE_PRXFIX_EFS_LEN		   	(4)

#define YOPEN_INTERNAL_FILENAME_MAX_LEN	(64)

#ifndef QFILE
typedef INT32  QFILE;          /* file handler, returned by yopen_fopen  */
typedef uint32_t fs_off_t;
typedef uint32_t fs_size_t;
typedef uint32_t fs_block_t;
#endif

/*========================================================================
 *	Enumeration Definition
*========================================================================*/
/** @brief 文件及目录操作相关错误码*/
typedef enum
{
	YOPEN_FS_OK = 0,
	YOPEN_FS_INVALID_INPUT_VALUE	 	= -1,     				//输入值无效
	YOPEN_FS_LARGER_THAN_FILESIZE	 	= -2,					//大于文件大小
	YOPEN_FS_READ_ZERO                 = -3,					//读取0字节
	YOPEN_FS_DRIVE_FULL                = -4,					//驱动已满
	YOPEN_FS_MOV_ERROR                 = -5,					//移除文件错误
	YOPEN_FS_FILE_NOT_FOUND       	    = -6,					//未找到文件
	YOPEN_FS_INVALID_FILE_NAME         = -7,					//无效文件名
	YOPEN_FS_FILE_ALREADY_EXISTED		= -8,					//文件已存在
	YOPEN_FS_FAILED_TO_CREATE_FILE    	= -9,					//创建文件失败
	YOPEN_FS_FAILED_TO_WRITE_FILE     	= -10,					//数据写入文件失败
	YOPEN_FS_FAILED_TO_OPEN_FILE   	= -11,					//打开文件失败
	YOPEN_FS_FAILED_TO_READ_FILE      	= -12,					//读取文件失败
	YOPEN_FS_EXCEED_MAX_LENGTH        	= -13,					//超过最大长度
	YOPEN_FS_REACH_MAX_OPENFILE_NUM   	= -14,					//达到最大打开文件数
	YOPEN_FS_IS_READONLY              	= -15,					//文件只读
	YOPEN_FS_GET_SIZE_FAIL        	 	= -16,					//获取文件大小失败
	YOPEN_FS_INVALID_FILE_DESCRIPTOR  	=  -17,					//描述无效
	YOPEN_FS_LIST_FILE_FAIL           	= -18,					//列出文件列表失败
	YOPEN_FS_DELETE_FILE_FAIL         	= -19,					//删除文件失败
	YOPEN_FS_GET_DISK_INFO_FAIL       	= -20,					//获取磁盘信息失败
	YOPEN_FS_NO_SPACE             	 	= -21,					//磁盘空间不足
	YOPEN_FS_TIME_OUT                 	= -22,					//超时
	YOPEN_FS_FILE_NOT_FOUND_2         	=  -23,					//未找到文件
	YOPEN_FS_FILE_TOO_LARGE           	= -24,					//文件过大
	YOPEN_FS_FILE_ALREADY_EXIST       	= -25,					//文件已存在
	YOPEN_FS_INVALID_PARAMETER    	 	= -26,					//无效参数
	YOPEN_FS_ALREADY_OPERATION        	= -27,					//文件已打开
	YOPEN_FS_ERROR_GENERAL			 	= -28, 					//通用错误码
	YOPEN_FS_RENAME_ERROR 			 	= -29, 					//重命名文件失败
	YOPEN_FS_QUIT_DATE_MODE 			= -30,
	YOPEN_FS_CLOSE_FAIL			    = -31, 					//关闭文件失败
	YOPEN_FS_SEEK_FAIL				 	= -32, 					//移动文件指针失败
	YOPEN_FS_TELL_FAIL				 	= -33, 					//获取文件指针位置失败
	YOPEN_FS_FAILED_TO_GET_STAT		= -34,					//获取文件信息失败
	YOPEN_FS_NOT_EXIST				 	= -35,					//文件不存在
	YOPEN_FS_RENAME_FAIL				= -36, 					//重命名文件失败
	YOPEN_FS_TRUNCATE_FAIL            	= -37, 					//裁剪文件失败

	//Dir error code
	YOPEN_FS_DIR_MAKE_FAIL 			= -38,					//创建目录失败
	YOPEN_FS_DIR_OPEN_FAIL   			= -39,					//打开目录失败
	YOPEN_FS_DIR_CLOSE_FAIL			= -40,					//关闭目录失败	
	YOPEN_FS_DIR_READ_FAIL				= -41,					//读取目录失败
	YOPEN_FS_DIR_TELL_FAIL				= -42,
	YOPEN_FS_DIR_SEEK_FAIL				= -43,
	YOPEN_FS_DIR_REMOVE_FAIL			= -44,					//移除目录失败
	YOPEN_FS_DIR_DIR_ALREADY_EXIST		= -45,					//目录已存在

	YOPEN_FS_ERROR_MAX,
}yopen_file_errcode_e;

/** @brief 文件系统区域*/
typedef enum
{
    YOPEN_FS_UFS_REGIONAL,		//UFS区域
    YOPEN_FS_EFS_REGIONAL,		//EFS区域
} yopen_fs_regional_e;

/*===========================================================================
 *	Struct Definition
 ===========================================================================*/
/** @brief 文件信息结构体*/
struct stat {
    UINT8       type;						//文件类型
    UINT32      size;						//文件大小
    CHAR        name[YOPEN_FS_NAME_MAX+1];		//文件名
};

/** @brief 目录信息句柄*/
typedef struct
{
    INT32         			fs_index;		//目录索引
    INT32         			_reserved;		//保留参数
	yopen_fs_regional_e  		fs_type;		//目录所属文件系统区域
}QDIR;

/** @brief 目录信息结构体*/
typedef struct
{
    INT32          d_ino;           //Inode号
    UINT8          d_type;		  	//目录类型
    CHAR           d_name[256];     //目录名
}qdirent;

/** @brief 文件系统配置结构体*/
struct yopen_fs_device
{
    void *context;	// 传递给block驱动代码的上下文
	int (*read)(const struct yopen_fs_device *c, fs_block_t block, fs_off_t off, void *buffer, fs_size_t size);	// 从设备读数据
	int (*prog)(const struct yopen_fs_device *c, fs_block_t block,fs_off_t off, const void *buffer, fs_size_t size); // 向设备写入数据，block设备在写入前必须已经erase了
    int (*erase)(const struct yopen_fs_device *c, fs_block_t block);	// 擦除block
	int (*sync)(const struct yopen_fs_device *c);		// sync块设备的状态
	fs_size_t read_size;		//最小的读取单元大小
	fs_size_t prog_size;		//最小的写入数据单元大小
	fs_size_t block_size;		//最小的擦除单元大小，必须是read size和prog_size的倍数
	fs_size_t block_count;		//文件系统的block数量
	int32_t block_cycles;		//文件系统进行垃圾回收时的block的擦除次数，推荐取值100-1000
	fs_size_t cache_size;		//缓存区大小，必须是block的read_size和program_size的倍数，同时是block_size的因数
	fs_size_t lookahead_size;	//lookahead_buffer的尺寸，必须是8的倍数
	void *read_buffer;			//读取缓存区
	void *prog_buffer;			//写入缓存区
	void *lookahead_buffer;		//静态分配预测缓存
	fs_size_t name_max;			//文件名的最大长度
	fs_size_t file_max;			//文件的最大长度
	fs_size_t attr_max;			//用户属性的最大长度
	fs_size_t metadata_max;		//元数据的最大长度
};

/*========================================================================
 *	function Definition
 *========================================================================*/
/**
 * @brief					打开/创建一个文件
 * @param	file_name		文件名
 * @param	mode			打开模式
 * @return	QFILE:文件句柄
 */
extern QFILE yopen_fopen
(
	const char *file_name,
	const char *mode
);


/**
 * @brief					关闭一个文件
 * @param	fd				文件句柄
 * @return	yopen_file_errcode_e
 */
extern int yopen_fclose
(
	QFILE fd
);


/**
 * @brief					移除一个文件
 * @param	file_name		文件名
 * @return	yopen_file_errcode_e
 */
extern int yopen_remove
(
	const char *file_name
);

/*****************************************************************
* Function: yopen_fread
*
* Description:从指定文件中读取数据
*	
* Parameters:
*	buffer			[out]		存储读取数据的内存地址。不可为空指针
*	size			[in]		需读取的每个元素大小。单位：字节。
*	num				[in]		需读取的元素个数。最终读取数据大小为size*num
*	fd				[in]		文件句柄
*
* Return:
*	> 0 					成功读取的字节数。
*	other					错误码。
*
*****************************************************************/
/**
 * @brief					从指定文件中读取数据
 * @param	buffer			存储读取数据的内存地址。不可为空指针
 * @param	size			需读取的每个元素大小。单位：字节。
 * @param	num				需读取的元素个数。最终读取数据大小为size*num
 * @param	fd				文件句柄
 * @return	int				函数执行结果: >0:成功读取的字节数; other:错误码(yopen_file_errcode_e)
 */
extern int yopen_fread
(
	void * buffer, 
	size_t size, 
	size_t num, 
	QFILE fd
);


/**
 * @brief					向指定文件中写入数据
 * @param	buffer			存储写入数据的内存地址。不可为空指针
 * @param	size			需写入的每个元素大小。单位：字节
 * @param	num				需写入的元素个数。最终写入数据大小为size*num
 * @param	fd				文件句柄
 * @return	int				函数执行结果: >0:成功写入的字节数; other:错误码(yopen_file_errcode_e)
 */
extern int yopen_fwrite
(
	void * buffer, 
	size_t size, 
	size_t num, 
	QFILE fd
);


/**
 * @brief					设置文件指针的位置
 * @param	fd				文件句柄
 * @param	offset			偏移量
 * @param	origin			偏移起始位置
 * @return	yopen_file_errcode_e
 */
extern int yopen_fseek
(
	QFILE fd, 
	long offset, 
	int origin
);


/**
 * @brief					设置文件指针于文件开头
 * @param	fd				文件句柄
 * @return	yopen_file_errcode_e
 */
extern int yopen_frewind
(
	QFILE fd
);


/**
 * @brief					获取文件指针相对于文件开头的偏移值
 * @param	fd				文件句柄
 * @return	yopen_file_errcode_e
 */
extern int yopen_ftell
(
	QFILE fd
);


/**
 * @brief					通过文件句柄获取文件状态
 * @param	fd				文件句柄
 * @param	st				文件状态结构体
 * @return	yopen_file_errcode_e
 */
extern int yopen_fstat
(
	QFILE fd, 
	struct stat *st
);


/**
 * @brief					通过文件名获取文件状态
 * @param	file_name		文件名
 * @param	st				文件状态结构体
 * @return	yopen_file_errcode_e
 */
extern int yopen_stat
(
	const char *file_name, 
	struct stat *st
);


/**
 * @brief					从文件开头截取文件为指定长度
 * @param	fd				文件句柄
 * @param	length			指定长度
 * @return	yopen_file_errcode_e
 */
extern int yopen_ftruncate
(
	QFILE fd, 
	uint length
);


/**
 * @brief					获取文件大小
 * @param	fd				文件句柄
 * @return	文件大小
 */
extern int yopen_fsize
(
	QFILE fd
);


/**
 * @brief					判断文件是否存在
 * @param	file_path		文件名
 * @return	0:存在;	other:错误码
 */
extern int yopen_file_exist
(
	const char *file_path
);


/**
 * @brief					创建一个目录
 * @param	path			目录名(带路径)
 * @param	mode			模式
 * @return	yopen_file_errcode_e
 */
extern int yopen_mkdir
(
	const char *path, 
	uint mode
);


/**
 * @brief					打开指定目录
 * @param	path			目录名(带路径)
 * @return	other:目录句柄;	NULL:失败
 */
extern QDIR *yopen_opendir
(
	const char *path
);


/**
 * @brief					关闭指定目录
 * @param	pdir			目录句柄
 * @return	yopen_file_errcode_e
 */
extern int yopen_closedir
(
	QDIR *pdir
);


/**
 * @brief					从指定目录中获取文件信息
 * @param	pdir			目录句柄
 * @return	other:目录信息结构体;	NULL:失败
 */
extern qdirent *yopen_readdir
(
	QDIR *pdir
);


/**
 * @brief					删除指定目录
 * @param	path			目录名(带路径)
 * @return	yopen_file_errcode_e
 */
extern INT32 yopen_rmdir
(
	const CHAR *path
);

/**
 * @brief					删除指定目录(递归删除)
 * @param	path			目录名(带路径)
 * @return	yopen_file_errcode_e
 */
extern int yopen_rmdir_recursive
(
	const CHAR *path
);


/**
 * @brief					重命名文件。
 * @param	oldpath			原文件名
 * @param	newpath			新文件名
 * @return	yopen_file_errcode_e
 */
extern int yopen_rename
(
	const char *oldpath, 
	const char *newpath
);

/**
 * @brief						同步内存中已修改的文件数据到储存设备
 * @param	fd					文件句柄
 * @return	yopen_file_errcode_e
 */
extern int yopen_fsync(QFILE fd);


/**
 * @brief					查询UFS空闲空间大小
 * @param
 * @return	UFS空闲空间大小
 */
extern int yopen_internal_fs_free_size_get(void);

/**
 * @brief					查询UFS总空间大小
 * @param
 * @return	UFS总空间大小
 */
extern INT32 yopen_fs_total_size(void);

/**
 * @brief					扩展文件系统挂载
 * @param
 * @return	true:成功 false 失败
 */
extern bool yopen_fs_ext_mount(struct yopen_fs_device *config);

/**
 * @brief					扩展文件系统卸载
 * @param
 * @return	true:成功 false 失败
 */
extern bool yopen_fs_ext_unmount(struct yopen_fs_device *config);

/**
 * @brief					扩展文件系统格式化
 * @param
 * @return	true:成功 false 失败
 */
extern bool yopen_fs_ext_format(void);

/**
 * @brief	查询扩展文件系统可用空间大小
 * @param
 * @return	扩展文件系统可用空间大小
 */
extern INT32 yopen_fs_ext_free_size_get(void);

/**
 * @brief	查询扩展文件系统总空间大小
 * @param
 * @return	扩展文件系统总空间大小
 */
extern INT32 yopen_fs_ext_total_size(void);

#ifdef __cplusplus
} /*"C" */
#endif

#endif

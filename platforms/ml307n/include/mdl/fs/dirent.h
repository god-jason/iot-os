/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        dirent.h
 *
 * @brief       文件目录数据接口定义.
 *
 * @revision
 *
 * 日期            作者               修改内容
 * 2023-11-27   ict team        创建
 ************************************************************************************
 */
#ifndef __SYS_DIRENT_H__
#define __SYS_DIRENT_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Ref: http://www.opengroup.org/onlinepubs/009695399/basedefs/dirent.h.html */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14

struct dirent
{
    off_t         d_off;       /* file number    */
    size_t        d_size;      /* file size: vliad when d_type is DT_REG */
    unsigned char d_type;      /* The type of the file. */
    char          d_name[256]; /* The null-terminated file name. */
};

/*
 * This is the data type of directory stream objects.
 * The actual structure is opaque to users.
 */
typedef void DIR;

extern int          mkdir(const char *, mode_t);
extern DIR*         opendir(const char *path);
extern int          closedir(DIR *pdir);
extern struct dirent*readdir(DIR *pdir);
extern void         rewinddir(DIR *pdir);
extern void         seekdir(DIR *pdir, long ofst);
extern long         telldir(DIR *pdir);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_DIRENT_H__ */

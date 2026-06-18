#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_fs.h"

#define DEMO_FS_TRACE(fmt, ...) yopen_trace("[FS]"fmt, ##__VA_ARGS__)

#define DATA_LENGTH				11
#define FSEEK_LENGTH			2
#define TRUNCATE_LENGTH			6
#define DEMO_FILE				"demo.txt"
#define DEMO_FILE_RENAME		"rename.txt"

#define DIR_NAME				"/testDir"
#define DIR_NAME_111			"/testDir111"
#define DIR_FILE				"/testDir/dir.txt"

#define YOPEN_NV_DEMO_CFG	"yopen_nv_demo_cfg.nvm"
#define FS_FREE free
#define FS_MALLOC malloc

typedef struct _nv_demo
{
	int 	parameter_1;
	char 	parameter_2[10];
}nv_demo;

void yopen_fs_ls(void)
{
	QDIR*	dir_test = NULL;
	qdirent *dir_info = NULL;

	dir_test = yopen_opendir("/");
	
	DEMO_FS_TRACE("=== ls / begin ===");
	
	if (dir_test == NULL)
	{
		assert(0);
	}

	do
	{
		dir_info = yopen_readdir(dir_test);
		if (dir_info == NULL)
		{
			DEMO_FS_TRACE("=== close dir /. ret(%d) ===", yopen_closedir(dir_test));	
			break;
		}
		else
		{
			DEMO_FS_TRACE("=== read dir name %s, type %d ===", dir_info->d_name, dir_info->d_type);
			free(dir_info);
		}
		

	} while (1);

	DEMO_FS_TRACE("=== ls / end ===");
}

void yopen_fs_demo_task(void * arg)
{
	struct 	stat fstat_st;
	struct 	stat stat_st;
	int		ret = -1;
	QFILE	fd = -1;
	char* 	read_buff = FS_MALLOC(DATA_LENGTH - FSEEK_LENGTH + 1);
	char* 	frewind_buff = FS_MALLOC(DATA_LENGTH + 1);
	char* 	truncate_buff = FS_MALLOC((DATA_LENGTH - TRUNCATE_LENGTH) + 1);
	char* 	dir_buff = FS_MALLOC(DATA_LENGTH + 1);
	
	memset(&fstat_st, 0, sizeof(struct stat));
	memset(&stat_st, 0, sizeof(struct stat));
	
	yopen_rtos_task_sleep_ms(2000);
	DEMO_FS_TRACE("========== fs demo start ==========");
	
	DEMO_FS_TRACE("========== fs total size %d ==========", yopen_fs_total_size());
	DEMO_FS_TRACE("========== fs free size %d ==========", yopen_internal_fs_free_size_get());

	yopen_fs_ls();

	ret = yopen_file_exist(DEMO_FILE);
	DEMO_FS_TRACE("=== before create, check file exist. ret(%d) ===", ret);
	if(ret == 0)
	{
		DEMO_FS_TRACE("=== remove file. ret(%d) ===", yopen_remove(DEMO_FILE));
		DEMO_FS_TRACE("=== remove file, check file exist. ret(%d) ===", yopen_file_exist(DEMO_FILE));
	}
	
	DEMO_FS_TRACE("=== creat file ===\r\n");
	fd = yopen_fopen(DEMO_FILE, "wb+");
	if(fd < YOPEN_FS_OK)
	{
		DEMO_FS_TRACE("=== create file fail. fd(%d) ===", fd);
	}
	
	DEMO_FS_TRACE("=== write data to file ====");
	ret = yopen_fwrite("Hello World", 1, DATA_LENGTH, fd);
	if(ret != DATA_LENGTH)
	{
		DEMO_FS_TRACE("=== write data fail. ret(%d) ===", ret);
	}
	
	DEMO_FS_TRACE("=== close file ===");
	ret = yopen_fclose(fd);
	if(ret != YOPEN_FS_OK)
	{
		DEMO_FS_TRACE("=== close file fail. ret(%d) ===", ret);
	}
	
	yopen_rtos_task_sleep_ms(1000);
	
	DEMO_FS_TRACE("=== open file ===");
	fd = yopen_fopen(DEMO_FILE, "r");
	if(fd < YOPEN_FS_OK)
	{
		DEMO_FS_TRACE("=== open file fail. fd(%d) ===", fd);
	}
	
	DEMO_FS_TRACE("=== file size, ret(%d) ===", yopen_fsize(fd));
	
	yopen_fseek(fd, FSEEK_LENGTH, YOPEN_SEEK_SET);
	
	DEMO_FS_TRACE("=== read data from file ====");
	ret = yopen_fread(read_buff, 1, DATA_LENGTH - FSEEK_LENGTH, fd);
	if(ret != (DATA_LENGTH - FSEEK_LENGTH))
	{
		DEMO_FS_TRACE("=== read data fail. ret(%d) ===",ret);
	}
	else
	{
		DEMO_FS_TRACE("=== read data success(%s) ===", read_buff);
	}
	
	ret = yopen_ftell(fd);
	if(ret > 0)
	{
		DEMO_FS_TRACE("=== yopen_ftell, ret(%d) ===", ret);
		DEMO_FS_TRACE("=== yopen_frewind, ret(%d) ===", yopen_frewind(fd));
		memset(frewind_buff, 0, DATA_LENGTH+1);
		ret = yopen_fread(frewind_buff, 1, DATA_LENGTH, fd);
		if(ret != DATA_LENGTH)
		{
			DEMO_FS_TRACE("=== read data fail. ret(%d) ===",ret);
		}
		else
		{
			DEMO_FS_TRACE("=== after rewind file. data(%s) ===", frewind_buff);
		}
	}
	
	DEMO_FS_TRACE("=== close file. ret(%d) ===", yopen_fclose(fd));

	DEMO_FS_TRACE("=== after create, check file exist. ret(%d) ===", yopen_file_exist(DEMO_FILE));
	
	DEMO_FS_TRACE("=== rename file. ret(%d) ===", yopen_rename(DEMO_FILE, DEMO_FILE_RENAME));
	
	DEMO_FS_TRACE("=== after rename, check file exist. ret(%d) ===", yopen_file_exist(DEMO_FILE_RENAME));
	
	fd = yopen_fopen(DEMO_FILE_RENAME, "r+");

	if(fd < YOPEN_FS_OK)
	{
		DEMO_FS_TRACE("=== open file fail. fd(%d) ===", fd);
	}

	DEMO_FS_TRACE("=== truncate file. ret(%d) ===", yopen_ftruncate(fd, TRUNCATE_LENGTH));
	
	DEMO_FS_TRACE("=== yopen_fstat. ret(%d) name(%s) size(%d) ===", yopen_fstat(fd, &fstat_st), fstat_st.name, fstat_st.size);
	
	DEMO_FS_TRACE("=== yopen_stat. ret(%d) name(%s) size(%d) ===", yopen_stat(DEMO_FILE_RENAME, &stat_st), stat_st.name, stat_st.size);
	
	DEMO_FS_TRACE("=== close rename file. ret(%d) ===", yopen_fclose(fd));
	
	DEMO_FS_TRACE("=== remove rename file. ret(%d) ===", yopen_remove(DEMO_FILE_RENAME));

	DEMO_FS_TRACE("=== check rename file exist. ret(%d) ===", yopen_file_exist(DEMO_FILE));
	
	DEMO_FS_TRACE("====== Dir demo ======");

	yopen_fs_ls();
	
	DEMO_FS_TRACE("========== fs demo end ==========");
	
	FS_FREE(read_buff);
	FS_FREE(frewind_buff);
	FS_FREE(truncate_buff);
	FS_FREE(dir_buff);
	
	yopen_rtos_task_delete(NULL);  // kill itself
}



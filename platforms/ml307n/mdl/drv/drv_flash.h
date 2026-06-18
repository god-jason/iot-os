/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_flash.h
 *
 * @brief       Flash驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */
#ifndef __DRIVER_FLASH_H__
#define __DRIVER_FLASH_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/

#include <os.h>
#include "drv_common.h"

/**
 * @addtogroup Flash
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

 /****** Flash print macro ******/
#define FLASH_INFO "INFO:"
#define FLASH_WARN "WARN:"
#define FLASH_DBG  "DBG:"
#define FLASH_ERR  "ERR:"

#define DBG_LEVEL 4

#if DBG_LEVEL == 4
#define FLASH_Trace_(fmt, ...) \
    osPrintf("%s" fmt "%s\n", FLASH_INFO, __VA_ARGS__)
#define FLASH_Trace(...) FLASH_Trace_(__VA_ARGS__, "")
#else
#define FLASH_Trace(...)
#endif

#if DBG_LEVEL == 3
#define FLASH_Warn_(fmt, ...) \
    osPrintf("%s" fmt "%s\n", FLASH_WARN, __VA_ARGS__)
#define FLASH_Warn(...) FLASH_Warn_(__VA_ARGS__, "")
#else
#define FLASH_Warn(...)
#endif

#if DBG_LEVEL == 2
#define FLASH_Debug_(fmt, ...) \
    osPrintf("%s" fmt "%s\n", FLASH_DBG, __VA_ARGS__)
#define FLASH_Debug(...) FLASH_Debug_(__VA_ARGS__, "")
#else
#define FLASH_Debug(...)
#endif

#if DBG_LEVEL == 1
#define FLASH_Error_(fmt, ...) \
    osPrintf("%s" fmt "%s\n", FLASH_ERR, __VA_ARGS__)
#define FLASH_Error(...) FLASH_Error_(__VA_ARGS__, "")
#else
#define FLASH_Error(...)
#endif

 /****** Flash MFR ******/
#define CONFIG_SPI_FLASH_GD 1

 /****** Common function macro ******/
#define REG32(addr)                  (*(volatile uint32_t *)(uintptr_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uintptr_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uintptr_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))

#define MAX(a, b)        ((a) > (b) ? (a) : (b))
#define MIN(a, b)        ((a) < (b) ? (a) : (b))

#define SPI_CACHELINE_BUFF_LEN    0x1000

 /****** SPI mode flags ******/
#define SPI_CPHA    BIT(0)          /* clock phase */
#define SPI_CPOL    BIT(1)          /* clock polarity */
#define SPI_MODE_0  (0|0)           /* (original MicroWire) */
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)
#define SPI_TX_BYTE BIT(8)          /* transmit with 1 wire byte */
#define SPI_TX_DUAL BIT(9)          /* transmit with 2 wires */
#define SPI_TX_QUAD BIT(10)         /* transmit with 4 wires */
#define SPI_RX_SLOW BIT(11)         /* receive with 1 wire slow */
#define SPI_RX_DUAL BIT(12)         /* receive with 2 wires */
#define SPI_RX_QUAD BIT(13)         /* receive with 4 wires */
#define SPI_QPI     BIT(14)         /* receive & transmit with 4 wires */

 /****** Common commands ******/
#define CMD_WRITE_ENABLE             0x06
#define CMD_WRITE_DISABLE            0x04
#define CMD_READ_STATUS1_REG         0x05
#define CMD_READ_STATUS2_REG         0x35
#define CMD_WRITE_STATUS_REG         0x01
#define CMD_WRITE_WBSTATUS_REG       0x31
#define CMD_READ_EXTEND_REG          0xC8
#define CMD_WRITE_EXTEND_REG         0xC5
#define CMD_VOLATILE_SR_WR_EN        0x50
#define CMD_FAST_READ_4B_ADDR        0x0C
#define CMD_BURST_WITH_WRAP          0x77
#define CMD_CHIP_ERASE               0xC7
#define CMD_ENTER_4B_ADDR_MODE       0xB7
#define CMD_EXIT_4B_ADDR_MODE        0xE9
#define CMD_DEVICE_ID                0x90
#define CMD_JEDEC_ID                 0x9F
#define CMD_ENABLE_RESET             0x66
#define CMD_RESET                    0x99
#define CMD_SUSPEND                  0x75
#define CMD_RESUME                   0x7A
#define CMD_DEEP_POWERDOWN           0xB9
#define CMD_RELEASE_DEEP_POWERDOWN   0xAB
#define CMD_ENABLE_QPI               0x38
#define CMD_READ_WITH_4B_ADDR        0x13
#define CMD_FAST_READ_DUAL_4B_ADDR   0x3C
#define CMD_FAST_READ_QUAD_4B_ADDR   0x6C
#define CMD_FAST_READ_DUALIO_4B_ADDR 0xBC
#define CMD_FAST_READ_QUADIO_4B_ADDR 0xEC
#define CMD_PAGE_PROGRAM_4B          0x12
#define CMD_QUAD_PAGE_PROGRAM_4B     0x34
#define CMD_SECTOR_ERASE_4B          0x21
#define CMD_BLOCK32_ERASE_4B         0x5C
#define CMD_BLOCK64_ERASE_4B         0xDC

 /****** SPI (3/4)-Bytes Addr mode commands ******/
#define CMD_READ_DATA                0x03
#define CMD_FAST_READ_DATA           0x0B 
#define CMD_DUAL_FAST_READ_DATA      0x3B 
#define CMD_QUAD_FAST_READ_DATA      0x6B
#define CMD_DUALIO_FAST_READ_DATA    0xBB
#define CMD_QUADIO_FAST_READ_DATA    0xEB
#define CMD_PAGE_PROGRAM             0x02
#define CMD_QUAD_PAGE_PROGRAM        0x32
#define CMD_SECTOR_ERASE             0x20
#define CMD_BLOCK32_ERASE            0x52
#define CMD_BLOCK64_ERASE            0xD8
#define CMD_READ_UNIQUE_ID           0x4B
#define CMD_SR_ERASE                 0x44
#define CMD_SR_PROGRAM               0x42
#define CMD_SR_READ                  0x48

 /****** QPI commands ******/
#define CMD_SET_READ_PARA            0xC0
#define CMD_DISABLE_QPI              0xFF
#define CMD_RAED_DISCOVERABLE_PARA   0x5A
#define CMD_QPI_BURST_WITH_WRAP      0x0C

 /****** Bits of Status register ******/
#define L_STAT_WIP                   BIT(0)
#define L_STAT_WEL                   BIT(1)
#define L_BLOCK_BP0                  BIT(2)  /* Block protect 0 */
#define L_BLOCK_BP1                  BIT(3)  /* Block protect 1 */
#define L_BLOCK_BP2                  BIT(4)  /* Block protect 2 */
#define L_BLOCK_BP3                  BIT(5)  /* Block protect 4 */
#define L_BLOCK_BP4                  BIT(6)  /* Block protect 5 */
#define L_SR_SRP0                    BIT(7)  /* Status Register protect bit0 */

#define H_SR_SRP1                    BIT(0)  /* Status Register protect bit1 */
#define H_STAT_QE                    BIT(1)

#define H_STAT_ADS                   BIT(3)

#define H_SR_LB1                     BIT(3)
#define H_SR_LB2                     BIT(4)
#define H_SR_LB3                     BIT(5)

#define H_BLOCK_CMP                  BIT(6)

#define H_STAT_SUS1                  BIT(7)
#define H_STAT_SUS2                  BIT(2)

#define L_BLOCK_LOCK_ALL             (L_BLOCK_BP0 | L_BLOCK_BP1 | L_BLOCK_BP2 | L_BLOCK_BP3 | L_BLOCK_BP4)

 /****** Security registers addr bits ******/
#define SR_N1                        BIT(12)
#define SR_N2                        BIT(13)
#define SR_N3                        (SR_N1 | SR_N2)
#define SR_ADDR_MASK                 0x3FF    /* SR data addr mask*/
#define SR_REGISTER_SIZE             (0x400)

 /****** Flash timeout values ******/
#define SPI_FLASH_STAT_WR_TIMEOUT       (1 * 200)
#define SPI_FLASH_PROG_TIMEOUT          (1 * 500)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT  (1 * 1000)
#define SPI_FLASH_BLOCK_ERASE_TIMEOUT   (2 * 1000)
#define SPI_FLASH_CHIP_ERASE_TIMEOUT    (64 * 1000)

 /****** Flash ID check ******/
#define JEDEC_MFR(info)     ((info)->id[0])
#define JEDEC_ID(info)      (((info)->id[1]) << 8 | ((info)->id[2]))

#define SPI_FLASH_MAX_ID_LEN    6

 /****** CFI Manufacture ID's ******/
#define SPI_FLASH_CFI_MFR_SPANSION  0x01
#define SPI_FLASH_CFI_MFR_STMICRO   0x20
#define SPI_FLASH_CFI_MFR_MACRONIX  0xc2
#define SPI_FLASH_CFI_MFR_SST       0xbf
#define SPI_FLASH_CFI_MFR_WINBOND   0xef
#define SPI_FLASH_CFI_MFR_ATMEL     0x1f
#define SPI_FLASH_CFI_MFR_GD        0xc8
#define SPI_FLASH_CFI_MFR_FM        0xa1
#define SPI_FLASH_CFI_MFR_GT        0xc4
#define SPI_FLASH_CFI_MFR_SK        0x25
#define SPI_FLASH_CFI_MFR_PR        0x85
#define SPI_FLASH_CFI_MFR_TH        0xcd

 /****** Flash info flags ******/
#define SECT_4K         BIT(0)  /* CMD_ERASE_4K works uniformly */
#define WR_QPP          BIT(3)  /* use Quad Page Program */
#define RD_QUAD         BIT(4)  /* use Quad Read */
#define RD_DUAL         BIT(5)  /* use Dual Read */
#define RD_QUADIO       BIT(6)  /* use Quad IO Read */
#define RD_DUALIO       BIT(7)  /* use Dual IO Read */
#define RD_FULL         (RD_QUAD | RD_DUAL | RD_QUADIO | RD_DUALIO)

 /****** Flash operation config macro ******/
#define SPI_FLASH_OP_CMD(__opcode, __buswidth) \
    {                                          \
        .buswidth = __buswidth,                \
        .opcode = __opcode,                    \
    }

#define SPI_FLASH_OP_ADDR(__nbytes, __val, __buswidth) \
    {                                                  \
        .nbytes = __nbytes,                            \
        .val = __val,                                  \
        .buswidth = __buswidth,                        \
    }

#define SPI_FLASH_OP_NO_ADDR    { }

#define SPI_FLASH_OP_DUMMY(__nbytes, __buswidth) \
    {                                            \
        .nbytes = __nbytes,                      \
        .buswidth = __buswidth,                  \
    }

#define SPI_FLASH_OP_NO_DUMMY    { }

#define SPI_FLASH_OP_DATA_IN(__nbytes, __buf, __buswidth) \
    {                                                     \
        .dir = FLASH_DATA_IN,                             \
        .nbytes = __nbytes,                               \
        .buf.in = __buf,                                  \
        .buswidth = __buswidth,                           \
    }

#define SPI_FLASH_OP_DATA_OUT(__nbytes, __buf, __buswidth) \
    {                                                      \
        .dir = FLASH_DATA_OUT,                             \
        .nbytes = __nbytes,                                \
        .buf.out = __buf,                                  \
        .buswidth = __buswidth,                            \
    }

#define SPI_FLASH_OP_NO_DATA    { }

#define SPI_FLASH_OP(__cmd, __addr, __dummy, __data) \
    {                                                         \
        .cmd = __cmd,                                         \
        .addr = __addr,                                       \
        .dummy = __dummy,                                     \
        .data = __data,                                       \
    }

/****** Partition access macro ******/

#define FLASH_PART_NAME_MAX      (16)

#define PART_INFO_MEM_SIZE_MAX   (0x300)

#define PART_INFO_FLAGS_LOCKED   (1 << 0)

#define PART_Name(part)          ((part)->name)

#define PART_Offset(part)        ((part)->offset)

#define PART_LFlag(part)         ((part)->lflag)

#define PART_Size(part)          ((part)->size)

#define PART_BlockSize(part)     (g_pFlash->block_size)

#define PART_PageSize(part)      (g_pFlash->page_size)

#define FLASH_XIP_PART_ADDR      (0xC8009000)

#define FLASH_BASE_ADDR          (0x08000000)

#define FLASH_SECTOR_SIZE        (0x1000)

#define FLASH_PAGE_SIZE          (0x100)

/****** FLASH specific error codes *****/
#define FLASH_ERR_NOMEM        (DRV_ERR_SPECIFIC - 1) ///< Specified Mode not supported
#define FLASH_ERR_NODEV        (DRV_ERR_SPECIFIC - 2) ///< No such device
#define FLASH_ERR_PERM         (DRV_ERR_SPECIFIC - 3) ///< Access not permitted
#define FLASH_ERR_NOSPC        (DRV_ERR_SPECIFIC - 4) ///< No space left on device
#define FLASH_ERR_IO           (DRV_ERR_SPECIFIC - 5) ///< I/O acess error
#define FLASH_ERR_INVAL        (DRV_ERR_SPECIFIC - 6) ///< Invalid value

#define OS_EXTERN_FLASH_NAME  "nor_flash"

#define FLASH_OP_LOCK()        (osMutexAcquire(&g_fManager.fMutex, osWaitForever))
#define FLASH_OP_UNLOCK()      (osMutexRelease(&g_fManager.fMutex))

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
 * @brief  Flash operate direction
 */
typedef enum
{
    FLASH_DATA_IN,
    FLASH_DATA_OUT,
} FLASH_DataDir;

/**
 * @brief  Flash operate state
 */
typedef enum
{
    OP_IDLE,
    OP_WRITE,
    OP_ERASE,
    OP_WAIT,
    OP_BP,
    OP_RSR,
    OP_WSR,
    OP_MAX,
} FLASH_OpState;

/**
 * @brief  Flash info
 */
typedef struct
{
    const char *name;
    uint8_t     id[SPI_FLASH_MAX_ID_LEN];
    uint8_t     id_len;
    uint32_t    block_size;
    uint32_t    n_blocks;
    uint16_t    page_size;
    uint16_t    flags;
} FLASH_Info;

/**
 * @brief  Flash driver operate
 */
typedef struct
{
    struct {
        uint8_t buswidth;
        uint8_t opcode;
    } cmd;

    struct {
        uint8_t nbytes;
        uint8_t buswidth;
        uint32_t val;
    } addr;

    struct {
        uint8_t nbytes;
        uint8_t buswidth;
    } dummy;

    struct {
        uint8_t buswidth;
        FLASH_DataDir dir;
        unsigned int nbytes;
        /* buf.{in,out} must be DMA-able. */
        union {
            void *in;
            const void *out;
        } buf;
    } data;
} FLASH_DrvOp;

/**
 * @brief  Flash SR operate
 */
typedef struct
{
    uint32_t offset;
    uint32_t len;
    uint8_t *buf;
} FLASH_SrOp;

/**
 * @brief  Define FLASH_Nor struct type
 */
typedef struct _FLASH_NOR FLASH_Nor;

/**
 * @brief  Nor flash 
 */
struct _FLASH_NOR
{
    void *pController;

    uint32_t flash_size;
    uint32_t block_size;
	uint32_t erase_size;
	uint32_t page_size;

    uint8_t erase_cmd;
    uint8_t read_cmd;
    uint8_t write_cmd;
    uint8_t dummy_byte;
    uint8_t erase_block_cmd;

    uint8_t read_buswidth;
    uint8_t write_buswidth;

    uint8_t addr_bytes;

	FLASH_DrvOp *read_op;
	FLASH_DrvOp *op;

    int32_t (*read_reg)(FLASH_Nor *nor, uint8_t code, uint8_t *val, int32_t len);
    int32_t (*write_reg)(FLASH_Nor *nor, uint8_t code, uint8_t *buf, int32_t len);
    uint32_t bp_status;
    FLASH_SrOp *srOp;

    FLASH_OpState state;
};

/**
 * @brief  Define FLASH_Handle struct type
 */
typedef struct _FLASH_HANDLE FLASH_Handle;

/**
 * @brief  Flash operate APIs
 */
typedef struct
{
    int32_t (*read)(FLASH_Handle *flash, uint32_t offset, uint8_t *buff, size_t size);
    int32_t (*write)(FLASH_Handle *flash, uint32_t offset, const uint8_t *buff, size_t size);
    int32_t (*erase)(FLASH_Handle *flash, uint32_t offset, size_t size);
} FLASH_Ops;

/**
 * @brief  Flash handle
 */
struct _FLASH_HANDLE
{
    char name[FLASH_PART_NAME_MAX];

    uint32_t page_size;  /* flash memory page (without spare area) size measured in bytes */
    uint32_t block_size; /* flash memory block size measured in number of pages */
    uint32_t capacity;   /* flash capacity measured in bytes */
    FLASH_Ops ops;
    void *priv;
#ifdef CONFIG_USE_FLASH2
    void *priv2;
#endif
};

/**
 * @brief  Flash mutex
 */
typedef struct {
    struct osMutex  fMutex;    ///< 读写 flash 操作互斥体
} FLASH_Manager;

/**
 * @brief  Part handle
 */
typedef struct
{
    char name[FLASH_PART_NAME_MAX];

    uint32_t offset;
    uint32_t size;
    uint32_t lflag;
} PART_Handle;

/************************************************************************************
 *                                 变量定义
 ************************************************************************************/
extern FLASH_Manager g_fManager;
extern FLASH_Handle *g_pFlash;
extern const FLASH_Info g_FlashIds[];

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************************************************
 * @brief       查找分区.
 * @param[in]   name             要查找的Flash名称，如：“xboot”.
 * @return      PART_Handle *.
 * @retval      Not NULL         找到的分区句柄.
 * @retval      NULL             失败.
 ************************************************************************************************************************
 */
PART_Handle *PART_Find(const char *name);

/**
 ************************************************************************************************************************
 * @brief       从分区读数据.
 * @param[in]   part             分区句柄.
 * @param[in]   offs             待读数据所在分区内偏移地址.
 * @param[in]   buf              读取数据存放的buffer.
 * @param[in]   size             待读数据长度.
 * @return      int32_t.
 * @retval      >=0              成功读取字节个数.
 * @retval      < 0              读取错误返回类型.
 ************************************************************************************************************************
 */
int32_t PART_Read(PART_Handle *part, uint32_t offs, uint8_t *buf, size_t size);

/**
 ************************************************************************************************************************
 * @brief       写数据到分区.
 * @param[in]   part             分区句柄.
 * @param[in]   offs             待写数据到分区内的偏移地址.
 * @param[in]   buf              代写数据存放的buffer.
 * @param[in]   size             待写数据长度.
 * @return      int32_t.
 * @retval      >=0              成功写入字节个数.
 * @retval      < 0              写入错误返回类型.
 ************************************************************************************************************************
 */
int32_t PART_Write(PART_Handle *part, uint32_t offs, const uint8_t *buf, size_t size);

/**
 ************************************************************************************************************************
 * @brief       擦除部分分区.
 * @param[in]   part             分区句柄.
 * @param[in]   offs             待擦分区内的偏移地址.
 * @param[in]   size             待擦长度.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 * @retval      < 0              失败.
 ************************************************************************************************************************
 */
int32_t PART_Erase(PART_Handle *part, uint32_t offs, size_t size);

/**
 ************************************************************************************************************************
 * @brief       擦除整个分区.
 * @param[in]   part             分区句柄.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 * @retval      < 0              失败.
 ************************************************************************************************************************
 */
int32_t PART_EraseAll(PART_Handle *part);

/**
 ************************************************************************************************************************
 * @brief       锁定分区.
 * @param[in]   part             分区句柄.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 ************************************************************************************************************************
 */
int32_t PART_Lock(PART_Handle *part);

/**
 ************************************************************************************************************************
 * @brief       解锁定分区.
 * @param[in]   part             分区句柄.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 ************************************************************************************************************************
 */
int32_t PART_UnLock(PART_Handle *part);

/**
 ************************************************************************************************************************
 * @brief       获取分区上锁状态.
 * @param[in]   part          分区句柄.
 * @return      bool.
 * @retval      true          锁定状态.
 * @retval      false         解锁状态.
 ************************************************************************************************************************
 */
bool PART_GetLock(PART_Handle *part);

/**
 ************************************************************************************************************************
 * @brief       Flash受保护区域锁定、解锁.
 * @param[in]   protect            设置保护区域上锁或解锁.
 * @return      int32_t.
 * @retval      DRV_OK             成功.
 ************************************************************************************************************************
 */
int32_t FLASH_ProtectedArea(bool protect);

/**
 ************************************************************************************************************************
 * @brief       NorFlash设置RESUME.
 * @param[in]   nor              NorFlash.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 * @retval      < 0              失败.
 ************************************************************************************************************************
 */
int32_t Spi_NorPeResume(FLASH_Nor *nor);

/**
 ************************************************************************************************************************
 * @brief       NorFlash 安全寄存器读取.
 * @param[in]   nor              NorFlash.
 * @param[in]   offset           待读数据的安全寄存器及其内的偏移地址.
 * @param[in]   len              待读安全寄存器数据长度.
 * @param[in]   buf              待读寄存器值buff.
 * @return      int32_t.
 * @retval      DRV_OK           成功.
 * @retval      < 0              失败.
 ************************************************************************************************************************
 */
int32_t SR_ReadPort(FLASH_Nor *nor, uint32_t offset, uint32_t len, uint8_t *buf);

#endif

/** @} */
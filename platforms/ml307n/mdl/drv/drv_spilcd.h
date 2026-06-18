#ifndef __DRV_SPILCD__
#define __DRV_SPILCD__

/**
 * @addtogroup SpiLcd
 */

/**@{*/

#define LCD_EVENT_TRANS_DONE 0x1 << 0
// #define LCD_EVENT_FIFO_INT   0x1 << 1
// #define LCD_EVENT_TRANS_END  0x1 << 2

#define SPI_LCD_IRQ            (OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_47))

#define SPI_LCD_ADDR            0xF280B000


enum
{
    SPILCD_THRESHODE_4 = 0,
    SPILCD_THRESHODE_8,
    SPILCD_THRESHODE_12,
    SPILCD_THRESHODE_16
};

enum
{
    SPILCD_USE_UNIQUE_DIRECTION = 0,
    SPILCD_USE_BI_DIRECTION,
};

enum
{
    SPILCD_USE_DEDICATED_DCX_BIT = 0,
    SPILCD_USE_DEDICATED_DCX_LINE,
};

enum
{
    SPILCD_DUMMY_NUM_0 = 0,
    SPILCD_DUMMY_NUM_1,
    SPILCD_DUMMY_NUM_2,
    SPILCD_DUMMY_NUM_3,
};

enum
{
    SPILCD_SAMPLE_NEXT_RISE_CLK_EDGE = 0,
    SPILCD_SAMPLE_NEXT_FALL_CLK_EDGE,
};

enum
{
    SPILCD_DISABLE = 0,
    SPILCD_ENABLE,
};

enum
{
    SPILCD_SEND_FRAME_DATA = 0,
    SPILCD_SEND_REG_VALUE,
};

enum
{
    SPILCD_DIR_READ = 0,
    SPILCD_DIR_WRITE,
};

typedef volatile struct dbi_reg_s
{
	uint32_t VERSION;
	uint32_t PROTOCOL;
	uint32_t OPERATION;
	uint32_t DCS_CMD;

	uint32_t TRANS_LEN;
	uint32_t START;
	uint32_t TX_FIFO;
	uint32_t TRANS_STATUS;

	uint32_t WR_PARAM[8];
	uint32_t RD_VALUE[8];
}dbi_reg_t;

#define DBI_VERSION_POS					0
#define DBI_VERSION_MSK					0xffffffff
#define DBI_VERSION_H_POS				24
#define DBI_VERSION_H_MSK				0xff
#define DBI_VERSION_L_POS				16
#define DBI_VERSION_L_MSK				0xff

#define DBI_PROTOCOL_POS				0
#define DBI_PROTOCOL_MSK				0xffffffff
#define DBI_PROTOCOL_DMY_CYCLE_POS		4
#define DBI_PROTOCOL_DMY_CYCLE_MSK		0xf
#define DBI_PROTOCOL_DBI_SAMP_SEL_POS	2
#define DBI_PROTOCOL_DBI_SAMP_SEL_MSK	0x1
#define DBI_PROTOCOL_DBI_DCX_POS		1
#define DBI_PROTOCOL_DBI_DCX_MSK		0x1
#define DBI_PROTOCOL_SPI_BIDIR_POS		0
#define DBI_PROTOCOL_SPI_BIDIR_MSK		0x1

#define DBI_OPERATION_POS				0
#define DBI_OPERATION_MSK				0xffffffff
#define DBI_OPERATION_TRANS_INT_EN_POS	6
#define DBI_OPERATION_TRANS_INT_EN_MSK	0x1
#define DBI_OPERATION_FIFO_INT_EN_POS	5
#define DBI_OPERATION_FIFO_INT_EN_MSK	0x1
#define DBI_OPERATION_DMA_EN_POS		4
#define DBI_OPERATION_DMA_EN_MSK		0x1
#define DBI_OPERATION_FIFO_THRES_POS	2
#define DBI_OPERATION_FIFO_THRES_MSK	0x3
#define DBI_OPERATION_WR_MEM_POS		1
#define DBI_OPERATION_WR_MEM_MSK		0x1
#define DBI_OPERATION_LCD_RW_POS		0
#define DBI_OPERATION_LCD_RW_MSK		0x1

#define DBI_DCS_CMD_POS					0
#define DBI_DCS_CMD_MSK					0xff

#define DBI_TRANS_LEN_POS				0
#define DBI_TRANS_LEN_MSK				0x3ffff

#define DBI_START_POS					0
#define DBI_START_MSK					0x1

#define DBI_TX_FIFO_POS					0
#define DBI_TX_FIFO_MSK					0xffffffff

#define DBI_TRANS_STATUS_POS			0
#define DBI_TRANS_STATUS_MSK			0xffffffff
#define DBI_TRANS_STATUS_TRANS_INT_POS	10
#define DBI_TRANS_STATUS_TRANS_INT_MSK	0x1
#define DBI_TRANS_STATUS_FIFO_INT_POS	9
#define DBI_TRANS_STATUS_FIFO_INT_MSK	0x1
#define DBI_TRANS_STATUS_TXF_NOT_FULL_POS	8
#define DBI_TRANS_STATUS_TXF_NOT_FULL_MSK	0x1
#define DBI_TRANS_STATUS_TXF_SPACE_CNT_POS	0
#define DBI_TRANS_STATUS_TXF_SPACE_CNT_MSK	0x3f

#define DBI_WR_PARAM_POS				0
#define DBI_WR_PARAM_MSK				0xff

#define DBI_RD_VALUE_POS				0
#define DBI_RD_VALUE_MSK				0xff

#define  _POS(field)	(field##_POS)
#define  _MSK(field)	(field##_MSK)

#define SET_FIELD(field,val)			((val&_MSK(field))<<_POS(field))

#define GET_REG_FIELD(reg,field)	(((reg)>>_POS(field))&_MSK(field))
#define GET_REG_VALUE(reg)		(reg)


#define SET_REG_FIELD(reg,field,val)	\
		do { \
		(reg)=(((reg)&(~( _MSK(field)<<_POS(field))))|((val&_MSK(field))<<_POS(field)));\
	} while (0)
	
#define SET_REG_VALUE(reg,val)	\
		do { \
		(reg)=(val);			\
	} while (0)

typedef enum
{
	LCD_DBI_DCX_CLOSE = 0,
	LCD_DBI_DCX_OPEN = 1,
}LCD_DBI_DCX_SELECT;

typedef enum
{
	SPI_BIDIR_TWO = 0,
	SPI_BIDIR_ONE = 1,
}LCD_SPI_BIDIR_SELECT;

typedef enum
{
	DBI_CONFIG_DMY_CYC = 0,				// 配置dummy cycle的值
	DBI_CONFIG_DBI_DCX,					// 是否使用专用的D/CX接口
	DBI_CONFIG_SPI_BIDIR,				// 使用双向SDA还是两条DIN/DOUT
	DBI_CONFIG_DATA_MODE,				// 数据搬运方式：DMA或者FIFO
	DBI_CONFIG_FIFO_THRES,				// 产生FIFO/DMA中断的阈值
	DBI_CONFIG_TRANS_INT,				// 中断标志
	DBI_CONFIG_WRITE_CMD,				// 设置COMMAND
	DBI_CONFIG_WRITE_REG_DATA,			// 配置模块工作方式为写入寄存器值
	DBI_CONFIG_WRITE_FRAME_DATA,		// 配置模块工作方式为写入图像数据
	DBI_CONFIG_START_TRANSMISSION,		// 启动模块

	// 只读操作
	DBI_CONFIG_FIFO_INT,
	DBI_CONFIG_TXF_NOT_FULL,

}LCD_CONF_OPERATIONS;

typedef enum
{
	LCD_DATA_MODE_DMA = 0,
	LCD_DATA_MODE_FIFO,
	LCD_DATA_MODE_POLL,
}LCD_DATA_MODE_SEL;

typedef enum
{
	LCD_FIFO_THRES_4 = 0,
	LCD_FIFO_THRES_8,
	LCD_FIFO_THRES_12,
	LCD_FIFO_THRES_16,
}LCD_FIFO_THRES;




typedef struct mipi_dbi_typec_s
{
	// uint8_t mode;                       // dma fifo 或者 poll
    uint8_t dcx;                        // 是否使用dcx
    uint8_t bidir;                      // 是否使用双向sda
	uint8_t dummy;
	uint8_t thres;
	uint8_t sample_mode;
}mipi_dbi_typec_t;

void ict_spilcd_cfg_dummy(uint8_t dummy);
void ict_spilcd_cfg_thres(uint8_t thres);
void ict_spilcd_cfg_dcx(uint8_t en);
void ict_spilcd_use_bidir(uint8_t en);

void ict_spilcd_init(void);
void ict_spilcd_cfg_update(mipi_dbi_typec_t *config);

bool ict_spilcd_read(uint8_t cmd, uint32_t len, uint8_t param[]);
int32_t ict_spilcd_tx_poll_pre(uint8_t cmd, uint32_t len);
int32_t ict_spilcd_tx_poll(uint32_t data);

bool ict_spilcd_tx_intr_mode(uint8_t cmd, uint8_t *data, uint32_t len);
bool ict_spilcd_tx_greater_8_intr(uint8_t cmd, uint8_t *data, uint32_t len);
bool ict_spilcd_tx_smaller_8_intr(uint8_t cmd, uint8_t *data, uint32_t len);

void ict_spilcd_probe(void);

#endif/*_ICT_DBI_H*/
/** @} */

/**
 * @file        cm_demo_i2c.c
 * @brief       OpenCPU i2c测试例程
 * @copyright   Copyright ? 2021 China Mobile IOT. All rights reserved.
 */


 /****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_I2C_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_iomux.h"
#include "cm_i2c.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_DEMO_I2C_LOG     cm_demo_printf
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
#define EPROM_DEV_ADDR       0x50
#define BMP180_SLAVE_ADDR (0x77) /* 从设备地址 */
#define BMP180_ADDR (0x77) /* 气压传感器器件地址*/
#define BMP180_ADDR_WR (0x77) /* 气压传感器器件地址+写指令 */
#define BMP180_ADDR_RD (0x78) /* 气压传感器器件地址+读指令 */
#define BMP180_OSS (0) /* BMP180 OSS 等级 */
#define BMP180_ZERO_ALTITUDE (101325) /* 0海拔高度的气压值 单位Pa */
#define M177B_DEV_ADDR (0x45)/* 设备地址 */
#define cm_demo_printf osPrintf

/* 延时时间 */
#define BMP180_CONV_DELAY_MS     (20)
#define M177B_CONV_DELAY_MS      (4)
#define M177B_RESET_DELAY_MS     (200)
#define EEPROM_WRITE_DELAY_MS    (10)

/* CRC计算 */
#define CRC_INIT_VALUE           (0xFF)
#define CRC_POLYNOMIAL           (0x31)

/* I2C相关 */
#define I2C_BUF_SIZE             (16)
#define EEPROM_ADDR_SIZE         (2)
#define EEPROM_DATA_SIZE         (1)

/****************************************************************************
 * Private Types
 ****************************************************************************/
/* 定义结构体用来保存校准数值参数 */
typedef struct {
    short ac1;
    short ac2;
    short ac3;
    unsigned short ac4;
    unsigned short ac5;
    unsigned short ac6;
    short b1;
    short b2;
    short mb;
    short mc;
    short md;
} struct_bmp180_adjust_typedef;

/* bmp180结构体 */
typedef struct {
    struct_bmp180_adjust_typedef adjust;    /* 传感器寄存器校准值 */
    float temperature;  /* 温度值 */
    float pressure;     /* 气压值 */
    float altitude;     /* 海拔高度 */
    float temp_offset;  /* 温度补偿 */
    float pres_offset;  /* 气压补偿 */
    float alti_offset;  /* 海拔补偿 */
} struct_bmp180_typedef;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
static struct_bmp180_typedef g_bmp180 = {0}; /* 定义结构体用来保存bmp180传感器参数 */
static int g_i2c_dev = 0;
/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void cm_i2c_iomux_set(cm_i2c_dev_e dev)
{
    if(dev == CM_I2C_DEV_0)
    {
        cm_iomux_set_pin_func(CM_IOMUX_PIN_62, CM_IOMUX_FUNC_FUNCTION5); 
        cm_iomux_set_pin_func(CM_IOMUX_PIN_63, CM_IOMUX_FUNC_FUNCTION5); 
    }
    else if(dev == CM_I2C_DEV_1)
    {
        cm_iomux_set_pin_func(CM_IOMUX_PIN_54, CM_IOMUX_FUNC_FUNCTION4); 
        cm_iomux_set_pin_func(CM_IOMUX_PIN_55, CM_IOMUX_FUNC_FUNCTION4); 
    }
    else if(dev == CM_I2C_DEV_2)
    {
        cm_iomux_set_pin_func(CM_IOMUX_PIN_58, CM_IOMUX_FUNC_FUNCTION3); 
        cm_iomux_set_pin_func(CM_IOMUX_PIN_21, CM_IOMUX_FUNC_FUNCTION3); 
    }
    else
    {
		cm_demo_printf("%s():[%d], i2c dev=%d error\r\n", __func__, __LINE__, dev);
    }
    
}

/**
 * @brief eeprom写1字节数据
 *
 * @param [in] addr:地址，范围0-0x7fff
 * @param [in] data:待写入数据
 *
 * @return  
 *   = 0  - 成功.
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 */
static int32_t is24c256_write_byte(uint16_t addr, int8_t data)
{
    int8_t  tmp[3] = {0};
    int32_t ret = -1;

    tmp[0] = (addr >> 8) & 0xff;
    tmp[1] = addr & 0xff;
    tmp[2] = data;

    ret = cm_i2c_write(g_i2c_dev , EPROM_DEV_ADDR, tmp, 3);
    if(ret < 0)
    {
        CM_DEMO_I2C_LOG("i2c write e2prom err:0x%x\r\n", ret);
        return -1;
    }
        
    return 0;
}


/**
 * @brief eeprom读1字节数据
 *
 * @param [in] addr:地址，范围0-0x7fff
 * @param [out] data:待读取数据指针
 *
 * @return  
 *   = 0  - 成功.
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 */
static int32_t is24c256_read_byte(uint16_t addr, int8_t* data)
{
    int8_t tmp[2] = {0};
    int32_t ret;

    if(data == NULL)
    {
        CM_DEMO_I2C_LOG("is24c256_read_byte data ptr err\r\n");
        return -1;
    }

    tmp[0] = (addr >> 8) & 0xff;
    tmp[1] = addr & 0xff;

    ret = cm_i2c_write(g_i2c_dev, EPROM_DEV_ADDR, tmp, 2);
    if(ret < 0)
    {
        CM_DEMO_I2C_LOG("i2c read addr err(w):%08x\r\n", ret);
        return ret;
    }

    ret = cm_i2c_read(g_i2c_dev, EPROM_DEV_ADDR, data, 1);
    if(ret < 0)
    {
        CM_DEMO_I2C_LOG("i2c read addr err(r):%08x\r\n", ret);
        return ret;
    }

    return 0;    
}

 static int16_t bmp180_multiRead(uint8_t reg_addr)
 {
     int8_t tmp[1] = {0};
     int8_t data[2] = {0,0};
     int16_t return_data = 0;
     int32_t ret = -1;

     tmp[0] = reg_addr;
     memset(data,0,2);

     ret = cm_i2c_write_then_read(g_i2c_dev, BMP180_SLAVE_ADDR, tmp, 1,data,2);
     if(ret < 0)
     {
         CM_DEMO_I2C_LOG("L:%d,i2c read addr err(w):%08x\r\n", __LINE__, ret);
         return ret;
     }
    
     return_data = (int16_t)((0x00FF)&data[0]);
     return_data = (int16_t)((return_data << 8)&(0xFF00));
     return_data = (int16_t)((return_data | ((0x00FF)&data[1]))&(0xFFFF));
     return return_data;
 }
  
 static int16_t bmp180_read_temperature_reg(void)
 {
     int8_t tmp[2] = {0};
     int32_t ret = -1;

     tmp[0] = 0xF4;
     tmp[1] = 0x2E;

     ret = cm_i2c_write(g_i2c_dev, BMP180_SLAVE_ADDR, tmp, 2);
     if(ret < 0)
     {
         CM_DEMO_I2C_LOG("L:%d,i2c read addr err(w):%08x\r\n", __LINE__, ret);
         return ret;
     }
     
     osDelay(BMP180_CONV_DELAY_MS);
     
     return bmp180_multiRead(0xF6);
 }
  
 static uint16_t bmp180_read_pressure_reg(void)
 {
     int8_t tmp[2] = {0,0};
     int32_t ret = -1;

     tmp[0] = 0xF4;
     tmp[1] = 0x34;

     ret = cm_i2c_write(g_i2c_dev, BMP180_SLAVE_ADDR, tmp, 2);
     if(ret < 0)
     {
         CM_DEMO_I2C_LOG("i2c read addr err(w):%08x\r\n", ret);
         return ret;
     }
     
     osDelay(BMP180_CONV_DELAY_MS);
     
     return bmp180_multiRead(0xF6);
 }

 static void bmp180_struct_init(void)
 {
     g_bmp180.adjust.ac1 = bmp180_multiRead(0xAA);
     g_bmp180.adjust.ac2 = bmp180_multiRead(0xAC);
     g_bmp180.adjust.ac3 = bmp180_multiRead(0xAE);
     g_bmp180.adjust.ac4 = bmp180_multiRead(0xB0);
     g_bmp180.adjust.ac5 = bmp180_multiRead(0xB2);
     g_bmp180.adjust.ac6 = bmp180_multiRead(0xB4);
     g_bmp180.adjust.b1 = bmp180_multiRead(0xB6);
     g_bmp180.adjust.b2 = bmp180_multiRead(0xB8);
     g_bmp180.adjust.mb = bmp180_multiRead(0xBA);
     g_bmp180.adjust.mc = bmp180_multiRead(0xBC);
     g_bmp180.adjust.md = bmp180_multiRead(0xBE);
     
     CM_DEMO_I2C_LOG("ac1=0x%hx\r\n", g_bmp180.adjust.ac1);
     CM_DEMO_I2C_LOG("ac2=0x%hx\r\n", g_bmp180.adjust.ac2);
     CM_DEMO_I2C_LOG("ac3=0x%hx\r\n", g_bmp180.adjust.ac3);
     CM_DEMO_I2C_LOG("ac4=0x%hx\r\n", g_bmp180.adjust.ac4);
     CM_DEMO_I2C_LOG("ac5=0x%hx\r\n", g_bmp180.adjust.ac5);
     CM_DEMO_I2C_LOG("ac6=0x%hx\r\n", g_bmp180.adjust.ac6);
     CM_DEMO_I2C_LOG("b1=0x%hx\r\n", g_bmp180.adjust.b1);
     CM_DEMO_I2C_LOG("b2=0x%hx\r\n", g_bmp180.adjust.b2);
     CM_DEMO_I2C_LOG("mb=0x%hx\r\n", g_bmp180.adjust.mb);
     CM_DEMO_I2C_LOG("mc=0x%hx\r\n", g_bmp180.adjust.mc);
     CM_DEMO_I2C_LOG("md=0x%hx\r\n", g_bmp180.adjust.md);

     g_bmp180.temperature = 0.f;
     g_bmp180.temp_offset = 0.f;
     g_bmp180.pressure = 0.f;
     g_bmp180.pres_offset = 0.f;
     g_bmp180.altitude = 0.f;
     g_bmp180.alti_offset = 0.f;
 }

 static uint8_t bmp180_i2c_init(void)
{
    cm_i2c_cfg_t config = 
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式
        I2C_CLK_100KHZ
    };//master模式,(100KHZ)

    int32_t ret = -1;
    
    CM_DEMO_I2C_LOG("bmp180 dev=%d\r\n", g_i2c_dev);
    cm_i2c_iomux_set(g_i2c_dev);

    ret = cm_i2c_open(g_i2c_dev, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err,ret=%d\r\n", ret);
        return -1;
    }
    
    return 0;
 }

static void bmp180_init(void)
{
    uint8_t ret = -1;
    
    ret = bmp180_i2c_init();
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err,ret=%d\r\n", ret);
    }
}

static void bmp180_deinit(void)
{
    cm_i2c_close(g_i2c_dev);
}

static void bmp180_measure(void)
{
    int16_t reg_temp = 0;
    int16_t reg_pressure = 0;
    long x1 = 0;
    long x2 = 0;
    long b5 = 0;
    long b6 = 0;
    long x3 = 0;
    long b3 = 0;
    long p = 0;
    unsigned long b4 = 0;
    unsigned long b7 = 0;
    
    reg_temp = bmp180_read_temperature_reg();  /* 读取温度 */
    if(reg_temp < 0)
    {
        CM_DEMO_I2C_LOG("bmp180_read_temperature_reg error,reg_temp=%d\r\n",reg_temp);
        return;
    }
    
    reg_pressure = bmp180_read_pressure_reg();     /* 读取压强 */
    /* ------------------------ 温度值计算 ------------------------ */
    x1 = ((reg_temp - (long)g_bmp180.adjust.ac6)*(long)g_bmp180.adjust.ac5) >> 15;
    x2 = ((long) g_bmp180.adjust.mc << 11) / (x1 + g_bmp180.adjust.md);
    b5 = x1 + x2;
    g_bmp180.temperature = (((b5 + 8) >> 4) / 10.f) + ((float)(((b5 + 8) >> 4) % 10) / 10.f); /* 获取温度值 */
    /* ------------------------ 气压值计算 ------------------------ */
    b6 = b5 - 4000;
    /* 计算 b3 */
    x1 = (g_bmp180.adjust.b2 * (b6 * b6) >> 12) >> 11;
    x2 = (g_bmp180.adjust.ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long)g_bmp180.adjust.ac1)*4 + x3) << BMP180_OSS) + 2) >> 2;
    /* 计算 b4 */
    x1 = (g_bmp180.adjust.ac3 * b6) >> 13;
    x2 = (g_bmp180.adjust.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (g_bmp180.adjust.ac4 * (unsigned long)(x3 + 32768)) >> 15;
    
    b7 = ((reg_pressure - b3) * (50000 >> BMP180_OSS));
    if (b7 < 0x80000000) 
    {
        p = (b7 << 1) / b4;
    } 
    else 
    {
        p = (b7 / b4) << 1;
    }
    
    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;

    /* 得到气压值 */
    g_bmp180.pressure = (p + ((x1 + x2 + 3791) >> 4))/1000.0 + ((float)(( p +((x1 + x2 + 3791) >> 4))%1000)/1000.0);

    /* ------------------------ 海拔高度计算 ------------------------ */
    g_bmp180.altitude = (BMP180_ZERO_ALTITUDE - p) / 133.0 * 12.0;
    CM_DEMO_I2C_LOG("bmp108 sensor temperature=%f\r\n", g_bmp180.temperature);
    CM_DEMO_I2C_LOG("bmp108 sensor pressure=%f\r\n", g_bmp180.pressure);
}


static uint8_t cm_i2c_bmp180_start(void)
{
    bmp180_init();
    bmp180_struct_init();
    bmp180_measure();
    bmp180_deinit();
    return 0;
}

static int8_t cm_i2c_m117b(void)
{
    cm_i2c_cfg_t config = 
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式
        I2C_CLK_100KHZ
    };//master模式,(100KHZ)
    int32_t ret = 0;
    int32_t write_len = 0;
    int32_t read_len = 0;
    uint8_t send_buf[I2C_BUF_SIZE] = {0}; 
    char read_data[I2C_BUF_SIZE] = {0};
    uint8_t crc = CRC_INIT_VALUE;

    cm_i2c_iomux_set(g_i2c_dev);

    /* 开启I2C，进行相关配置 */
    ret = cm_i2c_open(g_i2c_dev, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("line:%d,i2c%d init err,ret=%d\r\n", __LINE__, g_i2c_dev, ret);
        return -1;
    }
    
    CM_DEMO_I2C_LOG("line:%d,i2c%d  init ok\r\n", __LINE__, g_i2c_dev);
    
    /*软复位*/
    memset(send_buf, 0, I2C_BUF_SIZE);
    send_buf[0] = 0x30;
    send_buf[1] = 0xa2;
    write_len = cm_i2c_write(g_i2c_dev, M177B_DEV_ADDR, send_buf, 2);
    if(write_len != 2)
    {
        CM_DEMO_I2C_LOG("line:%d,cm_i2c_write len=%d error\r\n", __LINE__, write_len);
        cm_i2c_close(g_i2c_dev);
        return -1;
    }
    
    osDelay(M177B_RESET_DELAY_MS);
    
    /* 启动温度转换 */
    memset(send_buf, 0, I2C_BUF_SIZE);
    send_buf[0] = 0xcc;
    send_buf[1] = 0x44;
    write_len = cm_i2c_write(g_i2c_dev, M177B_DEV_ADDR, send_buf, 2);
    if(write_len != 2)
    {
        CM_DEMO_I2C_LOG("line:%d,cm_i2c_write len=2 error,ret=%d\r\n", __LINE__, write_len);
        cm_i2c_close(g_i2c_dev);
        return -1;
    }
    
    osDelay(M177B_CONV_DELAY_MS);
    
    /* 等待转换完成后读取（Temp_lsb、Temp_msb、CRC）*/
    memset(read_data, 0, I2C_BUF_SIZE);
    read_len = cm_i2c_read(g_i2c_dev,M177B_DEV_ADDR,read_data,3);
    if(read_len != 3)
    {
        CM_DEMO_I2C_LOG("line:%d,cm_i2c_read len=3 error,ret=%d\r\n", __LINE__, write_len);
        cm_i2c_close(g_i2c_dev);
        return -1;
    }
    
    CM_DEMO_I2C_LOG("line:%d,read data1:%#x\r\n",__LINE__, read_data[0]);
    CM_DEMO_I2C_LOG("line:%d,read data2:%#x\r\n",__LINE__, read_data[1]);
    CM_DEMO_I2C_LOG("line:%d,read data3:%#x\r\n",__LINE__, read_data[2]);
    
    /*计算CRC*/
	int byte_num = 0;
    for(byte_num = 0; byte_num < 2; byte_num++)
    {
        crc ^= (read_data[byte_num]);
        int bit = 8;
        for(bit = 8; bit > 0; --bit)
        {
            if(crc & 0x80) 
            {
                    crc = (crc <<1) ^ CRC_POLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    
    if(read_data[2] != crc)
    {
        CM_DEMO_I2C_LOG("line:%d,crc=0x%#x error!\r\n",__LINE__, crc);
        cm_i2c_close(g_i2c_dev);
        return -1;
    }

    //计算摄氏度温度
    int16_t temp_int = read_data[0] << 8 | read_data[1];
    CM_DEMO_I2C_LOG("line:%d,temperature:%f celsius\r\n",__LINE__, ((float)temp_int/256.0 + 40.0));
    cm_i2c_close(g_i2c_dev);
    return 0;
 }

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief  eeprom利用i2c接口读写数据测试
 *
 * @return  void
 *
 * @details More details
 */
void cm_test_i2c_eeprom(void)
{

    cm_i2c_cfg_t config = 
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式
        I2C_CLK_100KHZ
    };//master模式,(100KHZ)

    uint16_t eprom_addr = 0x100; //选取所支持的任意E2PROM地址进行测试,可修改
    int8_t w_data = 'B',r_data =0;
    int32_t ret;

    CM_DEMO_I2C_LOG("i2c test start, i2c num:%d!!\r\n",g_i2c_dev);
    cm_i2c_iomux_set(g_i2c_dev);

    ret = cm_i2c_open(g_i2c_dev, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err,ret=%d\r\n", ret);
        return;
    }
    CM_DEMO_I2C_LOG("i2c init ok\r\n");
    
    //写入测试数据
    ret = is24c256_write_byte(eprom_addr, w_data);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write e2prom err %d\r\n", (uint32_t)ret);
        cm_i2c_close(g_i2c_dev);
        return;
    }
    CM_DEMO_I2C_LOG("i2c write e2prom ok\r\n");
    osDelay(EEPROM_WRITE_DELAY_MS);//延时等待写入完成

    //读取数据
    ret = is24c256_read_byte(eprom_addr, &r_data);
    cm_i2c_close(g_i2c_dev);

    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c read e2prom err:%d\r\n", ret);
        return;
    }

    CM_DEMO_I2C_LOG("i2c read e2prom,%c\r\n",r_data);
    CM_DEMO_I2C_LOG("i2c test end!!\r\n");

    return;

}

void cm_test_i2c_bmp180(void)
{
    cm_i2c_bmp180_start();
}

void cm_test_i2c(int argc, char **argv)
{
    if(argv == NULL || argc < 3)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    if(argv[1] == NULL || argv[2] == NULL)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }

    const char *cmd = argv[1];
    g_i2c_dev = atoi(argv[2]);

    /* 检查i2c设备号是否有效 */
    if(g_i2c_dev < 0 || g_i2c_dev > CM_I2C_DEV_2)
    {
        cm_demo_printf("invalid i2c dev number\r\n");
        return;
    }

    if (strncmp(cmd, "eeprom", STR_LEN("eeprom")) == 0)
    {
        cm_test_i2c_eeprom();
    }
    else if(strncmp(cmd, "bmp180", STR_LEN("bmp180")) == 0)
    {
        cm_test_i2c_bmp180();
    }
    else if(strncmp(cmd, "m177b", STR_LEN("m177b")) == 0)
    {
        cm_i2c_m117b();
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\r\n",cmd);
    }
}

NR_SHELL_CMD_EXPORT(i2c, cm_test_i2c);
#endif
#endif

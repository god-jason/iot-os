/**
 * @file        cm_demo_i2c.c
 * @brief       OpenCPU i2c测试例程
 * @copyright   Copyright ? 2021 China Mobile IOT. All rights reserved.
 */

#ifdef CM_DEMO_I2C_SUPPORT
#ifdef OS_USING_SHELL

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <os.h>
#include <stdint.h>
#include <nr_micro_shell.h>
#include "cm_i2c.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_DEMO_I2C_LOG(fmt, args...)  osPrintf("[I2C_DEM]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)

#define ES8311_CHD1_REGFD    0xFD /* CHIP ID1 reg */
#define ES8311_ID1_VAL       0x83 /* CHIP ID1  value */
#define ES8311_CHD2_REGFE    0xFE /* CHIP ID2 reg */
#define ES8311_ID2_VAL       0x11 /* CHIP ID2  value */

#define ES8311_DEV_ADDR      0x18 /* slave address */

#define ES8311_I2C_ID        CM_I2C_DEV_2


/****************************************************************************
 * Private Types
 ****************************************************************************/

/*static int cm_i2c_device_init(cm_i2c_dev_e dev)
{
    I2C_BusDevice * i2c_dev = board_get_i2c_device();
    if(i2c_dev == NULL)
    {
        return -1;
    }
    i2c_dev->addr = ES8311_DEV_ADDR;
    I2C_BusDevice_Register(dev, i2c_dev);

    return 0;
}*/



/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/
static int cm_i2c_init(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式
        I2C_CLK_100KHZ
    };//master模式,(100KHZ)
    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err,ret=%d\n", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");

    return ret;
}

static int cm_i2c_deinit(void)
{
    int ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err %d\n", (uint32_t)ret);
        return -1;
    }

    return ret;
}

/**
 * @brief  eeprom利用i2c接口读写数据测试
 *
 * @return  void
 *
 * @details More details
 */
int cm_test_i2c(uint8_t reg)
{
    uint8_t es8311_addr = reg; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t r_data =0;
    int32_t ret = 0;

    CM_DEMO_I2C_LOG("i2c test start, i2c num:%d\n",ES8311_I2C_ID);

    //写入测试数据
    //ret = is_es8311_write_byte(es8311_addr, w_data);
    ret = cm_i2c_write_v2(ES8311_I2C_ID , ES8311_DEV_ADDR, &es8311_addr, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err %d\n", (uint32_t)ret);
        cm_i2c_close(ES8311_I2C_ID);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c write es8311 ok\n");
    //osDelay(10);//延时等待写入完成

    //读取数据
    //ret = is_es8311_read_byte(es8311_addr, &r_data);
    ret = cm_i2c_read_v2(ES8311_I2C_ID , ES8311_DEV_ADDR, &r_data, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err %d\n", (uint32_t)ret);
        cm_i2c_close(ES8311_I2C_ID);
        return -2;
    }
    CM_DEMO_I2C_LOG("i2c read es8311 ret %d, r_data %02x\n", ret, r_data);

    return 0;
}

int cm_test_i2c_1(uint8_t reg)
{
    uint8_t es8311_addr = reg; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t r_data =0;
    int32_t ret = 0;

    CM_DEMO_I2C_LOG("cm_i2c_write_then_read start, i2c num:%d\n",ES8311_I2C_ID);

    //写入测试数据
    //ret = is_es8311_write_byte(es8311_addr, w_data);
    ret = cm_i2c_write_then_read(ES8311_I2C_ID, ES8311_DEV_ADDR, &es8311_addr, 1, &r_data, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_write_then_read err %d\n", (uint32_t)ret);
        cm_i2c_close(ES8311_I2C_ID);
        return -1;
    }
    CM_DEMO_I2C_LOG("cm_i2c_write_then_read ret %d, r_data %02x\n", ret, r_data);

    return 0;
}

static void read_es8311_id(void)
{
    int cnt = 1;
    cm_i2c_init();
    while(cnt --)
    {
        int ret = cm_test_i2c(ES8311_CHD1_REGFD);
        if(ret < 0)
        {
            CM_DEMO_I2C_LOG("cm_test_i2c err ret %d!!\n",ret);
        }
        ret = cm_test_i2c_1(ES8311_CHD2_REGFE);
        if(ret < 0)
        {
            CM_DEMO_I2C_LOG("cm_test_i2c err ret %d!!\n",ret);
        }
         osDelay(200);
    }


    cm_i2c_deinit();
    CM_DEMO_I2C_LOG("i2c test success!!\n");
}


static int tc_read_es8311_id_normal(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式master模式,(100KHZ)
        I2C_CLK_100KHZ
    };

    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err,ret=%d\n", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");


    uint8_t es8311_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t r_data = 0;
    //写入测试数据
    ret = cm_i2c_write_v2(ES8311_I2C_ID , ES8311_DEV_ADDR, &es8311_addr, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err %d\n", ret);
        cm_i2c_close(ES8311_I2C_ID);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok\n", es8311_addr);
    //osDelay(10);//延时等待写入完成

    //读取数据
    ret = cm_i2c_read_v2(ES8311_I2C_ID , ES8311_DEV_ADDR, &r_data, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err %d\n", ret);
        cm_i2c_close(ES8311_I2C_ID);
        return -2;
    }
    CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02x]\n", r_data);

    ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err %d\n", ret);
        return -1;
    }

    CM_DEMO_I2C_LOG("i2c test success!!\n");

    return ret;
}

static int tc_read_es8311_id_abnormal(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式master模式,(100KHZ)
        I2C_CLK_100KHZ
    };

    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err1[%d]", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");

    // test with wrong slave addr
    uint8_t register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t slave_addr    = 0x2F; //0x18
    uint8_t r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err2[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err3[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02X]", r_data);

    // test with right slave addr
    register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    slave_addr    = ES8311_DEV_ADDR;
    r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err4[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err5[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02x]", r_data);

    ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err6[%d]", ret);
    }

    CM_DEMO_I2C_LOG("i2c test success");

    return ret;
}


static int tc_read_es8311_id_abnormal_1(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式master模式,(100KHZ)
        I2C_CLK_100KHZ
    };

    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err1[%d]", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");

    // test with wrong slave addr
    uint8_t register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t slave_addr    = 0x2F;
    uint8_t r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err2[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err3[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02X]", r_data);

    ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err4[%d]", ret);
    }

    // test with right slave addr
    ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err5[%d]", ret);
        return -5;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");
    register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    slave_addr    = ES8311_DEV_ADDR;
    r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err6[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err7[%d]", ret);
    }
    CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02X]", r_data);

    ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err8[%d]", ret);
    }

    CM_DEMO_I2C_LOG("i2c test success");

    return ret;
}


static int tc_read_es8311_id_abnormal_2(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式master模式,(100KHZ)
        I2C_CLK_100KHZ
    };

    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err[%d]", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");

    //
    // test with wrong slave addr
    ///////////////////////////////////
    uint8_t register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t slave_addr    = 0x2F; //0x18
    uint8_t r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 slave_addr[%X] err[%d]", slave_addr, ret);

        // test with right slave addr
        slave_addr = 0x18;
        r_data = 0;
        // write register value which is read value from
        ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
        if (ret != 0)
        {
            CM_DEMO_I2C_LOG("i2c write es8311  slave_addr[%X] err[%d]", slave_addr, ret);
        }
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c write es8311 slave_addr[%X] REG[%X]ok", slave_addr, register_addr);
    }
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 slave_addr[%X] err[%d]", slave_addr, ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c read es8311 slave_addr[%X] r_data[%02X]", slave_addr, r_data);
    }


    //
    //test with right slave addr
    ///////////////////////////////////
    register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    slave_addr    = ES8311_DEV_ADDR;
    r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    }
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err5[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02x]", r_data);
    }
    ret = cm_i2c_close(ES8311_I2C_ID);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close err6[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c test success");
    }

    return ret;
}


static int tc_read_es8311_id_abnormal_3(void)
{
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER, //目前仅支持模式master模式,(100KHZ)
        I2C_CLK_100KHZ
    };

    int ret = cm_i2c_open_v2(ES8311_I2C_ID, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c init err[%d]", ret);
        return -1;
    }
    CM_DEMO_I2C_LOG("i2c init ok\n");

    //
    // test with WRONG slave addr
    ///////////////////////////////////
    uint8_t register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    uint8_t slave_addr    = 0x2F; //0x18
    uint8_t r_data = 0;
    uint8_t id_1 = 0;
    uint8_t id_2 = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 slave_addr[%X] err[%d]", slave_addr, ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c write es8311 slave_addr[%X] REG[%X]ok", slave_addr, register_addr);
    }
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 slave_addr[%X] err[%d]", slave_addr, ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c read es8311 slave_addr[%X] r_data[%02X]", slave_addr, r_data);
    }

    //
    //test with CORRECT slave addr to read ID1
    ///////////////////////////////////
    register_addr = ES8311_CHD1_REGFD; //选取所支持的任意es8311地址进行测试,可修改
    slave_addr    = ES8311_DEV_ADDR;
    r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    }
    //osDelay(10);//延时等待写入完成

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err5[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02x]", r_data);
    }
    id_1 = r_data;

    //
    //test with right slave addr to read ID2
    ///////////////////////////////////
    register_addr = ES8311_CHD2_REGFE; //选取所支持的任意es8311地址进行测试,可修改
    slave_addr    = ES8311_DEV_ADDR;
    r_data = 0;
    // write register value which is read value from
    ret = cm_i2c_write_v2(ES8311_I2C_ID , slave_addr, &register_addr, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("i2c write es8311 err[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c write es8311 REG[%X]ok", register_addr);
    }

    //read the value from the register which is written before
    ret = cm_i2c_read_v2(ES8311_I2C_ID , slave_addr, &r_data, 1);
    if (ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_read_v2 err5[%d]", ret);
    }
    else
    {
        CM_DEMO_I2C_LOG("i2c read es8311 r_data[%02x]", r_data);
    }
    id_2 = r_data;

    CM_DEMO_I2C_LOG("cm_i2c_close ret[%d]", cm_i2c_close(ES8311_I2C_ID));

    if ((id_1 != ES8311_ID1_VAL) && (id_2 != ES8311_ID2_VAL))
    {
        CM_DEMO_I2C_LOG("ERR i2c read es8311 ID1 ID2", id_1, id_2);
        ret = -1;
    }
    CM_DEMO_I2C_LOG("i2c read es8311 id_1[0x%02X]id_2[0x%02X]", id_1, id_2);

    return ret;
}


static int i2c_test_common_open(cm_i2c_dev_e dev, cm_i2c_addr_type_e addr_type,
                                         cm_i2c_mode_e mode, int clock)
{
    //目前仅支持模式master模式,(100KHZ)
    cm_i2c_cfg_t config =
    {
        CM_I2C_ADDR_TYPE_7BIT,
        CM_I2C_MODE_MASTER,
        I2C_CLK_100KHZ
    };

    if (addr_type <= CM_I2C_ADDR_TYPE_10BIT)
    {
        config.addr_type = addr_type;
    }
    if (mode <= CM_I2C_MODE_SLAVE)
    {
        config.mode = mode;
    }
    if ((clock == I2C_CLK_100KHZ) || (clock == I2C_CLK_200KHZ) || (clock == I2C_CLK_400KHZ) ||
        (clock == I2C_CLK_1MHZ) || (clock == I2C_CLK_3M4HZ))
    {
        config.clk = clock;
    }
    CM_DEMO_I2C_LOG("dev[%d]addr_type[%d]mode[%d]clock[%d]\n", dev, addr_type, mode, clock);

    int ret = cm_i2c_open_v2(dev, &config);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_open_v2 err ret[%d]\n", ret);
    }

    return ret;
}

static int i2c_test_common_close(cm_i2c_dev_e dev)
{
    CM_DEMO_I2C_LOG("dev[%d]\n", dev);

    int ret = cm_i2c_close(dev);
    if(ret != 0)
    {
        CM_DEMO_I2C_LOG("cm_i2c_close[%d] err[%d]\n", dev, ret);
    }
    return ret;
}

static int i2c_test_common_rw(cm_i2c_dev_e dev, uint16_t slave_addr, uint8_t op_rw,
                                  uint8_t * p_data, uint32_t data_len)
{
    int ret = -1;
    CM_DEMO_I2C_LOG("dev[%d]slave_addr[%X]op_rw[%d]data[%X][%u]\n", dev, slave_addr, op_rw, p_data, data_len);
    if (p_data == NULL)
    {
        CM_DEMO_I2C_LOG("p_data NULL");
        return -1;
    }
    if (op_rw == 0)
    {
        //读取数据
        ret = cm_i2c_read_v2(dev, slave_addr, p_data, data_len);
        if(ret != 0)
        {
            CM_DEMO_I2C_LOG("cm_i2c_read_v2[%d]serr[%d]\n", dev, ret);
            cm_i2c_close(dev);
            return -2;
        }
        CM_DEMO_I2C_LOG("i2c read es8311 ret %d, r_data[%02x]\n", ret, *p_data);
    }
    else
    {
        //写入测试数据
        ret = cm_i2c_write_v2(dev , slave_addr, p_data, data_len);
        if(ret != 0)
        {
            CM_DEMO_I2C_LOG("cm_i2c_write_v2[%d]err[%d]\n", dev, ret);
            cm_i2c_close(dev);
            return -3;
        }
        CM_DEMO_I2C_LOG("i2c write es8311 ok\n");
    }

    CM_DEMO_I2C_LOG("i2c test success!!\n");

    return ret;
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
void SHELL_testI2c(char argc, char **argv)
{
    int ret = -1;
    int cmd = 0;

    if (argc == 1)
    {
        read_es8311_id();
        return ret;
    }
    if (argc > 1)
    {
#if 0
        if ((OS_NULL != strstr(argv[1], "help")) ||
            (OS_NULL != strstr(argv[1], "-help")) ||
            (OS_NULL != strstr(argv[1], "--help")) ||
             (OS_NULL != strstr(argv[1], "-h")) ||
             (OS_NULL != strstr(argv[1], "--h")))
        {
            lbs_shell_help();
            return ret;
        }
#endif
        cmd = (int)strtol(argv[1], OS_NULL, 0);
    }
    CM_DEMO_I2C_LOG("argc[%d]argv2[%X]argv3[%X]argv4[%X]argv5[%X]", argc, argv[2], argv[3], argv[4], argv[5]);


    uint8_t dev_id         = CM_I2C_DEV_1;
    uint8_t dev_open_close = 1;
    uint8_t addr_type      = CM_I2C_ADDR_TYPE_7BIT;
    uint8_t mode           = CM_I2C_MODE_MASTER;
    int clock              = I2C_CLK_100KHZ;
    uint8_t op_rw          = 0;
    uint16_t slave_addr    = 0x18;

    switch (cmd)
    {
        case 0:
        {
            // open/close: cm_i2c 0 <dev_id> <open/close>
            if (argc == 4)
            {
                dev_id         = (uint8_t)strtol(argv[2], OS_NULL, 0);
                dev_open_close = (uint8_t)strtol(argv[3], OS_NULL, 0);

                if (dev_open_close != 0)
                {
                    ret = i2c_test_common_open(dev_id, addr_type, mode, clock);
                }
            }
            // open/close: cm_i2c 0 <dev_id> <open/close> <addr_bits> <mode> <clock>
            if (argc == 7)
            {
                dev_id         = (uint8_t)strtol(argv[2], OS_NULL, 0);
                dev_open_close = (uint8_t)strtol(argv[3], OS_NULL, 0);
                addr_type      = (uint8_t)strtol(argv[4], OS_NULL, 0);
                mode           = (uint8_t)strtol(argv[5], OS_NULL, 0);
                clock          = (int)strtol(argv[6], OS_NULL, 0);

                if (dev_open_close != 0)
                {
                    ret = i2c_test_common_open(dev_id, addr_type, mode, clock);
                }
                else
                {
                    ret = i2c_test_common_close(dev_id);
                }
            }
            // write/read:cm_i2c 0 <dev_id> <slave_addr> <op_rw>
            if (argc == 5)
            {
                dev_id         = (uint8_t)strtol(argv[2], OS_NULL, 0);
                slave_addr     = (uint8_t)strtol(argv[3], OS_NULL, 0);
                op_rw          = (uint8_t)strtol(argv[4], OS_NULL, 0);
                uint8_t i2c_data = 0;
                ret = i2c_test_common_rw(dev_id, slave_addr, op_rw, &i2c_data, 1);
            }
            break;
        }
        case 1:
        {
            if (argc > 2)
            {
                uint8_t sub_cmd = (uint8_t)strtol(argv[2], OS_NULL, 0);
                if (sub_cmd == 0)
                {
                    ret = tc_read_es8311_id_normal();
                }
                if (sub_cmd == 1)
                {
                    ret = tc_read_es8311_id_abnormal();
                }
                if (sub_cmd == 2)
                {
                    ret = tc_read_es8311_id_abnormal_1();
                }
                if (sub_cmd == 3)
                {
                    ret = tc_read_es8311_id_abnormal_2();
                }
                if (sub_cmd == 4)
                {
                    ret = tc_read_es8311_id_abnormal_3();
                }
            }
            else
            {
                read_es8311_id();
                ret = 0;
            }
            break;
        }
        default:
        {
            CM_DEMO_I2C_LOG("cmd[%d]", cmd);
            break;
        }
    }

    CM_DEMO_I2C_LOG("TC_%u %s", cmd, (ret == 0) ? "SUCC" : "FAILED");
}

NR_SHELL_CMD_EXPORT(cm_i2c, SHELL_testI2c);

#endif
#endif


#include "stdio.h"
#include "string.h"

#include "yopen_debug.h"
#include "yopen_spi.h"
#include "yopen_os.h"


#define DEMO_SPI_TRACE(fmt, ...) yopen_trace("[SPI]"fmt, ##__VA_ARGS__)
#define __ALIGNED(x)    __attribute__((aligned(x)))
static __ALIGNED(4) uint8_t master_buffer_out[64];
static __ALIGNED(4) uint8_t master_buffer_in[64];

#define DEMO_SPI_PORT YOPEN_SPI_PORT0 //YOPEN_SPI_PORT1

static void yopen_spi_pin_init(void)
{   
#ifdef TYPE_EC718M
    //SPI0
    //SSN
    int ret = yopen_pin_set_func(YOPEN_PIN_GPIO8, 1);
    DEMO_SPI_TRACE("========== SSN ret %d ==========", ret);
    //MOSI
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO9, 1);
    DEMO_SPI_TRACE("========== MOSI ret %d ==========", ret);
    //MISO
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO10, 1);
    DEMO_SPI_TRACE("========== MISO ret %d ==========", ret);
    //SCLK
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO11, 1);
    DEMO_SPI_TRACE("========== SCLK ret %d ==========", ret);

    //SPI 1
    //SSN
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO12, 1);
    DEMO_SPI_TRACE("========== SSN ret %d ==========", ret);
    //MOSI
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO13, 1);
    DEMO_SPI_TRACE("========== MOSI ret %d ==========", ret);
    //MISO
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO14, 1);
    DEMO_SPI_TRACE("========== MISO ret %d ==========", ret);
    //SCLK
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO15, 1);
    DEMO_SPI_TRACE("========== SCLK ret %d ==========", ret);
#else
    //SSN
    int ret = yopen_pin_set_func(YOPEN_PIN_GPIO2, 1);
    DEMO_SPI_TRACE("========== SSN ret %d ==========", ret);
    //MOSI
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO3, 1);
    DEMO_SPI_TRACE("========== MOSI ret %d ==========", ret);
    //MISO
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO4, 1);
    DEMO_SPI_TRACE("========== MISO ret %d ==========", ret);
    //SCLK
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO5, 1);
    DEMO_SPI_TRACE("========== SCLK ret %d ==========", ret);
#endif
}

// TX RX短接，测试自发自收
void yopen_spi_demo_task(void * arg)
{
    yopen_spi_config_s spi_config = {0};
    int i, ret;

    yopen_rtos_task_sleep_ms(2000);
	DEMO_SPI_TRACE("========== spi demo start ==========");

    yopen_spi_pin_init();

    spi_config.port = DEMO_SPI_PORT;
    spi_config.framesize = 8;
    spi_config.spiclk = YOPEN_SPI_CLK_812_5KHZ;
    
    ret = yopen_spi_init(spi_config);
	DEMO_SPI_TRACE("========== yopen_spi_init ret %d ==========", ret);

	memset(master_buffer_in, 0, sizeof(master_buffer_in));
	memset(master_buffer_out, 0, sizeof(master_buffer_out));

    for (i=0; i<sizeof(master_buffer_out); i++)
    {
        master_buffer_out[i] = i+1;
    }
	ret = yopen_spi_write_read(DEMO_SPI_PORT, master_buffer_in, master_buffer_out, sizeof(master_buffer_out));

	DEMO_SPI_TRACE("========== yopen_spi_write_read ret %d ==========", ret);

    if (memcmp(master_buffer_out, master_buffer_in, sizeof(master_buffer_out)) == 0)
    {
        DEMO_SPI_TRACE("========== yopen_spi_write_read result success ==========");

    }
    else 
    {
        DEMO_SPI_TRACE("========== yopen_spi_write_read result fail ==========");

        for (i=0; i<sizeof(master_buffer_out); i++)
        {
                if (master_buffer_out[i] != master_buffer_in[i])
                {
                    DEMO_SPI_TRACE("========== %d/%d ==========", master_buffer_out[i], master_buffer_in[i]);
                }
            
        }
    }

    DEMO_SPI_TRACE("========== spi demo end ==========");

    yopen_rtos_task_delete(NULL);  // kill itself
}

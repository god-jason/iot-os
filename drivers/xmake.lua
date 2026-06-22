-- drivers xmake 配置

target("drivers")
    set_kind("static")
    add_files("*.c")
    add_includedirs("..", ".")
    add_deps("iot")

    -- 添加各个驱动子目录
    for _, dir in ipairs({
        "bme280", "bh1750", "dht", "ssd1306", "mpu6050",
        "pcf8574", "ads1115", "ds18b20", "hmc5883l",
        "lcd1602", "relay", "led", "buzzer"
    }) do
        add_files(dir .. "/*.c")
        add_includedirs(dir)
    end

    if is_config("use_i2c") then
        add_defines("USE_I2C")
    end

    if is_config("use_spi") then
        add_defines("USE_SPI")
    end

    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
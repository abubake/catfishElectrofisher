#include <stdio.h>
#include <time.h>


#include "ANY_LCD.h"
#include <driver/i2c.h>
#include "esp_log.h"

/* Test comment */

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_PORT_NUM         0 
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define ESP_SLAVE_ADDR              0x27                       /*!< 0x38 if manufacturer of i2c LCD interface device is not TI */
#define ACK_CHECK_EN                0x1                        /*!< I2C master will check ack from slave*/
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency 400000*/
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */

// I2C address is 0x27 or 0x38

/* USING ANY_LCD */
/*

Requires: 
    1. write function - i2c_master_write_slave
    2. Delay function - ets_delay_us

    Creation of LCD object
*/

/* 1. I2C Config */
int i2c_master_port = I2C_MASTER_PORT_NUM;
int data = 100;

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}



/* Communication as master */
void i2c_master_write_slave(uint8_t data_wr) // CHANGED FROM POINTER
{
    i2c_port_t i2c_num = 0;
    size_t size = 1; // previously 16

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN); // ack_check is 1
    i2c_master_write(cmd, &data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}


void app_main(){

/* I2C Peripheral Init */
ESP_ERROR_CHECK(i2c_master_init());
ESP_LOGI(TAG, "I2C initialized successfully");


/****LCD INIT*****/
any_lcd_t lcd;
lcd.delay_us  = &ets_delay_us;
lcd.lcd_write = &i2c_master_write_slave; // FIXME: Needs to only take data
LCD_Init(lcd);
char buffer[32];

while(1){

    LCD_Reset(lcd); // clear and reset cursor
    sprintf(buffer,"Sensor Value: %d",data);
    LCD_WriteString(buffer,lcd);
    lcd.delay_us(1E6);
    }

}
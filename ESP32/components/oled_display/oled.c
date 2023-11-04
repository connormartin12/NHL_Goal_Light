#include "oled.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "../https_interface/https_interface.h"

static const char *TAG = "OLED";

// Basic pin number definitions and hardware information
#define I2C_HOST              0
#define LCD_PIXEL_CLOCK_HZ    (400 * 1000) // Default 400,000hz for SSD3016
#define PIN_NUM_SCL           1
#define PIN_NUM_SDA           2
#define PIN_NUM_RST           -1
#define I2C_HW_ADDR           0x3C
// Vertical/Horizontal resolution
#define LCD_H_RES              128
#define LCD_V_RES              64
// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

lv_disp_t *disp;
lv_obj_t *info_screen;
lv_obj_t *score_screen;
lv_obj_t *info_label;
lv_obj_t *score_label;

static lv_style_t info_style;
static lv_style_t score_style;

void set_oled_text(const char *text)
{
    if (score_screen == lv_disp_get_scr_act(disp))
        lv_scr_load(info_screen);
    lv_label_set_text(info_label, text);
}

void set_team_scored(const char *text)
{
    if (info_screen == lv_disp_get_scr_act(disp))
        lv_scr_load(info_screen);
    lv_label_set_text(score_label, text);
}

void update_oled_score()
{
    if (info_screen == lv_disp_get_scr_act(disp))
        lv_scr_load(score_screen);
    lv_label_set_text_fmt(score_label, "%s: %d\n%s: %d", user_team_abbr, user_team_score, 
                                                   other_team_abbr, other_team_score);
}

void setup_ui()
{
    // Create information screen
    info_screen = lv_obj_create(NULL);
    info_label = lv_label_create(info_screen);
    lv_style_init(&info_style);
    lv_style_set_text_font(&info_style, &lv_font_montserrat_14);
    lv_obj_add_style(info_label, &info_style, 0);
    lv_obj_set_width(info_label, disp->driver->hor_res);
    lv_obj_align(info_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // Create score screen
    score_screen = lv_obj_create(NULL);
    score_label = lv_label_create(score_screen);
    lv_style_init(&score_style);
    lv_style_set_text_font(&score_style, &lv_font_montserrat_20);
    lv_obj_add_style(score_label, &score_style, 0);
    lv_obj_set_width(score_label, disp->driver->hor_res);
    lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 0, 0);

    // Set information screen as initial screen
    lv_scr_load(info_screen);
}

void initialize_oled()
{
    // Initialize I2C bus
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_NUM_SDA,
        .scl_io_num = PIN_NUM_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = LCD_PIXEL_CLOCK_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_HOST, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_HOST, I2C_MODE_MASTER, 0, 0, 0));

    // Install panel IO
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = I2C_HW_ADDR,
        .control_phase_bytes = 1,           // According to SSD1306 datasheet
        .lcd_cmd_bits = LCD_CMD_BITS,       // According to SSD1306 datasheet
        .lcd_param_bits = LCD_PARAM_BITS,   // According to SSD1306 datasheet
        .dc_bit_offset = 6,                 // According to SSD1306 datasheet
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_HOST, &io_config, &io_handle));

    // Install SSD1306 panel driver
    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Initialize LVGL
    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * LCD_V_RES,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_180); // Note: Screen was upside down

    // Display LVGL Text
    ESP_LOGI(TAG, "Display LVGL Text");
    setup_ui(disp);
}
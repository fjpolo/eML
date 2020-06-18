/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "bsp.h"
#include "fpioa.h"
#include "lcd.h"
#include "sysctl.h"
#include "nt35310.h"
#include "board_config.h"
#include "ns2009.h"
#include "tscal.h"
#include "sleep.h"

#include "lvgl.h"
#include "demo.h"

uint32_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX / 2] __attribute__((aligned(128)));

static void io_set_power(void)
{
#if BOARD_LICHEEDAN
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
#endif
}

static void io_mux_init(void)
{
#if BOARD_LICHEEDAN
    /**
    *lcd_cs	    36
    *lcd_rst	37
    *lcd_dc	    38
    *lcd_wr 	39
    * */
    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);

    fpioa_set_function(30, FUNC_I2C0_SCLK);
    fpioa_set_function(31, FUNC_I2C0_SDA);

    sysctl_set_spi0_dvp_data(1);
#else
    fpioa_set_function(8, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(6, FUNC_SPI0_SS3);
    fpioa_set_function(7, FUNC_SPI0_SCLK);
    sysctl_set_spi0_dvp_data(1);
#endif
}

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    lcd_draw_picture_by_half((uint16_t)x1, (uint16_t)y1, (uint16_t)(x2 - x1 + 1), (uint16_t)(y2 - y1 + 1), (uint16_t *)color_p);
    lv_flush_ready();
}

static struct ts_ns2009_pdata_t *ts_ns2009_pdata;

bool lvgl_tp_read(lv_indev_data_t *data)
{
    ts_ns2009_poll(ts_ns2009_pdata);

    switch (ts_ns2009_pdata->event->type)
    {
    case TOUCH_BEGIN:
        data->point.x = ts_ns2009_pdata->event->x;
        data->point.y = ts_ns2009_pdata->event->y;
        data->state = LV_INDEV_STATE_PR;
        return false;

    case TOUCH_MOVE:
        data->point.x = ts_ns2009_pdata->event->x;
        data->point.y = ts_ns2009_pdata->event->y;
        data->state = LV_INDEV_STATE_PR;
        return false;

    case TOUCH_END:
        data->point.x = ts_ns2009_pdata->event->x;
        data->point.y = ts_ns2009_pdata->event->y;
        data->state = LV_INDEV_STATE_REL;
        return false;

    default:
        break;
    }

    return false;
}

void do_lvgl_init(void)
{
    lcd_init();
#if BOARD_LICHEEDAN
    lcd_set_direction(DIR_YX_LRUD);
#endif
    lcd_clear(WHITE);
    lcd_draw_string(0, 0, "LVGL Test ", RED);
    msleep(500);

    //lv_init
    lv_init();

    //dis
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/
    disp_drv.disp_flush = my_disp_flush;
    lv_disp_drv_register(&disp_drv);

    //indev
    ts_ns2009_pdata = ts_ns2009_probe();
    if (ts_ns2009_pdata == NULL)
        printf("ns1009 init error!\r\n");

    // do_tscal();

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read = lvgl_tp_read;
    lv_indev_drv_register(&indev_drv);
}

int main(void)
{
    uint64_t core_id = current_coreid();

    if (core_id == 0)
    {
        printf("lvgl test\n");
        io_mux_init();
        io_set_power();

        do_lvgl_init();

        demo_create();

        while (1)
        {
            lv_task_handler();
            lv_tick_inc(1);
            msleep(1);
            // ts_ns2009_poll();
            // msleep(ts_ns2009_pdata->interval);
        }
    }
    else
    {
        while (1)
        {
            // lv_tick_inc(1);
            // msleep(1);
        }
    }
    while (1)
        ;
}

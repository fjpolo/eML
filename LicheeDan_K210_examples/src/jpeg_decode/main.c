#include <stdio.h>
#include "sysctl.h"
#include "bsp.h"
#include "fpioa.h"
#include "sleep.h"
#include "lcd.h"
#include "nt35310.h"

#include "picojpeg.h"
#include "picojpeg_util.h"
//19 clk
//21 mosi
//18 miso
//20 ss

//23 ready
//25 reset
static void io_mux_init(void)
{
    /* Init SPI IO map and function settings */
    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);

    sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void)
{
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

extern unsigned char jpeg_data[43756];

int main(void)
{
    io_set_power();
    io_mux_init();

    lcd_init();
    lcd_set_direction(DIR_YX_RLDU);
    lcd_clear(BLUE);
    lcd_draw_string(16, 40, "Canaan", RED);

    uint64_t tm = sysctl_get_time_us();

    jpeg_image_t *jpeg = pico_jpeg_decode(jpeg_data, sizeof(jpeg_data));

    printf("decoede use :%ld us\r\n", sysctl_get_time_us() - tm);

    if (jpeg != NULL)
    {
        printf("Width: %d, Height: %d, Comps: %d\r\n", jpeg->width, jpeg->height, jpeg->comps);

        char *p = NULL;
        switch (jpeg->scan_type)
        {
        case PJPG_GRAYSCALE:
            p = "GRAYSCALE";
            break;
        case PJPG_YH1V1:
            p = "H1V1";
            break;
        case PJPG_YH2V1:
            p = "H2V1";
            break;
        case PJPG_YH1V2:
            p = "H1V2";
            break;
        case PJPG_YH2V2:
            p = "H2V2";
            break;
        }

        printf("Scan type: %s\r\n", p);
        jpeg_display(0, 0, jpeg);

        free(jpeg->img_data);
        free(jpeg);
    }

    while (1)
        ;
}

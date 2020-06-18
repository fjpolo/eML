/* Copyright 2019 Sipeed Inc.
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "dvp.h"
#include "fpioa.h"
#include "lcd.h"
#include "ov2640.h"
#include "plic.h"
#include "sysctl.h"
#include "uarths.h"
#include "nt35310.h"
#include "utils.h"
#include "kpu.h"
#include "layer.h"
#include "board_config.h"
#include "sleep.h"
#include "encoding.h"
#include "gpiohs.h"

#define PIN_KEY 16
#define GPIO_KEY 4

#define PLL0_OUTPUT_FREQ 600000000UL
#define PLL1_OUTPUT_FREQ 400000000UL
#define PLL2_OUTPUT_FREQ 45158400UL

volatile uint8_t g_ai_done_flag;

//plic_irq_callback_t
static int kpu_done(void *ctx)
{
	g_ai_done_flag = 1;
	return 0;
}

int key_flag = 0;
int key_state = 1;
uint8_t g_ai_buf_in[320 * 240 * 3] __attribute__((aligned(128)));
uint8_t g_ai_buf_out[320 * 240 * 3] __attribute__((aligned(128)));

uint32_t g_lcd_gram[38400] __attribute__((aligned(64)));

//uint8_t g_ai_buf[320 * 240 *3] __attribute__((aligned(128)));
volatile uint8_t g_dvp_finish_flag = 0;
volatile uint8_t g_ram_mux = 0;

/* clang-format off */

//  卷积	池化	批归一化	激活	输出偏置
float conv_data[9*3*3] ={
//R
-1,-1,-1,-1,8,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//G
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,8,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
//B
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,8,-1,-1,-1,-1,
};

#define DEMO_CNT 4
int demo_index=0;
const float conv_data_demo[DEMO_CNT][9*3*3] ={
{	//origin
//R
0,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//G
0,0,0,0,0,0,0,0,0,
0,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//B
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
0,0,0,0,1,0,0,0,0,},
{	//edge
//R
-1,-1,-1,-1,8,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//G
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,8,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
//B
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,8,-1,-1,-1,-1,},
{	//sharp
//R
-1,-1,-1,-1,9,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//G
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,9,-1,-1,-1,-1,
0,0,0,0,0,0,0,0,0,
//B
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
-1,-1,-1,-1,9,-1,-1,-1,-1,},
{	//relievo
//R
2,0,0,0,-1,0,0,0,-1,
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
//G
0,0,0,0,0,0,0,0,0,
2,0,0,0,-1,0,0,0,-1,
0,0,0,0,0,0,0,0,0,
//B
0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
2,0,0,0,-1,0,0,0,-1,},
};
/* clang-format on */

void print_2d_u8(uint8_t *data, uint16_t w, uint16_t h)
{
	uint16_t i;
	for (i = 0; i < w * h; i++)
	{
		printf("%d\t", data[i]);
		if ((i + 1) % w == 0)
			printf("\r\n");
	}
	printf("\r\n");
	return;
}

void print_2d_u16(uint16_t *data, uint16_t w, uint16_t h)
{
	uint16_t i;
	for (i = 0; i < w * h; i++)
	{
		printf("%d\t", data[i]);
		if ((i + 1) % w == 0)
			printf("\r\n");
	}
	printf("\r\n");
	return;
}

static int on_irq_dvp(void *ctx)
{
	if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
	{
		/* switch gram */
		//dvp_set_display_addr(g_ram_mux ? (uint32_t)g_lcd_gram0 : (uint32_t)g_lcd_gram1);

		dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
		g_dvp_finish_flag = 1;
	}
	else
	{
		if (g_dvp_finish_flag == 0)
			dvp_start_convert();
		dvp_clear_interrupt(DVP_STS_FRAME_START);
	}

	return 0;
}

/**
 *	dvp_pclk	io47
 *	dvp_xclk	io46
 *	dvp_hsync	io45
 *	dvp_pwdn	io44
 *	dvp_vsync	io43
 *	dvp_rst		io42
 *	dvp_scl		io41
 *	dvp_sda		io40
 * */
/**
 *  lcd_cs	    36
 *  lcd_rst	37
 *  lcd_dc	    38
 *  lcd_wr 	39
 * */

static void io_mux_init(void)
{
	/* Init DVP IO map and function settings */
	fpioa_set_function(42, FUNC_CMOS_RST);
	fpioa_set_function(44, FUNC_CMOS_PWDN);
	fpioa_set_function(46, FUNC_CMOS_XCLK);
	fpioa_set_function(43, FUNC_CMOS_VSYNC);
	fpioa_set_function(45, FUNC_CMOS_HREF);
	fpioa_set_function(47, FUNC_CMOS_PCLK);
	fpioa_set_function(41, FUNC_SCCB_SCLK);
	fpioa_set_function(40, FUNC_SCCB_SDA);

	/* Init SPI IO map and function settings */
	fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
	fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
	fpioa_set_function(36, FUNC_SPI0_SS3);
	fpioa_set_function(39, FUNC_SPI0_SCLK);

	sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void)
{
	/* Set dvp and spi pin to 1.8V */
	sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
	sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

void rgb888_to_565(uint8_t *src_r, uint8_t *src_g, uint8_t *src_b, uint16_t *dst, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i += 2)
	{
		dst[i] = (((uint16_t)(src_r[i + 1] >> 3)) << 11) + (((uint16_t)src_g[i + 1] >> 2) << 5) + (((uint16_t)src_b[i + 1]) >> 3);
		dst[i + 1] = (((uint16_t)(src_r[i] >> 3)) << 11) + (((uint16_t)src_g[i] >> 2) << 5) + (((uint16_t)src_b[i]) >> 3);
	}
}

void lcd_ram_cpyimg(char *lcd, int lcdw, char *img, int imgw, int imgh, int x, int y)
{
	int i;
	for (i = 0; i < imgh; i++)
	{
		memcpy(lcd + lcdw * 2 * (y + i) + x * 2, img + imgw * 2 * i, imgw * 2);
	}
	return;
}

const char logo[11] = {'S', 'i', 'p', 'e', 'e', 'd', '.', 'c', 'o', 'm', 0};
void draw_conv(void)
{
	char string_buf[8 * 16 * 2 * 16]; //16个字符
	char line[20];
	sprintf(line, "% 2d % 2d % 2d", (int)conv_data[0], (int)conv_data[1], (int)conv_data[2]);
	lcd_ram_draw_string(line, (uint32_t *)string_buf, RED, BLACK);
	lcd_ram_cpyimg((char *)g_lcd_gram, 320, string_buf, strlen(line) * 8, 16, 0, 0);
	sprintf(line, "% 2d % 2d % 2d", (int)conv_data[3], (int)conv_data[4], (int)conv_data[5]);
	lcd_ram_draw_string(line, (uint32_t *)string_buf, RED, BLACK);
	lcd_ram_cpyimg((char *)g_lcd_gram, 320, string_buf, strlen(line) * 8, 16, 0, 16);
	sprintf(line, "% 2d % 2d % 2d", (int)conv_data[6], (int)conv_data[7], (int)conv_data[8]);
	lcd_ram_draw_string(line, (uint32_t *)string_buf, RED, BLACK);
	lcd_ram_cpyimg((char *)g_lcd_gram, 320, string_buf, strlen(line) * 8, 16, 0, 32);
	//Draw logo
	sprintf(line, "Sipeed.com");
	lcd_ram_draw_string(line, (uint32_t *)string_buf, RED, BLACK);
	lcd_ram_cpyimg((char *)g_lcd_gram, 320, string_buf, strlen(line) * 8, 16, 0, 240 - 16);
	return;
}

//return 1, update; 0, no update.
int parse_input(void)
{
	static char buf[100];
	static int i = 0;
	float data[9];
	int ch, j;
	while (((ch = uarths_getc()) & 0xff) != 0xff)
	{
		buf[i] = (char)(ch);
		/*uarths_putchar((buf[i]/100)+'0');
		uarths_putchar((buf[i]/10)%10+'0'); 
		uarths_putchar((buf[i]/1)%10+'0');
		uarths_putchar(' '); */
		uarths_putchar(buf[i]); //echo
		i++;
		if ((char)ch == '\r')
		{
			buf[i] = 0;
			i = 0;
			if (strlen(buf) < 16) //command
			{
				if (buf[0] >= '0' && buf[0] <= '0' + DEMO_CNT - 1)
				{
					memcpy((void *)conv_data, (void *)(conv_data_demo[buf[0] - '0']), 3 * 3 * 3 * 3 * sizeof(float));
				}
				else
				{
					printf("err cmd!\r\n");
					return 0;
				}
				//for(j=0;j<81;j++){printf("%.0f,",conv_data[j]);if((j%9)==8)printf("\r\n");}
			}
			else //conv_data
			{
				sscanf(buf, "%f %f %f %f %f %f %f %f %f",
					   data + 0, data + 1, data + 2,
					   data + 3, data + 4, data + 5,
					   data + 6, data + 7, data + 8);
				memset((void *)conv_data, 0, 3 * 3 * 3 * sizeof(float)); //清空
				for (j = 0; j < 9; j++)
					conv_data[0 * 27 + 0 * 9 + j] = data[j];
				for (j = 0; j < 9; j++)
					conv_data[1 * 27 + 1 * 9 + j] = data[j];
				for (j = 0; j < 9; j++)
					conv_data[2 * 27 + 2 * 9 + j] = data[j];
			}
			return 1;
		}
	}
	return 0;
}

void irq_gpiohs2(void *gp)
{
	key_flag = 1;
	key_state = gpiohs_get_pin(GPIO_KEY);
	return;
}

int main(void)
{
	int i;
	unsigned long t0, t1;

#if 1
	/* Set CPU and dvp clk */
	sysctl_pll_set_freq(SYSCTL_PLL0, PLL0_OUTPUT_FREQ);
	sysctl_pll_set_freq(SYSCTL_PLL1, PLL1_OUTPUT_FREQ);
	sysctl_pll_set_freq(SYSCTL_PLL2, PLL2_OUTPUT_FREQ);
	sysctl_clock_enable(SYSCTL_CLOCK_AI);
	uarths_init();
	io_mux_init();
	io_set_power();
	plic_init();

	/* KEY init */
	fpioa_set_function(PIN_KEY, FUNC_GPIOHS0 + GPIO_KEY);
	gpiohs_set_drive_mode(GPIO_KEY, GPIO_DM_INPUT);
	gpiohs_set_pin_edge(GPIO_KEY, GPIO_PE_BOTH);
	gpiohs_set_irq(GPIO_KEY, 1, irq_gpiohs2);

	/* LCD init*/
	printf("LCD init\r\n");
	lcd_init();
	lcd_set_direction(DIR_YX_RLDU);
	lcd_clear(BLACK);
	lcd_draw_string(136, 70, "DEMO 1", WHITE);
	lcd_draw_string(104, 150, "KPU conv", WHITE);
	msleep(500);
	/* DVP init */
	printf("DVP init\r\n");
	dvp_init(8);
	dvp_set_xclk_rate(24000000);
	dvp_enable_burst();
	dvp_set_output_enable(0, 1); //enable to ai
	dvp_set_output_enable(1, 0); //disable to lcd
	dvp_set_image_format(DVP_CFG_RGB_FORMAT);
	dvp_set_image_size(320, 240);
	ov2640_init();
	//ov7725_init();

	dvp_set_ai_addr((uint32_t)g_ai_buf_in, (uint32_t)(g_ai_buf_in + 320 * 240), (uint32_t)(g_ai_buf_in + 320 * 240 * 2));

	//dvp_set_display_addr((uint32_t)g_lcd_gram0);
	dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
	dvp_disable_auto();

	/* DVP interrupt config */
	printf("DVP interrupt config\r\n");
	plic_set_priority(IRQN_DVP_INTERRUPT, 1);
	plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
	plic_irq_enable(IRQN_DVP_INTERRUPT);

	/* enable global interrupt */
	sysctl_enable_irq();
	/* system start */
	printf("system start\r\n");
	g_ram_mux = 0;
	g_dvp_finish_flag = 0;
	dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
	dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
#endif
	/* init kpu */
	kpu_task_t task;
	layer_conv_init(&task, 320, 240, 3, 3, conv_data);
	printf("KPU TASK INIT, FREE MEM: %ld\r\n", get_free_heap_size());
	printf("enter 3x3 kernel or enter preset kernel number:(0:origin,1:edge,2:sharp,3:relievo)\r\n");
	while (1)
	{
		while (g_dvp_finish_flag == 0)
			;
		t0 = read_cycle();
		layer_conv_run(&task, g_ai_buf_in, g_ai_buf_out, kpu_done);
		while (!g_ai_done_flag)
			;
		g_ai_done_flag = 0;
		t1 = read_cycle();
		//printf("%ld-%ld=%ld, %ld us!\r\n",t1,t0,(t1-t0),((t1-t0)*1000000/PLL0_OUTPUT_FREQ*2));
		g_dvp_finish_flag = 0;
		rgb888_to_565(g_ai_buf_out, g_ai_buf_out + 320 * 240, g_ai_buf_out + 320 * 240 * 2, (uint16_t *)g_lcd_gram, 320 * 240);
		//print_2d_u8(g_ai_buf_in+320, 16, 1);
		//print_2d_u8(g_ai_buf_out+320, 16, 1);
		//printf("---------------\r\n");
		draw_conv();
		lcd_draw_picture(0, 0, 320, 240, g_lcd_gram);
		if (parse_input()) //串口输入卷积核的值
			layer_conv_init(&task, 320, 240, 3, 3, conv_data);
		if (key_flag) //使用按键选择的卷积核
		{
			if (key_state == 0) //按下
			{
				msleep(20); //延迟去抖
				key_flag = 0;
				demo_index = (demo_index + 1) % DEMO_CNT;
				memcpy((void *)conv_data, (void *)(conv_data_demo[demo_index]), 3 * 3 * 3 * 3 * sizeof(float));
				layer_conv_init(&task, 320, 240, 3, 3, conv_data_demo[demo_index]);
			}
			else //弹起
			{
				msleep(20); //延迟去抖
				key_flag = 0;
			}
		}
	}
	return 0;
}

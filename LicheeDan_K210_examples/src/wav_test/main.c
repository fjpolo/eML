#include <stdio.h>
#include "fpioa.h"
#include "sysctl.h"
#include "dmac.h"
#include "fpioa.h"
#include "i2s.h"
#include "plic.h"
#include "wav_decode.h"
#include "uarths.h"
#include "bsp.h"

#include "wav.h"

void io_mux_init(void)
{
#if 1
    fpioa_set_function(34, FUNC_I2S0_OUT_D0);
    fpioa_set_function(35, FUNC_I2S0_SCLK);
    fpioa_set_function(33, FUNC_I2S0_WS);

    // fpioa_set_function(34, FUNC_I2S2_OUT_D1);
    // fpioa_set_function(35, FUNC_I2S2_SCLK);
    // fpioa_set_function(33, FUNC_I2S2_WS);

#else
    fpioa_set_function(33, FUNC_I2S0_OUT_D0);
    fpioa_set_function(35, FUNC_I2S0_SCLK);
    fpioa_set_function(34, FUNC_I2S0_WS);
#endif
}

int core1_func(void *ctx)
{
    printf("core1 running!\r\n");
    while (1)
        ;
}

struct wav_file_t wav_file;

static int on_irq_dma3(void *ctx)
{
    if (wav_file.buff_end)
    {
        wav_file.buff_end = 2;
        return 0;
    }
    if (wav_file.buff_index == 0)
    {
        if (wav_file.buff1_used == 0)
        {
            printf("error\r\n");
            return 0;
        }
        wav_file.buff0_used = 0;
        wav_file.buff_index = 1;
        wav_file.buff_current = wav_file.buff1;
        wav_file.buff_current_len = wav_file.buff1_read_len;
        if (wav_file.buff1_len > wav_file.buff1_read_len)
            wav_file.buff_end = 1;
    }
    else if (wav_file.buff_index == 1)
    {
        if (wav_file.buff0_used == 0)
        {
            printf("error\r\n");
            return 0;
        }
        wav_file.buff1_used = 0;
        wav_file.buff_index = 0;
        wav_file.buff_current = wav_file.buff0;
        wav_file.buff_current_len = wav_file.buff0_read_len;
        if (wav_file.buff0_len > wav_file.buff0_read_len)
            wav_file.buff_end = 1;
    }

    i2s_play(I2S_DEVICE_0,
             DMAC_CHANNEL3, (void *)wav_file.buff_current, wav_file.buff_current_len, wav_file.buff_current_len, 16, 2);

    return 0;
}

static int wav_test(uint8_t *wav_data, uint32_t wav_data_len)
{
    enum errorcode_e status;

    printf("/*******************wav test*******************/\r\n");

    wav_file.wave_file = wav_data;
    wav_file.wave_file_len = wav_data_len;
    status = wav_init(&wav_file);

    printf("result:%d\r\n", status);
    printf("numchannels:%d\r\n", wav_file.numchannels);
    printf("samplerate:%d\r\n", wav_file.samplerate);
    printf("byterate:%d\r\n", wav_file.byterate);
    printf("blockalign:%d\r\n", wav_file.blockalign);
    printf("bitspersample:%d\r\n", wav_file.bitspersample);
    printf("datasize:%d\r\n", wav_file.datasize);

    dmac_set_irq(DMAC_CHANNEL3, on_irq_dma3, NULL, 1);

    i2s_init(I2S_DEVICE_0, I2S_TRANSMITTER, 0x03);

    i2s_tx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0,
                          RESOLUTION_16_BIT, SCLK_CYCLES_32,
                          /*TRIGGER_LEVEL_1*/ TRIGGER_LEVEL_4,
                          RIGHT_JUSTIFYING_MODE);

    i2s_set_sample_rate(I2S_DEVICE_0, wav_file.samplerate);

    printf("start decode\r\n");
    status = wav_decode_init(&wav_file);
    if (OK != status)
    {
        printf("decode init fail\r\n");
        return -1;
    }

    i2s_play(I2S_DEVICE_0, DMAC_CHANNEL3, (void *)wav_file.buff_current, wav_file.buff_current_len, wav_file.buff_current_len, 16, 2);

    printf("start paly\r\n");
    while (1)
    {
        if (wav_file.buff_end)
        {
            printf("decode finish 1\r\n");
            break;
        }
        status = wav_decode(&wav_file);
        printf("status:%d\r\n", status);
        if (FILE_END == status)
        {
            while (wav_file.buff_end != 2)
                ;
            printf("decode finish\r\n");
            break;
        }
        else if (FILE_FAIL == status)
        {
            printf("decode fail\r\n");
            break;
        }
        else if (OK == status)
        {
            printf("emmm\r\n");
            break;
        }
    }
    wav_decode_finish(&wav_file);
    return 0;
}

int main(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    uarths_init();
    register_core1(core1_func, NULL);

    io_mux_init();
    dmac_init();
    plic_init();
    sysctl_enable_irq();

    while (1)
    {
        if (wav_test(data, sizeof(data) - 4))
        {
            printf("Play music err\r\n");
            return -1;
        }
    }

    return 0;
}

#ifndef __PICOJPEG_UTIL_H__
#define __PICOJPEG_UTIL_H__

#include <stdint.h>

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t comps;
    uint8_t scan_type;
    uint8_t *img_data;
} jpeg_image_t;

void jpeg_display(uint16_t startx, uint16_t starty, jpeg_image_t *jpeg);
jpeg_image_t *pico_jpeg_decode(uint8_t *buf, uint32_t buf_len);

#endif

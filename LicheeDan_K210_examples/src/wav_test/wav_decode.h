#ifndef _WAV_DECODE_H
#define _WAV_DECODE_H

/* .WAV file format :

 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             // 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             // 0x57415645

Next, the fmt chunk describes the sample format:

  big         12          4 bytes     'fmt '          // 0x666D7420
  little      16          4 bytes     0x00000010      // Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001          // Format tag: 1 = PCM
  little      22          2 bytes     <channels>      // Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>   // Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>  // sample rate * block align
  little      32          2 bytes     <block align>   // channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>   // 8 or 16
(option)
Finally, the data chunk contains the sample data:

  big         36          4 bytes     'data'        // 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>

*/

#include "stdint.h"

/* Audio file information structure */
struct wav_file_t
{
	uint8_t *wave_file;
	uint32_t wave_file_len;
	uint32_t wave_file_curpos;
	uint8_t *buff0;
	uint8_t *buff1;
	uint8_t *buff_current;
	uint32_t buff0_len;
	uint32_t buff1_len;
	uint32_t buff0_read_len;
	uint32_t buff1_read_len;
	uint32_t buff_current_len;
	uint16_t buff0_used : 1;
	uint16_t buff1_used : 1;
	uint16_t buff_index : 12;
	uint16_t buff_end : 2;
	uint16_t numchannels;
	uint32_t samplerate;
	uint32_t byterate;
	uint16_t blockalign;
	uint16_t bitspersample;
	uint32_t datasize;
};

/* Error Identification structure */
enum errorcode_e
{
	OK = 0,						   //0
	FILE_END,					   //1
	FILE_FAIL,					   //2
	UNVALID_RIFF_ID,			   //3
	UNVALID_RIFF_SIZE,			   //4
	UNVALID_WAVE_ID,			   //5
	UNVALID_FMT_ID,				   //6
	UNVALID_FMT_SIZE,			   //7
	UNSUPPORETD_FORMATTAG,		   //8
	UNSUPPORETD_NUMBER_OF_CHANNEL, //9
	UNSUPPORETD_SAMPLE_RATE,	   //10
	UNSUPPORETD_BITS_PER_SAMPLE,   //11
	UNVALID_LIST_SIZE,			   //12
	UNVALID_DATA_ID,			   //13
};

enum errorcode_e wav_init(struct wav_file_t *wav_file);
enum errorcode_e wav_decode_init(struct wav_file_t *wav_file);
enum errorcode_e wav_decode(struct wav_file_t *wav_file);
enum errorcode_e wav_decode_finish(struct wav_file_t *wav_file);

#endif /* _WAV_DECODE_H */

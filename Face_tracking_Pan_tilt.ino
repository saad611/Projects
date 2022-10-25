#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include "soc/soc.h"      
#include "soc/rtc_cntl_reg.h"
#include "fb_gfx.h"
#include "fd_forward.h"


const char* ssid = "";
const char* password = "";

#define FACE_COLOR_GREEN  0x0000FF00

typedef enum
{
  START_STREAM,
  STOP_STREAM,
  TOGGLE_MOTORS_1_2,
  ACTION_MOTORS_2,
  AUTO_MODE_ON,
  AUTO_MODE_OFF,
} en_fsm_state;
en_fsm_state g_state;

boolean auto_mode = false;

const uint8_t index_html_gz[] = {0x1f,0x8b,0x08,0x08,0xc3,0x62,0x99,0x61,0x00,0xff,0x69,0x6e,0x64,0x65,0x78,0x2e,0x68,0x74,0x6d,0x6c,0x2e,0x67,0x7a,0x00,0xb5,0x59,0xff,0x93,0xda,0xb8,0x15,0xff,0xf9,0xf8,0x2b,0x74,0x64,0x7a,0xb0,0x0d,0x18,0xb2,0x9b,0x5c,0x13,0x0e,0xb6,0x93,0x5c,0x32,0x73,0x99,0xb9,0xcc,0x65,0xba,0x69,0x93,0x4c,0xa7,0xb3,0x23,0x6c,0x19,0x94,0xd8,0x96,0x2b,0xcb,0x0b,0xb4,0xd9,0xff,0xfd,0x3e,0xfa,0x62,0x5b,0xf6,0x02,0xcb,0xde,0xb4,0xfc,0x00,0xb6,0xf4,0x3e,0xef,0xbb,0x9e,0x9e,0xc4,0x7c,0xad,0xd2,0xe4,0xb2,0x37,0x5f,0x33,0x1a,0xe1,0x27,0x65,0x8a,0x92,0x8c,0xa6,0x6c,0x31,0xb8,0xe1,0x6c,0x93,0x0b,0xa9,0x06,0xa4,0xfa,0x84,0x22,0x53,0x2c,0x53,0x8b,0xc1,0x86,0x47,0x6a,0xbd,0x88,0xd8,0x0d,0x0f,0xd9,0xd8,0xbc,0x8c,0x6a,0x22,0x9e,0x71,0xc5,0x69,0x32,0x2e,0x42,0x9a,0xb0,0xc5,0x93,0x60,0xda,0x4c,0x95,0x05,0x93,0x66,0x9c,0x2e,0x31,0x95,0x89,0x01,0x99,0x40,0xa4,0xe2,0x2a,0x61,0x97,0xef,0x69,0x46,0x68,0x16,0x91,0x0f,0x3c,0x51,0xe4,0xcd,0xd5,0xfb,0x8b,0xf3,0xf1,0xcf,0x2f,0xdf,0xcd,0x27,0x76,0xb6,0x37,0x2f,0xd4,0x0e,0xbf,0xa4,0xf7,0x48,0x2b,0x41,0x79,0xc6,0x24,0xf9,0x6f,0xcf,0x70,0x35,0xf2,0x67,0xe4,0xc9,0x74,0xfa,0xa7,0x9f,0xec,0xc8,0x9a,0xf1,0xd5,0x5a,0xcd,0xc8,0xd3,0x17,0x37,0x6b,0x37,0xb4,0xa4,0xe1,0xd7,0x95,0x14,0x65,0x16,0x8d,0x43,0x91,0x08,0x39,0x23,0x8f,0x2e,0x2e,0x2e,0xdc,0x64,0xc4,0x8b,0x3c,0xa1,0xbb,0x19,0x89,0x13,0xb6,0x75,0x63,0x34,0xe1,0xab,0x6c,0xcc,0x15,0x4b,0x8b,0x19,0x09,0x61,0x35,0x93,0x6e,0xe6,0x4b,0x59,0x28,0x1e,0xef,0xc0,0xc7,0x78,0xa3,0x33,0x2b,0x6e,0x98,0x8c,0x13,0xb1,0x99,0x91,0x35,0x8f,0x22,0x96,0x55,0xf2,0x85,0x8c,0x60,0xbc,0xa4,0x11,0x2f,0xc1,0xf0,0x2f,0x79,0x25,0x47,0x89,0x32,0x5c,0x8f,0x69,0xa8,0xb8,0xc8,0x66,0x24,0x13,0x19,0xfb,0xa9,0x77,0xdb,0x23,0x8f,0xb4,0xe4,0xbb,0x16,0xd6,0xb0,0xca,0x44,0x7f,0xec,0xae,0x8d,0x72,0xb5,0x1c,0x9e,0x3f,0x7d,0x36,0x22,0xe7,0x17,0x08,0xc3,0x8b,0x17,0x67,0x2d,0x6d,0x34,0x3a,0xdf,0x92,0x42,0x24,0x3c,0xd2,0xa4,0x74,0xf8,0xe4,0xe2,0xc7,0x11,0x69,0xbe,0x82,0x67,0x6d,0x40,0xad,0xfe,0xb3,0xda,0xd5,0xfb,0xd4,0xf7,0x62,0xcd,0x12,0x16,0x42,0xcb,0xb6,0x59,0x33,0x4d,0x7e,0xc3,0x2a,0xeb,0xf6,0xaa,0x0d,0x5d,0x7e,0x7c,0x0e,0xbd,0x9f,0xe8,0xaf,0x73,0x28,0x8f,0x44,0x9a,0x42,0x9b,0x9a,0xc7,0x5a,0x3b,0xba,0x62,0x11,0x96,0xb2,0xd0,0xc0,0x5c,0x70,0x2f,0x14,0x4e,0x69,0xe7,0xbd,0x73,0xe3,0x28,0x8d,0xa7,0x92,0xd1,0x0a,0x99,0x8b,0x02,0xe9,0xaa,0x75,0x8f,0xf9,0x96,0x45,0x0e,0x29,0xad,0x73,0xa7,0xb5,0x91,0xb9,0x79,0xb9,0xed,0xf5,0x62,0xbe,0x2a,0x25,0x73,0xe0,0x3d,0x79,0x96,0xd3,0x28,0xe2,0xd9,0x6a,0x56,0x41,0x53,0x2a,0x57,0x3c,0xab,0x5f,0xc7,0x1b,0xb6,0xfc,0xca,0xd5,0xd8,0x0e,0x8f,0x97,0x2c,0x16,0x92,0x75,0x88,0xc7,0xcb,0x44,0x84,0x5f,0xc7,0x85,0xa2,0x52,0x1d,0x02,0xd2,0x18,0x66,0xee,0xc7,0xb1,0x2c,0x3a,0x84,0x6a,0xb3,0x74,0x83,0x3c,0x4b,0xb0,0x9a,0x8e,0x8b,0xf3,0x79,0xb6,0x61,0x66,0x86,0xc0,0x33,0xd6,0x31,0x84,0xa7,0x2b,0xe7,0x9c,0x6a,0x51,0x19,0xad,0x1c,0xd6,0x86,0x82,0x96,0x4a,0xb4,0xd3,0xd8,0x5f,0xa8,0xad,0x4c,0x7b,0x5a,0x27,0xb7,0x13,0xab,0x43,0xf1,0x1c,0x49,0xab,0x63,0x31,0x9f,0xd8,0xa2,0x80,0x07,0x57,0xbd,0x96,0x22,0xda,0xe1,0x87,0x67,0x79,0xa9,0x88,0xda,0xe5,0xa8,0x62,0x8a,0x6d,0x61,0x19,0xa3,0x03,0xc2,0xa3,0xc5,0xc0,0x3e,0x41,0x35,0x5d,0x81,0x22,0x53,0x7e,0x22,0x7e,0x63,0xa6,0x44,0x09,0x97,0xfe,0x5c,0x15,0x97,0xc1,0x65,0xcf,0xc8,0x9d,0x3b,0xbb,0x8c,0xa4,0x45,0xdf,0xd3,0xb7,0x7f,0x69,0x15,0xd3,0x44,0x8e,0x47,0xbf,0x50,0xe0,0x9f,0x9a,0xca,0x60,0xb8,0xf4,0x49,0x98,0xd0,0xa2,0x58,0xf4,0x79,0x4a,0x57,0xcc,0x1b,0xbf,0x24,0x73,0x78,0xca,0xc3,0xf4,0x49,0x21,0xc3,0x45,0x5f,0x4f,0x4c,0xc0,0xcd,0xf2,0x9e,0x4f,0xac,0x74,0xfb,0xe6,0x14,0xaa,0x64,0xb9,0xf2,0x33,0x36,0xb9,0xda,0x55,0xc6,0x89,0x5d,0x96,0x4a,0x89,0xac,0xc0,0x6c,0x3d,0x0d,0x02,0x3b,0xea,0x84,0x23,0xea,0x88,0xbd,0x55,0xc1,0xa1,0x12,0x16,0x83,0xe1,0x95,0x9e,0x22,0x57,0x66,0x6a,0x3e,0xb1,0x98,0x46,0x4a,0x97,0x8d,0xc8,0x0f,0x71,0x11,0xf9,0x41,0x26,0x07,0xb8,0x29,0xb1,0x5a,0x25,0x6c,0x9c,0x0a,0x25,0x64,0xd1,0xe1,0xf7,0xd6,0xec,0x2e,0x8a,0x11,0x3b,0x7b,0x32,0x4f,0x9d,0x73,0xe0,0x18,0xb1,0xb1,0xc8,0x6a,0x96,0xce,0x75,0x2f,0x31,0x07,0x7e,0x11,0x23,0xbf,0x65,0xf7,0x59,0xea,0xf1,0x89,0xe3,0x23,0x8c,0xe2,0xb8,0xe1,0x74,0x80,0xd5,0x3b,0x6d,0xc1,0xb5,0x2d,0x9d,0x5d,0x4e,0x66,0x8e,0xd8,0xb9,0x3b,0x2a,0xb5,0x32,0xc4,0x3c,0xea,0xe7,0x3a,0x91,0xeb,0x2c,0x43,0x0e,0xfb,0x19,0x33,0xd0,0x75,0x73,0xe0,0x27,0x58,0xfd,0x84,0x15,0x64,0xf9,0x60,0x9f,0x0d,0x25,0xcf,0xd5,0x65,0x0f,0x5c,0x0a,0x45,0x74,0x0f,0x40,0x16,0x24,0x12,0x61,0x99,0x22,0xd9,0x82,0x15,0x53,0x6f,0x12,0xa6,0x1f,0x5f,0xed,0xde,0x46,0xc3,0x81,0x8d,0xf9,0x00,0x85,0xd9,0xd2,0x7f,0xbc,0xba,0xfe,0xfb,0xdf,0x7e,0x05,0xa2,0xbf,0x29,0x66,0x93,0x49,0x9f,0x3c,0xc6,0x9a,0xcf,0x22,0xb1,0x09,0x50,0x05,0xa8,0xb6,0x26,0x58,0x0b,0xd0,0x3d,0x26,0xfd,0xd9,0xf3,0xf3,0x7e,0x05,0xdb,0x14,0x80,0x64,0x10,0xf5,0x91,0x2d,0xaf,0x50,0x2e,0x98,0x1a,0x5a,0x4e,0x60,0xec,0x48,0xac,0xa4,0x57,0xd6,0x7d,0x87,0x35,0x6a,0x67,0x74,0xad,0x97,0x4e,0xd0,0xd3,0x39,0x34,0xc9,0xdc,0xc8,0xb7,0x49,0x69,0xb3,0xee,0x18,0xba,0x9d,0xbc,0x0d,0x5e,0x27,0x8e,0x4e,0x8d,0xeb,0xe3,0xc2,0x5b,0x79,0x5a,0xab,0xdf,0x80,0xe3,0xf8,0x44,0x34,0xb2,0xb3,0x11,0x6e,0xb4,0xb1,0xd9,0x74,0x0c,0xde,0xca,0x48,0x8d,0xee,0x6d,0x8a,0x40,0x64,0x29,0x2b,0x0a,0x94,0x2f,0x20,0xeb,0xa7,0x4b,0xec,0x9f,0xdf,0xf1,0x98,0x0c,0xdd,0x48,0x10,0x51,0xf4,0x8d,0x1c,0xb2,0x68,0x16,0x32,0x11,0x93,0x57,0x89,0x58,0x9e,0x69,0x22,0x42,0x6e,0xa8,0x24,0xa5,0x4c,0x7e,0x5b,0x7e,0x41,0x2f,0x00,0x26,0x08,0x6b,0x10,0xc2,0xbb,0x8a,0xd9,0x21,0xbc,0xb7,0xd8,0x40,0xb0,0x46,0x21,0xf3,0x02,0xd4,0x44,0x00,0x6a,0x30,0x26,0x6e,0x7b,0xb7,0xd0,0x4b,0xb3,0x8c,0x24,0x5d,0xbd,0xd5,0x4d,0x92,0x67,0xd0,0xbf,0x4b,0x26,0x77,0x57,0xa6,0xe9,0x10,0x72,0x38,0x30,0x9d,0x82,0x4e,0x4e,0x4d,0xdf,0xf4,0x8d,0x47,0x00,0xcd,0xda,0xe9,0xa2,0x3e,0xea,0xad,0x0b,0xd0,0x7a,0x20,0x80,0x8b,0x0b,0xa6,0xcc,0x78,0x87,0xf6,0x17,0xb3,0x4b,0xec,0x21,0xb6,0x13,0x96,0x3a,0xa5,0xdb,0xd7,0x30,0xe1,0x17,0x21,0xf9,0x7f,0x34,0x59,0xe2,0xd3,0x5b,0x69,0x13,0x72,0xde,0xa2,0xfd,0x07,0x93,0x8a,0xa3,0x7d,0xf6,0x29,0x9d,0x2c,0x43,0x7a,0x70,0x99,0x9a,0x6d,0xef,0x2c,0xb8,0xa1,0x49,0xa9,0xc3,0x68,0xfb,0xf7,0x19,0x19,0x60,0x1d,0xde,0x55,0xe3,0x31,0xc6,0xab,0xce,0xc6,0xa3,0xa8,0x84,0x5b,0x8d,0x5c,0x0b,0xb7,0x20,0x31,0x4d,0x0a,0xf4,0x76,0xc6,0xfe,0x52,0x4a,0x88,0xfc,0xd4,0x7a,0xfb,0x6c,0xdf,0xdc,0xa9,0xc0,0xcd,0xb9,0x37,0x37,0xb7,0x45,0xb5,0x84,0x73,0xc0,0x0c,0x3d,0x86,0x1e,0xd8,0x75,0x07,0x50,0x1b,0xd5,0x07,0x6c,0xe4,0x23,0xf3,0x74,0x85,0xa6,0x63,0x44,0x0a,0x7c,0x7f,0xe0,0x29,0xc3,0xbe,0x6d,0x96,0x89,0x73,0x34,0xfa,0xaf,0x37,0x37,0x90,0xfb,0x2b,0x2f,0xb0,0x3d,0x32,0x04,0xd5,0xb4,0xa8,0xa6,0x2c,0x0c,0x46,0x26,0x6b,0xcc,0xce,0x36,0xb2,0x9a,0xdb,0x25,0x76,0x1c,0x0b,0x41,0x0e,0xf9,0x46,0x0b,0x3e,0x19,0x97,0xa2,0x45,0x75,0xc0,0xd3,0x50,0xa9,0x40,0xd7,0x8c,0x62,0x99,0x3d,0x5c,0x51,0x03,0x2d,0xf3,0x87,0xea,0x69,0x60,0x7b,0xf5,0xec,0xf9,0xe5,0x12,0x45,0x20,0x4c,0x78,0xf8,0x15,0x11,0x19,0x9e,0xd9,0xd5,0x6f,0xda,0xb9,0x22,0xd0,0x51,0xb8,0x5b,0x74,0xf5,0xa4,0x59,0xc0,0xd2,0x30,0x7f,0xa9,0x94,0xe4,0xd8,0xc4,0xd8,0x70,0x50,0xb5,0x5e,0x7a,0x7d,0xd9,0xd6,0xba,0x29,0xab,0xf7,0x53,0x7b,0x45,0xf4,0x21,0xc4,0x71,0x7c,0x3f,0xb5,0x57,0x22,0xef,0x27,0x6e,0x5c,0x7a,0x94,0x14,0x3d,0x6a,0x77,0xe3,0xb9,0xdf,0x93,0xad,0xcd,0xa7,0x76,0x24,0x96,0xc3,0x3e,0x21,0x23,0x4f,0x5e,0x47,0xb3,0x13,0x10,0x7b,0x42,0x70,0x32,0xca,0x0f,0xc5,0x1f,0x00,0x21,0x24,0x27,0xa3,0xfc,0xd0,0x9c,0x04,0xba,0xd5,0xcd,0x72,0xcb,0xac,0x7b,0xbc,0x7e,0x67,0xd3,0x46,0xe4,0x7c,0x03,0x1d,0xfc,0x00,0x5a,0xb7,0x7d,0xd7,0x8e,0xd4,0x6c,0x9b,0x7f,0xdc,0xab,0xad,0x54,0x79,0x00,0xaa,0x93,0x64,0xff,0x3f,0xd7,0xb6,0x3d,0x83,0x28,0x9e,0xec,0x1a,0xd7,0x90,0x3c,0x7c,0xd1,0xb7,0x2c,0x3b,0x81,0xba,0xe3,0x8b,0xff,0xdd,0xca,0xd7,0xb6,0xfb,0xc4,0xc7,0x4d,0xef,0x36,0x53,0x00,0xf7,0xe2,0x32,0xb3,0x4d,0x58,0x5d,0xdd,0x87,0x4c,0x77,0x49,0xe6,0xde,0x0c,0xdd,0x14,0x0b,0xf4,0xa1,0x95,0x2c,0x16,0xd8,0xa3,0xbd,0x6d,0xab,0x22,0xf1,0xae,0xd7,0x3e,0x21,0x95,0x41,0x6d,0xb6,0xa7,0xe2,0x9f,0xd3,0x7f,0x05,0x50,0x45,0xef,0xbd,0x64,0x5c,0x6d,0xa7,0xee,0xdc,0xdc,0x40,0x3e,0xef,0x87,0x7c,0x06,0xc4,0x6d,0xb8,0x16,0x72,0x4b,0x18,0xb6,0x80,0x43,0x22,0x4f,0x95,0x73,0x88,0xb9,0x6f,0x2b,0x8e,0xf4,0x7a,0x97,0x87,0xb5,0x55,0x33,0xe7,0x5b,0x5a,0xb7,0x18,0x4a,0x96,0xee,0x56,0x09,0xdd,0x5f,0xc7,0x89,0xd8,0xe9,0x3c,0x17,0x36,0x8a,0x36,0xbd,0x48,0x47,0xb3,0xa6,0x2d,0x69,0xc9,0x70,0x6d,0x4c,0x97,0x7d,0x3b,0x3c,0x96,0xda,0xd7,0x91,0x05,0xb9,0x64,0x7a,0x4b,0x7d,0xcd,0x62,0x5a,0x26,0x6a,0x58,0x5d,0x98,0x1d,0x0a,0xa8,0xd9,0x6b,0x7d,0x0e,0xee,0xfa,0xca,0x28,0x7b,0x38,0xa6,0x4d,0xfb,0xb4,0x07,0x77,0x24,0xb0,0x4d,0xa3,0x55,0x61,0xee,0x44,0xf7,0x8e,0x02,0x0f,0x92,0x7a,0x54,0x54,0xfd,0xd4,0x74,0x78,0xed,0xb8,0xe8,0x4f,0xd3,0xec,0xb5,0x43,0x53,0xb9,0xf0,0x1d,0x55,0xeb,0x80,0x2e,0x8b,0x61,0x35,0x7d,0x46,0xe6,0x77,0x1a,0xe2,0x1f,0x7e,0x20,0x5d,0xba,0x4f,0x1e,0x5d,0xd3,0xdd,0x76,0x5d,0x0f,0xc9,0x1f,0x24,0xcd,0x8a,0x04,0xc7,0x91,0x1a,0x39,0xaa,0x55,0xb1,0x5d,0x91,0xc9,0xcc,0x7d,0x76,0x9d,0xdc,0x70,0x7f,0xb2,0xad,0x74,0xed,0x67,0xdd,0x63,0x7f,0x6e,0x8d,0x39,0xab,0x4d,0x86,0x4f,0x26,0x24,0xe1,0x29,0xc7,0xb1,0x15,0x95,0x04,0xb7,0x89,0x28,0x98,0x04,0x37,0xa8,0xf5,0xf1,0x0b,0x19,0x27,0x77,0xe4,0x62,0x4a,0xd2,0x42,0xd3,0xae,0x95,0xca,0xf5,0x69,0x7b,0xc5,0xd5,0xba,0x5c,0x06,0xa1,0x48,0x27,0x19,0x13,0x19,0x47,0x6f,0x37,0xc1,0x4d,0xf4,0x97,0xe2,0x9a,0x15,0xf9,0xc5,0xf9,0x35,0xdb,0xd2,0x34,0x4f,0x58,0x31,0xc1,0x95,0xdc,0x72,0x92,0xa2,0x93,0x66,0xb2,0xa6,0xc4,0xde,0xc5,0x26,0x21,0x4b,0x92,0x7c,0x8d,0xa7,0x6b,0xdd,0x47,0x48,0x91,0xa0,0xf2,0x5d,0xcb,0xf0,0x3a,0xa4,0x72,0xb2,0xd9,0x6c,0x26,0x38,0xca,0xb3,0x6d,0xa0,0xff,0x31,0x68,0x96,0x89,0xa9,0x76,0x6a,0xab,0x2a,0xcf,0xea,0x7e,0x3d,0x13,0xfa,0xd6,0x40,0x1f,0xe8,0x5f,0x6b,0xbf,0x9e,0x69,0xff,0xe8,0x5e,0xbd,0x5a,0x1e,0x3c,0x1e,0x56,0x8d,0xbc,0x59,0x1b,0xb8,0xfb,0x65,0x31,0xba,0x96,0x88,0x7c,0xfb,0x66,0xc0,0xe3,0xba,0xd1,0x27,0x97,0x0b,0x58,0xea,0x87,0x4d,0xc1,0xf6,0x76,0x10,0xab,0x9a,0xab,0xb5,0x68,0xe7,0x6a,0x23,0xe5,0x88,0x36,0xd5,0x47,0x32,0x55,0xca,0xea,0xc2,0xde,0x2c,0x15,0x5c,0x59,0x84,0xeb,0xa6,0x06,0x78,0xff,0x83,0xe0,0xd2,0x9c,0xe1,0x4e,0x43,0x17,0x88,0x6e,0x62,0xd8,0xef,0xea,0xc8,0xa2,0xeb,0xd7,0xd6,0x15,0x3f,0x98,0xfd,0xbd,0x77,0x6c,0xf1,0xd9,0x9a,0x53,0x9e,0xbe,0x58,0xa8,0x35,0xfe,0xbe,0xe5,0x97,0xbf,0xea,0x70,0x8f,0xc9,0xb0,0xed,0x9c,0x33,0x32,0xc3,0xb8,0xbf,0x5e,0x86,0x60,0x32,0x27,0x53,0xdb,0x0c,0x56,0x1f,0xc3,0xd8,0x23,0xf3,0x74,0xc0,0x84,0x5e,0x02,0xf6,0x65,0xe8,0x6f,0x64,0xfb,0x89,0xb3,0x32,0xc1,0x09,0xb0,0x3b,0x6f,0xa2,0xa9,0xad,0xf5,0x9d,0x31,0x82,0x5c,0xf7,0xde,0xae,0xdb,0xad,0x35,0xb7,0x7d,0x2f,0x8a,0x11,0xd9,0x99,0x6f,0x56,0x2f,0x4f,0x96,0x04,0xe6,0xc6,0x35,0x50,0x9a,0x10,0x97,0xe4,0xfa,0x98,0x3f,0x30,0x2f,0x1a,0x75,0x81,0x25,0x86,0xa5,0xa3,0xb1,0x7a,0x29,0xe5,0xdb,0x91,0x59,0x4a,0x9a,0x4b,0xf5,0x3e,0x3d,0x1b,0xb8,0xf6,0x19,0x9e,0xcd,0x69,0xf6,0x9a,0xad,0x24,0x63,0xda,0x11,0x06,0xf5,0x67,0xf2,0x62,0x8a,0x43,0xf3,0x9d,0xfa,0xd0,0x60,0x14,0xfe,0x88,0x6a,0x40,0x86,0x75,0x1b,0xd4,0x1c,0x88,0xbf,0x33,0x3e,0xf0,0x84,0x40,0x87,0x91,0x56,0xc8,0xe3,0x61,0x3a,0x00,0xdc,0x58,0xbb,0xeb,0x35,0x5c,0xea,0xd9,0xbb,0xea,0x89,0xf9,0xff,0xed,0x77,0xff,0xc3,0x21,0x5b,0x86,0x1b,0x00,0x00 
};
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
 
camera_fb_t * fb = NULL;

 dl_matrix3du_t *image_matrix = NULL;
 
using namespace websockets;
WebsocketsServer WSserver;
AsyncWebServer webserver(80);

static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config_t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}

mtmn_config_t mtmn_config = app_mtmn_config();
int pan_center = 90; // center the pan servo
int tilt_center = 90; // center the tilt servo
 
// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 180) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);
  ledcWrite(channel, duty);
}


static void draw_face_boxes(dl_matrix3du_t *image_matrix, box_array_t *boxes)
{
  int x, y, w, h, i, half_width, half_height;
  uint32_t color = FACE_COLOR_GREEN;
  fb_data_t fb;
  fb.width = image_matrix->w;
  fb.height = image_matrix->h;
  fb.data = image_matrix->item;
  fb.bytes_per_pixel = 3;
  fb.format = FB_BGR888;
  for (i = 0; i < boxes->len; i++) {
 
    // Convoluted way of finding face centre...
    x = ((int)boxes->box[i].box_p[0]);
    w = (int)boxes->box[i].box_p[2] - x + 1;
    half_width = w / 2;
    int face_center_pan = x + half_width; // image frame face centre x co-ordinate
 
    y = (int)boxes->box[i].box_p[1];
    h = (int)boxes->box[i].box_p[3] - y + 1;
    half_height = h / 2;
    int face_center_tilt = y + half_height;  // image frame face centre y co-ordinate
 
    //    assume QVGA 320x240
    //    int sensor_width = 320;
    //    int sensor_height = 240;
    //    int lens_fov = 45
    //    float diagonal = sqrt(sq(sensor_width) + sq(sensor_height)); // pixels along the diagonal
    //    float pixels_per_degree = diagonal / lens_fov;
    //    400/45 = 8.89
    if(g_state==AUTO_MODE_ON)
    {
      float move_to_x = pan_center + ((-160 + face_center_pan) / 8.89) ;
      pan_center = (pan_center + move_to_x) / 2;
      Serial.println(pan_center);
      ledcAnalogWrite(2, pan_center); // channel, 0-180
      
    }

    else 
    {
      float move_to_x = pan_center + ((-160 + face_center_pan) / 8.89) ;
      float move_to_y = tilt_center + ((-120 + face_center_tilt) / 8.89) ;
   
      pan_center = (pan_center + move_to_x) / 2;
      Serial.println(pan_center);
      ledcAnalogWrite(2, pan_center); // channel, 0-180
   
      tilt_center = (tilt_center + move_to_y) / 2;
      int reversed_tilt_center = map(tilt_center, 0, 180, 180, 0);
      ledcAnalogWrite(4, reversed_tilt_center); // channel, 0-180
    //}

    fb_gfx_drawFastHLine(&fb, x, y, w, color);
    fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
    fb_gfx_drawFastVLine(&fb, x, y, h, color);
    fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
 
  }
}}
 
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);

  //DC motors
  pinMode(14,OUTPUT);   

  //micro motor
  pinMode(15,OUTPUT); 

  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
  // Ai-Thinker: pins 2 and 12
  ledcSetup(2, 50, 16); //channel, freq, resolution
  ledcAttachPin(2, 2); // pin, channel
  ledcSetup(4, 50, 16);
  ledcAttachPin(12, 4);
   
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
   
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
 
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, sizeof(index_html_gz));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
 
  webserver.begin();
  WSserver.listen(82);
}
 
 
void handle_message(WebsocketsMessage msg) {
  
  Serial.println(msg.data());

  if(msg.data()=="start-stream")
  {
    g_state= START_STREAM;
  }

  else if(msg.data()=="stop-stream")
  {
    g_state= STOP_STREAM;
  }

   else if(msg.data()=="toggle-motors")
  {
    //g_state= STOP_STREAM;
    digitalWrite(14, HIGH);
    digitalWrite(15, HIGH);
    ledcAnalogWrite(2, 90); // channel, 0-180
    ledcAnalogWrite(4, 90); // channel, 0-180
  }

  else if(msg.data()=="Auto_mode_on")
  {
    auto_mode = true;
  }

  else if(msg.data()=="Auto_mode_off")
  {
    auto_mode = false;
  }
  
  else if(msg.data()=="Motor_action")
  {
    int i =0;
    while(1)
    {
      i++;
      delay(1000);
      if(i<=1)
      {
        digitalWrite(15, HIGH);
        i=0;  
      }
    }      
  }
  
  
  int commaIndex = msg.data().indexOf(',');
  int panValue = msg.data().substring(0, commaIndex).toInt();
  int tiltValue = msg.data().substring(commaIndex + 1).toInt();
 
  panValue = map(panValue, -90, 90, 0, 180); // 0-180
  tiltValue = map(tiltValue, -90, 90, 180, 0); // 0-180 reversed
 
  ledcAnalogWrite(2, panValue); // channel, value
  ledcAnalogWrite(4, tiltValue);
}

uint32_t timeMotor_14;
uint32_t timeMotor_15;

boolean motor_14_state = false;
boolean motor_15_state = false;

 
void loop() {
  auto client = WSserver.accept();
  client.onMessage(handle_message);

 
  
  while (client.available()) {
    client.poll();

    if(g_state==START_STREAM)
    {
       fb = esp_camera_fb_get();

      image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
         
      fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
 
      box_array_t *net_boxes = NULL;
      net_boxes = face_detect(image_matrix, &mtmn_config);
      
      
      if(auto_mode==true)
      {
          if ((net_boxes)&&(false==motor_14_state)&&(false==motor_15_state))
          {
            draw_face_boxes(image_matrix, net_boxes);
            free(net_boxes->score);
            free(net_boxes->box);
            free(net_boxes->landmark);
            free(net_boxes);
  
            //Capture time for enabling motor 14
            timeMotor_14 = millis();
  
            //Set state of motor 14 to true to switch it on
            motor_14_state = true;
  
            //Capture time for enabling motor 15
            timeMotor_15 = millis();
  
            //Set state of motor 15 to true to switch it on
            motor_15_state = true;
         }

           //if face detection enables motor_14...
          if( motor_14_state == true )
          {
              //switch on the motors for 1 second
              if( (millis() - timeMotor_14) <= 1000ul )
              {
                  //if yes, turn on the motor
                  digitalWrite(14, HIGH);
              }//if
      
              else 
              {
                 //turn off the motor
                  digitalWrite(14, LOW);
                  //and indiacte the motor is now off
                  motor_14_state = false;
              }
              
          }//if

             //if face detection enables motor_15...
            if( motor_15_state == true )
            {
                //switch on the motors for 5 seconds
                if( (millis() - timeMotor_15) <= 5000ul )
                {
                    //if yes, turn on the motor
                    digitalWrite(15, HIGH);
                }//if
        
                else 
                {
                    //turn off the motor
                    digitalWrite(15, LOW);
                    //and indiacte the motor is now off
                    motor_15_state = false;
                }
                
            }//if
     }
      else 
      {
        //do not process face traking
      }

//     //if face detection enables motor_14...
//    if( motor_14_state == true )
//    {
//        //switch on the motors for 1 second
//        if( (millis() - timeMotor_14) <= 1000ul )
//        {
//            //if yes, turn on the motor
//            digitalWrite(14, HIGH);
//        }//if
//
//        else 
//        {
//           //turn off the motor
//            digitalWrite(14, LOW);
//            //and indiacte the motor is now off
//            motor_14_state = false;
//        }
//        
//    }//if
//
//     //if face detection enables motor_15...
//    if( motor_15_state == true )
//    {
//        //switch on the motors for 5 seconds
//        if( (millis() - timeMotor_15) <= 5000ul )
//        {
//            //if yes, turn on the motor
//            digitalWrite(15, HIGH);
//        }//if
//
//        else 
//        {
//           //turn off the motor
//            digitalWrite(15, LOW);
//            //and indiacte the motor is now off
//            motor_15_state = false;
//        }
//        
//    }//if

      client.sendBinary((const char *)fb->buf, fb->len);
      esp_camera_fb_return(fb);
      fb = NULL;
      dl_matrix3du_free(image_matrix);
      }
  }
}

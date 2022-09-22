#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "hidsdi.h"

#include "divaio.h"
#include "../utils/config.h"
#include "../utils/hid_impl.h"

#define INPUT_REPORT_ID           0x01
#define REPORTID_LIGHT_OUTPUT_1   0x04
#define REPORTID_LIGHT_OUTPUT_2   0x05
#define REPORTID_LIGHT_OUTPUT_3   0x06
#define REPORTID_LIGHT_COMPRESSED 0x0B

//#define DEBUG 0 //let meson handle it

#define VID 0x0f0d
#define PID 0x00fb //not used anyways

#if DEBUG == 1
FILE *logfile;
#define VERBOSE_DEBUG(...) do { fprintf(logfile, __VA_ARGS__); fflush(logfile); } while (0)
#else
#define VERBOSE_DEBUG(...) ;
#endif

static unsigned int __stdcall diva_io_slider_thread_proc(void *ctx);

static bool diva_io_coin;
static uint16_t diva_io_coins;
static HANDLE diva_io_slider_thread;
static bool diva_io_slider_stop_flag;
static struct diva_io_config diva_io_cfg;

static HANDLE g_device_handle;
static HANDLE g_lights_handle;

// bitmasks for uint16_t g_controller_data.buttons {BUTTONDOWN,BUTTONL3,BUTTONR3,BUTTONCAPTURE,
#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08

#define TRIANGLE 0x08 //X
#define SQUARE 0x01 //Y
#define CROSS 0x02 //B
#define CIRCLE 0x04 //A  
#define START   0x200
#define SERVICE 0x100
#define TEST	0x1000

#pragma pack(1)
typedef struct joy_report_s {
	uint8_t  report_id;
	uint16_t buttons; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;	// HAT switch; one nibble w/ unused nibble
	uint32_t axis;
	uint8_t  VendorSpec;
} joy_report_t;

joy_report_t g_controller_data;

uint16_t __cdecl diva_io_get_api_version(void)
{
	return 0x0100;
}

bool g_avail[4] = {false};
uint8_t g_brg[101] = {0};
uint8_t g_compressed_buffer[128] = {0};

static HANDLE diva_io_led_thread;
static bool diva_io_led_stop_flag;

static unsigned int __stdcall diva_io_led_thread_proc(void *ctx)
{
	while (!diva_io_led_stop_flag) {
		controller_write_leds(g_brg);
	}
	
	return 0;
}

static int controller_read_buttons(uint8_t *pressure){
	/* read hid report then convert axis data to pressure sensor bytes */
	(void) hid_get_report(g_device_handle, (uint8_t *)&g_controller_data, INPUT_REPORT_ID, sizeof(joy_report_t));

	#if DEBUG == 1
	fprintf(logfile, "raw controller data :\r\n");
	uint8_t *asbyte = (uint8_t*)&g_controller_data;
for (int i = 0; i < 9; i++)
{
 fprintf(logfile, "%02x ", asbyte[i]);
}
fprintf(logfile, "\r\n");	
fprintf(logfile, "received input report with buttons %04x and axis %08x\n", g_controller_data.buttons, g_controller_data.axis);	
fflush(logfile);
	#endif
	/* expand pressure tab */
	uint32_t slider = g_controller_data.axis ^ ((uint32_t)0x80808080); //xor with center stick value for each of the 4 axis;
	for (int i=0; i<32; i++)
	{
		if ((slider >> i)&1) pressure[31-i] = 200;
	}
	
	return 0;
}

static int controller_write_leds(const uint8_t *lamp_bits){
	if (g_lights_handle == NULL)
		return -1;
			
	static uint8_t but_led[4]; /* button leds */
	
	if (diva_io_cfg.hid_offset == 0)
	{
		/* no HID offset in conf, fallback to reactive mode */
		but_led[0] = g_controller_data.buttons & TRIANGLE ? 0x00:0xFF;
		but_led[1] = g_controller_data.buttons & SQUARE   ? 0x00:0xFF;
		but_led[2] = g_controller_data.buttons & CROSS    ? 0x00:0xFF;
		but_led[3] = g_controller_data.buttons & CIRCLE   ? 0x00:0xFF;
	}
	else
	{
		/* using HID values from offset in conf (e.g. diva.exe+EDA460) */
		uint8_t button_leds = *((uint8_t *) diva_io_cfg.hid_offset);
		//printf("buttons: %02x\n\n",button_leds);
		but_led[0] = button_leds&0x08 ? 0xFF: 0x00; //Triangle
		but_led[1] = button_leds&0x40 ? 0xFF: 0x00; //Square
		but_led[2] = button_leds&0x10 ? 0xFF: 0x00; //Cross
		but_led[3] = button_leds&0x80 ? 0xFF: 0x00; //Circle
	}
	
	hid_set_report(g_lights_handle, but_led, REPORTID_LIGHT_OUTPUT_3, 4);
	
	/* slider lights */
	if (g_avail[3])
	{
		hid_set_feature_report_raw(g_lights_handle, g_compressed_buffer, REPORTID_LIGHT_COMPRESSED, 64);
		g_avail[3] = false;
	}
	else
	{
		/* fallback to partial updates */
		if (g_avail[0])
		{
			hid_set_report_raw(g_lights_handle, g_brg, REPORTID_LIGHT_OUTPUT_1, 49); //first 48 leds are on report 1
			g_avail[0] = false;
		}
		if (g_avail[1])
		{
			hid_set_report_raw(g_lights_handle, g_brg+49, REPORTID_LIGHT_OUTPUT_2, 49); //last 48 on another one
			g_avail[1] = false;
		}
	}
	
	return 0;
	
}

HRESULT __cdecl diva_io_jvs_init(void)
{
	#if DEBUG == 1
logfile = fopen("divaiolog.txt", "w");
#endif
VERBOSE_DEBUG("CRM diva jvs init\n");

	diva_io_config_load(&diva_io_cfg, L".\\segatools.ini");
	
	/* open slider controller interface (no interface for Hori official, interface 0 for RedBoard) */
	if ( hid_open_device(&g_device_handle, VID, PID, 0) != 0 
	  && hid_open_device(&g_device_handle, VID, PID, -1) != 0) /* hori official controller has only one interface */
	{
		VERBOSE_DEBUG("USB Device not found\n");
		return -1;		
	}
	
	/* try to open the HID lights interface (will fail for Hori official, no biggie) */
	if ( hid_open_device(&g_lights_handle, VID, PID, 1) != 0 )
	{
		g_lights_handle = NULL;		
	}

	g_brg[0] = REPORTID_LIGHT_OUTPUT_1;
	g_brg[49] = REPORTID_LIGHT_OUTPUT_2;
	g_compressed_buffer[0] = REPORTID_LIGHT_COMPRESSED;

	int hidres = HidD_SetNumInputBuffers(g_device_handle, 2);
	if (!hidres)
	{
VERBOSE_DEBUG("ERROR SETNUMINPUTBUFFERS\n");
		printf("Error %lu setnuminputbuff\r\n",GetLastError());
		return -1;
	}
	
VERBOSE_DEBUG("Init Ok\n");
	return S_OK;
}

void __cdecl diva_io_jvs_read_coin_counter(uint16_t *out)
{
	if (out == NULL) {
		return;
	}

	if (GetAsyncKeyState(diva_io_cfg.vk_coin) & 0x8000) {
		if (!diva_io_coin) {
			diva_io_coin = true;
			diva_io_coins++;
		}
	} else {
		diva_io_coin = false;
	}

	*out = diva_io_coins;
}

void __cdecl diva_io_jvs_poll(uint8_t *opbtn, uint8_t *gamebtn)
{
	size_t i;
	
	*opbtn = 0;
	if (GetAsyncKeyState(diva_io_cfg.vk_test) & 0x8000) {
		*opbtn |= 0x01;
	}

	if (GetAsyncKeyState(diva_io_cfg.vk_service) & 0x8000) {
		*opbtn |= 0x02;
	}
	
	*gamebtn = 0;
	for (i = 0 ; i < _countof(diva_io_cfg.vk_buttons) ; i++) {
		if (GetAsyncKeyState(diva_io_cfg.vk_buttons[i]) & 0x8000) {
			*gamebtn |= 1 << i;
		}
	}
	*opbtn |= ((!!(g_controller_data.buttons & SERVICE))<<1 | !!(g_controller_data.buttons & TEST))&0x03;
	//swap 0x01 and 0x04
	*gamebtn |= ((!!(g_controller_data.buttons & START))<<4) | (g_controller_data.buttons & 0x0A | (g_controller_data.buttons&0x01)<<2 | !!(g_controller_data.buttons&0x04));
}

HRESULT __cdecl diva_io_slider_init(void)
{
	return S_OK;
}

void __cdecl diva_io_slider_start(diva_io_slider_callback_t callback)
{
	if (diva_io_slider_thread == NULL) {
		diva_io_slider_thread = (HANDLE) _beginthreadex(
		NULL,
		0,
		diva_io_slider_thread_proc,
		callback,
		0,
		NULL);
	}

	if (g_lights_handle == NULL)
		return; //hori pad doesn't have hid leds
	
	if (diva_io_led_thread == NULL) {
		diva_io_led_thread = (HANDLE) _beginthreadex(
		NULL,
		0,
		diva_io_led_thread_proc,
		NULL,
		0,
		NULL);
	}
}

void __cdecl diva_io_slider_stop(void)
{
	if (diva_io_slider_thread != NULL) {
		diva_io_slider_stop_flag = true;
		WaitForSingleObject(diva_io_slider_thread, INFINITE);
		CloseHandle(diva_io_slider_thread);
		diva_io_slider_thread = NULL;
		diva_io_slider_stop_flag = false;
	}
	
	if (diva_io_led_thread != NULL) {
		diva_io_led_stop_flag = true;
		WaitForSingleObject(diva_io_led_thread, INFINITE);
		CloseHandle(diva_io_led_thread);
		diva_io_led_thread = NULL;
		diva_io_led_stop_flag = false;
	}
}

void __cdecl diva_io_slider_set_leds(const uint8_t *brg)
{
	if (g_lights_handle == NULL)
		return;
	
	/* do not actually send leds as it will cause diva/pdloader to reset and reinit slider... 
	 * we're taking care of sending data in another thread.
	 */
	if (g_avail[3])
	{
		/* compressed transfer pending */
		return;
	}
		
	unsigned int size = 63;
	int ret = lzfx_compress(brg, 96, g_compressed_buffer+2, &size);
	if (ret == 0)
	{
		VERBOSE_DEBUG("using compression (packet size %d)\n", size);
		g_compressed_buffer[1] = size;
		/* signal compressed transfer */
		g_avail[3] = true;
		return;
	}
	
	VERBOSE_DEBUG("transfer too large for compressed update\n");
	/* Fallback to partial updates when not compressible enough */
	/* hid lights are in reverse order in redboard */
	static uint8_t rev_bits[96];
	
	for (uint8_t i = 0; i<32; i++)
	{
		rev_bits[3*i]   = brg[3*(31-i)];
		rev_bits[3*i+1] = brg[3*(31-i)+1];
		rev_bits[3*i+2] = brg[3*(31-i)+2];
	}
	if (!g_avail[0] && memcmp(g_brg+1, rev_bits, 48) != 0)
	{
		memcpy(g_brg+1, rev_bits, 48);
		g_avail[0] = true;
	}
	if (!g_avail[1] && memcmp(g_brg+50, rev_bits+48, 48) != 0)
	{
		memcpy(g_brg+50, rev_bits+48, 48);
		g_avail[1] = true;
	}
}

static unsigned int __stdcall diva_io_slider_thread_proc(void *ctx)
{
	diva_io_slider_callback_t callback;
	uint8_t pressure[32];
	size_t i;

	callback = ctx;

	while (!diva_io_slider_stop_flag) {
		uint8_t pressure_val = 0;
		for (i = 0 ; i < 8 ; i++) {
			if (GetAsyncKeyState(diva_io_cfg.vk_slider[i]) & 0x8000) {
				pressure_val = 20;
			} else {
				pressure_val = 0;
			}

			memset(&pressure[4 * i], pressure_val, 4);
		}
		controller_read_buttons(pressure);
		callback(pressure);
	}

	return 0;
}

#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "hidsdi.h"

#include "chuniio.h"
#include "../utils/config.h"
#include "../utils/hid_impl.h"

#define INPUT_REPORT_ID           0x01
#define REPORTID_LIGHT_OUTPUT_1   0x04
#define REPORTID_LIGHT_OUTPUT_2   0x05
#define REPORTID_LIGHT_OUTPUT_3   0x06
#define REPORTID_LIGHT_COMPRESSED 0x0B

#define WITH_COMPRESSION 1
//#define DEBUG 1 //let meson handle it
//#define SHMEM 1 //let meson handle it

#if FUFUBOT == 1
#pragma message( "building for fufubot fork")
#endif

#if SHMEM == 1
#pragma message( "SHMEM is ON, building for chusan")
//#error test
#else
#pragma message( "SHMEM is OFF, building for chuni")
//#error test
#endif

#if DEBUG == 1 || DEBUG_FUFU == 1
FILE *logfile;
#define VERBOSE_DEBUG(...) do { fprintf(logfile, __VA_ARGS__); fflush(logfile); } while (0)
#else
#define VERBOSE_DEBUG(...) ;
#endif

#if FUFUBOT == 1
bool g_real_led = false;
#endif

uint8_t g_brg_active[18] = {0};
uint8_t g_brg_inactive[18] = {0};
uint8_t* g_brg_current = g_brg_inactive;

#if SHMEM == 1
#define BUF_SIZE 1024

#define SHMEM_WRITE(buf, size) CopyMemory((PVOID)g_pBuf, buf, size)
#define SHMEM_READ(buf, size) CopyMemory(buf,(PVOID)g_pBuf, size)

TCHAR g_shmem_name[]=TEXT("Local\\ChuniShmem");
HANDLE g_hMapFile;
LPVOID g_pBuf;

bool shmem_create()
{
   g_hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 g_shmem_name);                 // name of mapping object

   if (g_hMapFile == NULL)
   {
      VERBOSE_DEBUG("shmem_create : Could not create file mapping object (%d).\n",
             GetLastError());
      return 0;
   }
   g_pBuf = MapViewOfFile(g_hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        BUF_SIZE);

   if (g_pBuf == NULL)
   {
      VERBOSE_DEBUG("shmem_create : Could not map view of file (%d).\n",
             GetLastError());

       CloseHandle(g_hMapFile);

      return 0;
   }

	return 1;
}

bool shmem_load()
{
   g_hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   g_shmem_name);               // name of mapping object

   if (g_hMapFile == NULL)
   {
      VERBOSE_DEBUG("shmem_load : Could not open file mapping object (%d).\n",
             GetLastError());
      return 0;
   }

   g_pBuf = MapViewOfFile(g_hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);

   if (g_pBuf == NULL)
   {
      VERBOSE_DEBUG("shmem_load : Could not map view of file (%d).\n",
             GetLastError());

      CloseHandle(g_hMapFile);

      return 0;
   }

   return 1;
}

void shmem_free()
{
   UnmapViewOfFile(g_pBuf);
   CloseHandle(g_hMapFile);
}

#endif

static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx);

static bool chuni_io_coin;
static uint16_t chuni_io_coins;
static uint8_t chuni_io_hand_pos;
static HANDLE chuni_io_slider_thread;
static bool chuni_io_slider_stop_flag;
static struct chuni_io_config chuni_io_cfg;

static HANDLE g_device_handle;
static HANDLE g_lights_handle;
static HANDLE g_billboard_handle;

// bitmasks for uint16_t g_controller_data.buttons
#define TOWER_1 0x08
#define TOWER_2 0x01
#define TOWER_3 0x02
#define TOWER_4 0x04
#define TOWER_5 0x10
#define TOWER_6 0x20
#define START   0x200
#define SERVICE 0x100
#define TEST    0x1000

#pragma pack(1)
typedef struct joy_report_s {
	uint8_t  report_id;
	uint16_t buttons; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint32_t axis;
	uint8_t  VendorSpec;
} joy_report_t;

joy_report_t g_controller_data;

	/* TODO: take care of extra inputs for redboard (down l3 r3 capture), discard capture as this is the card scan button */
	/* TODO: double check whether to set square and cross to the first 2 tower for better divanithm handling? */
	/* TODO: write divaio and add an option to make lider edges act as dpad left/right */
uint16_t __cdecl chuni_io_get_api_version(void)
{
    return 0x0101;
}

bool g_avail[4] = {false};
bool g_billboard_avail[11] = {false};
uint8_t g_brg[98] = {0};
uint8_t g_compressed_buffer[128] = {0};
static HANDLE chuni_io_led_thread;
static bool chuni_io_led_stop_flag;
static HANDLE chuni_io_billboard_thread;
static bool chuni_io_billboard_stop_flag;

uint8_t g_billboard[11][10] = {0};

static unsigned int __stdcall chuni_io_led_thread_proc(void *ctx)
{
	while (!chuni_io_led_stop_flag) {
		controller_write_leds(g_brg);
	}
	
	return 0;
}

static unsigned int __stdcall chuni_io_billboard_thread_proc(void *ctx)
{
	while (!chuni_io_billboard_stop_flag) {
		controller_write_billboard_leds(g_billboard);
	}
	return 0;
}

static int controller_read_buttons(uint8_t *pressure){
	
	#if SHMEM != 1
	/* read hid report then convert axis data to pressure sensor bytes */
	(void) hid_get_report(g_device_handle, (uint8_t *)&g_controller_data, INPUT_REPORT_ID, sizeof(joy_report_t));
	#else
	#ifdef _WIN64
	/* in x64 we just read from shmem */
	SHMEM_READ(&g_controller_data, sizeof(joy_report_t));
	#else
	/* read hid report then convert axis data to pressure sensor bytes */
	(void) hid_get_report(g_device_handle, (uint8_t *)&g_controller_data, INPUT_REPORT_ID, sizeof(joy_report_t));
	SHMEM_WRITE(&g_controller_data, sizeof(joy_report_t));
	
	/* now is time to check if beams should be active for new */
	static uint8_t* prev_brg = g_brg_inactive;
	if ( (g_controller_data.buttons & 0x3F) != 0) g_brg_current = g_brg_active;
	else g_brg_current = g_brg_inactive;
	
	if (prev_brg != g_brg_current)
	{
		prev_brg = g_brg_current;
		g_avail[2] = true;
	}
	#endif
	#endif

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
		if ((slider >> i)&1) pressure[i] = 200;
	}
	return 0;
}

static int controller_write_leds(const uint8_t *lamp_bits){
	#if SHMEM == 1
	/* x86 only */
	#ifdef _WIN64
	return 0;
	#endif
	#endif
	
	if (g_lights_handle == NULL)
		return -1;

	static uint8_t cooldown = 0;
	
	
	if (g_avail[3])
	{
		hid_set_feature_report_raw(g_lights_handle, g_compressed_buffer, REPORTID_LIGHT_COMPRESSED, 64);
		cooldown = 0;
		g_avail[3] = false;
	}
	else
	{
		/* fallback to partial updates */
		if (g_avail[0])
		{
			hid_set_report_raw(g_lights_handle, lamp_bits, REPORTID_LIGHT_OUTPUT_1, 49); //first 48 leds are on report 1
			cooldown = 0;
			g_avail[0] = false;
		}
		if (g_avail[1])
		{
			hid_set_report_raw(g_lights_handle, lamp_bits+49, REPORTID_LIGHT_OUTPUT_2, 49); //last 45 on another one
			cooldown = 0;
			g_avail[1] = false;
		}
	}
	if (g_avail[2] || cooldown > 180)
	{
		hid_set_report(g_lights_handle, g_brg_current, REPORTID_LIGHT_OUTPUT_3, 18); //tower leds from conf
		cooldown = 0;
		g_avail[2] = false;
	}
	
	cooldown++;
	
	return 0;
}

static int controller_write_billboard_leds(const uint8_t **billboard){
	#if SHMEM == 1
	/* x86 only */
	#ifdef _WIN64
	return 0;
	#endif
	#endif
	
	if (g_billboard_handle == NULL)
		return -1;

	for (int i = 0; i<11; i++)
	{
		if ( g_billboard_avail[i] )
		{
			hid_set_report_raw(g_billboard_handle, billboard[i], 1+i, 31); //10 logical RGB leds per report, report id 1-11
			cooldown = 0;
			g_billboard_avail[i] = false;
		}
	}
	
	return 0;
}


#define VID 0x0f0d
#define PID 0x0092
#define VID_BILLBOARD 0x0f0d
#define PID_BILLBOARD 0x1248
HRESULT __cdecl chuni_io_jvs_init(void)
{
	#if DEBUG == 1
logfile = fopen("chuniiolog.txt", "w");
#endif
VERBOSE_DEBUG("CRM chuni jvs init\n");

    chuni_io_config_load(&chuni_io_cfg, L".\\segatools.ini");

	g_real_led = chuni_io_cfg.real_led;
#if DEBUG_FUFU == 1
VERBOSE_DEBUG("real led = %d\n", g_real_led);
#endif		
	/* parse rgb colors */
	uint8_t red, green, blue;
	swscanf( chuni_io_cfg.tower_color_active, L"#%02hhx%02hhx%02hhx", &red, &green, &blue );
	for (int i=0; i<6; i++)
	{
		g_brg_active[3*i+0] = blue;
		g_brg_active[3*i+1] = red;
		g_brg_active[3*i+2] = green;	
	}
	
	swscanf( chuni_io_cfg.tower_color_inactive, L"#%02hhx%02hhx%02hhx", &red, &green, &blue );
	for (int i=0; i<6; i++)
	{
		g_brg_inactive[3*i+0] = blue;
		g_brg_inactive[3*i+1] = red;
		g_brg_inactive[3*i+2] = green;	
	}
#if SHMEM == 1

#ifdef _WIN64
/* chusan in x64 must just open the shmem and do nothing else */
int errcount = 0;
while (!shmem_load())
{
	if (errcount >= 10)
		return -1;
	Sleep(5000);
	errcount++;
}
	Sleep(1000);
	return S_OK;
#endif
/* chusan in x86 has to open the device and create the shmem */
#endif

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

	/* try to open the HID billboard interface */
	if ( hid_open_device(&g_billboard_handle, VID_BILLBOARD, PID_BILLBOARD, 1) != 0 )
	{
        g_billboard_handle = NULL;		
	}

	g_brg[0] = REPORTID_LIGHT_OUTPUT_1;
	g_brg[49] = REPORTID_LIGHT_OUTPUT_2;
	g_compressed_buffer[0] = REPORTID_LIGHT_COMPRESSED;

	for (int i=0; i<11; i++)
	{
			g_billboard[i][0] = 1+i; //init billboard report IDs
	}
	
	hid_set_report(g_lights_handle, g_brg_inactive, REPORTID_LIGHT_OUTPUT_3, 18); //light tower led	

	int hidres = HidD_SetNumInputBuffers(g_device_handle, 2);
    if (!hidres)
    {
        printf("Error %lu setnuminputbuff\r\n",GetLastError());
        return -1;
    }

#if SHMEM == 1
	if (!shmem_create())
	{
		return -1;
	}
#endif
    return S_OK;
}

void __cdecl chuni_io_jvs_read_coin_counter(uint16_t *out)
{
	#if SHMEM == 1
	#ifndef _WIN64
	return;
	#endif
	#endif
    if (out == NULL) {
        return;
    }

    if (GetAsyncKeyState(chuni_io_cfg.vk_coin)) {
        if (!chuni_io_coin) {
            chuni_io_coin = true;
            chuni_io_coins++;
        }
    } else {
        chuni_io_coin = false;
    }

    *out = chuni_io_coins;
}

void __cdecl chuni_io_jvs_poll(uint8_t *opbtn, uint8_t *beams)
{
	#if SHMEM == 1
	#ifndef _WIN64
	/* chusan x64 only */
	return;
	#else
	/* make sure to retrieve up to date g_controller_data */
	SHMEM_READ(&g_controller_data, sizeof(joy_report_t));
	#endif
	#endif
    size_t i;
	
	*opbtn = 0;
    if (GetAsyncKeyState(chuni_io_cfg.vk_test) & 0x8000) {
        *opbtn |= 0x01; 
    }

    if (GetAsyncKeyState(chuni_io_cfg.vk_service) & 0x8000) {
        *opbtn |= 0x02;
    }
	
	
#define START   0x200
#define SERVICE 0x100
#define TEST    0x1000

	*opbtn |= ((!!(g_controller_data.buttons & (SERVICE|TEST)))<<1 | !!(g_controller_data.buttons & (START)))&0x03;
	
    if (GetAsyncKeyState(chuni_io_cfg.vk_ir)) {
        if (chuni_io_hand_pos < 6) {
            chuni_io_hand_pos++;
        }
    } else {
        if (chuni_io_hand_pos > 0) {
            chuni_io_hand_pos--;
        }
    }

	*beams = 0;
    for (i = 0 ; i < 6 ; i++) {
        if (chuni_io_hand_pos > i) {
            *beams |= (1 << i);
        }
    }
/*
button report order is (MSB first) :	
	6 5  1 4 3 2
we convert to :
	6 5  4 3 2 1
*/	
	*beams |= ((g_controller_data.buttons & 0x30) | ((g_controller_data.buttons & 0x07) << 1) | (g_controller_data.buttons & 0x08) >> 3);

#if SHMEM == 0	
	if ( !g_real_led )
	{
		static uint8_t *prev_brg = g_brg_inactive;
		
		if (*beams != 0) g_brg_current = g_brg_active;
		else g_brg_current = g_brg_inactive;
		
		if (prev_brg != g_brg_current)
		{
			g_avail[2] = true;
		}
	
		prev_brg = g_brg_current;
	}
#endif
}

HRESULT __cdecl chuni_io_slider_init(void)
{
    return S_OK;
}

void __cdecl chuni_io_slider_start(chuni_io_slider_callback_t callback)
{
	#if SHMEM == 1
	#ifdef _WIN64
	return;
	#endif
	#endif
	
    if (chuni_io_slider_thread == NULL) {
		
		chuni_io_slider_thread = (HANDLE) _beginthreadex(
            NULL,
            0,
            chuni_io_slider_thread_proc,
            (void *) callback,
            0,
            NULL);
    }
			
	if (g_lights_handle == NULL)
		return; //hori pad doesn't have hid leds
	
	if (chuni_io_led_thread == NULL) {
		chuni_io_led_thread = (HANDLE) _beginthreadex(
		NULL,
		0,
		chuni_io_led_thread_proc,
		NULL,
		0,
		NULL);
	}

	if (chuni_io_billboard_thread == NULL) {
		chuni_io_billboard_thread = (HANDLE) _beginthreadex(
		NULL,
		0,
		chuni_io_billboard_thread_proc,
		NULL,
		0,
		NULL);
	}
}

void __cdecl chuni_io_slider_stop(void)
{
	#if SHMEM == 1
	#ifdef _WIN64
	return;
	#endif
	#endif
	
	if (chuni_io_slider_thread != NULL) {
		chuni_io_slider_stop_flag = true;
		WaitForSingleObject(chuni_io_slider_thread, INFINITE);
		CloseHandle(chuni_io_slider_thread);
		chuni_io_slider_thread = NULL;
		chuni_io_slider_stop_flag = false;
	}
	
	if (chuni_io_led_thread != NULL) {
		chuni_io_led_stop_flag = true;
		WaitForSingleObject(chuni_io_led_thread, INFINITE);
		CloseHandle(chuni_io_led_thread);
		chuni_io_led_thread = NULL;
		chuni_io_led_stop_flag = false;
	}
	
	if (chuni_io_billboard_thread != NULL) {
		chuni_io_billboard_stop_flag = true;
		WaitForSingleObject(chuni_io_billboard_thread, INFINITE);
		CloseHandle(chuni_io_billboard_thread);
		chuni_io_billboard_thread = NULL;
		chuni_io_billboard_stop_flag = false;
	}
}

void __cdecl chuni_io_slider_set_leds(const uint8_t *brg)
{
	#if SHMEM == 1
	/* x86 only */
	#ifdef _WIN64
	return;
	#endif
	#endif
		#if DEBUG == 1
fprintf(logfile, "CRM chuni io slider set leds\n");
fflush(logfile);
	#endif

	if (g_lights_handle == NULL)
		return;

	/* do not actually send leds to speed up input loop... 
	 * we're taking care of sending data in another thread.
	 */
	if (g_avail[3]){
		/* compressed transfer pending */
		return;
	}

	#if WITH_COMPRESSION == 1	
	unsigned int size = 63;
	int ret = lzfx_compress(brg, 93, g_compressed_buffer+2, &size);
	if (ret == 0)
	{
		VERBOSE_DEBUG("using compression (packet size %d)\n", size);
		g_compressed_buffer[1] = size;
		/* signal compressed transfer */
		g_avail[3] = true;
		return;
	}
	
	VERBOSE_DEBUG("transfer too large for compressed update\n");
	#endif
	/* Fallback to partial updates when not compressible enough */
	if (!g_avail[0] && memcmp(g_brg+1, brg, 48) != 0)
	{
		memcpy(g_brg+1, brg, 48);
		g_avail[0] = true;
	}
	if (!g_avail[1] && memcmp(g_brg+50, brg+48, 45) != 0)
	{
		memcpy(g_brg+50, brg+48, 45);
		g_avail[1] = true;
	}
	
}

static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx)
{
    chuni_io_slider_callback_t callback;
    uint8_t pressure[32];
    size_t i;

    callback = ctx;

    while (!chuni_io_slider_stop_flag) {
        for (i = 0 ; i < 32; i++) {
            if (GetAsyncKeyState(chuni_io_cfg.vk_cell[i]) & 0x8000) {
                pressure[i] = 128;
            } else {
                pressure[i] = 0;
            }
        }
		controller_read_buttons(pressure);
		
        callback(pressure);
        //Sleep(1);
    }

    return 0;
}

#if FUFUBOT == 1
int __cdecl chuni_io_led_init()
{
	fprintf(stderr, "CALLED chuni_io_led_init\n");
	return 0;
}

void __cdecl chuni_io_led_set_colors(uint8_t side, uint8_t *data)
{
	if ( !g_real_led )
		return;

	uint8_t blue;
	uint8_t red;
	uint8_t green;

	#if DEBUG_FUFU == 1
fprintf(logfile, "CALLED chuni_io_led_set_colors %d 0x%p\n", side, data);

for (int i = 0x90; i < 0xA0; i++)
{
 fprintf(logfile, "%02x ", data[i]);
}
fprintf(logfile, "\r\n");	
fflush(logfile);
	#endif
	if (side == 0)
	{
//		memcpy(g_brg_current, data+0x96, 9);
		red   = data[0x96];
		green = data[0x97];
		blue  = data[0x98];
		
		if ( g_brg_current[0] != blue || g_brg_current[1] != red || g_brg_current[2] != green )
			g_avail[2] = true;
		
		for (int i=0; i<3; i++)
		{
			g_brg_current[3*i+0] = blue;
			g_brg_current[3*i+1] = red;
			g_brg_current[3*i+2] = green;
		}
	}
	else if (side == 1)
	{
		//memcpy(g_brg_current+9, data+0xb4, 9);
		red = data[0xb4];
		green = data[0xb5];
		blue = data[0xb6];
		
		if ( g_brg_current[0] != blue || g_brg_current[1] != red || g_brg_current[2] != green )
			g_avail[2] = true;
		
		for (int i=3; i<6; i++)
		{
			g_brg_current[3*i+0] = blue;
			g_brg_current[3*i+1] = red;
			g_brg_current[3*i+2] = green;
		}
	}
	
	//billboard leds
		for (int i=0; i<(side+5); i++)
		{
			memcpy( &(g_billboard[6*side+i][1]), data+30*i, 30);
			g_billboard_avail[6*side+i] = true;
		}
}
#endif
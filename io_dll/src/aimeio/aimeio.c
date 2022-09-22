#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aimeio.h"

#define REPORTID_LIGHT_OUTPUT_AIME   0x0A
#define REPORTID_INPUT_OLD	  0x01
#define REPORTID_INPUT_FELICA   0x02
#define ITF_NUM_LIGHT		   0x01
#define ITF_NUM_CARDIO		  0x02

//#define DEBUG 0

#if DEBUG == 1
FILE *logfile;
#define VERBOSE_DEBUG(...) do { fprintf(logfile, __VA_ARGS__); fflush(logfile); } while (0)
#else
#define VERBOSE_DEBUG(...) ;
#endif

struct aime_io_config {
	wchar_t aime_path[MAX_PATH];
	wchar_t felica_path[MAX_PATH];
	bool felica_gen;
	uint8_t vk_scan;
};

static struct aime_io_config aime_io_cfg;
static uint8_t aime_io_aime_id[10];
static uint8_t aime_io_felica_id[8];
static bool aime_io_aime_id_present;
static bool aime_io_felica_id_present;

static HANDLE g_device_handle;
static HANDLE g_lights_handle;

uint8_t g_brg[3] = {255, 255, 255};

static int aime_read_uid(uint8_t *uid){
	
	(void) hid_get_report(g_device_handle, uid, REPORTID_INPUT_OLD, 9);
	if (uid[0] != 0 || uid[1] != 0)
	{
		/* found card */
		return uid[0];
	}
	
	(void) hid_get_report(g_device_handle, uid, REPORTID_INPUT_FELICA, 9);
	if (uid[0] != 0 || uid[1] != 0)
	{
		/* found card */
		return uid[0];
	}
	
	/* found nothing */
	return 0;
}

static int aime_write_led(const uint8_t *brg){
	if (g_lights_handle == NULL)
		return -1;
	
	hid_set_report(g_lights_handle, brg, REPORTID_LIGHT_OUTPUT_AIME, 3);
	
	return 0;
}


static void aime_io_config_read(
		struct aime_io_config *cfg,
		const wchar_t *filename);

static HRESULT aime_io_read_id_file(
		const wchar_t *path,
		uint8_t *bytes,
		size_t nbytes);

static HRESULT aime_io_generate_felica(
		const wchar_t *path,
		uint8_t *bytes,
		size_t nbytes);

static void aime_io_config_read(
		struct aime_io_config *cfg,
		const wchar_t *filename)
{
	VERBOSE_DEBUG("aime_io_config_read\n");
	assert(cfg != NULL);
	assert(filename != NULL);

	GetPrivateProfileStringW(
			L"aime",
			L"aimePath",
			L"DEVICE\\aime.txt",
			cfg->aime_path,
			_countof(cfg->aime_path),
			filename);

	GetPrivateProfileStringW(
			L"aime",
			L"felicaPath",
			L"DEVICE\\felica.txt",
			cfg->felica_path,
			_countof(cfg->felica_path),
			filename);

	cfg->felica_gen = GetPrivateProfileIntW(
			L"aime",
			L"felicaGen",
			1,
			filename);

	cfg->vk_scan = GetPrivateProfileIntW(
			L"aime",
			L"scan",
			VK_RETURN,
			filename);
}

static HRESULT aime_io_read_id_file(
		const wchar_t *path,
		uint8_t *bytes,
		size_t nbytes)
{
	VERBOSE_DEBUG("aime_io_read_id_file\n");
	HRESULT hr;
	FILE *f;
	size_t i;
	int byte;
	int r;

	f = _wfopen(path, L"r");

	if (f == NULL) {
		return S_FALSE;
	}

	memset(bytes, 0, nbytes);

	for (i = 0 ; i < nbytes ; i++) {
		r = fscanf(f, "%02x ", &byte);

		if (r != 1) {
			hr = E_FAIL;
			#if DEBUG == 1
printf("AimeIO DLL: %S: fscanf[%i] failed: %i\n",path,
					(int) i,
					r);
#endif
					

			goto end;
		}

		bytes[i] = byte;
	}

	hr = S_OK;

end:
	if (f != NULL) {
		fclose(f);
	}

	return hr;
}

static HRESULT aime_io_generate_felica(
		const wchar_t *path,
		uint8_t *bytes,
		size_t nbytes)
{
	VERBOSE_DEBUG("aime_io_generate_felica\n");
	size_t i;
	FILE *f;

	assert(path != NULL);
	assert(bytes != NULL);
	assert(nbytes > 0);

	srand(time(NULL));

	for (i = 0 ; i < nbytes ; i++) {
		bytes[i] = rand();
	}

	/* FeliCa IDm values should have a 0 in their high nibble. I think. */
	bytes[0] &= 0x0F;

	f = _wfopen(path, L"w");

	if (f == NULL) {
		#if DEBUG == 1
printf("AimeIO DLL: %S: fopen failed: %i\n", path, (int) errno);
#endif

		return E_FAIL;
	}

	for (i = 0 ; i < nbytes ; i++) {
		fprintf(f, "%02X", bytes[i]);
	}

	fprintf(f, "\n");
	fclose(f);

	#if DEBUG == 1
printf("AimeIO DLL: Generated random FeliCa ID\n");
#endif

	return S_OK;
}

static HANDLE aime_io_poll_thread;
static bool aime_io_poll_stop_flag;

#pragma pack(1)
typedef struct aime_report_s {
	uint8_t  report_id; //1 for old, 2 for felica, 0 to rearm poll
	uint8_t  uid[8];
} aime_report_t;

aime_report_t g_aime_report = {0};

static unsigned int __stdcall aime_io_poll_thread_proc(void *ctx)
{
	while (!aime_io_poll_stop_flag) {
		if (g_aime_report.report_id != 0)
			continue;
		
		(void) hid_get_report(g_device_handle, (uint8_t *) &g_aime_report, REPORTID_INPUT_OLD, 9);
	}
	
	return 0;
}

#if WITH_AIME_LED_THREAD != 0
static HANDLE aime_io_led_thread;
static bool aime_io_led_stop_flag;
static unsigned int __stdcall aime_io_led_thread_proc(void *ctx)
{
	while (!aime_io_led_stop_flag) {
		//printf("write led %02x %02x %02x to handle\n", g_brg[1], g_brg[2], g_brg[0]);
		aime_write_led(g_brg);
		Sleep(100);
	}
	
	return 0;
}

#endif

uint16_t __cdecl aime_io_get_api_version(void)
{
	return 0x0100;
}

HRESULT aime_io_init(void)
{
	#if DEBUG == 1
	#ifdef _WIN64
logfile = fopen("aimeiolog64.txt", "w");
	#else
logfile = fopen("aimeiolog.txt", "w");
	#endif
#endif
	VERBOSE_DEBUG("aime_io_init\n");
	aime_io_config_read(&aime_io_cfg, L".\\segatools.ini");

	VERBOSE_DEBUG("config read, opening USB device\n");   
	if ( hid_open_device(&g_device_handle, 0x0f0d, 0x00fb, ITF_NUM_CARDIO) != 0 )
	{
		VERBOSE_DEBUG("USB Device not found\n");
		return -1;		
	}

	VERBOSE_DEBUG("Cardio found\n");
	/* try to open the HID lights interface (will fail for Hori official, no biggie) */
	if ( hid_open_device(&g_lights_handle, 0x0f0d, 0x00fb, ITF_NUM_LIGHT) != 0 )
	{
		VERBOSE_DEBUG("no lights handle\n");
		g_lights_handle = NULL;		
	}
	
	/* start poll thread */
	if (aime_io_poll_thread != NULL) {
		VERBOSE_DEBUG("poll thread already init\n");
		return S_OK;
	}

	aime_io_poll_thread = (HANDLE) _beginthreadex(
			NULL,
			0,
			aime_io_poll_thread_proc,
			NULL,
			0,
			NULL);
			
#if WITH_AIME_LED_THREAD != 0
	if (aime_io_led_thread != NULL) {
		VERBOSE_DEBUG("led thread already init\n");
		return S_OK;
	}

	aime_io_led_thread = (HANDLE) _beginthreadex(
			NULL,
			0,
			aime_io_led_thread_proc,
			NULL,
			0,
			NULL);
#endif
	VERBOSE_DEBUG("init ok\n");
	return S_OK;
}

void aime_io_fini(void)
{
	VERBOSE_DEBUG("aime_io_fini\n");
	if (aime_io_poll_thread == NULL) {
		return;
	}

	aime_io_poll_stop_flag = true;

	WaitForSingleObject(aime_io_poll_thread, INFINITE);
	CloseHandle(aime_io_poll_thread);
	aime_io_poll_thread = NULL;
	aime_io_poll_stop_flag = false;
}

HRESULT aime_io_nfc_poll(uint8_t unit_no)
{
	VERBOSE_DEBUG("aime_io_nfc_poll\n");
	bool sense;
	HRESULT hr;
	
	if (unit_no != 0) {
		return S_OK;
	}

	/* Don't do anything more if the scan key is not held */
	sense = GetAsyncKeyState(aime_io_cfg.vk_scan) & 0x8000;

	if (!sense) {
		aime_io_aime_id_present = false;
		aime_io_felica_id_present = false;
		return S_OK;
	}

	/* Try AiMe IC */

	hr = aime_io_read_id_file(
			aime_io_cfg.aime_path,
			aime_io_aime_id,
			sizeof(aime_io_aime_id));

	if (SUCCEEDED(hr) && hr != S_FALSE) {
		aime_io_aime_id_present = true;

		return S_OK;
	}

	/* Try FeliCa IC */

	hr = aime_io_read_id_file(
			aime_io_cfg.felica_path,
			aime_io_felica_id,
			sizeof(aime_io_felica_id));

	if (SUCCEEDED(hr) && hr != S_FALSE) {
		aime_io_felica_id_present = true;

		return S_OK;
	}

	/* Try generating FeliCa IC (if enabled) */

	if (aime_io_cfg.felica_gen) {
		hr = aime_io_generate_felica(
				aime_io_cfg.felica_path,
				aime_io_felica_id,
				sizeof(aime_io_felica_id));

		if (FAILED(hr)) {
			return hr;
		}

		aime_io_felica_id_present = true;
	}

	return S_OK;
}

HRESULT aime_io_nfc_get_aime_id(
		uint8_t unit_no,
		uint8_t *luid,
		size_t luid_size)
{
	assert(luid != NULL);
	assert(luid_size == sizeof(aime_io_aime_id));

	VERBOSE_DEBUG("aime_io_nfc_get_aime_id\n");
	if(unit_no!=0)
		return S_FALSE;

	if(aime_io_aime_id_present) 
	{
		VERBOSE_DEBUG("Found OLD present from conf\n");
		memcpy(luid, aime_io_aime_id, luid_size);
		return S_OK;
	}

	if (g_aime_report.report_id == 1)
	{
		VERBOSE_DEBUG("Found OLD present from NFC\n");
		uint64_t val = 0;

		for (int i = 4; i < 8; i++) {
			val = (val << 8) | g_aime_report.uid[i];
		}
		VERBOSE_DEBUG("uid %02x %02x %02x %02x -> %lld\nluid: ", g_aime_report.uid[4], g_aime_report.uid[5], g_aime_report.uid[6], g_aime_report.uid[7], val);
		
		for (int i = 0; i<10; i++)
		{
			luid[9-i] = val%10;
			val /= 10;
		}

		printf("\n");
		g_aime_report.report_id = 0; // rearm poll in thread
		return S_OK;
	}

	return S_FALSE;
}

HRESULT aime_io_nfc_get_felica_id(uint8_t unit_no, uint64_t *IDm)
{
	uint64_t val;
	size_t i;

	assert(IDm != NULL);
	VERBOSE_DEBUG("aime_io_nfc_get_felica_id\n");

	if (unit_no != 0)
		return S_FALSE;

	if(aime_io_felica_id_present) 
	{
		VERBOSE_DEBUG("Found FeliCa present from conf\n");
		val = 0;

		for (i = 0; i < 8; i++) {
			val = (val << 8) | aime_io_felica_id[i];
		}

		*IDm = val;
		return S_OK;
	}

	unsigned char ID[10];
	if (g_aime_report.report_id == 2)
	{
		VERBOSE_DEBUG("Found FeliCa present from NFC\n");
		val = 0;

		for (i = 0; i < 8; i++) {
			val = (val << 8) | g_aime_report.uid[i];
		}

		*IDm = val;
		g_aime_report.report_id = 0; // rearm poll in thread
		return S_OK;
	}

	return S_FALSE;
}

void aime_io_led_set_color(uint8_t unit_no, uint8_t r, uint8_t g, uint8_t b)
{
	VERBOSE_DEBUG("aime_io_led_set_color\n");
	/* aime_io functions are not called every cycle so we save the color and use aime_io_led_thread_proc to cheat reactive fallback */
	g_brg[0] = b;
	g_brg[1] = r;
	g_brg[2] = g;
#if WITH_AIME_LED_THREAD == 0
	aime_write_led(g_brg); //otherwise let aime_io_led_thread_proc handle it
#endif
}

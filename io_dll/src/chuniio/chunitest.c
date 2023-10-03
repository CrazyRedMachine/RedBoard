#include <windows.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "chuniio.h"

#define PRESSURE_THRESHOLD 40

static void reactive_led(uint8_t *rgb_data, const uint8_t *state)
{
	for (int i=0; i<31; i++)
	{
		if (i&1)
		{
			rgb_data[3*i] = 0x7F;
			rgb_data[3*i+1] = 0x23;
			rgb_data[3*i+2] = 0x00;
		}
		else if (state[i] > PRESSURE_THRESHOLD)
		{
			rgb_data[3*i] = 0x23;
			rgb_data[3*i+1] = 0x00;
			rgb_data[3*i+2] = 0x7F;
		}
		else if (state[i+1] > PRESSURE_THRESHOLD)
		{
			rgb_data[3*i] = 0;
			rgb_data[3*i+1] = 0x7f;
			rgb_data[3*i+2] = 0x23;
		}
		else
		{
			rgb_data[3*i] = 0;
			rgb_data[3*i+1] = 0;
			rgb_data[3*i+2] = 0;
		}
	}
}

static void status_print(const uint8_t *state)
{
	static uint8_t rgb_data[96];
	static uint8_t prev[32];
	static uint8_t prev_opbtn, prev_beams;
	uint8_t opbtn, beams;
	
	if ( memcmp(state, prev, 32) != 0 )
	{
		//system("cls");
		printf("Init OK. Running input loop. Press Ctrl+C to exit.\r\n");
		printf("slider value changed! pressure array:\r\n");	
		for (int i=0; i<32; i++)
		{
			printf("%02x ", state[i]);
		}
		printf("\r\n");
	
		memcpy(prev, state, 32);
	}
	
	reactive_led(rgb_data, state);
	chuni_io_slider_set_leds(rgb_data);
	
	chuni_io_jvs_poll(&opbtn, &beams);
	{
		if (opbtn != prev_opbtn)
		{
			printf("opbtn changed!\r\n%02x\r\n",opbtn);
			prev_opbtn = opbtn;
		}
		if (beams != prev_beams)
		{
			printf("beams changed!\r\n%02x\r\n",beams);
			prev_beams = beams;
		}
	}
}

int main()
{
	printf("CHUNITEST (for fufubot)\r\n---------\r\n");
	printf("api version = %04x\r\n", chuni_io_get_api_version()); /* not compatible with older dlls */
	printf("chuni_io_jvs_init() : ");
	if (chuni_io_jvs_init() == -1)
	{
		printf("ERROR\r\n");
		exit(-1);
	}
	printf("OK\r\n");
	
	printf("chuni_io_slider_init() : ");
	chuni_io_slider_init();
	printf("OK\r\n");
	
	printf("chuni_io_slider_start() : ");
	chuni_io_slider_start(status_print);
	printf("OK\r\n");
	
	printf("Init OK. Running input loop. Press Ctrl+C to exit.\r\n");
	
	#ifdef _WIN64
	static uint8_t prev_opbtn, prev_beams;
	uint8_t opbtn, beams;
	while(true)
	{
		chuni_io_jvs_poll(&opbtn, &beams);
	
	{
		if (opbtn != prev_opbtn)
		{
			printf("opbtn changed!\r\n%02x\r\n",opbtn);
			prev_opbtn = opbtn;
		}
		if (beams != prev_beams)
		{
			printf("beams changed!\r\n%02x\r\n",beams);
			prev_beams = beams;
		}
	}
	//0x96 97 98  for side 0,     0xb4 b5 b6 for side 1     
		chuni_io_led_set_colors(0, tower_colors);
	}
	#else
		while(true);
	#endif
		return 0;
}
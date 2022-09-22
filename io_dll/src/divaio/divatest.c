#include <windows.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "divaio.h"

#define PRESSURE_THRESHOLD 40

static void reactive_led(uint8_t *rgb_data, const uint8_t *state)
{
	for (int i=0; i<32; i++)
	{
		if (state[i] > PRESSURE_THRESHOLD)
		{
			rgb_data[3*i] = 0x7F;
			rgb_data[3*i+1] = 0x23;
			rgb_data[3*i+2] = 0x00;
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
	static uint8_t prev_opbtn, prev_buttons;
	uint8_t opbtn, buttons;
	
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
	diva_io_slider_set_leds(rgb_data);
	
	diva_io_jvs_poll(&opbtn, &buttons);
	{
		if (opbtn != prev_opbtn)
		{
			printf("opbtn changed!\r\n%02x\r\n",opbtn);
			prev_opbtn = opbtn;
		}
		if (buttons != prev_buttons)
		{
			printf("buttons changed!\r\n%02x\r\n",buttons);
			prev_buttons = buttons;
		}
	}
}

int main()
{
	printf("divaTEST\r\n---------\r\n");
	//printf("api version = %04x\r\n", diva_io_get_api_version()); /* not compatible with older dlls */
	printf("diva_io_jvs_init() : ");
	if (diva_io_jvs_init() == -1)
	{
		printf("ERROR\r\n");
		exit(-1);
	}
	printf("OK\r\n");
	
	printf("diva_io_slider_init() : ");
	diva_io_slider_init();
	printf("OK\r\n");
	
	printf("diva_io_slider_start() : ");
	diva_io_slider_start(status_print);
	printf("OK\r\n");
	
	printf("Init OK. Running input loop. Press Ctrl+C to exit.\r\n");
	while(true);
	return 0;
}
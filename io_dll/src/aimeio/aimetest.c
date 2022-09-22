#include <windows.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "aimeio.h"

int main()
{
	printf("AIMETEST\r\n---------\r\n");
	//printf("api version = %04x\r\n", chuni_io_get_api_version()); /* not compatible with older dlls */
	printf("aime_io_init() : ");
	if (aime_io_init() == -1)
	{
		printf("ERROR\r\n");
		exit(-1);
	}
	printf("OK\r\n");
	
	printf("aime_io_led_set_color(red) : ");
	aime_io_led_set_color(0, 255, 0, 0);
	printf("OK\r\n");
	Sleep(2000);
	printf("aime_io_led_set_color(green) : ");
	aime_io_led_set_color(0, 0, 255, 0);
	printf("OK\r\n");
	Sleep(2000);
	printf("aime_io_led_set_color(blue) : ");
	aime_io_led_set_color(0, 0, 0, 255);
	printf("OK\r\n");
	Sleep(2000);
	aime_io_led_set_color(0, 0, 0, 0);
		
	printf("Init OK. Running input loop. Press Ctrl+C to exit.\r\n");

	uint8_t luid[10] = {0};
	uint64_t IDm = 0;
	while(1)
	{
		if ( aime_io_nfc_poll(0) == S_OK )
		{
			if ( aime_io_nfc_get_felica_id(0, &IDm) == S_OK )
			{
				aime_io_led_set_color(0, 0, 255, 0);
				printf("Found FeliCa card with uid %llx\r\n", IDm);
				continue;
			}
			if ( aime_io_nfc_get_aime_id(0, luid, 10) == S_OK )
			{
				aime_io_led_set_color(0, 0, 0, 255);
				printf("Found old card with uid ");
				for (int i=0; i<10; i++)
				{
					printf("%02x ", luid[i]);
				}
				printf("\r\n");
				continue;
			}
			//printf("poll ok but no card?!\r\n");
		}
		//printf("No card detected, trying again in 1sec\r\n");
		Sleep(300);
		aime_io_led_set_color(0, 0, 0, 0);
	}

	return 0;
}
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef _countof
#define _countof(a) (sizeof(a)/sizeof(*(a)))
#endif

struct chuni_io_config {
    uint8_t vk_test;
    uint8_t vk_service;
    uint8_t vk_coin;
    uint8_t vk_ir;
    uint8_t vk_cell[32];
	wchar_t tower_color_active[8];
	wchar_t tower_color_inactive[8];
	uint8_t real_led;
};

void chuni_io_config_load(
        struct chuni_io_config *cfg,
        const wchar_t *filename);
		
struct diva_io_config {
    uint8_t vk_buttons[5];
    uint8_t vk_slider[8];
    uint8_t vk_test;
    uint8_t vk_service;
    uint8_t vk_coin;
    uint64_t hid_offset;
};

void diva_io_config_load(
        struct diva_io_config *cfg,
        const wchar_t *filename);

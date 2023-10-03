#include <windows.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

static const int chuni_io_default_cells[] = {
    'L', 'L', 'L', 'L',
    'K', 'K', 'K', 'K',
    'J', 'J', 'J', 'J',
    'H', 'H', 'H', 'H',
    'G', 'G', 'G', 'G',
    'F', 'F', 'F', 'F',
    'D', 'D', 'D', 'D',
    'S', 'S', 'S', 'S',
};

void chuni_io_config_load(
        struct chuni_io_config *cfg,
        const wchar_t *filename)
{
    wchar_t key[16];
    int i;

    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->vk_test = GetPrivateProfileIntW(L"io3", L"test", '1', filename);
    cfg->vk_service = GetPrivateProfileIntW(L"io3", L"service", '2', filename);
    cfg->vk_coin = GetPrivateProfileIntW(L"io3", L"coin", '3', filename);
    cfg->vk_ir = GetPrivateProfileIntW(L"io3", L"ir", VK_SPACE, filename);
	GetPrivateProfileStringW(
                L"chuniio",
                L"tower_color_active",
                L"#007f23",
                cfg->tower_color_active,
                _countof(cfg->tower_color_active),
                filename);
	GetPrivateProfileStringW(
                L"chuniio",
                L"tower_color_inactive",
                L"#23007f",
                cfg->tower_color_inactive,
                _countof(cfg->tower_color_inactive),
                filename);
#if FUFUBOT == 1
	cfg->real_led = GetPrivateProfileIntW(L"chuniio", L"real_led", 0, filename);
	cfg->real_led |= GetPrivateProfileIntW(L"zhousensor", L"real_led", 0, filename);
#endif

    for (i = 0 ; i < 32 ; i++) {
        swprintf(key, 7, L"cell%i", i + 1);
        cfg->vk_cell[i] = GetPrivateProfileIntW(
                L"slider",
                key,
                chuni_io_default_cells[i],
                filename);
    }
}

static const int diva_io_default_buttons[] = {
    VK_RIGHT, VK_DOWN, VK_LEFT, VK_UP, VK_SPACE
};

static const int diva_io_default_slider[] = {
    'Q', 'W', 'E', 'R', 'U', 'I', 'O', 'P'
};

void diva_io_config_load(
        struct diva_io_config *cfg,
        const wchar_t *filename)
{
    wchar_t key[5];
    wchar_t cell[8];
    unsigned int i;
    unsigned int c;

    assert(cfg != NULL);
    assert(filename != NULL);

	cfg->hid_offset = GetPrivateProfileIntW(L"divaio", L"hid_offset", 0, filename);
	if (cfg->hid_offset != 0)
		cfg->hid_offset += (uint64_t) GetModuleHandleA(NULL);
    cfg->vk_test = GetPrivateProfileIntW(L"io3", L"test", '1', filename);
    cfg->vk_service = GetPrivateProfileIntW(L"io3", L"service", '2', filename);
    cfg->vk_coin = GetPrivateProfileIntW(L"io3", L"coin", '3', filename);

    for (i = 0 ; i < _countof(cfg->vk_buttons) ; i++) {
        swprintf(key, 6, L"key%i", i + 1);
        cfg->vk_buttons[i] = GetPrivateProfileIntW(
                L"buttons",
                key,
                diva_io_default_buttons[i],
                filename);
    }

    for (c = 0 ; c < _countof(cfg->vk_slider) ; c++) {
        swprintf(cell, 7, L"cell%i", c + 1);
        cfg->vk_slider[c] = GetPrivateProfileIntW(
                L"slider",
                cell,
                diva_io_default_slider[c],
                filename);
    }
}

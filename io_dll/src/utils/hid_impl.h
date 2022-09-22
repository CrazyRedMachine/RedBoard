#pragma once

#include <stddef.h>
#include <stdint.h>

int lzfx_compress(const void *const ibuf, const unsigned int ilen, void *obuf, unsigned int *const olen);
int hid_open_device(HANDLE *device_handle, uint16_t vid, uint16_t pid, uint8_t mi);
int hid_get_report(HANDLE device_handle, uint8_t *buf, uint8_t report_id, uint8_t nb_bytes);
int hid_set_report(HANDLE device_handle, const uint8_t *buf, uint8_t report_id, uint8_t nb_bytes);
int hid_set_report_raw(HANDLE device_handle, const uint8_t *buf, uint8_t report_id, uint8_t nb_bytes);
int hid_set_feature_report_raw(HANDLE device_handle, const uint8_t *buf, uint8_t report_id, uint8_t nb_bytes);
#pragma once
#include <main.h>

#define	KEY_SHIFT				        1
#define	KEY_CAPS				        2
#define	KEY_NUM				            4


MAPPED_KEY MapScancode(PKEY_DATA KeyData);
bool IsSpecialMakecode(USHORT Makecode);
#ifndef __LIB_KERNEL_PRINT_H
#define __LIB_KERNEL_PRINT_H
#include "stdint.h"
void putchar(uint8_t char_asci);
void putstr(char* message);
void putint(uint32_t num);	 // 以16进制打印
#endif


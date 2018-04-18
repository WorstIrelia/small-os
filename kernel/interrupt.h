#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H
#include "stdint.h"
typedef void* intr_handler;
void idt_init(void);

typedef enum intr_statu{
	INTR_ON,
	INTR_OFF
}Intr_Statu;
Intr_Statu intr_get_status();
Intr_Statu intr_set_status(Intr_Statu);
Intr_Statu intr_enable();
Intr_Statu intr_disable();
#endif


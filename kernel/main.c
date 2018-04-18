#include "print.h"
#include "interrupt.h"
#include "debug.h"
#include "string.h"
#include "memory.h"
int main(){

    idt_init();
    int a[20];

    mem_init();
	void* addr = get_kernel_pages(3);
   put_str("\n get_kernel_page start vaddr is ");
   put_int((uint32_t)addr);
   put_str("\n");
    //ASSERT(1==2);
    while(1);
}


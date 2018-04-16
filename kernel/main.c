#include "print.h"
#include "interrupt.h"
int main(){
    put_str("hello world\n");
    put_int(0);
    put_char('\n');
    put_int(9);
    put_char('\n');
    put_int(0x00021a3f);
    put_char('\n');
    put_int(0x12345678);
    put_char('\n');
    put_int(0x00000000);
    idt_init();
    asm volatile("sti");	     // 为演示中断处理,在此临时开中断
    while(1);
}

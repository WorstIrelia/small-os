#include "interrupt.h"
#include "print.h"
#include "global.h"
#include "io.h"

#define PIC_M_CTRL 0x20	       // 这里用的可编程中断控制器是8259A,主片的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从片的数据端口是0xa1

#define IDT_DESC_CNT 0x21

#define EFLAGS_IF 0x00000200
#define GET_EFLAG(VAR) asm volatile("pushfl;popl %0":"=g" (VAR))
typedef struct interrupt_descriptor{
	uint16_t func_low_addr;
	uint16_t int_selector;
	uint8_t empty;
	uint8_t attr;
	uint16_t func_high_addr;
}int_desc;
extern intr_handler interrupt_ptr[IDT_DESC_CNT];
intr_handler interrupt_handler[IDT_DESC_CNT];

static int_desc int_desc_array[IDT_DESC_CNT];
static intr_handler general_interrupt_handler(uint8_t num){
    put_str("interrupt 0x");
    put_int(num);
    put_char('\n');

}
static void pic_init(void) {

   /* 初始化主片 */
   outb (PIC_M_CTRL, 0x11);   // ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_M_DATA, 0x20);   // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27.
   outb (PIC_M_DATA, 0x04);   // ICW3: IR2接从片.
   outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

   /* 初始化从片 */
   outb (PIC_S_CTRL, 0x11);	// ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_S_DATA, 0x28);	// ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F.
   outb (PIC_S_DATA, 0x02);	// ICW3: 设置从片连接到主片的IR2引脚
   outb (PIC_S_DATA, 0x01);	// ICW4: 8086模式, 正常EOI

   /* 打开主片上IR0,也就是目前只接受时钟产生的中断 */
   outb (PIC_M_DATA, 0xfe);
   outb (PIC_S_DATA, 0xff);

   put_str("   pic_init done\n");
}
static void make_idt_desc(int_desc *p,uint8_t attr,intr_handler func){
		p->func_low_addr=(uint32_t)func & 0x0000ffff;
		p->int_selector=SELECTOR_K_CODE;
		p->empty=0;
		p->attr=attr;
		p->func_high_addr=((uint32_t)func & (0xffff0000))>>16;
}

static void init_idt_desc(){

	for(int i=0;i<IDT_DESC_CNT;i++){
		make_idt_desc(int_desc_array+i,IDT_DESC_ATTR_DPL0,interrupt_ptr[i]);
	}

	put_str("init idt_desc end\n");
}
static void init_handler(){

    for(int i=0;i<IDT_DESC_CNT;i++){
        interrupt_handler[i]=general_interrupt_handler;
    }
}

void idt_init(){
	put_str("init idt begin\n");
	init_idt_desc();
    init_handler();
	pic_init();
	uint64_t idt_operand = ((sizeof(int_desc_array) - 1) | ((uint64_t)(uint32_t)int_desc_array << 16));
   	asm volatile("lidt %0" : : "m" (idt_operand));
	put_str("init idt end\n");

}

Intr_Statu intr_get_status(){
	uint32_t eflags=0;
	GET_EFLAG(eflags);
	return (EFLAGS_IF &eflags)? INTR_ON:INTR_OFF;
}
Intr_Statu intr_set_status(Intr_Statu status){
	return status & INTR_ON? INTR_ON: INTR_OFF;
}
Intr_Statu intr_disable(){
	//Inter_Status old_status;
	if(intr_get_status()!=INTR_ON){
		asm volatile("cli"::: "memory");
		return INTR_OFF;
	}
	return INTR_ON;
}
Intr_Statu intr_enable(){

	if(intr_get_status()!=INTR_OFF){
		asm volatile("sti");
		return INTR_ON;
	}
	return INTR_OFF;
}







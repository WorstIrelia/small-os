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
int set_cursor(uint8_t);


typedef struct interrupt_descriptor {
    uint16_t func_low_addr;
    uint16_t int_selector;
    uint8_t empty;
    uint8_t attr;
    uint16_t func_high_addr;
} int_desc;
extern intr_handler interrupt_ptr[IDT_DESC_CNT];
intr_handler interrupt_handler[IDT_DESC_CNT];

static int_desc int_desc_array[IDT_DESC_CNT];
char* intr_name[IDT_DESC_CNT];

static void general_interrupt_handler(uint8_t vec_nr) {
    if (vec_nr == 0x27 || vec_nr == 0x2f) {	// 0x2f是从片8259A上的最后一个irq引脚，保留
        return;	//IRQ7和IRQ15会产生伪中断(spurious interrupt),无须处理。
    }
    /* 将光标置为0,从屏幕左上角清出一片打印异常信息的区域,方便阅读 */
    set_cursor(0);
    int cursor_pos = 0;
    while(cursor_pos < 320) {
        put_char(' ');
        cursor_pos++;
    }

    set_cursor(0);	 // 重置光标为屏幕左上角
    put_str("!!!!!!!      excetion message begin  !!!!!!!!\n");
    set_cursor(88);	// 从第2行第8个字符开始打印
    put_str(intr_name[vec_nr]);
      //put_int(vec_nr);
    if (vec_nr == 14) {	  // 若为Pagefault,将缺失的地址打印出来并悬停
        int page_fault_vaddr = 0;
        asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr));	  // cr2是存放造成page_fault的地址
        put_str("\npage fault addr is ");
        put_int(page_fault_vaddr);
    }
    put_str("\n!!!!!!!      excetion message end    !!!!!!!!\n");
    // 能进入中断处理程序就表示已经处在关中断情况下,
    // 不会出现调度进程的情况。故下面的死循环不会再被中断。
    while(1);

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
static void make_idt_desc(int_desc *p,uint8_t attr,intr_handler func) {
    p->func_low_addr=(uint32_t)func & 0x0000ffff;
    p->int_selector=SELECTOR_K_CODE;
    p->empty=0;
    p->attr=attr;
    p->func_high_addr=((uint32_t)func & (0xffff0000))>>16;
}

static void init_idt_desc() {

    for(int i=0; i<IDT_DESC_CNT; i++) {
        make_idt_desc(int_desc_array+i,IDT_DESC_ATTR_DPL0,interrupt_ptr[i]);
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    // intr_name[15] 第15项是intel保留项，未使用
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
    put_str("init idt_desc end\n");
}
static void init_handler() {

    for(int i=0; i<IDT_DESC_CNT; i++) {
        interrupt_handler[i]=general_interrupt_handler;
    }
}

void idt_init() {
    put_str("init idt begin\n");
    init_idt_desc();
    init_handler();
    pic_init();
    uint64_t idt_operand = ((sizeof(int_desc_array) - 1) | ((uint64_t)(uint32_t)int_desc_array << 16));
    asm volatile("lidt %0" : : "m" (idt_operand));
    put_str("init idt end\n");

}

Intr_Statu intr_get_status() {
    uint32_t eflags=0;
    GET_EFLAG(eflags);
    return (EFLAGS_IF &eflags)? INTR_ON:INTR_OFF;
}
Intr_Statu intr_set_status(Intr_Statu status) {
    return status & INTR_ON? INTR_ON: INTR_OFF;
}
Intr_Statu intr_disable() {
    //Inter_Status old_status;
    if(intr_get_status()==INTR_OFF) {

        return INTR_OFF;
    }
    asm volatile("cli"::: "memory");
    return INTR_ON;
}
Intr_Statu intr_enable() {

    if(intr_get_status()==INTR_ON) {

        return INTR_ON;
    }
    asm volatile("sti");
    return INTR_OFF;
}
void register_handler(uint8_t vector_no, intr_handler function){
	interrupt_handler[vector_no]=function;
}






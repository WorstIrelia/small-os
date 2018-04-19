#include "thread.h"
#include "debug.h"
#include "memory.h"
#include "string.h"
#include "global.h"
#include "print.h"
#include "interrupt.h"
#define MAGIC 0x19870916


struct task_struct* main_thread;

struct list thread_ready_list;
struct list thread_all_list;

static struct list_elem* thread_tag;
extern void switch_to(struct task_struct* cur, struct task_struct* next);




static void kernel_thread(thread_func *function,void *func_arg){
        intr_enable();
	function(func_arg);
}


void thread_create(struct task_struct* pthread, thread_func function, void* func_arg){
	char *p=(char*)pthread->self_kstack;
	p-=sizeof(struct intr_stack);//?
	p-=sizeof(struct thread_stack);//?
	struct thread_stack *q=(struct thread_stack*)p;
    pthread->self_kstack=(uint32_t*)p;
	q->function=function;
	q->func_arg=func_arg;
	q->eip=kernel_thread;//为什么要在包含一层
	q->ebp = q->ebx = q->esi = q->edi = 0;//这个呢?
}
void init_thread(struct task_struct* pthread, char* name, int prio){
	memset(pthread,0,sizeof(struct task_struct));
	pthread->priority=prio;
	strcpy(pthread->name,name);
	if(pthread==main_thread){//????????????
		pthread->status=TASK_RUNNING;
	}
	else pthread->status=TASK_READY;//

	pthread->ticks=prio;
	pthread->elapsed_ticks=0;
	pthread->pgdir=NULL;

	pthread->self_kstack=(uint32_t*)((uint32_t)pthread+PG_SIZE);
	pthread->stack_magic=MAGIC;

}
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg){
    ASSERT(func_arg);
    //put_int((uint32_t)func_arg);
	struct task_struct* pthread=(struct task_struct*)get_kernel_pages(1);
    ASSERT(pthread);

	init_thread(pthread,name,prio);
	thread_create(pthread,function,func_arg);
	//while(1);
	ASSERT(!elem_find(&thread_ready_list,&pthread->general_tag));
	list_append(&thread_ready_list,&pthread->general_tag);

	ASSERT(!elem_find(&thread_all_list,&pthread->all_list_tag));
	list_append(&thread_all_list,&pthread->all_list_tag);

	return pthread;
	//asm volatile ("movl %0, %%esp; pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; ret" : : "g" (pthread->self_kstack) : "memory");//为什么要用ret调用
};
struct task_struct* running_thread(void){
	uint32_t esp;
	asm("mov %%esp,%0":"=g"(esp));
	return (struct task_struct*)(esp&0xfffff000);
};
void schedule(void){
	//while(1);
	ASSERT(intr_get_status()==INTR_OFF);
	struct task_struct * cur =running_thread();
	if(cur->status==TASK_RUNNING){

		ASSERT(!elem_find(&thread_ready_list,&cur->general_tag));
		list_append(&thread_ready_list,&cur->general_tag);
		cur->ticks=cur->priority;
		cur->status=TASK_READY;
	}
	else{

	}

	ASSERT(!list_empty(&thread_ready_list));
	thread_tag=NULL;
	thread_tag=list_pop(&thread_ready_list);
	struct task_struct * next=elem2entry(struct task_struct,\
										general_tag,\
										thread_tag);
	next->status=TASK_RUNNING;
	switch_to(cur,next);

}
static void make_main_thread(){
	main_thread=running_thread();
	init_thread(main_thread,"main",31);

	ASSERT(!elem_find(&thread_all_list,&main_thread->all_list_tag));
	list_append(&thread_all_list,&main_thread->all_list_tag);
}

void thread_init(void) {
   	put_str("thread_init start\n");
   	list_init(&thread_ready_list);
   	list_init(&thread_all_list);
/* 将当前main函数创建为线程 */
   	make_main_thread();
   	put_str("thread_init done\n");
}


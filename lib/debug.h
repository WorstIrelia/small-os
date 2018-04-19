#ifndef DEBUG_H_
#define DEBUG_H_

void panic_spin(char *filename,int line,const char *func,const char *condition);

#define PANIC_SPIN(...) panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)

#ifdef NODEBUG
	#define ASSERT(CONDITION) ((void)0)
#else
	#define ASSERT(CONDITION)          \
		if(CONDITION){}else          \
		PANIC_SPIN(#CONDITION)
#endif


#endif // __LIB_STDINT_H


#include "debug.h"
#include "string.h"
void memset(void* dst_, uint8_t value, uint32_t size){
	ASSERT(dst_!=0);
	char *p=(char *)dst_;
	while(size--){
		*p++=value;
	}
	return;

}
void memcpy(void* dst_, const void* src_, uint32_t size){
	ASSERT(dst_!=0&&src_!=0);
	char *p=(char*)dst_;
	const char *q=(char*)src_;
	while(size--){
		*p++=*q++;
	}
	return ;
}
int memcmp(const void* a_, const void* b_, uint32_t size){
	ASSERT(a_!=0&&b_!=0);
	const char *p=(const char *)a_;
	const char *q=(const char *)b_;
	while(size--){
		if(*p!=*q){
			return *p>*q?1:-1;
		}
		p++;
		q++;
	}
	return 0;
}
char* strcpy(char* dst_, const char* src_){
	ASSERT(dst_!=0&&src_!=0);
	char *beg=dst_;
	while(*src_){
		*dst_++=*src_++;
	}
	return beg;
}
uint32_t strlen(const char* str){
	ASSERT(str!=0);
	uint32_t res=0;
	while(*str++) res++;
	return res;
}
int8_t strcmp (const char *a, const char *b){
	ASSERT(a!=0&&b!=0);
	while(*a&&*b){
		if(*a!=*b) return *a-*b;
		a++,b++;
	}
	if(*a) return *a;
	return 0-*b;
}
char* strchr(const char* string, const uint8_t ch){
	ASSERT(string);
	while(*string){
		if(*string==ch) return (char*)string;
		string++;
	}
	return 0;
}
char* strrchr(const char* string, const uint8_t ch){
	ASSERT(string);
	char *res=0;
	while(*string){
		if(*string==ch) res=(char*)string;
		string++;
	}
	return res;
}
char* strcat(char* dst_, const char* src_){
	ASSERT(dst_&&src_);
	int len=strlen(dst_);
	strcpy(dst_+len,src_);
}
uint32_t strchrs(const char* filename, uint8_t ch){
	ASSERT(filename);
	uint8_t res=0;
	while(*filename){
		if(ch==*filename) res++;
		filename++;
	}
	return res;
}


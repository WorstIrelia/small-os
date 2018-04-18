#include"global.h"
#include"bitmap.h"
#include"debug.h"
#include"string.h"
void bitmap_init(struct bitmap* btmp){//初始化位图
	ASSERT(btmp);
	memset(btmp->bits,0,(btmp->btmp_bytes_len+7)/8*8);
}
int	bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx){
	ASSERT(btmp);
	uint32_t idx=bit_idx/8;
	uint32_t left=bit_idx%8;
	uint8_t *p=btmp->bits;
	p+=idx;
	return (*p)&(BITMAP_MASK<<left);
}	//判断bit_idx是否为1
int bitmap_scan(struct bitmap* btmp, uint32_t cnt){
	ASSERT(btmp);
	int idx=0;
	uint32_t tot_idx=btmp->btmp_bytes_len/8;
	uint32_t left=btmp->btmp_bytes_len%8;
	uint8_t *p=btmp->bits;
	while(idx<tot_idx&&(*p)==0xff) p++,idx++;
	int count=0;
	uint8_t tmp=BITMAP_MASK;
	uint32_t num=0;
	while(idx*8+num<btmp->btmp_bytes_len){
		if((*p)&tmp){
			count=0;
		}
		else count++;
		num++;
		if(count==cnt){
			//printf("%d %d %d\n",idx,num,cnt);
			return idx*8+num-cnt;
		}
		tmp<<=1;
		if(!(tmp&0xff)) {
			tmp=BITMAP_MASK;
			p++;
			idx++;
			num=0;
		}
	}
	return -1;

}//连续申请cnt位 成功返回下标,失败返回-1
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value){
	ASSERT(btmp&&(value==0||value==1));
	uint32_t idx=bit_idx/8;
	uint32_t left=bit_idx%8;
	uint8_t *p=btmp->bits;
	p+=idx;
	if(value){
		(*p)|=(BITMAP_MASK<<left);
	}
	else (*p)&=~(BITMAP_MASK<<left);

}

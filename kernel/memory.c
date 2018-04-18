#include "memory.h"

#include "print.h"

#include "stdint.h"

#include "debug.h"

#include "string.h"

#include "bitmap.h"
#define PG_SIZE 4096

#define MEM_BITMAP_BASE 0xc009a000

#define K_HEAP_START 0xc0100000

#define ALL_MEM 0x02000000

#define KERNEL_MEM 0x00100000

#define PDE(vaddr) (vaddr>>22)
#define PTE(vaddr) ((vaddr&0x003ff000)>>12)
typedef struct pool{
	struct bitmap pool_bitmap;
	uint32_t phy_addr_start;
	uint32_t pool_size;
}Pool;

Pool kernel_pool,user_pool;

Virtual_Addr kernel_vaddr;

static void mem_pool_init(){
	put_str("mem_pool init start\n");
	uint32_t page_table_size=PG_SIZE*256;
	uint32_t free_mem=(ALL_MEM-page_table_size-KERNEL_MEM);
	uint32_t free_page=free_mem/PG_SIZE;

	uint32_t kernel_page=free_page/2;
	uint32_t user_page=free_page-kernel_page;

	kernel_pool.phy_addr_start=KERNEL_MEM+page_table_size;
	user_pool.phy_addr_start=kernel_pool.phy_addr_start+kernel_page*PG_SIZE;

	kernel_pool.pool_size=kernel_page*PG_SIZE;//byte
	user_pool.pool_size=user_page*PG_SIZE;//byte

	kernel_pool.pool_bitmap.bits=(void*)MEM_BITMAP_BASE;
	kernel_pool.pool_bitmap.btmp_bytes_len=kernel_page;

	user_pool.pool_bitmap.bits=(void*)(MEM_BITMAP_BASE+(kernel_page+7)/8);
	user_pool.pool_bitmap.btmp_bytes_len=user_page;



	put_str("kernel_pool_bitmap_start\n");
	put_int((int)kernel_pool.pool_bitmap.bits);
	put_char('\n');
	put_str("kernel_pool_phy_addr_start\n");
	put_int((int)kernel_pool.phy_addr_start);
	put_char('\n');
	user_pool.pool_bitmap.btmp_bytes_len=user_page;
	put_str("user_pool_bitmap_start\n");
	put_int((int)user_pool.pool_bitmap.bits);
	put_char('\n');
	put_str("user_pool_phy_addr_start\n");
	put_int((int)user_pool.phy_addr_start);
	put_char('\n');


	bitmap_init(&kernel_pool.pool_bitmap);
	bitmap_init(&user_pool.pool_bitmap);

	kernel_vaddr.vaddr_bitmap.btmp_bytes_len=kernel_page;

	kernel_vaddr.vaddr_bitmap.bits=(void*)((uint32_t)user_pool.pool_bitmap.bits+(user_page+7)/8);
	kernel_vaddr.vaddr_start=K_HEAP_START;
	bitmap_init(&kernel_vaddr.vaddr_bitmap);

	put_str("mem_pool init done\n");


}

static void* vaddr_get(enum pool_flags pf,uint32_t pg_cnt){
	uint32_t res=0;
	if(pf==PF_KERNEL){

		int idx=bitmap_scan(&kernel_vaddr.vaddr_bitmap,pg_cnt);
		if(idx==-1) return (void*)res;
		res=((uint32_t)kernel_vaddr.vaddr_start+idx*PG_SIZE);
		while(pg_cnt--){
			bitmap_set(&kernel_vaddr.vaddr_bitmap,idx++,1);
		}
		

	}
	else{

		//user_pool
	}
	return (void*)res;
}

uint32_t * pte_ptr(uint32_t vaddr){
	return (uint32_t *)(0xffc00000+((vaddr&0xffc00000)>>10)+(PTE(vaddr)<<2));
}
uint32_t * pde_ptr(uint32_t vaddr){

	return (uint32_t *)(0xfffff000+(PDE(vaddr)<<2));
}


static void *palloc(struct pool* m_pool){
	
	int idx=bitmap_scan(&m_pool->pool_bitmap,1);
	
	if(idx==-1) return (void*) 0;
	bitmap_set(&m_pool->pool_bitmap,idx,1);
	
	return (void *)(m_pool->phy_addr_start+idx*PG_SIZE);
}


static void page_table_add(void *_vaddr,void *_page_phyaddr){
	uint32_t vaddr=(uint32_t)_vaddr,page_phyaddr=(uint32_t)_page_phyaddr;
	uint32_t *pde=pde_ptr(vaddr);
	uint32_t *pte=pte_ptr(vaddr);
	
	if(*pde&0x00000001){
		//while(1);
		ASSERT(!(*pte&0x00000001));
		//while(1);
		if(!(*pte&0x00000001)){
			*pte=(page_phyaddr|PG_US_U|PG_RW_W|PG_P_1);
		}
		else{
			PANIC_SPIN("pte repeat");
			*pte=(page_phyaddr|PG_US_U|PG_RW_W|PG_P_1);
		}
		
	}
	else{
		
		uint32_t pde_phyaddr=(uint32_t) palloc(&kernel_pool);
		
		*pde=(pde_phyaddr|PG_US_U|PG_RW_W|PG_P_1);

		memset((void*)((uint32_t)pte&0xfffff000),0,PG_SIZE);

		ASSERT(!(*pte&0x00000001));

		*pte=(page_phyaddr|PG_US_U|PG_RW_W|PG_P_1);
	}
	
}

void *malloc_page(enum pool_flags pf,uint32_t pg_cnt){
	ASSERT(pg_cnt>0&&pg_cnt<3840);

	void *vaddr_start=vaddr_get(pf,pg_cnt);
	if(vaddr_start==0) return 0;

	uint32_t vaddr=(uint32_t)vaddr_start,cnt=pg_cnt;
	struct pool* mem_pool=(pf&PF_KERNEL)?&kernel_pool:&user_pool;
	
	while(cnt--){
		void *page_phyaddr=palloc(mem_pool);
		
		if(page_phyaddr==0) return 0;
		
		page_table_add((void*)vaddr,page_phyaddr);
		
		vaddr+=PG_SIZE;
	}
	
	return vaddr_start;
}
void *get_kernel_pages(uint32_t pg_cnt){
	void *vaddr=malloc_page(PF_KERNEL,pg_cnt);
	if(vaddr){
		memset(vaddr,0,pg_cnt*PG_SIZE);
	}
	return vaddr;
}
void mem_init(){
	put_str("mem_init start\n");
	mem_pool_init();
	put_str("mem_init done\n");
}



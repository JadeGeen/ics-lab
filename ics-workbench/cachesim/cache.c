#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;

static uint32_t cache[exp2(19)][19];//0：有效位，1：脏位，2：tag，3-18：data
static uint64_t cache_row = 0;//cache总行数
static uint64_t groupindex_width = 0;//编码所有组所需的位数
static uint64_t group_size = 0;//一组对应cache中几行
static uint64_t width_mask = 0;//取组号用


void cycle_increase(int n) { cycle_cnt += n; }

// TODO: implement the following functions


void writeback(uint32_t group_index,uint64_t rowindex){
	if(cache[rowindex][1]==1){
		uintptr_t addr = (cache[rowindex][2]<<groupindex_width)+group_index;
		mem_write(addr, (uint8_t *)(&cache[rowindex][3]));
	}
	cache[rowindex][0]=0;
	cache[rowindex][1]=0;
}

uint64_t cachemissload(uintptr_t addr, uint64_t start, uint64_t end, uint32_t tag,uint32_t group_index){
	uint64_t rowindex = start;
	for(;rowindex<end;rowindex++)
		if(cache[rowindex][0]==0)
			break;
	if(rowindex>=end){
		rowindex = rand()%group_size+start;
		writeback(group_index, rowindex);
	}
	mem_read(addr,(uint8_t *)(&cache[rowindex][3]));
	cache[rowindex][0] = 1;
	cache[rowindex][1] = 0;
	cache[rowindex][2] = tag;
	return rowindex;
}

uint32_t cache_read(uintptr_t addr) {
	uint32_t tag = addr>>(BLOCK_WIDTH+groupindex_width);
	uint32_t group_index = (addr>>BLOCK_WIDTH)&width_mask;
	uint32_t inblock_addr = ((addr&0x3f)>>2)+3;//索引data
	uint64_t group_start = group_index*group_size;
	uint64_t group_end = group_start+group_size;//左闭右开
	uint64_t rowindex = group_start;
	for(;rowindex<group_end;rowindex++)
		if(cache[rowindex][2]==tag&&cache[rowindex][0]==1)
			break;
	if(rowindex>=group_end){
		uintptr_t memaddr = addr>>BLOCK_WIDTH;
		rowindex = cachemissload(memaddr,group_start,group_end,tag,group_index);
	}
	return cache[rowindex][inblock_addr];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
	uint32_t tag = addr>>(BLOCK_WIDTH+groupindex_width);
	uint32_t group_index = (addr>>BLOCK_WIDTH)&width_mask;
	uint32_t inblock_addr = ((addr&0x3f)>>2)+3;//索引data
	uint64_t group_start = group_index*group_size;
	uint64_t group_end = group_start+group_size;//左闭右开
	uint64_t rowindex = group_start;
	for(;rowindex<group_end;rowindex++)
		if(cache[rowindex][2]==tag&&cache[rowindex][0]==1)
			break;
	if(rowindex>=group_end){
		uintptr_t memaddr = addr>>BLOCK_WIDTH;
		rowindex = cachemissload(memaddr,group_start,group_end,tag,group_index);
	}
	cache[rowindex][1] = 1;
	uint32_t temp = data&wmask;
	cache[rowindex][inblock_addr] &= (~wmask);
	cache[rowindex][inblock_addr] |= temp;
}

void init_cache(int total_size_width, int associativity_width) {
	cache_row = exp2(total_size_width - BLOCK_WIDTH);//get cache size
	groupindex_width = total_size_width - BLOCK_WIDTH - associativity_width;
	group_size = exp2(associativity_width);
	width_mask = mask_with_len(groupindex_width);
	for(uint64_t i = 0;i<cache_row;i++){
		cache[i][0] = 0;//invalid
		cache[i][1] = 0;//no dirty
	}
}

void display_statistic(void) {
}

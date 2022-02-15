#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
	asm volatile("addq %%rbx,%%rax"
			:"=a"(a)
			:"a"(a), "b"(b)
	   );
	return a;
}

int asm_popcnt(uint64_t x) {
	int i = 0;
	int s;
	asm volatile("L1:cmpl $64,%%ecx\n\t"
			"jge L2\n\t"
			"movq %%rbx,%%rax\n\t"
			"shrq %%cl,%%rax\n\t"
			"andq $1,%%rax\n\t"
			"testq %%rax,%%rax\n\t"
			"je L3\n"
			"addl $1,%%edx\n\t"
			"L3:addl $1, %%ecx\n\t"
			"jmp L1\n"
			"L2:"
			:"=d"(s)
			:"c"(i),"b"(x),"d"(i)
			:"rax"
			);
	return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
	int d0,d1,d2;
	asm volatile("rep;movsl\n\t"//movl (%esi),(%edi);esi+=4;edi+=4;ecx--;
			"testl $2,%%ebx\n\t"
			"je L4\n\t"
			"movsw\n"//movw (%si),(%di);esi+=2;edi+=2;
			"L4:\ttestl $1,%%ebx\n\t"
			"je L5\n\t"
			"movsb\n"
			"L5:"
			:"=&c"(d0),"=&D"(d1),"=&S"(d2)
			:"0"(n>>2),"b"(n),"1"(dest),"2"(src)
			:"memory"
			);
	return dest;
}


int asm_setjmp(asm_jmp_buf env) {
	asm volatile("movq %%rbx,%0":"=m"(env[0]));
	asm volatile("movq (%%rsp),%%rbp\n\t"
			"movq %%rbp,%0"
			:"=m"(env[1])
			);
	asm volatile("movq %%rsp,%0":"=m"(env[2]));
	asm volatile("movq 8(%%rsp),%%rdx\n\t"
			"movq %%rdx,%0"
			:"=m"(env[3])
			);
	return 0;

}

void asm_longjmp(asm_jmp_buf env, int val) {
	asm volatile("movl %0,%%eax\n\t"
			"test %%eax,%%eax\n\t"
			"jne L6\n\t"
			"addl $1,%%eax\n\t"
			"L6:"
			:
			:"d"(val)
			:"rax"
			);
	asm volatile("movq %0,%%rbx"::"m"(env[0]));
	asm volatile("movq %0,%%rsp"::"m"(env[2]));
	asm volatile("movq %0,%%rbp\n\t"
			"movq %%rbp,(%%rsp)"
			:
			:"m"(env[1])
			);
	asm volatile("movq %0,%%rdx\n\t"
			"movq %%rdx,8(%%rsp)"
			:
			:"m"(env[3])
			);
}


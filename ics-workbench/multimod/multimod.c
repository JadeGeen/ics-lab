#include <stdint.h>

static uint64_t max=-1;
static uint64_t leftmax=(uint64_t)1<<63;

//pick out 'i'th number
uint64_t bitof(uint64_t x,uint64_t i){
	return (x>>i)&1;
}

//mod
uint64_t mod(uint64_t x,uint64_t m){
	while(x>=m){
		uint64_t m0=m;
		while(x>m0&&m0<leftmax){
			m0=m0<<1;
		}
		if(x<m0&&m<m0){
			m0=m0>>1;
		}
		x-=m0;
	}
	return x;
}

//modadd
uint64_t modadd(uint64_t a,uint64_t b,uint64_t m){
	a=mod(a,m);
	b=mod(b,m);
	uint64_t sum=a+b;
	//overflow
	while(a!=0&&b!=0&&a>max-b){
		a=mod(sum,m);
		b=mod(max,m)+(uint64_t)1;
		sum=a+b;
	}
	return mod(sum,m);
}

//multiadd
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m){
	uint64_t res=0;
	for(int i=0;i<64;i++){
		if(bitof(a,i)==1){
			uint64_t partres=b;
			for(int j=0;j<i;j++){
				partres=modadd(partres,partres,m);
			}
			res=modadd(res,partres,m);
		}
	}
	return res;
}

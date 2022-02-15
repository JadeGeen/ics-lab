#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define N 10000000

static bool is_prime[N];
static int  primes[N];
int count=0;

int *sieve(int n) {
	assert(n + 1 < N);
	primes[count++]=2;
	for(int i=3;i<=n;i+=2){
		if(!is_prime[i])  
			primes[count++]=i;
		for(int j=1;j<count;j++){
			if(i*primes[j]>n) break;
			is_prime[i*primes[j]]=1;
			if(i%primes[j]==0) break;
		}
	}
	primes[count]=0;
	return primes;
}

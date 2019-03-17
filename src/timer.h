/*
 * timer.h
 *
 *  Created on: 26-Dec-2011
 *      Author: raghav
 */

#ifndef TIMER_H_
#define TIMER_H_


#include <stdint.h>

extern "C" {
__inline__ uint64_t clock_(void) {
	uint32_t lo, hi;
	__asm__ __volatile__ ( // serialize
			"xorl %%eax,%%eax \n        cpuid"
			::: "%rax", "%rbx", "%rcx", "%rdx");
	/* We cannot use "=A", since this would use %rax on x86_64 and return only the lower 32bits of the TSC */
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t) hi << 32 | lo;
}
}

class timer {

	uint64_t start_;
	uint64_t stop_;

public:
	timer() :
			start_(), stop_() {
	}

	inline void tic() {
		stop_ = 0;
		start_ = clock_();
	}

	inline uint64_t toc() {

		stop_ = clock_();
		uint64_t diff_  = stop_ - start_;
		start_ = stop_;
		return diff_;
	}

	inline uint64_t diff() {
		return stop_ - start_;
	}

};

#endif /* TIMER_H_ */

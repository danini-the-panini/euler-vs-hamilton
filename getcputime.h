#ifndef GETCPUTIME_H
#define GETCPUTIME_H

double getCPUTime();

#define START_BENCH double __start_bench__ = getCPUTime()
#define END_BENCH double __end_bench__ = getCPUTime()
#define BENCH_RESULT (__end_bench__ - __start_bench__)


#endif

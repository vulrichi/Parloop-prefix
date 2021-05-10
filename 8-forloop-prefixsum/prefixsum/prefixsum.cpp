#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include "omploop.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void generatePrefixSumData(int *arr, size_t n);

void checkPrefixSumResult(int *arr, size_t n);

#ifdef __cplusplus
}
#endif


int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <n> <nbthreads>" << std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int *arr = new int[n];
    generatePrefixSumData(arr, n);
    int *pr = new int[n + 1];
    pr[0] = 0;
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    
    int ceilLog2N = 1;
    while ((1 << ceilLog2N) < n) ceilLog2N += 2;
    
    int *current = arr;
    int *next = pr + 1;
    OmpLoop loop;
    for (int i = 0; i < ceilLog2N; i++) {
        loop.parfor<int>(
            0, n, 1, [](auto _) { }, [&](int j, auto _)
            {
                if (j < (1 << i)) {
                    next[j] = current[j];
                } else {
                    next[j] = current[j] + current[j - (1 << i)];
                }
            }, [](auto _) { }
        );
        
        std::swap(current, next);
    }
    
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    std::cerr<<elapsed_seconds.count()<<std::endl;
    
    
    checkPrefixSumResult(pr, n);
    
    delete[] arr;
    delete[] pr;
    
    return 0;
}


#pragma once
#include <random>

#include "common_include/definitions.h"


typedef unsigned long int ul_int;

class Primes
{
public:
    static inline ul_int sqrt(ul_int i);
    static inline ul_int isPrime(ul_int i);
    static inline ul_int nextPrime(ul_int i);
    static ul_int genPrime(size_t size, std::mt19937 &randNumGen);

    static double round(double x);
};

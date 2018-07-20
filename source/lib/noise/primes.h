#pragma once
#include <random>

#include "common_include/definitions.h"


#define Int unsigned long int

class Primes
{
public:
    Primes();
    ~Primes();

    inline Int sqrt(Int i);
    inline Int isPrime(Int i);
    inline Int nextPrime(Int i);
    Int genPrime(Int size, std::mt19937 &randNumGen);

    static double round(double x);
};

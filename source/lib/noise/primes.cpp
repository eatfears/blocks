#include "primes.h"

#include <cmath>


inline ul_int Primes::sqrt(ul_int i)
{
    ul_int r = 1, rnew = 1, rold = r;

    do
    {
        rold = r;
        r = rnew;
        rnew = (r + (i/r));
        rnew >>= 1;
    }
    while(rold != rnew);
    return rnew;
}

inline ul_int Primes::isPrime(ul_int i)
{
    ul_int si, j;
    si = sqrt(i);

    for (j = 2; j <= si; j++)
    {
        if (i%j == 0)
        {
            return 0;
        }
    }

    return 1;
}

inline ul_int Primes::nextPrime(ul_int i)
{
    ul_int si = i;

    while (!isPrime(si))
    {
        si++;
    }

    return si;
}

ul_int Primes::genPrime(size_t size, std::mt19937 &randNumGen)
{
    std::uniform_int_distribution<ul_int> dist((ul_int)1 << (size -1), (ul_int)1 << size);
    ul_int beg = dist(randNumGen);
    beg = nextPrime(beg);
    return beg;
}

double Primes::round(double x)
{
    if (x - floor(x) >= 0.5) return ceil(x);
    return floor(x);
}

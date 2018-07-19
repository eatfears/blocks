#include "Primes.h"
#include <math.h>

Primes::Primes()
{
}

Primes::~Primes()
{
}

inline Int Primes::sqrt(Int i)
{
    Int r = 1, rnew = 1, rold = r;

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

inline Int Primes::isPrime(Int i)
{
    Int si, j;
    si = sqrt(i);

    for (j=2; (j <= si); j++)
    {
        if (i%j == 0)
            return 0;
    }

    return 1;
}

inline Int Primes::nextPrime(Int i)
{
    Int si = i;

    while(!isPrime(si))
        si++;

    return si;
}

Int Primes::genPrime(Int size, boost::mt19937 &randNumGen)
{
    Int beg = 1;

    boost::uniform_int<> degen_dist(1 << (size-1), 1 << size);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > sampler(randNumGen, degen_dist);

    beg = sampler();
    beg = nextPrime(beg);

    return beg;
}

double Primes::round(double x)
{
    if(x - floor(x) >= 0.5) return ceil(x);
    return floor(x);
}

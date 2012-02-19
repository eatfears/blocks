#include "Primes.h"
#include <math.h>

Primes::Primes()
{
}

Primes::~Primes()
{
}

inline Int Primes::Sqrt(Int i)
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

inline Int Primes::IsPrime(Int i)
{
	Int si, j;
	si = Sqrt(i);

	for (j=2; (j <= si); j++)
	{
		if (i%j == 0)
			return 0;
	}

	return 1;
}

inline Int Primes::NextPrime(Int i)
{
	Int si = i;

	while(!IsPrime(si))
		si++;

	return si;
}

Int Primes::GenPrime(Int size, boost::mt19937 *randNumGen)
{
	Int beg = 1;

	boost::uniform_int<> degen_dist(1 << (size-1), 1 << size);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > sampler(*randNumGen, degen_dist);

	beg = sampler();
	beg = NextPrime(beg);

	return beg;
}

double Primes::Round(double x)
{
	if(x - floor(x) >= 0.5) return ceil(x);
	return floor(x);
}

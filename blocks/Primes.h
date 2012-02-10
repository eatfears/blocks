#pragma once
#include "Blocks_Definitions.h"
#include "gsl/gsl_randist.h"

#define Int unsigned long int

class Primes
{
public:
	Primes();
	~Primes();

	inline Int Sqrt(Int i);
	inline Int IsPrime(Int i);
	inline Int NextPrime(Int i);
	Int GenPrime(Int size, gsl_rng *randNumGen);

	static double Round(double x);
};

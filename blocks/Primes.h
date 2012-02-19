#pragma once
#include "Blocks_Definitions.h"
#include <boost/random.hpp>

#define Int unsigned long int

class Primes
{
public:
	Primes();
	~Primes();

	inline Int Sqrt(Int i);
	inline Int IsPrime(Int i);
	inline Int NextPrime(Int i);
	Int GenPrime(Int size, boost::mt19937 *randNumGen);

	static double Round(double x);
};

#pragma once

#include "Blocks_Definitions.h"
#include "GLWindow.h"
#include "Game.h"
#include "gsl/gsl_randist.h"

#define Int unsigned long int

class Primes
{
public:
	Primes(void);
	~Primes(void);

	inline Int Sqrt(Int i);
	inline Int IsPrime(Int i);
	inline Int NextPrime(Int i);
	Int GenPrime(Int size);

};
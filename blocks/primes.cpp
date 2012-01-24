//#include <stdlib.h>
#include <iostream>
#include <math.h>

inline long sqrt(long i)
{
	long r,rnew=1,rold=r;

	do
	{
		rold=r;
		r=rnew;
		rnew = (r+(i/r));
		rnew >>= 1;
	}
	while(rold != rnew);
	return rnew;
}

inline int isprime(long i)
{
	long si,j;

	si = sqrt(i);

	for (j=2; (j<=si); j++)
	{
		if (i%j == 0)
			return 0;
	}

	return 1;

}
/*
void main(int argc, char *argv[])
{
	long i,i1,i2,f=0;

	if (argc < 2)
	{
		cout << endl << endl;
		cout << "Prime number finder: Hugo Elias 1998" << endl << "http://freespace.virgin.net/hugo.elias" << endl << endl;
		cout << "Usage:" << endl << "  primes a b > primes.txt" << endl << endl;
		cout << "will find all primes between a and b" << endl << "and will write the results to the file PRIMES.TXT" << endl;
		return;
	}

	i1 = atol(argv[1]);
	i2 = atol(argv[2]);

	for (i=i1; i<i2; i++)
		if (isprime(i))
		{
			f++;
			if (f==16)
			{
				cout << endl;
				f=0;
			}
			cout << i << " ";
		}

}
*/
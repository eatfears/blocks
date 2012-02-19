#include "PerlinNoise.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Primes.h"

PerlinNoise::PerlinNoise(double persistence, int NumberOfOctaves)
{
	PerlinNoise::persistence = persistence;
	PerlinNoise::NumberOfOctaves = NumberOfOctaves;

	interpolation = COSINE_INTERPOLATE;

	a = 15731;
	b = 789221;
	c = 1376312589;
	d = 57;
	e = 107;
}

void PerlinNoise::InitNoise(boost::mt19937 *randNumGen)
{
	Primes p;
	a = p.GenPrime(14, randNumGen);
	b = p.GenPrime(20, randNumGen);
	c = p.GenPrime(30, randNumGen);
	d = p.GenPrime(6, randNumGen);
	e = p.GenPrime(7, randNumGen);
}

PerlinNoise::~PerlinNoise()
{
}

double PerlinNoise::Noise1d(int x)
{
	x = (x<<13) ^ x;
	return (1.0 - ((x*(x*x*a + b) + c) & 0x7fffffff) / 1073741824.0);
}

double PerlinNoise::Noise2d(int x, int y)
{
	int n = x + y*d;

	return Noise1d(n);
}

double PerlinNoise::Noise3d(int x, int y, int z)
{
	int n = x + (y + z*e)*d;

	return Noise1d(n);
}

double PerlinNoise::InterpolatedNoise1d(double x)
{
	int iX = (int) x;

	double fractionalX = x - iX;

	double v1 = UseNoise1d(iX);
	double v2 = UseNoise1d(iX + 1);

	return Interpolate(v1, v2, fractionalX);
}

double PerlinNoise::InterpolatedNoise2d(double x, double y)
{
	int iX = (int) x;
	int iY = (int) y;

	double fractionalX = x - iX;
	double fractionalY = y - iY;

	double v1 = UseNoise2d(iX, iY);
	double v2 = UseNoise2d(iX + 1, iY);
	double v3 = UseNoise2d(iX, iY + 1);
	double v4 = UseNoise2d(iX + 1, iY + 1);

	double i1 = Interpolate(v1, v2, fractionalX);
	double i2 = Interpolate(v3, v4, fractionalX);

	return Interpolate(i1, i2, fractionalY);
}

double PerlinNoise::InterpolatedNoise3d(double x, double y, double z)
{
	int iX = (int) x;
	int iY = (int) y;
	int iZ = (int) z;

	double fractionalX = x - iX;
	double fractionalY = y - iY;
	double fractionalZ = z - iZ;

	double v1 = UseNoise3d(iX, iY, iZ);
	double v2 = UseNoise3d(iX + 1, iY, iZ);
	double v3 = UseNoise3d(iX, iY + 1, iZ);
	double v4 = UseNoise3d(iX + 1, iY + 1, iZ);
	double v5 = UseNoise3d(iX, iY, iZ + 1);
	double v6 = UseNoise3d(iX + 1, iY, iZ + 1);
	double v7 = UseNoise3d(iX, iY + 1, iZ + 1);
	double v8 = UseNoise3d(iX + 1, iY + 1, iZ + 1);

	double i1 = Interpolate(v1, v2, fractionalX);
	double i2 = Interpolate(v3, v4, fractionalX);
	double i3 = Interpolate(v5, v6, fractionalX);
	double i4 = Interpolate(v7, v8, fractionalX);

	double w1 = Interpolate(i1, i2, fractionalY);
	double w2 = Interpolate(i3, i4, fractionalY);

	return Interpolate(w1, w2, fractionalZ);
}

double PerlinNoise::LinearInterpolate(double a, double b, double x)
{
	return a*(1.0 - x) + b*x;
}

double PerlinNoise::CosineInterpolate(double a, double b, double x)
{
	double ft = x * M_PI;
	double f = (1.0 - cos(ft)) * 0.5;

	return a*(1.0 - f) + b*f;
}

double PerlinNoise::CubicInterpolate(double v0, double v1, double v2, double v3, double x)
{
	double P = (v3 - v2) - (v0 - v1);
	double Q = (v0 - v1) - P;
	double R = v2 - v0;
	double S = v1;

	return P*x*x*x + Q*x*x + R*x + S;
}

double PerlinNoise::Interpolate(double a, double b, double x)
{
	if(interpolation == LINEAR_INTERPOLATE)
		return LinearInterpolate(a, b, x);
	if(interpolation == COSINE_INTERPOLATE)
		return CosineInterpolate(a, b, x);
	return 0;
}

double PerlinNoise::SmoothNoise1d(int x)
{
	return Noise1d(x)/2.0 + Noise1d(x-1)/4.0 + Noise1d(x+1)/4.0;
}

double PerlinNoise::SmoothNoise2d(int x, int y)
{
	double corners	= (	Noise2d(x-1, y-1) + Noise2d(x+1, y-1) +
						Noise2d(x-1, y+1) + Noise2d(x+1, y+1)) / 16.0;

	double sides	= (	Noise2d(x-1, y) + Noise2d(x+1, y) +
						Noise2d(x, y-1) + Noise2d(x, y+1)) / 8.0;

	double center	=	Noise2d(x, y) / 4.0;

	return corners + sides + center;
}

double PerlinNoise::SmoothNoise3d(int x, int y, int z)
{
	double corners	= (	Noise3d(x-1, y-1, z-1) + Noise3d(x+1, y-1, z-1) +
						Noise3d(x-1, y-1, z+1) + Noise3d(x+1, y-1, z+1) +
						Noise3d(x-1, y+1, z-1) + Noise3d(x+1, y+1, z-1) +
						Noise3d(x-1, y+1, z+1) + Noise3d(x+1, y+1, z+1)) / 64.0;

	double sides	= (	Noise3d(x-1, y-1, z) + Noise3d(x-1, y+1, z) +
						Noise3d(x+1, y-1, z) + Noise3d(x+1, y+1, z) +
						Noise3d(x-1, y, z-1) + Noise3d(x-1, y, z+1) +
						Noise3d(x+1, y, z-1) + Noise3d(x+1, y, z+1) +
						Noise3d(x, y-1, z-1) + Noise3d(x, y-1, z+1) +
						Noise3d(x, y+1, z-1) + Noise3d(x, y+1, z+1)) / 32.0;

	double planes	= (	Noise3d(x-1, y, z) + Noise3d(x+1, y, z) +
						Noise3d(x, y-1, z) + Noise3d(x, y+1, z) +
						Noise3d(x, y, z-1) + Noise3d(x, y, z+1)) / 16.0;

	double center	=	Noise3d(x, y, z) / 8.0;

	return corners + sides + planes + center;
}

double PerlinNoise::PerlinNoise1d(double x)
{
	double total = 0;
	int n = NumberOfOctaves - 1;
	int frequency = 1;
	double amplitude = 1.0;

	for(int i = 0; i < n; i++)
	{
		total += InterpolatedNoise1d(x*frequency) * amplitude;

		frequency *= 2;
		amplitude *= persistence;
	}
	return total;
}

double PerlinNoise::PerlinNoise2d(double x, double y)
{
	double total = 0;
	int n = NumberOfOctaves - 1;
	int frequency = 1;
	double amplitude = 1.0;

	for(int i = 0; i < n; i++)
	{
		total += InterpolatedNoise2d(x*frequency, y*frequency) * amplitude;

		frequency *= 2;
		amplitude *= persistence;
	}
	return total;
}

double PerlinNoise::PerlinNoise3d(double x, double y, double z)
{
	double total = 0;
	int n = NumberOfOctaves - 1;
	int frequency = 1;
	double amplitude = 1.0;

	for(int i = 0; i < n; i++)
	{
		total += InterpolatedNoise3d(x*frequency, y*frequency, z*frequency) * amplitude;

		frequency *= 2;
		amplitude *= persistence;
	}
	return total;
}

double PerlinNoise::UseNoise1d(int x)
{
	return SmoothNoise1d(x);
}

double PerlinNoise::UseNoise2d(int x, int y)
{
	return SmoothNoise2d(x, y);
}

double PerlinNoise::UseNoise3d(int x, int y, int z)
{
	return SmoothNoise3d(x, y, z);
}


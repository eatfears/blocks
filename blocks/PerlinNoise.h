#pragma once
#include <boost/random.hpp>

#define LINEAR_INTERPOLATE		1
#define COSINE_INTERPOLATE		2

class PerlinNoise
{
public:
	PerlinNoise(double persistence = 0.5, int NumberOfOctaves = 4);
	~PerlinNoise();

	void InitNoise(boost::mt19937 *randNumGen);

	double PerlinNoise1d(double x);
	double PerlinNoise2d(double x, double y);
	double PerlinNoise3d(double x, double y, double z);

	double persistence;
	int NumberOfOctaves;

private:
	double Noise1d(int x);
	double Noise2d(int x, int y);
	double Noise3d(int x, int y, int z);

	double SmoothNoise1d(int x);
	double SmoothNoise2d(int x, int y);
	double SmoothNoise3d(int x, int y, int z);

	double UseNoise1d(int x);
	double UseNoise2d(int x, int y);
	double UseNoise3d(int x, int y, int z);

	double InterpolatedNoise1d(double x);
	double InterpolatedNoise2d(double x, double y);
	double InterpolatedNoise3d(double x, double y, double z);

	double LinearInterpolate(double a, double b, double x);
	double CosineInterpolate(double a, double b, double x);
	double CubicInterpolate(double v0, double v1, double v2, double v3, double x);
	double Interpolate(double a, double b, double x);

	int interpolation;

	int a, b, c, d, e;
};


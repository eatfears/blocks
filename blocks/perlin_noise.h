#pragma once
#include <boost/random.hpp>

#define LINEAR_INTERPOLATE		1
#define COSINE_INTERPOLATE		2

class PerlinNoise
{
public:
    PerlinNoise(double m_Persistence = 0.5, int m_NumberOfOctaves = 4);
    ~PerlinNoise();

    void initNoise(boost::mt19937 &randNumGen);

    double perlinNoise1d(double x);
    double perlinNoise2d(double x, double y);
    double perlinNoise3d(double x, double y, double z);

private:
    double noise1d(int x);
    double noise2d(int x, int y);
    double noise3d(int x, int y, int z);

    double smoothNoise1d(int x);
    double smoothNoise2d(int x, int y);
    double smoothNoise3d(int x, int y, int z);

    double useNoise1d(int x);
    double useNoise2d(int x, int y);
    double useNoise3d(int x, int y, int z);

    double interpolatedNoise1d(double x);
    double interpolatedNoise2d(double x, double y);
    double interpolatedNoise3d(double x, double y, double z);

    double linearInterpolate(double a, double b, double x);
    double cosineInterpolate(double a, double b, double x);
    double cubicInterpolate(double v0, double v1, double v2, double v3, double x);
    double interpolate(double a, double b, double x);

    int m_Interpolation;
    double m_Persistence;
    int m_NumberOfOctaves;

    int a, b, c, d, e;
};


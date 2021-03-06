#include "perlin_noise.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "primes.h"

PerlinNoise::PerlinNoise(double persistence, int NumberOfOctaves)
{
    PerlinNoise::m_Persistence = persistence;
    PerlinNoise::m_NumberOfOctaves = NumberOfOctaves;

    m_Interpolation = COSINE_INTERPOLATE;

    a = 15731;
    b = 789221;
    c = 1376312589;
    d = 57;
    e = 107;
}

void PerlinNoise::initNoise(std::mt19937 &randNumGen)
{
    a = Primes::genPrime(14, randNumGen);
    b = Primes::genPrime(20, randNumGen);
    c = Primes::genPrime(30, randNumGen);
    d = Primes::genPrime(6, randNumGen);
    e = Primes::genPrime(7, randNumGen);
}

double PerlinNoise::noise1d(int x) const noexcept
{
    x = (x<<13) ^ x;
    return (1.0 - ((x*(x*x*a + b) + c) & 0x7fffffff) / 1073741824.0);
}

double PerlinNoise::noise2d(int x, int y) const noexcept
{
    return noise1d(x + y*d);
}

double PerlinNoise::noise3d(int x, int y, int z) const noexcept
{
    return noise1d(x + (y + z*e)*d);
}

double PerlinNoise::interpolatedNoise1d(double x) const noexcept
{
    int i_x = floor(x);

    double frac_x = x - i_x;

    double v1 = useNoise1d(i_x);
    double v2 = useNoise1d(i_x + 1);

    return interpolate(v1, v2, frac_x);
}

double PerlinNoise::interpolatedNoise2d(double x, double y) const noexcept
{
    int i_x = floor(x);
    int i_y = floor(y);

    double frac_x = x - i_x;
    double frac_y = y - i_y;

    double v1 = useNoise2d(i_x, i_y);
    double v2 = useNoise2d(i_x + 1, i_y);
    double v3 = useNoise2d(i_x, i_y + 1);
    double v4 = useNoise2d(i_x + 1, i_y + 1);

    double i1 = interpolate(v1, v2, frac_x);
    double i2 = interpolate(v3, v4, frac_x);

    return interpolate(i1, i2, frac_y);
}

double PerlinNoise::interpolatedNoise3d(double x, double y, double z) const noexcept
{
    int i_x = floor(x);
    int i_y = floor(y);
    int i_z = floor(z);

    double frac_x = x - i_x;
    double frac_y = y - i_y;
    double frac_z = z - i_z;

    double v1 = useNoise3d(i_x, i_y, i_z);
    double v2 = useNoise3d(i_x + 1, i_y, i_z);
    double v3 = useNoise3d(i_x, i_y + 1, i_z);
    double v4 = useNoise3d(i_x + 1, i_y + 1, i_z);
    double v5 = useNoise3d(i_x, i_y, i_z + 1);
    double v6 = useNoise3d(i_x + 1, i_y, i_z + 1);
    double v7 = useNoise3d(i_x, i_y + 1, i_z + 1);
    double v8 = useNoise3d(i_x + 1, i_y + 1, i_z + 1);

    double i1 = interpolate(v1, v2, frac_x);
    double i2 = interpolate(v3, v4, frac_x);
    double i3 = interpolate(v5, v6, frac_x);
    double i4 = interpolate(v7, v8, frac_x);

    double w1 = interpolate(i1, i2, frac_y);
    double w2 = interpolate(i3, i4, frac_y);

    return interpolate(w1, w2, frac_z);
}

double PerlinNoise::linearInterpolate(double a, double b, double x) const noexcept
{
    return a*(1.0 - x) + b*x;
}

double PerlinNoise::cosineInterpolate(double a, double b, double x) const noexcept
{
    double ft = x * M_PI;
    double f = (1.0 - cos(ft)) * 0.5;

    return a*(1.0 - f) + b*f;
}

double PerlinNoise::cubicInterpolate(double v0, double v1, double v2, double v3, double x) const noexcept
{
    double P = (v3 - v2) - (v0 - v1);
    double Q = (v0 - v1) - P;
    double R = v2 - v0;
    double S = v1;

    return P*x*x*x + Q*x*x + R*x + S;
}

double PerlinNoise::interpolate(double a, double b, double x) const noexcept
{
    if(m_Interpolation == LINEAR_INTERPOLATE)
        return linearInterpolate(a, b, x);
    if(m_Interpolation == COSINE_INTERPOLATE)
        return cosineInterpolate(a, b, x);
    return 0;
}

double PerlinNoise::smoothNoise1d(int x) const noexcept
{
    return noise1d(x)/2.0 + noise1d(x-1)/4.0 + noise1d(x+1)/4.0;
}

double PerlinNoise::smoothNoise2d(int x, int y) const noexcept
{
    double corners  = (noise2d(x-1, y-1) + noise2d(x+1, y-1) +
                       noise2d(x-1, y+1) + noise2d(x+1, y+1)) / 16.0;

    double sides    = (noise2d(x-1, y) + noise2d(x+1, y) +
                       noise2d(x, y-1) + noise2d(x, y+1)) / 8.0;

    double center   = noise2d(x, y) / 4.0;

    return corners + sides + center;
}

double PerlinNoise::smoothNoise3d(int x, int y, int z) const noexcept
{
    double corners  = (noise3d(x-1, y-1, z-1) + noise3d(x+1, y-1, z-1) +
                       noise3d(x-1, y-1, z+1) + noise3d(x+1, y-1, z+1) +
                       noise3d(x-1, y+1, z-1) + noise3d(x+1, y+1, z-1) +
                       noise3d(x-1, y+1, z+1) + noise3d(x+1, y+1, z+1)) / 64.0;

    double sides    = (noise3d(x-1, y-1, z) + noise3d(x-1, y+1, z) +
                       noise3d(x+1, y-1, z) + noise3d(x+1, y+1, z) +
                       noise3d(x-1, y, z-1) + noise3d(x-1, y, z+1) +
                       noise3d(x+1, y, z-1) + noise3d(x+1, y, z+1) +
                       noise3d(x, y-1, z-1) + noise3d(x, y-1, z+1) +
                       noise3d(x, y+1, z-1) + noise3d(x, y+1, z+1)) / 32.0;

    double planes   = (noise3d(x-1, y, z) + noise3d(x+1, y, z) +
                       noise3d(x, y-1, z) + noise3d(x, y+1, z) +
                       noise3d(x, y, z-1) + noise3d(x, y, z+1)) / 16.0;

    double center   = noise3d(x, y, z) / 8.0;

    return corners + sides + planes + center;
}

double PerlinNoise::perlinNoise1d(double x) const noexcept
{
    double total = 0;
    int n = m_NumberOfOctaves - 1;
    int frequency = 1;
    double amplitude = 1.0;

    for(int i = 0; i < n; i++)
    {
        total += interpolatedNoise1d(x*frequency) * amplitude;

        frequency *= 2;
        amplitude *= m_Persistence;
    }
    return total;
}

double PerlinNoise::perlinNoise2d(double x, double y) const noexcept
{
    double total = 0;
    int n = m_NumberOfOctaves - 1;
    int frequency = 1;
    double amplitude = 1.0;

    for(int i = 0; i < n; i++)
    {
        total += interpolatedNoise2d(x*frequency, y*frequency) * amplitude;

        frequency *= 2;
        amplitude *= m_Persistence;
    }
    return total;
}

double PerlinNoise::perlinNoise3d(double x, double y, double z) const noexcept
{
    double total = 0;
    int n = m_NumberOfOctaves - 1;
    int frequency = 1;
    double amplitude = 1.0;

    for(int i = 0; i < n; i++)
    {
        total += interpolatedNoise3d(x*frequency, y*frequency, z*frequency) * amplitude;

        frequency *= 2;
        amplitude *= m_Persistence;
    }
    return total;
}

double PerlinNoise::useNoise1d(int x) const noexcept
{
    return smoothNoise1d(x);
}

double PerlinNoise::useNoise2d(int x, int y) const noexcept
{
    return smoothNoise2d(x, y);
}

double PerlinNoise::useNoise3d(int x, int y, int z) const noexcept
{
    return smoothNoise3d(x, y, z);
}

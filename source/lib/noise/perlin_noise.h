#pragma once
#include <random>

#define LINEAR_INTERPOLATE		1
#define COSINE_INTERPOLATE		2

class PerlinNoise
{
public:
    PerlinNoise(double m_Persistence = 0.5, int m_NumberOfOctaves = 4);

    void initNoise(std::mt19937 &randNumGen);

    inline double perlinNoise1d(double x) const noexcept;
    double perlinNoise2d(double x, double y) const noexcept;
    double perlinNoise3d(double x, double y, double z) const noexcept;

private:
    inline double noise1d(int x) const noexcept;
    inline double noise2d(int x, int y) const noexcept;
    inline double noise3d(int x, int y, int z) const noexcept;

    inline double smoothNoise1d(int x) const noexcept;
    inline double smoothNoise2d(int x, int y) const noexcept;
    inline double smoothNoise3d(int x, int y, int z) const noexcept;

    inline double useNoise1d(int x) const noexcept ;
    inline double useNoise2d(int x, int y) const noexcept;
    inline double useNoise3d(int x, int y, int z) const noexcept;

    inline double interpolatedNoise1d(double x) const noexcept;
    inline double interpolatedNoise2d(double x, double y) const noexcept;
    inline double interpolatedNoise3d(double x, double y, double z) const noexcept;

    inline double linearInterpolate(double a, double b, double x) const noexcept;
    inline double cosineInterpolate(double a, double b, double x) const noexcept;
    inline double cubicInterpolate(double v0, double v1, double v2, double v3, double x) const noexcept;
    inline double interpolate(double a, double b, double x) const noexcept;

    int m_Interpolation;
    double m_Persistence;
    int m_NumberOfOctaves;

    int a, b, c, d, e;
};

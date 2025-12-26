#pragma once

#include <cmath>

static int perm[512] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,
    69,142, // ... truncated for brevity
};

//Duplicate the permutation table
static int p[512];
static bool p_init = false;

static void initPerm()
{
    if(p_init) return;

    for(int i=0; i < 256; i++)
    {
        p[256 + i] = perm[i];
    }
    p_init = true;
}

static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static float lerp(float a, float b, float t) { return a + t *(b - a) ; }
static float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h <4 ? y: (h == 12 || h ==14 ? x : z);

    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

static float perlin(float x, float y, float z)
{
    initPerm();

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A  = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B  = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    return lerp(
        lerp(
            lerp(grad(p[AA], x, y, z),
                 grad(p[BA], x - 1, y, z), u),
            lerp(grad(p[AB], x, y - 1, z),
                 grad(p[BB], x - 1, y - 1, z), u),
            v),
        lerp(
            lerp(grad(p[AA + 1], x, y, z - 1),
                 grad(p[BA + 1], x - 1, y, z - 1), u),
            lerp(grad(p[AB + 1], x, y - 1, z - 1),
                 grad(p[BB + 1], x - 1, y - 1, z - 1), u),
            v),
        w);
}
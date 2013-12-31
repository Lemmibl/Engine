#pragma once
#include <math.h>
#include "StructsAndEnums.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

class NoiseClass
{
public:
	NoiseClass();
	~NoiseClass();

	float Noise2D(float xin, float yin);
	float SimplexNoise3D(float xin, float yin, float zin);
	float SimplexNoise3DZeroToOne(float xin, float yin, float zin);
	float SimplexNoise4D(float x, float y, float z, float w);
	float PerlinNoise3D(float x, float y,  float z);

	void ReseedRandom();

	int perm[512];
	int p[260];
	float grads[512];


private:
	inline int MFloatToInt(float x)
	{
		x -= 0.5f;
		int t;
		__asm fld x;
		__asm fistp t;

		return t;
	}

	inline int fastfloor(float x) 
	{
		int xi = (int)x;
		return x < xi ? xi-1 : xi;
	}

	inline float Fade(float t) 
		{ return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

	inline float Lerp(float t, float a, float b) 
		{ return a + t * (b - a); }

	inline float Grad(int hash, float x, float y, float z) 
	{
		int h = hash & 15;                     // CONVERT LO 4 BITS OF HASH CODE
		float u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
			v = h<4 ? y : h==12||h==14 ? x : z;
		return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
	}

	inline float dot(int g[], float x, float y) 
	{
		return g[0]*x + g[1]*y; 
	}

	inline float dot(int g[], float x, float y, float z) 
	{
		return g[0]*x + g[1]*y + g[2]*z; 
	}

	inline float dot(int g[], float x, float y, float z, float w) 
	{
		return g[0]*x + g[1]*y + g[2]*z + g[3]*w; 
	}

	inline float dot(const Grads& g, float x, float y) 
	{
		return x*g.x + y*g.y; 
	}

	inline float dot(const Grads& g, float x, float y, float z)
	{
		return x*g.x + y*g.y + z*g.z; 
	}

	inline float dot(const int* g, const float x, const float y, const float z, const float w)
	{
		return g[0]*x + g[1]*y + g[2]*z + g[3]*w;
	}
};


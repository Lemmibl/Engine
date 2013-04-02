#pragma once
#include <math.h>
#include "structs.h"
#include <stdlib.h>
#include <stdio.h>

class SimplexNoise
{
public:
	SimplexNoise(void);
	~SimplexNoise(void);

	float noise2D(float xin, float yin);
	float noise3D(float xin, float yin, float zin);
	float noise3D2(float xin, float yin, float zin);

	int perm[512];

	int p[256];

	
private:
	int fastfloor(float x) {return x>0 ? (int)x : (int)x-1;}/*
	
	float dot(int g[], float x, float y) {return g[0]*x + g[1]*y; }

	float dot(int g[], float x, float y, float z) {return g[0]*x + g[1]*y + g[2]*z; }

	float dot(int g[], float x, float y, float z, float w) {return g[0]*x + g[1]*y + g[2]*z + g[3]*w;}

	float dot(int g[],int i, float x, float y) {return g[i]*x + g[i+1]*y; }

	float dot(int g[],int i, float x, float y, float z) {return g[i]*x + g[i+1]*y + g[i+2]*z; }

	float dot(int g[],int i, float x, float y, float z, float w) {return g[i]*x + g[i+1]*y + g[i+2]*z + g[i+3]*w;}*/

	

static double dot(int g[], float x, float y) {
	return g[0]*x + g[1]*y; }

static double dot(int g[], float x, float y, float z) {
	return g[0]*x + g[1]*y + g[2]*z; }

static double dot(int g[], float x, float y, float z, float w) {
	return g[0]*x + g[1]*y + g[2]*z + g[3]*w; }


static double dot(Grads g, float x, float y, float z) {
	return g.x*x + g.y*y + g.z*z; }

};


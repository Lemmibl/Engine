#include "SimplexNoise.h"

//http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java

const static Grads grad3 [] = 
{
	Grads(1,1,0),
	Grads(-1,1,0),
	Grads(1,-1,0),
	Grads(-1,-1,0),
	Grads(1,0,1),
	Grads(-1,0,1),
	Grads(1,0,-1),
	Grads(-1,0,-1),
	Grads(0,1,1),
	Grads(0,-1,1),
	Grads(0,1,-1),
	Grads(0,-1,-1)
};

struct DoubleXYZCoords
{
	int x1, y1, z1;
	int x2, y2, z2;

	DoubleXYZCoords(int x, int y, int z, int xx, int yy, int zz)
	{
		x1 = x;		y1 = y;		z1 = z;
		x2 = xx;	y2 = yy;	z2 = zz;
	}
};

static DoubleXYZCoords coords [] =
{
	DoubleXYZCoords(1, 0, 0, 1, 1, 0),
	DoubleXYZCoords(1, 0, 0, 1, 0, 1),
	DoubleXYZCoords(0, 0, 1, 1, 0, 1),

	DoubleXYZCoords(0, 0, 1, 0, 1, 1),
	DoubleXYZCoords(0, 1, 0, 0, 1, 1),
	DoubleXYZCoords(0, 1, 0, 1, 1, 0),
};

// Skew the input space to determine which simplex cell we're in
static const float F3 = 1.0f/3.0f; //1.0f / 3.0f
static const float G3 = 1.0f/6.0f; // Very nice and simple unskew factor, too. 1.0f / 6.0f
static const float G3TimesTwo = (1.0f/6.0f)*2.0f;
static const float G3TimesThree = (3.0f*(1.0f/6.0f));

SimplexNoise::SimplexNoise()
{
	ReseedRandom();
}

void SimplexNoise::ReseedRandom()
{
	//260 instead of 256 to avoid out of bounds errors that appear.
	for(int i=0; i<260; i++)
	{
		//0-250 instead of 0-256 to again avoid out of bounds errors. I don't even fucking know.
		p[i] = (rand() % 250);
	}

	for(int i=0; i<512; i++)
	{
		perm[i] = 1+(rand() % 255);
	}

	//for(int i=0; i<512; i++)
	//{
	//	float x = (-1.0f + 2.0f*(static_cast<float>(rand()) / static_cast<float>(RAND_MAX))); // * 0.7f; //
	//	float y = (-1.0f + 2.0f*(static_cast<float>(rand()) / static_cast<float>(RAND_MAX))); // * 0.7f; //
	//	float z = (-1.0f + 2.0f*(static_cast<float>(rand()) / static_cast<float>(RAND_MAX))); // * 0.7f; //

	//	grads[i] = Grad(p[i & 255], x, y, z);
	//}
}

SimplexNoise::~SimplexNoise()
{
}

//2D noise
float SimplexNoise::Noise2D(float xin, float yin)
{
	float n0, n1, n2;

	// Noise contributions from the three corners
	// Skew the input space to determine which simplex cell we're in
	float F2 = 0.5f*(float)(sqrt(3.0f)-1.0f);
	float s = (xin+yin)*F2;

	// Hairy factor for 2D
	int i = MFloatToInt(xin+s);
	int j = MFloatToInt(yin+s);
	float G2 = (float)(3.0f-sqrt(3.0f))/6.0f;
	float t = (i+j)*G2;
	float X0 = i-t;

	// Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	float x0 = xin-X0;

	// The x,y distances from the cell origin
	float y0 = yin-Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1;

	// Ofsets for second (middle) corner of simplex in (i,j) coords
	if(x0>y0) {i1=1; j1=0;}

	// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {i1=0; j1=1;}

	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	float x1 = x0 - i1 + G2;

	// Ofsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;

	// Ofsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii+perm[jj]]			% 12;
	int gi1 = perm[ii+i1+perm[jj+j1]]	% 12;
	int gi2 = perm[ii+1+perm[jj+1]]		% 12;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0*x0-y0*y0;
	if(t0<0) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
	}

	float t1 = 0.5f - x1*x1-y1*y1;
	if(t1<0) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	float t2 = 0.5f - x2*x2-y2*y2;
	if(t2<0) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 72.0f * (n0 + n1 + n2);
}

float SimplexNoise::SimplexNoise3D(float xin, float yin, float zin) 
{
	float n0, n1, n2, n3; // Noise contributions from the four corners

	const float s = (xin+yin+zin)*F3; // Very nice and simple skew factor for 3D
	const int i = MFloatToInt(xin+s);
	const int j = MFloatToInt(yin+s);
	const int k = MFloatToInt(zin+s);

	const float t = (i+j+k)*G3;

	//float X0 = i-t; // Unskew the cell origin back to (x,y,z) space
	//float Y0 = j-t;
	//float Z0 = k-t;

	const float x0 = xin-(i-t); // The x,y,z distances from the cell origin
	const float y0 = yin-(j-t);
	const float z0 = zin-(k-t);

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	DoubleXYZCoords* tempCoords;

	if(x0>=y0) 
	{
		if(y0>=z0){ tempCoords = &coords[0]; } // X Y Z order
		else if(x0>=z0) { tempCoords = &coords[1]; } // X Z Y order
		else { tempCoords = &coords[2]; } // Z X Y order
	}
	else 
	{ // x0<y0
		if(y0<z0) { tempCoords = &coords[3]; } // Z Y X order
		else if (x0<z0) { tempCoords = &coords[4]; } // Y Z X order
		else { tempCoords = &coords[5]; } // Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	const float x1 = x0 - tempCoords->x1 + G3; // Ofsets for second corner in (x,y,z) coords
	const float y1 = y0 - tempCoords->y1 + G3;
	const float z1 = z0 - tempCoords->z1 + G3;
	const float x2 = x0 - tempCoords->x2 + G3TimesTwo; // Offsets for third corner in (x,y,z) coords
	const float y2 = y0 - tempCoords->y2 + G3TimesTwo;
	const float z2 = z0 - tempCoords->z2 + G3TimesTwo;
	const float x3 = x0 - 1.0f + G3TimesThree; // Offsets for last corner in (x,y,z) coords
	const float y3 = y0 - 1.0f + G3TimesThree;
	const float z3 = z0 - 1.0f + G3TimesThree;

	// Work out the hashed gradient indices of the four simplex corners
	const int ii = i & 255;
	const int jj = j & 255;
	const int kk = k & 255;

	const int gi0 = perm[ii+perm[jj+perm[kk]]]			% 12;
	const int gi1 = perm[ii+tempCoords->x1+perm[jj+tempCoords->y1+perm[kk+tempCoords->z1]]]	% 12;
	const int gi2 = perm[ii+tempCoords->x2+perm[jj+tempCoords->y2+perm[kk+tempCoords->z2]]]	% 12;
	const int gi3 = perm[ii+1+perm[jj+1+perm[kk+1]]]	% 12;

	n0 = n1 = n2 = n3 = 0.0f;

	// Calculate the contribution from the four corners
	float t0 = 0.5f - x0*x0 - y0*y0 - z0*z0;
	if(t0 >= 0)
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0, z0);
	}

	float t1 = 0.5f - x1*x1 - y1*y1 - z1*z1;
	if(t1 >= 0)
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1, z1);
	}

	float t2 = 0.5f - x2*x2 - y2*y2 - z2*z2;
	if(t2 >= 0)
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2, z2);
	}

	float t3 = 0.5f - x3*x3 - y3*y3 - z3*z3;
	if(t3 >= 0)
	{
		t3 *= t3;
		n3 = t3 * t3 * dot(grad3[gi3], x3, y3, z3);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]

	//return (36.0f*(n0 + n1 + n2 + n3)+1)*0.5f;// 0 -> 1
	return (32.0f*(n0 + n1 + n2 + n3)); //-1 -> 1
}

float SimplexNoise::SimplexNoise3DZeroToOne( float xin, float yin, float zin )
{
	return (1.0f+SimplexNoise3D(xin, yin, zin))*0.5f;
}

//http://mrl.nyu.edu/~perlin/noise/
//float SimplexNoise::PerlinNoise3D( float x, float y, float z )
//{
//		int X = fastfloor(x);                  // FIND UNIT CUBE THAT
//		int	Y = fastfloor(y);                  // CONTAINS POINT.
//		int	Z = fastfloor(z);
//		x -= X;                                // FIND RELATIVE X,Y,Z
//		y -= Y;                                // OF POINT IN CUBE.
//		z -= Z;
//
//		X = X & 255;
//		Y = Y & 255;
//		Z = Z & 255;
//
//		float u = Fade(x);                                // COMPUTE FADE CURVES
//		float v = Fade(y);                                // FOR EACH OF X,Y,Z.
//		float w = Fade(z);
//		int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
//			B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,
//
//		float returnValue = Lerp(w, Lerp(v, Lerp(u, Grad(p[AA  ], x  , y  , z   ),	// AND ADD
//			Grad(p[BA  ], x-1, y  , z   )),								// BLENDED
//			Lerp(u, Grad(p[AB  ], x  , y-1, z   ),						// RESULTS
//			Grad(p[BB  ], x-1, y-1, z   ))),							// FROM  8
//			Lerp(v, Lerp(u, Grad(p[AA+1], x  , y  , z-1 ),				// CORNERS
//			Grad(p[BA+1], x-1, y  , z-1 )),								// OF CUBE
//			Lerp(u, Grad(p[AB+1], x  , y-1, z-1 ),
//			Grad(p[BB+1], x-1, y-1, z-1 ))));
//
//		//"Optimized" version that produces slightly blockier noise.
//		//http://www.gamedev.net/blog/73/entry-1382657-fast-perlin-noise/
//		//float returnValue = 		
//		//	(
//		//	Lerp(w, 
//		//	Lerp(v, Lerp(u, grads[AA],		grads[BA]), 
//		//	Lerp(u, grads[AB],		grads[BB])),
//
//		//	Lerp(v,	Lerp(u, grads[AA + 1],	grads[BA + 1]), 
//		//	Lerp(u, grads[AB + 1],	grads[BB + 1]))
//		//	)
//		//	);
//
//		return returnValue;
//}
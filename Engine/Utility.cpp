#include "Utility.h"

Utility::Utility()
{

}

Utility::Utility( const Utility& )
{

}

Utility::~Utility()
{

}

float Utility::VectorDistance( XMFLOAT3 vector1, XMFLOAT3 vector2 )
{
	//Not sqrt and all that. The fast version.
	return(abs(vector1.x-vector2.x)+abs(vector1.y-vector2.y)+abs(vector1.z-vector2.z));
}

float Utility::RandomFloat()
{
	float scale=RAND_MAX+1.0f;
	float base=rand()/scale;
	float fine=rand()/scale;
	return base+fine/scale;
}
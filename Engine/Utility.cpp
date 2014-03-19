#include "Utility.h"


Utility::Utility()
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

////Based on world Y position and surface normal, we determine material and texture ID.
//	//If you don't want to blend between the materials, just set both materials as same. Same applies to the textures. We'll be doing a comparison check in the shaders
//	//To make sure we don't blend two identical textures or materials
//	//Same goes for lerp, if you don't want to lerp anything.. leave it at default value.
//	void Utility::GenerateMaterialAndTextureData(const float WorldPositionY, const XMFLOAT3* const normal, float &lerpValue, unsigned int &materialAndTextureIDs )
//	{
//		uint8_t materialAndTextureValues[4];
//		unsigned int material1, material2, texture1, texture2;
//		float lerp = 0.0f, dotResult = 0.0f;
//		const XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
//
//		//Does dot between two vectors and gets the float value...
//		//XMVectorDot returns the dot value to each channel, so I just fetch the x channel, but it really doesn't matter
//		dotResult = XMVectorGetX(XMVector3Dot(XMLoadFloat3(normal), upVector));
//
//		//Big clunky bunch of if()elseIf(), but it does the job.
//		if(WorldPositionY <= 30.0f)
//		{
//			texture1 = TEXTURE_ROCK;
//			texture2 = TEXTURE_ROCK;
//
//			material1 = MATERIAL_ROCK;
//			material2 = material1;
//		}
//		else if(WorldPositionY <= 30.8f)
//		{
//			lerp = 30.0f;
//
//			texture1 = TEXTURE_ROCK;
//			texture2 = TEXTURE_GRASS;
//
//			material1 = MATERIAL_ROCK;
//			material2 = MATERIAL_GRASS;
//		}
//		else if(WorldPositionY <= 44.8f)
//		{
//			texture1 = TEXTURE_GRASS;
//			texture2 = TEXTURE_GRASS;
//
//			material1 = MATERIAL_GRASS;
//			material2 = material1;
//		}
//		else if( WorldPositionY <= 45.8f)
//		{
//			lerp = 45.0f;
//			texture1 = TEXTURE_GRASS;
//			texture2 = TEXTURE_ROCK;
//
//			material1 = MATERIAL_GRASS;
//			material2 = MATERIAL_ROCK;
//		}
//		else if(WorldPositionY <= 46.8f)
//		{
//			lerp = 46.0f;
//			texture1 = TEXTURE_ROCK;
//			texture2 = TEXTURE_SNOW;
//
//			material1 = MATERIAL_ROCK;
//			material2 = MATERIAL_SNOW;
//		}
//		else
//		{
//			texture1 = TEXTURE_SNOW; 
//			texture2 = TEXTURE_SNOW;
//
//			material1 = MATERIAL_SNOW;
//			material2 = material1;	
//		}
//		
//		//If surface is facing downwards, change textures but not materials
//		if(dotResult <= 0.0f)
//		{
//			texture1 = TEXTURE_ROCK;
//			texture2 = TEXTURE_ROCK;
//		}
//		 
//		lerpValue = lerp;
//
//		materialAndTextureValues[0] = texture1;
//		materialAndTextureValues[1] = texture2;
//		materialAndTextureValues[2] = material1;
//		materialAndTextureValues[3] = material2;
//
//		materialAndTextureIDs = PackIDsIntoInt32(materialAndTextureValues);
//	}

	//http://stackoverflow.com/questions/5772193/how-can-i-store-4-char-into-an-unsigned-int-using-bitwise-operation
	//Name should be self explanatory. The int will be used in the gbuffer shader later.
	uint32_t Utility::PackIDsIntoInt32( uint8_t data[4] )
	{
		uint32_t final = 0;
		final |= ( data[0] << 24 );
		final |= ( data[1] << 16 );
		final |= ( data[2] <<  8 );
		final |= ( data[3]       );

		return final;
	}

	float Utility::ConvertFromOneRangeToAnother(float OldValue, float OldMin, float OldMax, float NewMin, float NewMax)
	{
		//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
		return ((OldValue - OldMin) * (NewMax-NewMin) / (OldMax-OldMin)+NewMin);
	}

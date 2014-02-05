#pragma once

//OK, so these enums are used as identifiers for my DebugOverlayHUD handles, so that I can reuse the same functions and objects for different types while at the same time safely
namespace DataTypeEnumMappings
{
	typedef unsigned short DataType;
	enum
	{
		Int32 = 0,
		UInt32,
		Float,
		Bool,
		String,
		Float2,
		Float3,
		Float4,
		ITEM_COUNT //Always keep last
	};
};

struct DebugWindowHandle
{
	//ID to extract the right widget
	unsigned short id;

	//Datatype so that we can doublecheck that we're receiving the right data
	//DataTypeEnumMappings::DataType dataType;
};
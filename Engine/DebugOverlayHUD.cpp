#include "DebugOverlayHUD.h"

DebugOverlayHUD::DebugOverlayHUD()
	: currentActiveWidgets(0)
{
}

DebugOverlayHUD::~DebugOverlayHUD()
{
}

void DebugOverlayHUD::SetHUDVisibility( bool visible )
{
	if(visible == true)
	{
		rootWindow->activate();
		rootWindow->setVisible(true);

		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);
	}
	else
	{
		rootWindow->setVisible(false);
	}
}

bool DebugOverlayHUD::Initialize( float startPosX, float startPosY )
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	rootWindow = winMgr.createWindow("DefaultWindow", "debugRoot");
	rootWindow->setVisible(true);
	rootWindow->setSize( CEGUI::USize( CEGUI::UDim( 1.0f, 0.0f ), CEGUI::UDim( 1.0f, 0.0f ) ));
	rootWindow->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.0f, 0.0f ), CEGUI::UDim( 0.0f, 0.0f ) ));
	rootWindow->activate();

	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	//Populate prio queue with values 0 to 127, whilst ordering them properly.
	for(unsigned short i = 0; i < MaxWidgetCount; i++)
	{
		prioQueue.push(i);
	}

	return true;
}

bool DebugOverlayHUD::CreateDebugWindow(std::string identifier, DebugWindowHandle& outHandle)
{
	if(currentActiveWidgets < MaxWidgetCount)
	{
		outHandle.id = prioQueue.top();
		prioQueue.pop();
		currentActiveWidgets++;

		widgets[outHandle.id].identifierString = identifier;

		CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

		/* http://cegui.org.uk/forum/viewtopic.php?p=22795#p22795 */

		widgets[outHandle.id].ceguiWindow = winMgr.createWindow("TaharezLook/Label", identifier);
		rootWindow->addChild(widgets[outHandle.id].ceguiWindow);

		widgets[outHandle.id].ceguiWindow->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(20*outHandle.id)));
		widgets[outHandle.id].ceguiWindow->setRiseOnClickEnabled(false);
		widgets[outHandle.id].ceguiWindow->setPixelAligned(true);
		widgets[outHandle.id].ceguiWindow->setInheritsAlpha(true);
		widgets[outHandle.id].ceguiWindow->setAlpha(1.0f);
		widgets[outHandle.id].ceguiWindow->setProperty("HorzFormatting", "LeftAligned");
		widgets[outHandle.id].ceguiWindow->setProperty("VertFormatting", "TopAligned");
		widgets[outHandle.id].ceguiWindow->setWidth(CEGUI::UDim(5.0f, 0.0f));
		widgets[outHandle.id].ceguiWindow->setHeight(CEGUI::UDim(5.0f, 0.0f));

		//Potentially just call UpdateWindow instead after we've created the window
		widgets[outHandle.id].ceguiWindow->setText("Test nr: " + std::to_string((long long)currentActiveWidgets));
		widgets[outHandle.id].ceguiWindow->activate();

		//Success
		return true;
	}

	//Fail, no more free widget slots available
	return false;
}

#pragma region UpdateWindowText specializations
/************************************************************************/
/* Update Window Text specializations for expected use cases            */
/************************************************************************/

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, bool* val )
{
	if(*val == true)
	{
		widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": True");
	}
	else
	{
		widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": False");
	}
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, int* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": " + CEGUI::PropertyHelper<int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, unsigned int* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": " + CEGUI::PropertyHelper<unsigned int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, std::string* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": " + *val);
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, float* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + ": " + CEGUI::PropertyHelper<float>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT2* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) + 
		" Y: " + CEGUI::PropertyHelper<float>::toString((*val).y));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT3* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) +
		" Y: " + CEGUI::PropertyHelper<float>::toString((*val).y) + " Z: " + CEGUI::PropertyHelper<float>::toString((*val).z));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT4* val )
{
	widgets[handle.id].ceguiWindow->setText(widgets[handle.id].identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) + 
		" Y: " + CEGUI::PropertyHelper<float>::toString((*val).y) + " Z: " + CEGUI::PropertyHelper<float>::toString((*val).z) + " W: " + CEGUI::PropertyHelper<float>::toString((*val).w));
}
#pragma endregion

#pragma region Add new window stuff
bool DebugOverlayHUD::AddNewWindow(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType, DebugWindowHandle& outHandle)
{
	//If this call succeeded, we continue
	if(CreateDebugWindow(name, outHandle))
	{
		widgets[outHandle.id].dataType = dataType;
		widgets[outHandle.id].valuePointer = valueToBeMonitored;

		UpdateWidget(outHandle, dataType);

		return true;
	}

	return false;
}

bool DebugOverlayHUD::AddNewWindowWithoutHandle(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType)
{
	DebugWindowHandle outHandle;

	//If this call succeeded, we continue
	if(CreateDebugWindow(name, outHandle))
	{
		widgets[outHandle.id].dataType = dataType;
		widgets[outHandle.id].valuePointer = valueToBeMonitored;

		UpdateWidget(outHandle, dataType);

		return true;
	}

	return false;
}

void DebugOverlayHUD::UpdateWidget(DebugWindowHandle& handle, DataTypeEnumMappings::DataType dataType )
{
	switch(dataType)
	{
	case DataTypeEnumMappings::Bool:
		{	
			UpdateWindowText(handle, (bool*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::UInt32:
		{
			UpdateWindowText(handle, (unsigned int*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::Int32:
		{
			UpdateWindowText(handle, (int*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::String:
		{
			UpdateWindowText(handle, (std::string*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::Float:
		{
			UpdateWindowText(handle, (float*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::Float2:
		{
			UpdateWindowText(handle,  (XMFLOAT2*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::Float3:
		{
			UpdateWindowText(handle,  (XMFLOAT3*)((widgets[handle.id].valuePointer)));
		} break;

	case DataTypeEnumMappings::Float4:
		{
			UpdateWindowText(handle,  (XMFLOAT4*)((widgets[handle.id].valuePointer)));
		} break;
	}
}
#pragma endregion

bool DebugOverlayHUD::Update()
{
	//Because casting from ushort to a struct only containing an ushort is iffy
	DebugWindowHandle temp;

	for(unsigned short i = 0; i < currentActiveWidgets; i++)
	{
		temp.id = i;

		UpdateWidget(temp, widgets[i].dataType);
	}

	return true;
}

void DebugOverlayHUD::RemoveWindow(DebugWindowHandle handle)
{
	//Erase ID
	//widgets[handle.id];

	//TODO: Key container

	//Insert this index to be reused
	prioQueue.push(handle.id);
}

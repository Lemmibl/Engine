#include "DebugOverlayHUD.h"

DebugOverlayHUD::DebugOverlayHUD()
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
	widgets = std::make_shared<DODContainer<WindowContainer>>(MaxWidgetCount);


	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	rootWindow = winMgr.createWindow("DefaultWindow", "debugRoot");
	rootWindow->setVisible(true);
	rootWindow->setSize( CEGUI::USize( CEGUI::UDim( 1.0f, 0.0f ), CEGUI::UDim( 1.0f, 0.0f ) ));
	rootWindow->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.0f, 0.0f ), CEGUI::UDim( 0.0f, 0.0f ) ));
	rootWindow->activate();

	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	////Populate prio queue with values 0 to 127, whilst ordering them properly.
	//for(unsigned short i = 0; i < MaxWidgetCount; i++)
	//{
	//	prioQueue.push(i);
	//	internalHandles[i].id = i;
	//}

	return true;
}

bool DebugOverlayHUD::CreateDebugWindow(std::string identifier, DebugWindowHandle& outHandle)
{
	unsigned short newHandle;

	//TODO
	if(widgets->AddNewObject(newHandle))
	{
		outHandle.id = newHandle;

		WindowContainer* widget = widgets->GetSpecificObject(newHandle);

		widget->identifierString = identifier;

		CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

		/* http://cegui.org.uk/forum/viewtopic.php?p=22795#p22795 */

		widget->ceguiWindow = winMgr.createWindow("TaharezLook/Label", identifier);
		rootWindow->addChild(widget->ceguiWindow);

		widget->ceguiWindow->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(20.0f*(float)outHandle.id)));
		widget->ceguiWindow->setRiseOnClickEnabled(false);
		widget->ceguiWindow->setPixelAligned(true);
		widget->ceguiWindow->setInheritsAlpha(true);
		widget->ceguiWindow->setAlpha(1.0f);
		widget->ceguiWindow->setProperty("HorzFormatting", "LeftAligned");
		widget->ceguiWindow->setProperty("VertFormatting", "TopAligned");
		widget->ceguiWindow->setWidth(CEGUI::UDim(5.0f, 0.0f));
		widget->ceguiWindow->setHeight(CEGUI::UDim(5.0f, 0.0f));
		widget->ceguiWindow->activate();

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
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	if(*val == true)
	{
		widget->ceguiWindow->setText(widget->identifierString + ": True");
	}
	else
	{
		widget->ceguiWindow->setText(widget->identifierString + ": False");
	}
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, int* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, unsigned int* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<unsigned int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, std::string* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + ": " + *val);
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, float* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<float>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT2* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) + 
		" Y: " + CEGUI::PropertyHelper<float>::toString((*val).y));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT3* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) +	" Y: " + 
		CEGUI::PropertyHelper<float>::toString((*val).y) + " Z: " + CEGUI::PropertyHelper<float>::toString((*val).z));
}

template<> void DebugOverlayHUD::UpdateWindowText(DebugWindowHandle handle, XMFLOAT4* val )
{
	WindowContainer* widget = widgets->GetSpecificObject(handle.id);

	widget->ceguiWindow->setText(widget->identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) + " Y: " + 
		CEGUI::PropertyHelper<float>::toString((*val).y) + " Z: " + CEGUI::PropertyHelper<float>::toString((*val).z) + " W: " + CEGUI::PropertyHelper<float>::toString((*val).w));
}
#pragma endregion

#pragma region Add new window stuff
bool DebugOverlayHUD::AddNewWindow(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType, DebugWindowHandle& outHandle)
{
	//If this call succeeded, we continue
	if(CreateDebugWindow(name, outHandle))
	{
		auto widget = widgets->GetSpecificObject(outHandle.id);

		widget->dataType = dataType;
		widget->valuePointer = valueToBeMonitored;

		UpdateWidget(outHandle);

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
		auto widget = widgets->GetSpecificObject(outHandle.id);

		widget->dataType = dataType;
		widget->valuePointer = valueToBeMonitored;

		UpdateWidget(outHandle);

		return true;
	}

	return false;
}

void DebugOverlayHUD::UpdateWidget(DebugWindowHandle& handle)
{
	auto widget = widgets->GetSpecificObject(handle.id);
	widget->ceguiWindow->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(20*handle.id)));

	switch(widget->dataType)
	{
	case DataTypeEnumMappings::Bool:
		{	
			UpdateWindowText(handle,	static_cast<bool*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::UInt32:
		{
			UpdateWindowText(handle,	static_cast<unsigned int*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Int32:
		{
			UpdateWindowText(handle,	static_cast<int*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::String:
		{
			UpdateWindowText(handle,	static_cast<std::string*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float:
		{
			UpdateWindowText(handle,	static_cast<float*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float2:
		{
			UpdateWindowText(handle,	static_cast<XMFLOAT2*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float3:
		{
			UpdateWindowText(handle,	static_cast<XMFLOAT3*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float4:
		{
			UpdateWindowText(handle,	static_cast<XMFLOAT4*>(widget->valuePointer));
		} break;
	}
}
#pragma endregion

bool DebugOverlayHUD::Update()
{
	//Because casting from ushort to a struct only containing an ushort is iffy
	DebugWindowHandle temp;

	for(unsigned short i = 0; i < widgets->GetActiveObjectCount(); i++)
	{
		temp.id = i;

		UpdateWidget(temp);
	}

	return true;
}

void DebugOverlayHUD::RemoveWindow(DebugWindowHandle handle)
{
	auto widget = widgets->GetSpecificObject(handle.id);

	widget->identifierString = "";
	widget->valuePointer = nullptr;

	rootWindow->removeChild(widget->ceguiWindow);
	widgets->RemoveObject(handle.id);
}

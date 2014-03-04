#include "DebugOverlayHUD.h"

DebugOverlayHUD::DebugOverlayHUD()
{
	totalWindowsCount = 0;
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

	if(widgets->AddNewObject(newHandle))
	{
		outHandle.id = newHandle;

		WindowContainer* widget = widgets->GetSpecificObject(newHandle);

		CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

		/* http://cegui.org.uk/forum/viewtopic.php?p=22795#p22795 */

		widget->identifierString = identifier;//std::to_string(totalWindowsCount++);

		widget->ceguiWindow = winMgr.createWindow("TaharezLook/Label", widget->identifierString);
		rootWindow->addChild(widget->ceguiWindow);

		widget->ceguiWindow->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(25.0f*(float)outHandle.id)));
		widget->ceguiWindow->setRiseOnClickEnabled(false);
		widget->ceguiWindow->setPixelAligned(false);
		widget->ceguiWindow->setInheritsAlpha(true);
		widget->ceguiWindow->setAlpha(1.0f);
		widget->ceguiWindow->setProperty("HorzFormatting", "LeftAligned");
		widget->ceguiWindow->setProperty("VertFormatting", "TopAligned");
		widget->ceguiWindow->setWidth(CEGUI::UDim(5.0f, 0.0f));
		widget->ceguiWindow->setHeight(CEGUI::UDim(1.0f, 0.0f));
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

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, bool* val )
{
	if(*val == true)
	{
		widget->ceguiWindow->setText(widget->identifierString + ": True");
	}
	else
	{
		widget->ceguiWindow->setText(widget->identifierString + ": False");
	}
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, int* val )
{
	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, unsigned int* val )
{
	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<unsigned int>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, std::string* val )
{
	widget->ceguiWindow->setText(widget->identifierString + ": " + *val);
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, float* val )
{
	widget->ceguiWindow->setText(widget->identifierString + ": " + CEGUI::PropertyHelper<float>::toString(*val));
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, XMFLOAT2* val )
{
	widget->ceguiWindow->setText(widget->identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) + 
		" Y: " + CEGUI::PropertyHelper<float>::toString((*val).y));
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, XMFLOAT3* val )
{
	widget->ceguiWindow->setText(widget->identifierString + " X: " + CEGUI::PropertyHelper<float>::toString((*val).x) +	" Y: " + 
		CEGUI::PropertyHelper<float>::toString((*val).y) + " Z: " + CEGUI::PropertyHelper<float>::toString((*val).z));
}

template<> void DebugOverlayHUD::UpdateWindowText(WindowContainer* widget, XMFLOAT4* val )
{
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

		UpdateWidget(widget, outHandle.id);

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

		UpdateWidget(widget, outHandle.id);

		return true;
	}

	return false;
}

void DebugOverlayHUD::UpdateWidget(WindowContainer* widget, unsigned short index)
{
	widget->ceguiWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.0f, 20.0f*(float)index)));

	switch(widget->dataType)
	{
	case DataTypeEnumMappings::Bool:
		{	
			UpdateWindowText(widget,	static_cast<bool*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::UInt32:
		{
			UpdateWindowText(widget,	static_cast<unsigned int*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Int32:
		{
			UpdateWindowText(widget,	static_cast<int*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::String:
		{
			UpdateWindowText(widget,	static_cast<std::string*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float:
		{
			UpdateWindowText(widget,	static_cast<float*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float2:
		{
			UpdateWindowText(widget,	static_cast<XMFLOAT2*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float3:
		{
			UpdateWindowText(widget,	static_cast<XMFLOAT3*>(widget->valuePointer));
		} break;

	case DataTypeEnumMappings::Float4:
		{
			UpdateWindowText(widget,	static_cast<XMFLOAT4*>(widget->valuePointer));
		} break;
	}
}
#pragma endregion

bool DebugOverlayHUD::Update()
{
	for(unsigned short i = 0; i < widgets->GetActiveObjectCount(); i++)
	{
		UpdateWidget(&(*widgets)[i], i);
	}

	return true;
}

void DebugOverlayHUD::RemoveWindow(DebugWindowHandle handle)
{
	auto widget = widgets->GetSpecificObject(handle.id);

	if(widget->valuePointer != nullptr)
	{
		//rootWindow->removeChild(widget->identifierString);
		CEGUI::WindowManager::getSingleton().destroyWindow(widget->ceguiWindow);

		widget->ceguiWindow = nullptr;
		widget->valuePointer = nullptr;
		widget->identifierString = "deleted";

		widgets->RemoveObject(handle.id);
	}
}

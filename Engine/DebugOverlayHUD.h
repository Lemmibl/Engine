#pragma once
#include "DODContainer.h"
#include "CEGUI/CEGUI.h"
#include "DebugOverlayHUDHandle.h"
#include <Windows.h>
#include <xnamath.h>

//Container and manager for all of the windows and their respective values that represent values that are used for debugging.
class DebugOverlayHUD
{
private:
	struct WindowContainer
	{
		CEGUI::Window* ceguiWindow; //The cegui window that handles how and where the information should be rendered
		std::string identifierString; //The name of the window that is written before the actual value. Examples: "Position: " or "Amount of triangles rendered this frame: "
		void* valuePointer; //Pointer to value that this window will render
		DataTypeEnumMappings::DataType dataType; //What actual type the void pointer is pointing to

		~WindowContainer()
		{
			ceguiWindow = nullptr;
			identifierString = "deleted";
			valuePointer = nullptr;
			dataType = 0;
		}
	};

public:
	DebugOverlayHUD();
	~DebugOverlayHUD();

	void SetHUDVisibility(bool visible);

	bool Initialize(float startPosX, float startPosY);
	bool Update();

	bool AddNewWindow(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType, DebugWindowHandle& outHandle);
	bool AddNewWindowWithoutHandle(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType);

	unsigned short GetCount() { return widgets->GetActiveObjectCount(); }

	void RemoveWindow(DebugWindowHandle handle);

private:

	template<class T> void UpdateWindowText(WindowContainer* widget, T* val);
	bool CreateDebugWindow(std::string identifier, DebugWindowHandle& outHandle);
	void UpdateWidget(WindowContainer* widget, unsigned short index);

	static const unsigned short MaxWidgetCount = 128;
	unsigned long long totalWindowsCount;

	//http://cegui.org.uk/wiki/Sample_code_for_all_Widgets

	CEGUI::Window* rootWindow;

	std::shared_ptr<DODContainer<WindowContainer>> widgets;
};

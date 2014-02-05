#pragma once
#include "CEGUI/CEGUI.h"
#include "DebugOverlayHUDHandle.h"
#include <queue>
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
	};

public:
	DebugOverlayHUD();
	~DebugOverlayHUD();

	void SetHUDVisibility(bool visible);

	bool Initialize(float startPosX, float startPosY);
	bool Update();

	bool AddNewWindow(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType, DebugWindowHandle& outHandle);
	bool AddNewWindowWithoutHandle(std::string name, void* valueToBeMonitored, DataTypeEnumMappings::DataType dataType);

	template<class T> void UpdateWindowText(DebugWindowHandle handle, T* val);

	void RemoveWindow(DebugWindowHandle handle);

private:
	bool CreateDebugWindow(std::string identifier, DebugWindowHandle& outHandle);
	void UpdateWidget(DebugWindowHandle& index, DataTypeEnumMappings::DataType dataType);

	static const unsigned short MaxWidgetCount = 128;

	//http://cegui.org.uk/wiki/Sample_code_for_all_Widgets

	CEGUI::Window* rootWindow;

	unsigned short currentActiveWidgets;

	WindowContainer widgets[MaxWidgetCount];
	std::priority_queue<unsigned short, std::vector<unsigned short>, std::greater<unsigned short>> prioQueue;
};

#include "GUI.h"
#include "RestaurantCore.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Create restaurant core
    RestaurantCore restaurant;

    // Create and run GUI
    RestaurantGUI gui(&restaurant);
    if (gui.CreateMainWindow(hInstance)) {
        return gui.Run();
    }

    return 0;
}
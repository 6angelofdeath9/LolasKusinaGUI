#pragma once
#include "RestaurantCore.h"
#include <windows.h>
#include <string>
#include <vector>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

using namespace std;

// Control IDs
#define IDC_TAB_APPETIZERS     1001
#define IDC_TAB_MAIN           1002
#define IDC_TAB_DESSERTS       1003
#define IDC_TAB_BEVERAGES      1004
#define IDC_MENU_LIST          1005
#define IDC_ORDER_LIST         1006
#define IDC_BTN_ADD            1007
#define IDC_BTN_REMOVE         1008
#define IDC_BTN_CLEAR          1009
#define IDC_BTN_CHECKOUT       1010
#define IDC_BTN_QTY_UP         1011
#define IDC_BTN_QTY_DOWN       1012
#define IDC_EDIT_QTY           1013
#define IDC_COMBO_DISCOUNT     1014
#define IDC_STATIC_SUBTOTAL    1015

class RestaurantGUI {
private:
    RestaurantCore* core;
    string currentCategory;
    int selectedQty;

    // Window handles
    HWND hwndMain;
    HWND hwndMenuList;
    HWND hwndOrderList;
    HWND hwndSubtotalLabel;
    HWND hwndDiscountCombo;
    HWND hwndQtyEdit;
    HWND hwndTabAppetizers, hwndTabMain, hwndTabDesserts, hwndTabBeverages;

    // Fonts
    HFONT hFontTitle;
    HFONT hFontNormal;
    HFONT hFontBold;

public:
    RestaurantGUI(RestaurantCore* restaurantCore);
    ~RestaurantGUI();

    bool CreateMainWindow(HINSTANCE hInstance);
    int Run();

private:
    void CreateControls(HWND hwnd);
    void UpdateMenuDisplay();
    void UpdateOrderDisplay();
    void UpdateSubtotalDisplay();
    void SwitchCategory(const string& category);

    // Event handlers
    void OnCommand(WPARAM wParam, LPARAM lParam);
    void OnPaint(HWND hwnd);
    void OnAddToOrder(int itemId);
    void OnRemoveFromOrder();
    void OnClearOrder();
    void OnCheckout();
    void OnQuantityChange(int change);

    // Window procedure
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
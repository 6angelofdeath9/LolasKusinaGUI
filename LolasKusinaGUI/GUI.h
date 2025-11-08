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

// Modern POS Control IDs
#define IDC_MENU_GRID_CONTAINER 2000
#define IDC_MENU_ITEM_BASE      2100  // Base ID for dynamic menu item buttons

// Enhanced numeric keypad
#define IDC_KEYPAD_CONTAINER    3000
#define IDC_KEY_0               3001
#define IDC_KEY_1               3002
#define IDC_KEY_2               3003
#define IDC_KEY_3               3004
#define IDC_KEY_4               3005
#define IDC_KEY_5               3006
#define IDC_KEY_6               3007
#define IDC_KEY_7               3008
#define IDC_KEY_8               3009
#define IDC_KEY_9               3010
#define IDC_KEY_DECIMAL         3011
#define IDC_KEY_CLEAR           3012

// Payment controls
#define IDC_PAYMENT_AMOUNT      4000
#define IDC_CHANGE_AMOUNT       4001
#define IDC_DISCOUNT_COMBO      4002

// Visual elements
#define IDC_LOGO_IMAGE          5000
#define IDC_STATUS_BAR          5001

// Color definitions for Lola's Kusina theme
#define COLOR_PRIMARY         RGB(139, 69, 19)    // Saddle Brown
#define COLOR_SECONDARY       RGB(210, 105, 30)   // Chocolate
#define COLOR_ACCENT          RGB(255, 215, 0)    // Gold
#define COLOR_BACKGROUND      RGB(255, 248, 220)  // Cornsilk
#define COLOR_TEXT           RGB(51, 51, 51)     // Dark Gray
#define COLOR_WHITE          RGB(255, 255, 255)  // White
#define COLOR_HOVER          RGB(160, 82, 45)    // Sienna
#define COLOR_SUCCESS        RGB(40, 167, 69)    // Green
#define COLOR_DANGER         RGB(220, 53, 69)    // Red
#define COLOR_WARNING        RGB(255, 193, 7)    // Yellow

class RestaurantGUI {
private:
    RestaurantCore* core;
    string currentCategory;
    int selectedQty;

    // Original Window handles
    HWND hwndMain;
    HWND hwndMenuList;
    HWND hwndOrderList;
    HWND hwndSubtotalLabel;
    HWND hwndDiscountCombo;
    HWND hwndQtyEdit;
    HWND hwndTabAppetizers, hwndTabMain, hwndTabDesserts, hwndTabBeverages;

    // Modern POS Window handles
    HWND hwndMenuGridContainer;
    HWND hwndKeypadContainer;
    HWND hwndPaymentAmount;
    HWND hwndChangeAmount;
    HWND hwndLogoImage;
    HWND hwndStatusBar;
    vector<HWND> menuButtonHandles;  // Dynamic menu item buttons
    vector<HWND> keypadHandles;      // Numeric keypad buttons

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

    // Modern POS Layout Methods
    void CreateModernLayout(HWND hwnd);
    void CreateMenuGrid(HWND hwnd);
    void CreateNumericKeypad(HWND hwnd);
    void CreatePaymentSection(HWND hwnd);
    void UpdateMenuGrid(const string& category);
    void AnimateButtonPress(HWND button);
    void UpdateVisualFeedback();
    string GetFoodEmoji(const string& itemName);
    COLORREF GetCategoryColor(const string& category);

    // Window procedure
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
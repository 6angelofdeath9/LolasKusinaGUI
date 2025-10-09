#include "GUI.h"
#include <sstream>
#include <iomanip>

// Constructor
RestaurantGUI::RestaurantGUI(RestaurantCore* restaurantCore)
    : core(restaurantCore), currentCategory("Appetizers"), selectedQty(1),
    hwndMain(nullptr), hwndMenuList(nullptr), hwndOrderList(nullptr),
    hwndSubtotalLabel(nullptr), hwndDiscountCombo(nullptr), hwndQtyEdit(nullptr),
    hFontTitle(nullptr), hFontNormal(nullptr), hFontBold(nullptr) {
}

// Destructor
RestaurantGUI::~RestaurantGUI() {
    if (hFontTitle) DeleteObject(hFontTitle);
    if (hFontNormal) DeleteObject(hFontNormal);
    if (hFontBold) DeleteObject(hFontBold);
}

// Create main application window
bool RestaurantGUI::CreateMainWindow(HINSTANCE hInstance) {
    // Register window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 248, 240));
    wc.lpszClassName = L"LolasKusinaWindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    // Create main window
    hwndMain = CreateWindowEx(
        0,
        L"LolasKusinaWindowClass",
        L"LOLA'S KUSINA - MGA LUTO NI LOLA",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        NULL, NULL, hInstance, this
    );

    if (!hwndMain) {
        return false;
    }

    // Create fonts
    hFontTitle = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    hFontNormal = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hFontBold = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    CreateControls(hwndMain);
    UpdateMenuDisplay();
    UpdateOrderDisplay();

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);

    return true;
}

// Create all GUI controls
void RestaurantGUI::CreateControls(HWND hwnd) {
    // Category tabs (buttons)
    hwndTabAppetizers = CreateWindow(L"BUTTON", L"APPETIZERS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 80, 150, 40, hwnd, (HMENU)IDC_TAB_APPETIZERS, NULL, NULL);

    hwndTabMain = CreateWindow(L"BUTTON", L"MAIN DISHES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        180, 80, 150, 40, hwnd, (HMENU)IDC_TAB_MAIN, NULL, NULL);

    hwndTabDesserts = CreateWindow(L"BUTTON", L"DESSERTS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        340, 80, 150, 40, hwnd, (HMENU)IDC_TAB_DESSERTS, NULL, NULL);

    hwndTabBeverages = CreateWindow(L"BUTTON", L"BEVERAGES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        500, 80, 150, 40, hwnd, (HMENU)IDC_TAB_BEVERAGES, NULL, NULL);

    // Menu list box
    hwndMenuList = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
        20, 130, 630, 250, hwnd, (HMENU)IDC_MENU_LIST, NULL, NULL);

    // Quantity controls
    CreateWindow(L"STATIC", L"Quantity:",
        WS_VISIBLE | WS_CHILD,
        670, 130, 80, 25, hwnd, NULL, NULL, NULL);

    CreateWindow(L"BUTTON", L"-",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        670, 160, 40, 30, hwnd, (HMENU)IDC_BTN_QTY_DOWN, NULL, NULL);

    hwndQtyEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"1",
        WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER,
        715, 160, 50, 30, hwnd, (HMENU)IDC_EDIT_QTY, NULL, NULL);

    CreateWindow(L"BUTTON", L"+",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        770, 160, 40, 30, hwnd, (HMENU)IDC_BTN_QTY_UP, NULL, NULL);

    // Add to Order button
    CreateWindow(L"BUTTON", L"ADD TO ORDER",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        670, 200, 280, 40, hwnd, (HMENU)IDC_BTN_ADD, NULL, NULL);

    // Order display label
    CreateWindow(L"STATIC", L"CURRENT ORDER",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 390, 630, 25, hwnd, NULL, NULL, NULL);

    // Order list box
    hwndOrderList = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
        20, 420, 630, 150, hwnd, (HMENU)IDC_ORDER_LIST, NULL, NULL);

    // Remove from order button
    CreateWindow(L"BUTTON", L"REMOVE SELECTED",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        670, 420, 280, 35, hwnd, (HMENU)IDC_BTN_REMOVE, NULL, NULL);

    // Clear order button
    CreateWindow(L"BUTTON", L"CLEAR ORDER",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        670, 465, 280, 35, hwnd, (HMENU)IDC_BTN_CLEAR, NULL, NULL);

    // Subtotal display
    hwndSubtotalLabel = CreateWindow(L"STATIC", L"Subtotal: PHP 0.00",
        WS_VISIBLE | WS_CHILD | SS_RIGHT,
        400, 580, 250, 25, hwnd, (HMENU)IDC_STATIC_SUBTOTAL, NULL, NULL);

    // Discount combo box
    CreateWindow(L"STATIC", L"DISCOUNT:",
        WS_VISIBLE | WS_CHILD,
        20, 583, 100, 25, hwnd, NULL, NULL, NULL);

    hwndDiscountCombo = CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        120, 580, 150, 200, hwnd, (HMENU)IDC_COMBO_DISCOUNT, NULL, NULL);

    // Add discount options
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"None");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Student (10%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Senior (20%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"PWD (20%)");
    SendMessage(hwndDiscountCombo, CB_SETCURSEL, 0, 0);

    // Checkout button
    CreateWindow(L"BUTTON", L"CHECKOUT",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        670, 575, 280, 40, hwnd, (HMENU)IDC_BTN_CHECKOUT, NULL, NULL);

    // Apply fonts to controls
    SendMessage(hwndTabAppetizers, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabMain, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabDesserts, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabBeverages, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndMenuList, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndOrderList, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndSubtotalLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// Window procedure callback
LRESULT CALLBACK RestaurantGUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RestaurantGUI* pThis = nullptr;

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<RestaurantGUI*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else {
        pThis = reinterpret_cast<RestaurantGUI*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        switch (uMsg) {
        case WM_COMMAND:
            pThis->OnCommand(wParam, lParam);
            return 0;
        case WM_PAINT:
            pThis->OnPaint(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Handle command messages
void RestaurantGUI::OnCommand(WPARAM wParam, LPARAM lParam) {
    int wmId = LOWORD(wParam);

    switch (wmId) {
    case IDC_TAB_APPETIZERS:
        SwitchCategory("Appetizers");
        break;
    case IDC_TAB_MAIN:
        SwitchCategory("Main Dishes");
        break;
    case IDC_TAB_DESSERTS:
        SwitchCategory("Desserts");
        break;
    case IDC_TAB_BEVERAGES:
        SwitchCategory("Beverages");
        break;
    case IDC_BTN_ADD:
    {
        int selected = (int)SendMessage(hwndMenuList, LB_GETCURSEL, 0, 0);
        if (selected != LB_ERR) {
            int itemId = (int)SendMessage(hwndMenuList, LB_GETITEMDATA, selected, 0);
            OnAddToOrder(itemId);
        }
    }
    break;
    case IDC_BTN_REMOVE:
        OnRemoveFromOrder();
        break;
    case IDC_BTN_CLEAR:
        OnClearOrder();
        break;
    case IDC_BTN_CHECKOUT:
        OnCheckout();
        break;
    case IDC_BTN_QTY_UP:
        OnQuantityChange(1);
        break;
    case IDC_BTN_QTY_DOWN:
        OnQuantityChange(-1);
        break;
    case IDC_COMBO_DISCOUNT:
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            UpdateSubtotalDisplay();
        }
        break;
    }
}

// Handle paint messages
void RestaurantGUI::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Draw title background
    RECT titleRect = { 0, 0, 1000, 70 };
    HBRUSH hBrushTitle = CreateSolidBrush(RGB(139, 69, 19)); // Saddle brown
    FillRect(hdc, &titleRect, hBrushTitle);
    DeleteObject(hBrushTitle);

    // Draw title text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 215, 0)); // Gold
    SelectObject(hdc, hFontTitle);

    RECT textRect = { 0, 10, 1000, 60 };
    DrawText(hdc, L"LOLA'S KUSINA - MGA LUTO NI LOLA", -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    EndPaint(hwnd, &ps);
}

// Update menu display based on current category
void RestaurantGUI::UpdateMenuDisplay() {
    // Clear the menu list
    SendMessage(hwndMenuList, LB_RESETCONTENT, 0, 0);

    // Get menu items for current category
    const MenuItem* menu = core->getMenu();
    int menuSize = core->getMenuSize();

    for (int i = 0; i < menuSize; i++) {
        if (menu[i].category == currentCategory) {
            // Format: "1. Lumpiang Shanghai - PHP 120.00"
            wstring menuItem = to_wstring(menu[i].number) + L". " +
                wstring(menu[i].name.begin(), menu[i].name.end()) +
                L" - PHP " + to_wstring(menu[i].price);

            int index = (int)SendMessage(hwndMenuList, LB_ADDSTRING, 0,
                (LPARAM)menuItem.c_str());
            SendMessage(hwndMenuList, LB_SETITEMDATA, index, (LPARAM)menu[i].number);
        }
    }

    // Select first item by default
    if (SendMessage(hwndMenuList, LB_GETCOUNT, 0, 0) > 0) {
        SendMessage(hwndMenuList, LB_SETCURSEL, 0, 0);
    }
}

// Update order display
void RestaurantGUI::UpdateOrderDisplay() {
    // Clear the order list
    SendMessage(hwndOrderList, LB_RESETCONTENT, 0, 0);

    const vector<MenuItem>& order = core->getOrder();
    const vector<int>& quantities = core->getQuantities();

    for (size_t i = 0; i < order.size(); i++) {
        double itemTotal = order[i].price * quantities[i];
        wstring orderItem = to_wstring(quantities[i]) + L" x " +
            wstring(order[i].name.begin(), order[i].name.end()) +
            L" - PHP " + to_wstring(itemTotal);

        SendMessage(hwndOrderList, LB_ADDSTRING, 0, (LPARAM)orderItem.c_str());
    }

    UpdateSubtotalDisplay();
}

// Update subtotal display
void RestaurantGUI::UpdateSubtotalDisplay() {
    double subtotal = core->calculateSubtotal();

    // Get selected discount
    int discountIndex = (int)SendMessage(hwndDiscountCombo, CB_GETCURSEL, 0, 0);
    double discount = core->calculateDiscount(subtotal, discountIndex);
    double grandTotal = subtotal - discount;

    wstring subtotalText = L"Subtotal: PHP " + to_wstring(subtotal) +
        L"  |  Discount: PHP " + to_wstring(discount) +
        L"  |  Total: PHP " + to_wstring(grandTotal);

    SetWindowText(hwndSubtotalLabel, subtotalText.c_str());
}

// Switch category
void RestaurantGUI::SwitchCategory(const string& category) {
    currentCategory = category;
    UpdateMenuDisplay();
}

// Handle add to order
void RestaurantGUI::OnAddToOrder(int itemId) {
    // Get quantity from edit control
    wchar_t qtyText[10];
    GetWindowText(hwndQtyEdit, qtyText, 10);
    int quantity = _wtoi(qtyText);

    if (quantity <= 0) {
        MessageBox(hwndMain, L"Please enter a valid quantity (1 or more)", L"Invalid Quantity", MB_OK | MB_ICONWARNING);
        return;
    }

    core->addToOrder(itemId, quantity);
    UpdateOrderDisplay();

    // Show confirmation
    wstring message = L"Added " + to_wstring(quantity) + L" item(s) to your order!";
    MessageBox(hwndMain, message.c_str(), L"Order Updated", MB_OK | MB_ICONINFORMATION);
}

// Handle remove from order
void RestaurantGUI::OnRemoveFromOrder() {
    int selected = (int)SendMessage(hwndOrderList, LB_GETCURSEL, 0, 0);
    if (selected == LB_ERR) {
        MessageBox(hwndMain, L"Please select an item to remove", L"No Selection", MB_OK | MB_ICONWARNING);
        return;
    }

    core->removeFromOrder(selected);
    UpdateOrderDisplay();
}

// Handle clear order
void RestaurantGUI::OnClearOrder() {
    if (core->isOrderEmpty()) {
        MessageBox(hwndMain, L"Order is already empty", L"Clear Order", MB_OK | MB_ICONINFORMATION);
        return;
    }

    int result = MessageBox(hwndMain, L"Are you sure you want to clear the entire order?",
        L"Confirm Clear", MB_YESNO | MB_ICONQUESTION);

    if (result == IDYES) {
        core->clearOrder();
        UpdateOrderDisplay();
    }
}

// Handle quantity change
void RestaurantGUI::OnQuantityChange(int change) {
    wchar_t qtyText[10];
    GetWindowText(hwndQtyEdit, qtyText, 10);
    int quantity = _wtoi(qtyText);

    quantity += change;
    if (quantity < 1) quantity = 1;
    if (quantity > 99) quantity = 99;

    SetWindowText(hwndQtyEdit, to_wstring(quantity).c_str());
}

// Handle checkout
void RestaurantGUI::OnCheckout() {
    if (core->isOrderEmpty()) {
        MessageBox(hwndMain, L"Please add items to your order before checkout",
            L"Empty Order", MB_OK | MB_ICONWARNING);
        return;
    }

    // Get discount selection
    int discountIndex = (int)SendMessage(hwndDiscountCombo, CB_GETCURSEL, 0, 0);
    double subtotal = core->calculateSubtotal();
    double discount = core->calculateDiscount(subtotal, discountIndex);
    double grandTotal = subtotal - discount;

    // Simple payment input
    wchar_t paymentAmount[50];
    swprintf(paymentAmount, 50, L"%.2f", grandTotal);

    if (DialogBoxParam(GetModuleHandle(NULL), NULL, hwndMain, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        if (msg == WM_INITDIALOG) {
            wchar_t* defaultAmount = (wchar_t*)lParam;
            SetDlgItemText(hwnd, 1001, defaultAmount);
            return TRUE;
        }
        if (msg == WM_COMMAND) {
            if (LOWORD(wParam) == IDOK) {
                EndDialog(hwnd, 1);
                return TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwnd, 0);
                return TRUE;
            }
        }
        return FALSE;
        }, (LPARAM)paymentAmount)) {

        double payment = _wtof(paymentAmount);

        if (payment < grandTotal) {
            MessageBox(hwndMain, L"Insufficient payment! Transaction cancelled.",
                L"Payment Error", MB_OK | MB_ICONERROR);
            return;
        }

        double change = payment - grandTotal;

        // Generate and show receipt
        string receipt = core->generateReceipt(subtotal, discount, payment, change);
        MessageBox(hwndMain, wstring(receipt.begin(), receipt.end()).c_str(),
            L"Lola's Kusina - Receipt", MB_OK | MB_ICONINFORMATION);

        // Clear order after successful checkout
        core->clearOrder();
        UpdateOrderDisplay();

        // Reset quantity
        SetWindowText(hwndQtyEdit, L"1");
    }
}

// Run the application
int RestaurantGUI::Run() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
#include "GUI.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// Constructor
RestaurantGUI::RestaurantGUI(RestaurantCore* restaurantCore)
    : core(restaurantCore), currentCategory("Appetizers"), selectedQty(1), isLoggedIn(false),
    hwndMain(nullptr), hwndMenuList(nullptr), hwndOrderList(nullptr),
    hwndSubtotalLabel(nullptr), hwndDiscountCombo(nullptr), hwndQtyEdit(nullptr),
    hwndMenuGridContainer(nullptr), hwndKeypadContainer(nullptr),
    hwndPaymentAmount(nullptr), hwndChangeAmount(nullptr), hwndLogoImage(nullptr),
    hwndStatusBar(nullptr), hwndLoginUsername(nullptr), hwndLoginPassword(nullptr),
    hwndLoginButton(nullptr), hwndLoginPanel(nullptr),
    hFontTitle(nullptr), hFontNormal(nullptr), hFontBold(nullptr),
    hMenuFont(nullptr), hPriceFont(nullptr), hEmojiFont(nullptr) {
}

// Destructor
RestaurantGUI::~RestaurantGUI() {
    if (hFontTitle) DeleteObject(hFontTitle);
    if (hFontNormal) DeleteObject(hFontNormal);
    if (hFontBold) DeleteObject(hFontBold);
    if (hMenuFont) DeleteObject(hMenuFont);
    if (hPriceFont) DeleteObject(hPriceFont);
    if (hEmojiFont) DeleteObject(hEmojiFont);

    // Clean up dynamic controls
    for (HWND hwnd : menuButtonHandles) {
        if (hwnd) DestroyWindow(hwnd);
    }
    for (HWND hwnd : keypadHandles) {
        if (hwnd) DestroyWindow(hwnd);
    }
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
    wc.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wc.lpszClassName = L"LolasKusinaWindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    // Create main window with mobile dimensions (1080x1920px - portrait mode)
    hwndMain = CreateWindowEx(
        0,
        L"LolasKusinaWindowClass",
        L"🌺 LOLA'S KUSINA - MGA LUTO NI LOLA 🌺",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1080, 1920,
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

    // Create modern fonts
    hMenuFont = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hPriceFont = CreateFont(10, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hEmojiFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Emoji");

    CreateLoginScreen(hwndMain);
    // Don't create POS layout until login is successful

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

    // Handle login screen commands first
    if (!isLoggedIn) {
        switch (wmId) {
        case IDC_LOGIN_BUTTON:
            OnLoginAttempt();
            return;
        }
        return; // Don't process other commands until logged in
    }

    // POS commands (only when logged in)
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

    // Modern menu item buttons (dynamic IDs)
    case IDC_MENU_ITEM_BASE:
    case IDC_MENU_ITEM_BASE + 1:
    case IDC_MENU_ITEM_BASE + 2:
    case IDC_MENU_ITEM_BASE + 3:
    case IDC_MENU_ITEM_BASE + 4:
    case IDC_MENU_ITEM_BASE + 5:
    case IDC_MENU_ITEM_BASE + 6:
    case IDC_MENU_ITEM_BASE + 7:
    case IDC_MENU_ITEM_BASE + 8:
    case IDC_MENU_ITEM_BASE + 9:
    case IDC_MENU_ITEM_BASE + 10:
    case IDC_MENU_ITEM_BASE + 11:
    {
        int buttonIndex = wmId - IDC_MENU_ITEM_BASE;
        const MenuItem* menu = core->getMenu();
        int menuSize = core->getMenuSize();

        int currentIndex = 0;
        for (int i = 0; i < menuSize; i++) {
            if (menu[i].category == currentCategory) {
                if (currentIndex == buttonIndex) {
                    OnAddToOrder(menu[i].number);
                    AnimateButtonPress((HWND)lParam);
                    break;
                }
                currentIndex++;
            }
        }
    }
    break;

    // Numeric keypad buttons
    case IDC_KEY_0:
    case IDC_KEY_1:
    case IDC_KEY_2:
    case IDC_KEY_3:
    case IDC_KEY_4:
    case IDC_KEY_5:
    case IDC_KEY_6:
    case IDC_KEY_7:
    case IDC_KEY_8:
    case IDC_KEY_9:
    {
        wchar_t digit = L'0' + (wmId - IDC_KEY_0);
        wchar_t currentText[50];
        GetWindowText(hwndPaymentAmount, currentText, 50);

        // If current text is "0.00", replace it with the digit
        if (wcscmp(currentText, L"0.00") == 0) {
            swprintf(currentText, 50, L"%c", digit);
        } else {
            // Append the digit
            size_t len = wcslen(currentText);
            if (len < 49) {
                currentText[len] = digit;
                currentText[len + 1] = L'\0';
            }
        }
        SetWindowText(hwndPaymentAmount, currentText);
        UpdateSubtotalDisplay();
        AnimateButtonPress((HWND)lParam);
    }
    break;

    case IDC_KEY_DECIMAL:
    {
        wchar_t currentText[50];
        GetWindowText(hwndPaymentAmount, currentText, 50);
        if (wcsstr(currentText, L".") == nullptr) {
            wcscat_s(currentText, L".");
            SetWindowText(hwndPaymentAmount, currentText);
        }
        AnimateButtonPress((HWND)lParam);
    }
    break;

    case IDC_KEY_CLEAR:
        SetWindowText(hwndPaymentAmount, L"0.00");
        AnimateButtonPress((HWND)lParam);
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

    // Legacy menu list support
    case IDC_BTN_ADD:
    {
        int selected = (int)SendMessage(hwndMenuList, LB_GETCURSEL, 0, 0);
        if (selected != LB_ERR) {
            int itemId = (int)SendMessage(hwndMenuList, LB_GETITEMDATA, selected, 0);
            OnAddToOrder(itemId);
        }
    }
    break;
    }
}

// Handle paint messages
void RestaurantGUI::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Draw modern header with gradient effect
    RECT titleRect = { 0, 0, 1400, 100 };

    // Create gradient background
    HBRUSH hBrushTitle = CreateSolidBrush(COLOR_PRIMARY); // Saddle brown
    FillRect(hdc, &titleRect, hBrushTitle);
    DeleteObject(hBrushTitle);

    // Draw decorative border
    RECT borderRect = { 5, 5, 1395, 95 };
    HPEN hPenBorder = CreatePen(PS_SOLID, 2, COLOR_ACCENT); // Gold border
    SelectObject(hdc, hPenBorder);
    Rectangle(hdc, borderRect.left, borderRect.top, borderRect.right, borderRect.bottom);
    DeleteObject(hPenBorder);

    // Draw title text with emoji
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, COLOR_ACCENT); // Gold
    SelectObject(hdc, hFontTitle);

    RECT textRect = { 0, 25, 1400, 75 };
    DrawText(hdc, L"🌺 LOLA'S KUSINA - MGA LUTO NI LOLA 🌺", -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Draw category indicator
    SetTextColor(hdc, COLOR_BACKGROUND);
    SelectObject(hdc, hFontNormal);
    RECT categoryRect = { 0, 70, 1400, 90 };
    wstring categoryText = L"Current Category: " + wstring(currentCategory.begin(), currentCategory.end());
    DrawText(hdc, categoryText.c_str(), -1, &categoryRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    EndPaint(hwnd, &ps);
}

// Update menu display based on current category
void RestaurantGUI::UpdateMenuDisplay() {
    // Update modern menu grid
    CreateMenuGrid(hwndMain);

    // Also update legacy menu list for compatibility
    if (hwndMenuList) {
        SendMessage(hwndMenuList, LB_RESETCONTENT, 0, 0);

        const MenuItem* menu = core->getMenu();
        int menuSize = core->getMenuSize();

        for (int i = 0; i < menuSize; i++) {
            if (menu[i].category == currentCategory) {
                wstring menuItem = to_wstring(menu[i].number) + L". " +
                    wstring(menu[i].name.begin(), menu[i].name.end()) +
                    L" - ₱" + to_wstring(menu[i].price);

                int index = (int)SendMessage(hwndMenuList, LB_ADDSTRING, 0,
                    (LPARAM)menuItem.c_str());
                SendMessage(hwndMenuList, LB_SETITEMDATA, index, (LPARAM)menu[i].number);
            }
        }

        if (SendMessage(hwndMenuList, LB_GETCOUNT, 0, 0) > 0) {
            SendMessage(hwndMenuList, LB_SETCURSEL, 0, 0);
        }
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

    // Format with proper currency display and 2 decimal places
    wostringstream subtotalStream;
    subtotalStream << fixed << setprecision(2);
    subtotalStream << L"Subtotal: ₱" << subtotal
                   << L"  |  Discount: ₱" << discount
                   << L"  |  Total: ₱" << grandTotal;

    SetWindowText(hwndSubtotalLabel, subtotalStream.str().c_str());

    // Calculate change if payment amount is entered
    if (hwndPaymentAmount) {
        wchar_t paymentText[50];
        GetWindowText(hwndPaymentAmount, paymentText, 50);
        double payment = _wtof(paymentText);

        if (payment > 0) {
            double change = payment - grandTotal;
            wostringstream changeStream;
            changeStream << fixed << setprecision(2);
            changeStream << change;
            SetWindowText(hwndChangeAmount, changeStream.str().c_str());
        } else {
            SetWindowText(hwndChangeAmount, L"0.00");
        }
    }
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

    // Get payment amount from modern payment field
    wchar_t paymentText[50];
    GetWindowText(hwndPaymentAmount, paymentText, 50);
    double payment = _wtof(paymentText);

    if (payment < grandTotal) {
        MessageBox(hwndMain, L"Insufficient payment! Please enter a valid payment amount.",
            L"Payment Error", MB_OK | MB_ICONERROR);
        return;
    }

    double change = payment - grandTotal;

    // Generate and show receipt
    string receipt = core->generateReceipt(subtotal, discount, payment, change);
    MessageBox(hwndMain, wstring(receipt.begin(), receipt.end()).c_str(),
        L"🧾 Lola's Kusina - Receipt", MB_OK | MB_ICONINFORMATION);

    // Clear order after successful checkout
    core->clearOrder();
    UpdateOrderDisplay();

    // Reset payment fields
    SetWindowText(hwndPaymentAmount, L"0.00");
    SetWindowText(hwndChangeAmount, L"0.00");

    // Show success feedback
    UpdateVisualFeedback();
}

// Create modern layout for POS interface
void RestaurantGUI::CreateModernLayout(HWND hwnd) {
    // Clear any existing menu buttons
    for (HWND btn : menuButtonHandles) {
        if (btn) DestroyWindow(btn);
    }
    menuButtonHandles.clear();

    // Clear any existing keypad buttons
    for (HWND btn : keypadHandles) {
        if (btn) DestroyWindow(btn);
    }
    keypadHandles.clear();

    // Create category tabs (modern styling)
    hwndTabAppetizers = CreateWindow(L"BUTTON", L"APPETIZERS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 120, 150, 40, hwnd, (HMENU)IDC_TAB_APPETIZERS, NULL, NULL);

    hwndTabMain = CreateWindow(L"BUTTON", L"MAIN DISHES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        180, 120, 150, 40, hwnd, (HMENU)IDC_TAB_MAIN, NULL, NULL);

    hwndTabDesserts = CreateWindow(L"BUTTON", L"DESSERTS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        340, 120, 150, 40, hwnd, (HMENU)IDC_TAB_DESSERTS, NULL, NULL);

    hwndTabBeverages = CreateWindow(L"BUTTON", L"BEVERAGES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        500, 120, 150, 40, hwnd, (HMENU)IDC_TAB_BEVERAGES, NULL, NULL);

    // Apply fonts to category tabs
    SendMessage(hwndTabAppetizers, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabMain, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabDesserts, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndTabBeverages, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Create menu grid container
    hwndMenuGridContainer = CreateWindow(L"STATIC", NULL,
        WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        20, 170, 900, 400, hwnd, (HMENU)IDC_MENU_GRID_CONTAINER, NULL, NULL);

    // Create order display section (right panel)
    CreateWindow(L"STATIC", L"CURRENT ORDER",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        950, 170, 400, 30, hwnd, NULL, NULL, NULL);

    hwndOrderList = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
        950, 210, 400, 200, hwnd, (HMENU)IDC_ORDER_LIST, NULL, NULL);

    // Create numeric keypad
    CreateNumericKeypad(hwnd);

    // Create payment section
    CreatePaymentSection(hwnd);

    // Create action buttons
    CreateWindow(L"BUTTON", L"REMOVE SELECTED",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        950, 550, 190, 35, hwnd, (HMENU)IDC_BTN_REMOVE, NULL, NULL);

    CreateWindow(L"BUTTON", L"CLEAR ORDER",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        1160, 550, 190, 35, hwnd, (HMENU)IDC_BTN_CLEAR, NULL, NULL);

    // Create checkout button
    CreateWindow(L"BUTTON", L"CHECKOUT",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        950, 600, 400, 40, hwnd, (HMENU)IDC_BTN_CHECKOUT, NULL, NULL);

    // Apply fonts to controls
    SendMessage(hwndOrderList, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Set current category button color
    SendMessage(hwndTabAppetizers, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// Create visual menu grid
void RestaurantGUI::CreateMenuGrid(HWND hwnd) {
    // Clear existing menu buttons
    for (HWND btn : menuButtonHandles) {
        if (btn) DestroyWindow(btn);
    }
    menuButtonHandles.clear();

    // Get menu items for current category
    const MenuItem* menu = core->getMenu();
    int menuSize = core->getMenuSize();

    vector<MenuItem> categoryItems;
    for (int i = 0; i < menuSize; i++) {
        if (menu[i].category == currentCategory) {
            categoryItems.push_back(menu[i]);
        }
    }

    // Calculate grid layout
    const int buttonWidth = 140;
    const int buttonHeight = 140;
    const int spacing = 15;
    const int columns = 6;
    const int startX = 20;
    const int startY = 170;

    // Create menu item buttons
    for (size_t i = 0; i < categoryItems.size(); i++) {
        int row = i / columns;
        int col = i % columns;
        int x = startX + col * (buttonWidth + spacing);
        int y = startY + row * (buttonHeight + spacing);

        // Create button for menu item
        int controlId = IDC_MENU_ITEM_BASE + static_cast<int>(i);
        HWND menuButton = CreateWindow(L"BUTTON", NULL,
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
            x, y, buttonWidth, buttonHeight,
            hwnd, (HMENU)controlId, NULL, NULL);

        // Format button text with emoji and price
        string emoji = GetFoodEmoji(categoryItems[i].name);
        wostringstream buttonText;
        buttonText << emoji.c_str() << L"\n"
                   << wstring(categoryItems[i].name.begin(), categoryItems[i].name.end()).c_str() << L"\n"
                   << L"₱" << categoryItems[i].price;

        SetWindowText(menuButton, buttonText.str().c_str());
        SendMessage(menuButton, WM_SETFONT, (WPARAM)hMenuFont, TRUE);

        menuButtonHandles.push_back(menuButton);
    }
}

// Create numeric keypad
void RestaurantGUI::CreateNumericKeypad(HWND hwnd) {
    // Clear existing keypad buttons
    for (HWND btn : keypadHandles) {
        if (btn) DestroyWindow(btn);
    }
    keypadHandles.clear();

    const int keySize = 50;
    const int spacing = 5;
    const int startX = 950;
    const int startY = 650;

    // Keypad layout: 3x4 grid
    const wchar_t* keys[] = {L"7", L"8", L"9", L"4", L"5", L"6", L"1", L"2", L"3", L"C", L"0", L"."};
    const int keyIds[] = {IDC_KEY_7, IDC_KEY_8, IDC_KEY_9, IDC_KEY_4, IDC_KEY_5, IDC_KEY_6,
                         IDC_KEY_1, IDC_KEY_2, IDC_KEY_3, IDC_KEY_CLEAR, IDC_KEY_0, IDC_KEY_DECIMAL};

    for (int i = 0; i < 12; i++) {
        int row = i / 3;
        int col = i % 3;
        int x = startX + col * (keySize + spacing);
        int y = startY + row * (keySize + spacing);

        HWND keyButton = CreateWindow(L"BUTTON", keys[i],
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            x, y, keySize, keySize,
            hwnd, (HMENU)keyIds[i], NULL, NULL);

        // Special styling for clear button
        if (keyIds[i] == IDC_KEY_CLEAR) {
            SendMessage(keyButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        } else {
            SendMessage(keyButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        }

        keypadHandles.push_back(keyButton);
    }
}

// Create payment section
void RestaurantGUI::CreatePaymentSection(HWND hwnd) {
    // Payment amount display
    CreateWindow(L"STATIC", L"PAYMENT:",
        WS_VISIBLE | WS_CHILD,
        950, 430, 100, 25, hwnd, NULL, NULL, NULL);

    hwndPaymentAmount = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0.00",
        WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER,
        1050, 430, 120, 25, hwnd, (HMENU)IDC_PAYMENT_AMOUNT, NULL, NULL);

    // Change display
    CreateWindow(L"STATIC", L"CHANGE:",
        WS_VISIBLE | WS_CHILD,
        1180, 430, 100, 25, hwnd, NULL, NULL, NULL);

    hwndChangeAmount = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0.00",
        WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_READONLY,
        1280, 430, 70, 25, hwnd, (HMENU)IDC_CHANGE_AMOUNT, NULL, NULL);

    // Discount combo box
    CreateWindow(L"STATIC", L"DISCOUNT:",
        WS_VISIBLE | WS_CHILD,
        950, 470, 100, 25, hwnd, NULL, NULL, NULL);

    hwndDiscountCombo = CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        1050, 470, 300, 200, hwnd, (HMENU)IDC_DISCOUNT_COMBO, NULL, NULL);

    // Add discount options
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"None");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Student (10%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Senior (20%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"PWD (20%)");
    SendMessage(hwndDiscountCombo, CB_SETCURSEL, 0, 0);

    // Apply fonts
    SendMessage(hwndPaymentAmount, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndChangeAmount, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndDiscountCombo, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Subtotal display (modern styling)
    hwndSubtotalLabel = CreateWindow(L"STATIC", L"Subtotal: ₱0.00",
        WS_VISIBLE | WS_CHILD | SS_RIGHT,
        950, 510, 400, 25, hwnd, (HMENU)IDC_STATIC_SUBTOTAL, NULL, NULL);
    SendMessage(hwndSubtotalLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// Update menu grid based on category
void RestaurantGUI::UpdateMenuGrid(const string& category) {
    CreateMenuGrid(hwndMain);
}

// Get food emoji based on item name
string RestaurantGUI::GetFoodEmoji(const string& itemName) {
    if (itemName.find("Lumpiang") != string::npos) return "🥟";
    if (itemName.find("Calamares") != string::npos) return "🦑";
    if (itemName.find("Adobo") != string::npos) return "🍲";
    if (itemName.find("Kare-Kare") != string::npos) return "🥘";
    if (itemName.find("Sinigang") != string::npos) return "🍜";
    if (itemName.find("Bangus") != string::npos) return "🐟";
    if (itemName.find("Bulalo") != string::npos) return "🍖";
    if (itemName.find("Halo-Halo") != string::npos) return "🍧";
    if (itemName.find("Leche Flan") != string::npos) return "🍰";
    if (itemName.find("Buko Juice") != string::npos) return "🥥";
    if (itemName.find("Iced Tea") != string::npos) return "🥤";
    if (itemName.find("Coke") != string::npos) return "🥤";
    return "🍽️";  // Default plate emoji
}

// Get category color
COLORREF RestaurantGUI::GetCategoryColor(const string& category) {
    if (category == "Appetizers") return COLOR_SECONDARY;
    if (category == "Main Dishes") return COLOR_PRIMARY;
    if (category == "Desserts") return COLOR_ACCENT;
    if (category == "Beverages") return COLOR_SUCCESS;
    return COLOR_PRIMARY;
}

// Animate button press
void RestaurantGUI::AnimateButtonPress(HWND button) {
    // Simple visual feedback - in a real implementation, this would include scaling
    InvalidateRect(button, NULL, TRUE);
    UpdateWindow(button);
}

// Update visual feedback
void RestaurantGUI::UpdateVisualFeedback() {
    // Update status bar or other visual elements
    if (hwndStatusBar) {
        SetWindowText(hwndStatusBar, L"Ready");
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
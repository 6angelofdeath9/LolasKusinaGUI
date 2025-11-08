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
    for (HWND hwnd : qtyMinusHandles) {
        if (hwnd) DestroyWindow(hwnd);
    }
    for (HWND hwnd : qtyPlusHandles) {
        if (hwnd) DestroyWindow(hwnd);
    }
    for (HWND hwnd : qtyDisplayHandles) {
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
    hFontTitle = CreateFont(42, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hFontNormal = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hFontBold = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Create modern fonts
    hMenuFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hPriceFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hEmojiFont = CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Emoji");

    CreateLoginScreen(hwndMain);

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);

    return true;
}

// Create login screen
void RestaurantGUI::CreateLoginScreen(HWND hwnd) {
    // Create login panel background
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int panelWidth = 600;
    int panelHeight = 400;
    int panelX = (clientRect.right - panelWidth) / 2;
    int panelY = (clientRect.bottom - panelHeight) / 2;

    hwndLoginPanel = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL,
        WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        panelX, panelY, panelWidth, panelHeight,
        hwnd, (HMENU)IDC_LOGIN_TITLE, NULL, NULL);

    // Create title label
    HWND hwndTitle = CreateWindow(L"STATIC", L"🌺 LOLA'S KUSINA 🌺",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        panelX + 50, panelY + 30, panelWidth - 100, 60,
        hwnd, NULL, NULL, NULL);
    SendMessage(hwndTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

    // Create subtitle
    HWND hwndSubtitle = CreateWindow(L"STATIC", L"Point of Sale System",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        panelX + 50, panelY + 90, panelWidth - 100, 30,
        hwnd, NULL, NULL, NULL);
    SendMessage(hwndSubtitle, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Create username label
    CreateWindow(L"STATIC", L"Username:",
        WS_VISIBLE | WS_CHILD,
        panelX + 50, panelY + 150, 120, 30,
        hwnd, NULL, NULL, NULL);

    // Create username field
    hwndLoginUsername = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"admin",
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        panelX + 180, panelY + 150, 370, 35,
        hwnd, (HMENU)IDC_LOGIN_USERNAME, NULL, NULL);
    SendMessage(hwndLoginUsername, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Create password label
    CreateWindow(L"STATIC", L"Password:",
        WS_VISIBLE | WS_CHILD,
        panelX + 50, panelY + 200, 120, 30,
        hwnd, NULL, NULL, NULL);

    // Create password field
    hwndLoginPassword = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"admin",
        WS_VISIBLE | WS_CHILD | ES_PASSWORD | ES_AUTOHSCROLL,
        panelX + 180, panelY + 200, 370, 35,
        hwnd, (HMENU)IDC_LOGIN_PASSWORD, NULL, NULL);
    SendMessage(hwndLoginPassword, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Create login button
    hwndLoginButton = CreateWindow(L"BUTTON", L"🔐 LOGIN",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        panelX + 200, panelY + 260, panelWidth - 400, 50,
        hwnd, (HMENU)IDC_LOGIN_BUTTON, NULL, NULL);
    SendMessage(hwndLoginButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);

    // Create help text
    HWND hwndHelp = CreateWindow(L"STATIC", L"Default: admin / admin",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        panelX + 50, panelY + 330, panelWidth - 100, 25,
        hwnd, NULL, NULL, NULL);
    SendMessage(hwndHelp, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
}

// Validate login credentials
bool RestaurantGUI::ValidateLogin(const wstring& username, const wstring& password) {
    return (username == L"admin" && password == L"admin");
}

// Handle login attempt
void RestaurantGUI::OnLoginAttempt() {
    wchar_t username[256];
    wchar_t password[256];

    GetWindowText(hwndLoginUsername, username, 256);
    GetWindowText(hwndLoginPassword, password, 256);

    if (ValidateLogin(username, password)) {
        isLoggedIn = true;
        HideLoginScreen();
        CreateModernLayout(hwndMain);
        UpdateMenuDisplay();
        UpdateOrderDisplay();

        MessageBox(hwndMain, L"Welcome to Lola's Kusina POS System!",
                  L"Login Successful", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hwndMain, L"Invalid username or password!\n\nDefault: admin / admin",
                  L"Login Failed", MB_OK | MB_ICONERROR);
        SetWindowText(hwndLoginPassword, L"");
        SetFocus(hwndLoginPassword);
    }
}

// Hide login screen
void RestaurantGUI::HideLoginScreen() {
    if (hwndLoginPanel) DestroyWindow(hwndLoginPanel);
    if (hwndLoginUsername) DestroyWindow(hwndLoginUsername);
    if (hwndLoginPassword) DestroyWindow(hwndLoginPassword);
    if (hwndLoginButton) DestroyWindow(hwndLoginButton);

    hwndLoginPanel = nullptr;
    hwndLoginUsername = nullptr;
    hwndLoginPassword = nullptr;
    hwndLoginButton = nullptr;
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
                    // Add quantity from display
                    int quantity = menuQuantities[buttonIndex];
                    if (quantity > 0) {
                        core->addToOrder(menu[i].number, quantity);
                        UpdateOrderDisplay();

                        // Reset quantity
                        menuQuantities[buttonIndex] = 0;
                        SetWindowText(qtyDisplayHandles[buttonIndex], L"0");

                        // Show confirmation
                        wstring message = L"Added " + to_wstring(quantity) + L" x " +
                                       wstring(menu[i].name.begin(), menu[i].name.end()) + L" to order!";
                        MessageBox(hwndMain, message.c_str(), L"Order Updated", MB_OK | MB_ICONINFORMATION);
                    }
                    AnimateButtonPress((HWND)lParam);
                    break;
                }
                currentIndex++;
            }
        }
    }
    break;

    // Quantity button controls
    case IDC_QTY_MINUS_BASE:
    case IDC_QTY_MINUS_BASE + 1:
    case IDC_QTY_MINUS_BASE + 2:
    case IDC_QTY_MINUS_BASE + 3:
    case IDC_QTY_MINUS_BASE + 4:
    case IDC_QTY_MINUS_BASE + 5:
    case IDC_QTY_MINUS_BASE + 6:
    case IDC_QTY_MINUS_BASE + 7:
    case IDC_QTY_MINUS_BASE + 8:
    case IDC_QTY_MINUS_BASE + 9:
    case IDC_QTY_MINUS_BASE + 10:
    case IDC_QTY_MINUS_BASE + 11:
    {
        int itemIndex = wmId - IDC_QTY_MINUS_BASE;
        OnQuantityButtonPress(itemIndex, false);
        break;
    }

    case IDC_QTY_PLUS_BASE:
    case IDC_QTY_PLUS_BASE + 1:
    case IDC_QTY_PLUS_BASE + 2:
    case IDC_QTY_PLUS_BASE + 3:
    case IDC_QTY_PLUS_BASE + 4:
    case IDC_QTY_PLUS_BASE + 5:
    case IDC_QTY_PLUS_BASE + 6:
    case IDC_QTY_PLUS_BASE + 7:
    case IDC_QTY_PLUS_BASE + 8:
    case IDC_QTY_PLUS_BASE + 9:
    case IDC_QTY_PLUS_BASE + 10:
    case IDC_QTY_PLUS_BASE + 11:
    {
        int itemIndex = wmId - IDC_QTY_PLUS_BASE;
        OnQuantityButtonPress(itemIndex, true);
        break;
    }

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

        if (wcscmp(currentText, L"0.00") == 0) {
            swprintf(currentText, 50, L"%c", digit);
        } else {
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
    }
}

// Handle paint messages
void RestaurantGUI::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (!isLoggedIn) {
        // Draw login background
        DrawLoginBackground(hdc, ps.rcPaint);
    } else {
        // Draw POS interface header
        RECT titleRect = { 0, 0, 1080, 150 };

        // Create gradient background
        HBRUSH hBrushTitle = CreateSolidBrush(COLOR_PRIMARY);
        FillRect(hdc, &titleRect, hBrushTitle);
        DeleteObject(hBrushTitle);

        // Draw decorative separator line
        DrawSeparatorLine(hdc, 0, 150, 1080, 150, COLOR_ACCENT, 3);

        // Draw title text with emoji
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COLOR_ACCENT);
        SelectObject(hdc, hFontTitle);

        RECT textRect = { 20, 30, 1060, 120 };
        DrawText(hdc, L"🌺 LOLA'S KUSINA", -1, &textRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Draw subtitle
        SetTextColor(hdc, COLOR_BACKGROUND);
        SelectObject(hdc, hFontNormal);
        RECT subtitleRect = { 20, 70, 1060, 140 };
        DrawText(hdc, L"Point of Sale System", -1, &subtitleRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Draw category indicator
        RECT categoryRect = { 20, 110, 1060, 140 };
        wstring categoryText = L"Category: " + wstring(currentCategory.begin(), currentCategory.end());
        DrawText(hdc, categoryText.c_str(), -1, &categoryRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    EndPaint(hwnd, &ps);
}

// Draw login background with gradient
void RestaurantGUI::DrawLoginBackground(HDC hdc, const RECT& rect) {
    // Create gradient effect
    RECT gradientRect = rect;

    // Draw gradient background
    for (int i = 0; i < rect.bottom; i += 2) {
        int r = COLOR_LOGIN_BG & 0xFF;
        int g = (COLOR_LOGIN_BG >> 8) & 0xFF;
        int b = (COLOR_LOGIN_BG >> 16) & 0xFF;

        // Create gradient effect
        int gradientFactor = (i * 100) / rect.bottom;
        r = min(255, r + gradientFactor);
        g = min(255, g + gradientFactor);
        b = min(255, b + gradientFactor);

        HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
        RECT lineRect = { 0, i, rect.right, i + 2 };
        FillRect(hdc, &lineRect, hBrush);
        DeleteObject(hBrush);
    }

    // Draw decorative circles
    HPEN hPen = CreatePen(PS_SOLID, 2, COLOR_ACCENT);
    SelectObject(hdc, hPen);

    // Draw some decorative circles
    for (int i = 0; i < 5; i++) {
        int x = 100 + i * 200;
        int y = 100 + i * 150;
        int radius = 30 + i * 10;

        Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
    }

    DeleteObject(hPen);
}

// Create modern layout for POS interface
void RestaurantGUI::CreateModernLayout(HWND hwnd) {
    // Clear any existing controls
    for (HWND btn : menuButtonHandles) {
        if (btn) DestroyWindow(btn);
    }
    menuButtonHandles.clear();

    for (HWND btn : keypadHandles) {
        if (btn) DestroyWindow(btn);
    }
    keypadHandles.clear();

    for (HWND btn : qtyMinusHandles) {
        if (btn) DestroyWindow(btn);
    }
    qtyMinusHandles.clear();

    for (HWND btn : qtyPlusHandles) {
        if (btn) DestroyWindow(btn);
    }
    qtyPlusHandles.clear();

    for (HWND btn : qtyDisplayHandles) {
        if (btn) DestroyWindow(btn);
    }
    qtyDisplayHandles.clear();

    menuQuantities.clear();

    // Create category tabs (modern styling)
    hwndTabAppetizers = CreateWindow(L"BUTTON", L"🥟 APPETIZERS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        30, 180, 240, 60, hwnd, (HMENU)IDC_TAB_APPETIZERS, NULL, NULL);

    hwndTabMain = CreateWindow(L"BUTTON", L"🍲 MAIN DISHES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        280, 180, 240, 60, hwnd, (HMENU)IDC_TAB_MAIN, NULL, NULL);

    hwndTabDesserts = CreateWindow(L"BUTTON", L"🍰 DESSERTS",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        530, 180, 240, 60, hwnd, (HMENU)IDC_TAB_DESSERTS, NULL, NULL);

    hwndTabBeverages = CreateWindow(L"BUTTON", L"🥤 BEVERAGES",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        780, 180, 240, 60, hwnd, (HMENU)IDC_TAB_BEVERAGES, NULL, NULL);

    // Apply fonts to category tabs
    SendMessage(hwndTabAppetizers, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessage(hwndTabMain, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessage(hwndTabDesserts, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessage(hwndTabBeverages, WM_SETFONT, (WPARAM)hFontBold, TRUE);

    // Draw separator line
    HDC hdc = GetDC(hwnd);
    DrawSeparatorLine(hdc, 30, 260, 1050, 260, COLOR_BORDER_LIGHT, 2);
    ReleaseDC(hwnd, hdc);

    // Create enhanced menu grid
    CreateEnhancedMenuGrid(hwnd);

    // Create order display section (right panel)
    CreateWindow(L"STATIC", L"🛒 CURRENT ORDER",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        30, 850, 1020, 40, hwnd, NULL, NULL, NULL);

    hwndOrderList = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
        30, 900, 1020, 300, hwnd, (HMENU)IDC_ORDER_LIST, NULL, NULL);

    // Create numeric keypad
    CreateNumericKeypad(hwnd);

    // Create payment section
    CreatePaymentSection(hwnd);

    // Create action buttons
    CreateWindow(L"BUTTON", L"❌ REMOVE SELECTED",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        30, 1250, 500, 50, hwnd, (HMENU)IDC_BTN_REMOVE, NULL, NULL);

    CreateWindow(L"BUTTON", L"🗑️ CLEAR ORDER",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        550, 1250, 500, 50, hwnd, (HMENU)IDC_BTN_CLEAR, NULL, NULL);

    // Create checkout button
    CreateWindow(L"BUTTON", L"💳 CHECKOUT",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        30, 1320, 1020, 60, hwnd, (HMENU)IDC_BTN_CHECKOUT, NULL, NULL);

    // Apply fonts to controls
    SendMessage(hwndOrderList, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Set current category button color
    SendMessage(hwndTabAppetizers, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// Create enhanced menu grid with quantity controls
void RestaurantGUI::CreateEnhancedMenuGrid(HWND hwnd) {
    // Get menu items for current category
    const MenuItem* menu = core->getMenu();
    int menuSize = core->getMenuSize();

    vector<MenuItem> categoryItems;
    for (int i = 0; i < menuSize; i++) {
        if (menu[i].category == currentCategory) {
            categoryItems.push_back(menu[i]);
        }
    }

    // Calculate grid layout (2 columns for mobile portrait)
    const int cardWidth = 480;
    const int cardHeight = 280;
    const int spacing = 30;
    const int columns = 2;
    const int startX = 30;
    const int startY = 280;

    // Create menu item cards with quantity controls
    for (size_t i = 0; i < categoryItems.size(); i++) {
        int row = i / columns;
        int col = i % columns;
        int x = startX + col * (cardWidth + spacing);
        int y = startY + row * (cardHeight + spacing);

        // Create menu item card
        HWND menuCard = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL,
            WS_VISIBLE | WS_CHILD | SS_NOTIFY,
            x, y, cardWidth, cardHeight,
            hwnd, (HMENU)(IDC_MENU_ITEM_BASE + i), NULL, NULL);

        // Create menu item button
        int controlId = IDC_MENU_ITEM_BASE + static_cast<int>(i);
        HWND menuButton = CreateWindow(L"BUTTON", NULL,
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
            x + 10, y + 10, cardWidth - 20, 180,
            hwnd, (HMENU)controlId, NULL, NULL);

        // Format button text with emoji and price
        string emoji = GetFoodEmoji(categoryItems[i].name);
        wostringstream buttonText;
        buttonText << emoji.c_str() << L"\n\n"
                   << wstring(categoryItems[i].name.begin(), categoryItems[i].name.end()).c_str() << L"\n"
                   << L"₱" << fixed << setprecision(2) << categoryItems[i].price;

        SetWindowText(menuButton, buttonText.str().c_str());
        SendMessage(menuButton, WM_SETFONT, (WPARAM)hMenuFont, TRUE);

        // Initialize quantity
        menuQuantities.push_back(0);

        // Create quantity controls
        HWND qtyMinus = CreateWindow(L"BUTTON", L"➖",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            x + 10, y + 200, 50, 40,
            hwnd, (HMENU)(IDC_QTY_MINUS_BASE + i), NULL, NULL);
        SendMessage(qtyMinus, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        HWND qtyDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", L"0",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            x + 70, y + 200, 80, 40,
            hwnd, (HMENU)(IDC_QTY_DISPLAY_BASE + i), NULL, NULL);
        SendMessage(qtyDisplay, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        HWND qtyPlus = CreateWindow(L"BUTTON", L"➕",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            x + 160, y + 200, 50, 40,
            hwnd, (HMENU)(IDC_QTY_PLUS_BASE + i), NULL, NULL);
        SendMessage(qtyPlus, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        // Add to order button
        HWND addToOrderBtn = CreateWindow(L"BUTTON", L"🛒 ADD TO ORDER",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            x + 220, y + 200, 240, 40,
            hwnd, (HMENU)controlId, NULL, NULL);
        SendMessage(addToOrderBtn, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        // Store handles
        menuButtonHandles.push_back(menuButton);
        qtyMinusHandles.push_back(qtyMinus);
        qtyPlusHandles.push_back(qtyPlus);
        qtyDisplayHandles.push_back(qtyDisplay);
    }
}

// Handle quantity button press
void RestaurantGUI::OnQuantityButtonPress(int itemIndex, bool isIncrease) {
    if (itemIndex < 0 || itemIndex >= menuQuantities.size()) return;

    if (isIncrease) {
        menuQuantities[itemIndex]++;
    } else {
        if (menuQuantities[itemIndex] > 0) {
            menuQuantities[itemIndex]--;
        }
    }

    // Update display
    SetWindowText(qtyDisplayHandles[itemIndex], to_wstring(menuQuantities[itemIndex]).c_str());
    AnimateButtonPress(isIncrease ? qtyPlusHandles[itemIndex] : qtyMinusHandles[itemIndex]);
}

// Create numeric keypad
void RestaurantGUI::CreateNumericKeypad(HWND hwnd) {
    // Clear existing keypad buttons
    for (HWND btn : keypadHandles) {
        if (btn) DestroyWindow(btn);
    }
    keypadHandles.clear();

    const int keySize = 80;
    const int spacing = 10;
    const int startX = 30;
    const int startY = 1420;

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

        SendMessage(keyButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        keypadHandles.push_back(keyButton);
    }
}

// Create payment section
void RestaurantGUI::CreatePaymentSection(HWND hwnd) {
    // Payment section background
    RECT paymentRect = { 320, 1420, 730, 1600 };
    HDC hdc = GetDC(hwnd);
    DrawRoundedRect(hdc, paymentRect, COLOR_CARD_BG, COLOR_PRIMARY, 15);
    ReleaseDC(hwnd, hdc);

    // Payment amount display
    CreateWindow(L"STATIC", L"💰 PAYMENT:",
        WS_VISIBLE | WS_CHILD,
        340, 1440, 150, 30, hwnd, NULL, NULL, NULL);

    hwndPaymentAmount = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0.00",
        WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER,
        500, 1435, 200, 40, hwnd, (HMENU)IDC_PAYMENT_AMOUNT, NULL, NULL);

    // Change display
    CreateWindow(L"STATIC", L"💵 CHANGE:",
        WS_VISIBLE | WS_CHILD,
        340, 1490, 150, 30, hwnd, NULL, NULL, NULL);

    hwndChangeAmount = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0.00",
        WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_READONLY,
        500, 1485, 200, 40, hwnd, (HMENU)IDC_CHANGE_AMOUNT, NULL, NULL);

    // Apply fonts
    SendMessage(hwndPaymentAmount, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
    SendMessage(hwndChangeAmount, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Discount combo box
    CreateWindow(L"STATIC", L"🎫 DISCOUNT:",
        WS_VISIBLE | WS_CHILD,
        340, 1540, 150, 30, hwnd, NULL, NULL, NULL);

    hwndDiscountCombo = CreateWindow(L"COMBOBOX", NULL,
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        500, 1535, 210, 200, hwnd, (HMENU)IDC_DISCOUNT_COMBO, NULL, NULL);

    // Add discount options
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"None");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Student (10%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"Senior (20%)");
    SendMessage(hwndDiscountCombo, CB_ADDSTRING, 0, (LPARAM)L"PWD (20%)");
    SendMessage(hwndDiscountCombo, CB_SETCURSEL, 0, 0);
    SendMessage(hwndDiscountCombo, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

    // Subtotal display (modern styling)
    hwndSubtotalLabel = CreateWindow(L"STATIC", L"Subtotal: ₱0.00",
        WS_VISIBLE | WS_CHILD | SS_RIGHT,
        340, 1580, 370, 30, hwnd, (HMENU)IDC_STATIC_SUBTOTAL, NULL, NULL);
    SendMessage(hwndSubtotalLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
}

// Draw separator line
void RestaurantGUI::DrawSeparatorLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color, int thickness) {
    HPEN hPen = CreatePen(PS_SOLID, thickness, color);
    SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    DeleteObject(hPen);
}

// Draw rounded rectangle
void RestaurantGUI::DrawRoundedRect(HDC hdc, const RECT& rect, COLORREF fillColor, COLORREF borderColor, int radius) {
    HBRUSH hBrush = CreateSolidBrush(fillColor);
    HPEN hPen = CreatePen(PS_SOLID, 2, borderColor);

    SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);

    // Draw rounded rectangle (simplified)
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius * 2, radius * 2);

    DeleteObject(hBrush);
    DeleteObject(hPen);
}

// Update menu display based on current category
void RestaurantGUI::UpdateMenuDisplay() {
    if (!isLoggedIn) return;

    // Update enhanced menu grid
    CreateEnhancedMenuGrid(hwndMain);
}

// Update order display
void RestaurantGUI::UpdateOrderDisplay() {
    if (!isLoggedIn) return;

    SendMessage(hwndOrderList, LB_RESETCONTENT, 0, 0);

    const vector<MenuItem>& order = core->getOrder();
    const vector<int>& quantities = core->getQuantities();

    for (size_t i = 0; i < order.size(); i++) {
        double itemTotal = order[i].price * quantities[i];
        wstring orderItem = to_wstring(quantities[i]) + L" x " +
            wstring(order[i].name.begin(), order[i].name.end()) +
            L" - ₱" + to_wstring(itemTotal);

        SendMessage(hwndOrderList, LB_ADDSTRING, 0, (LPARAM)orderItem.c_str());
    }

    UpdateSubtotalDisplay();
}

// Update subtotal display
void RestaurantGUI::UpdateSubtotalDisplay() {
    if (!isLoggedIn) return;

    double subtotal = core->calculateSubtotal();

    int discountIndex = (int)SendMessage(hwndDiscountCombo, CB_GETCURSEL, 0, 0);
    double discount = core->calculateDiscount(subtotal, discountIndex);
    double grandTotal = subtotal - discount;

    wostringstream subtotalStream;
    subtotalStream << fixed << setprecision(2);
    subtotalStream << L"Subtotal: ₱" << subtotal
                   << L" | Discount: ₱" << discount
                   << L" | Total: ₱" << grandTotal;

    SetWindowText(hwndSubtotalLabel, subtotalStream.str().c_str());

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
    if (hwndQtyEdit) {
        wchar_t qtyText[10];
        GetWindowText(hwndQtyEdit, qtyText, 10);
        int quantity = _wtoi(qtyText);

        quantity += change;
        if (quantity < 1) quantity = 1;
        if (quantity > 99) quantity = 99;

        SetWindowText(hwndQtyEdit, to_wstring(quantity).c_str());
    }
}

// Handle checkout
void RestaurantGUI::OnCheckout() {
    if (core->isOrderEmpty()) {
        MessageBox(hwndMain, L"Please add items to your order before checkout",
            L"Empty Order", MB_OK | MB_ICONWARNING);
        return;
    }

    int discountIndex = (int)SendMessage(hwndDiscountCombo, CB_GETCURSEL, 0, 0);
    double subtotal = core->calculateSubtotal();
    double discount = core->calculateDiscount(subtotal, discountIndex);
    double grandTotal = subtotal - discount;

    wchar_t paymentText[50];
    GetWindowText(hwndPaymentAmount, paymentText, 50);
    double payment = _wtof(paymentText);

    if (payment < grandTotal) {
        MessageBox(hwndMain, L"Insufficient payment! Please enter a valid payment amount.",
            L"Payment Error", MB_OK | MB_ICONERROR);
        return;
    }

    double change = payment - grandTotal;

    string receipt = core->generateReceipt(subtotal, discount, payment, change);
    MessageBox(hwndMain, wstring(receipt.begin(), receipt.end()).c_str(),
        L"🧾 Lola's Kusina - Receipt", MB_OK | MB_ICONINFORMATION);

    core->clearOrder();
    UpdateOrderDisplay();

    SetWindowText(hwndPaymentAmount, L"0.00");
    SetWindowText(hwndChangeAmount, L"0.00");

    UpdateVisualFeedback();
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
    return "🍽️";
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
    InvalidateRect(button, NULL, TRUE);
    UpdateWindow(button);
}

// Update visual feedback
void RestaurantGUI::UpdateVisualFeedback() {
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
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

struct MenuItem {
    int number;
    string name;
    string category;
    double price;
};

class RestaurantCore {
private:
    vector<MenuItem> customerOrder;
    vector<int> orderQuantities;
    const int MENU_SIZE = 12;
    MenuItem menu[12] = {
        {1, "Lumpiang Shanghai", "Appetizers", 120.00},
        {2, "Crispy Calamares", "Appetizers", 150.00},
        {3, "Chicken Adobo", "Main Dishes", 180.00},
        {4, "Beef Kare-Kare", "Main Dishes", 220.00},
        {5, "Sinigang na Baboy", "Main Dishes", 200.00},
        {6, "Grilled Bangus", "Main Dishes", 160.00},
        {7, "Bulalo", "Main Dishes", 220.00},
        {8, "Halo-Halo", "Desserts", 100.00},
        {9, "Leche Flan", "Desserts", 90.00},
        {10, "Buko Juice", "Beverages", 60.00},
        {11, "Iced Tea", "Beverages", 50.00},
        {12, "Coke", "Beverages", 45.00}
    };

public:
    // Get menu for GUI to display
    const MenuItem* getMenu() const { return menu; }
    int getMenuSize() const { return MENU_SIZE; }

    // Get current order for GUI
    const vector<MenuItem>& getOrder() const { return customerOrder; }
    const vector<int>& getQuantities() const { return orderQuantities; }

    // Core functions
    void addToOrder(int itemNumber, int quantity) {
        for (int i = 0; i < MENU_SIZE; ++i) {
            if (menu[i].number == itemNumber) {
                customerOrder.push_back(menu[i]);
                orderQuantities.push_back(quantity);
                break;
            }
        }
    }

    void removeFromOrder(int index) {
        if (index >= 0 && index < customerOrder.size()) {
            customerOrder.erase(customerOrder.begin() + index);
            orderQuantities.erase(orderQuantities.begin() + index);
        }
    }

    double calculateSubtotal() const {
        double subtotal = 0.0;
        for (size_t i = 0; i < customerOrder.size(); ++i) {
            subtotal += customerOrder[i].price * orderQuantities[i];
        }
        return subtotal;
    }

    double calculateDiscount(double subtotal, int discountChoice) const {
        switch (discountChoice) {
        case 1: return subtotal * 0.10; // Student
        case 2: return subtotal * 0.20; // Senior
        case 3: return subtotal * 0.20; // PWD
        default: return 0.0;
        }
    }

    string generateReceipt(double subtotal, double discount, double payment, double change) const {
        stringstream receipt;
        receipt << fixed << setprecision(2);

        receipt << "=========================================\n";
        receipt << "           LOLAS KUSINA\n";
        receipt << "          \"MGA LUTO NI LOLA\"\n";
        receipt << "=========================================\n";
        receipt << "ITEM                QTY   PRICE   TOTAL\n";
        receipt << "-----------------------------------------\n";

        for (size_t i = 0; i < customerOrder.size(); ++i) {
            double itemTotal = customerOrder[i].price * orderQuantities[i];
            receipt << customerOrder[i].name << "\t\t"
                << orderQuantities[i] << "\tPHP " << customerOrder[i].price
                << "\tPHP " << itemTotal << endl;
        }

        receipt << "-----------------------------------------\n";
        receipt << "Subtotal:\t\t\tPHP " << subtotal << endl;
        receipt << "Discount:\t\t\tPHP " << discount << endl;
        receipt << "-----------------------------------------\n";
        receipt << "GRAND TOTAL:\t\t\tPHP " << (subtotal - discount) << endl;
        receipt << "Payment:\t\t\tPHP " << payment << endl;
        receipt << "Change:\t\t\t\tPHP " << change << endl;
        receipt << "=========================================\n";
        receipt << "  SALAMAT PO! MARAMING SALAMAT!\n";
        receipt << "=========================================\n";

        return receipt.str();
    }

    void clearOrder() {
        customerOrder.clear();
        orderQuantities.clear();
    }

    bool isOrderEmpty() const {
        return customerOrder.empty();
    }
};
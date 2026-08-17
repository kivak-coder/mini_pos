#pragma once

#include "domain/ProductCatalog.h"
#include "domain/Sale.h"
#include "application/CashPaymentService.h"
#include "application/CheckoutService.h"
#include "application/ReceiptRepository.h"
#include "application/ReceiptReader.h"
#include "ui/ReceiptHistoryDialog.h"


#include <QMainWindow>

class CartTableModel;
class ProductTableModel;
class QLabel;
class QPushButton;   
class QSpinBox;
class QTableView;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const pos::ProductCatalog& catalog, pos::Sale& sale, pos::application::ReceiptRepository& receiptRepository,
        const pos::application::ReceiptReader& receiptReader_, QWidget* parent = nullptr);

private:
    void buildUi();
    void connectSignals();

    void addSelectedProduct();
    void removeSelectedItem();
    void changeSelectedQuantity();
    void updateTotal();
    void payCurrentSale();

    const pos::ProductCatalog& catalog_;
    pos::Sale& sale_;
    const pos::application::ReceiptReader& receiptReader_;

    ProductTableModel* productModel_ = nullptr;
    CartTableModel* cartModel_ = nullptr;

    QTableView* productTable_ = nullptr;
    QTableView* cartTable_ = nullptr;

    QSpinBox* addQuantitySpinBox_ = nullptr;
    QSpinBox* cartQuantitySpinBox_ = nullptr;

    QPushButton* addButton_ = nullptr;
    QPushButton* historyButton = nullptr;
    QPushButton* removeButton_ = nullptr;
    QPushButton* changeQuantityButton_ = nullptr;

    QLabel* totalLabel_ = nullptr;
    pos::application::CashPaymentService paymentService_;


    pos::application::CheckoutService checkoutService_;

    QPushButton* paymentButton_ = nullptr;
};
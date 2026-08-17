#include "ui/MainWindow.h"

#include "ui/CartTableModel.h"
#include "ui/MoneyFormatter.h"
#include "ui/ProductTableModel.h"
#include "ui/PaymentDialog.h"
#include "application/CheckoutService.h"


#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(
    const pos::ProductCatalog& catalog,
    pos::Sale& sale,
    pos::application::ReceiptRepository&
        receiptRepository,
    QWidget* parent)
    : QMainWindow(parent),
      catalog_(catalog),
      sale_(sale),
      paymentService_(),
      checkoutService_(
          paymentService_,
          receiptRepository)
{
    productModel_ = new ProductTableModel(catalog_, this);
    cartModel_ = new CartTableModel(sale_, this);

    buildUi();
    connectSignals();
    updateTotal();
}

void MainWindow::buildUi()
{
    setWindowTitle(tr("Mini POS"));
    resize(1100, 650);

    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    auto* catalogLayout = new QVBoxLayout;
    auto* cartLayout = new QVBoxLayout;

    auto* catalogTitle = new QLabel(
        tr("Каталог товаров"),
        centralWidget);

    productTable_ = new QTableView(centralWidget);
    productTable_->setModel(productModel_);
    productTable_->setSelectionBehavior(
        QAbstractItemView::SelectRows);
    productTable_->setSelectionMode(
        QAbstractItemView::SingleSelection);
    productTable_->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    productTable_->horizontalHeader()
        ->setStretchLastSection(true);

    auto* addControlsLayout = new QHBoxLayout;

    addQuantitySpinBox_ = new QSpinBox(centralWidget);
    addQuantitySpinBox_->setRange(1, 99);
    addQuantitySpinBox_->setValue(1);
    addQuantitySpinBox_->setPrefix(tr("Количество: "));

    addButton_ = new QPushButton(
        tr("Добавить в чек"),
        centralWidget);

    addControlsLayout->addWidget(addQuantitySpinBox_);
    addControlsLayout->addWidget(addButton_);

    catalogLayout->addWidget(catalogTitle);
    catalogLayout->addWidget(productTable_);
    catalogLayout->addLayout(addControlsLayout);

    auto* cartTitle = new QLabel(
        tr("Текущий чек"),
        centralWidget);

    cartTable_ = new QTableView(centralWidget);
    cartTable_->setModel(cartModel_);
    cartTable_->setSelectionBehavior(
        QAbstractItemView::SelectRows);
    cartTable_->setSelectionMode(
        QAbstractItemView::SingleSelection);
    cartTable_->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    cartTable_->horizontalHeader()
        ->setStretchLastSection(true);

    auto* cartControlsLayout = new QHBoxLayout;

    cartQuantitySpinBox_ = new QSpinBox(centralWidget);
    cartQuantitySpinBox_->setRange(1, 99);
    cartQuantitySpinBox_->setValue(1);

    changeQuantityButton_ = new QPushButton(
        tr("Изменить количество"),
        centralWidget);

    removeButton_ = new QPushButton(
        tr("Удалить"),
        centralWidget);

    cartControlsLayout->addWidget(cartQuantitySpinBox_);
    cartControlsLayout->addWidget(changeQuantityButton_);
    cartControlsLayout->addWidget(removeButton_);

    totalLabel_ = new QLabel(centralWidget);
    paymentButton_ = new QPushButton(
    tr("Оплатить наличными"),
    centralWidget);

    paymentButton_->setMinimumHeight(42);

    QFont totalFont = totalLabel_->font();
    totalFont.setPointSize(16);
    totalFont.setBold(true);
    totalLabel_->setFont(totalFont);
    totalLabel_->setAlignment(Qt::AlignRight);

    cartLayout->addWidget(cartTitle);
    cartLayout->addWidget(cartTable_);
    cartLayout->addLayout(cartControlsLayout);
    cartLayout->addWidget(totalLabel_);
    cartLayout->addWidget(paymentButton_);

    mainLayout->addLayout(catalogLayout, 1);
    mainLayout->addLayout(cartLayout, 2);

    setCentralWidget(centralWidget);
}

void MainWindow::connectSignals()
{
    connect(
        addButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::addSelectedProduct);

    connect(
        productTable_,
        &QTableView::doubleClicked,
        this,
        [this](const QModelIndex&)
        {
            addSelectedProduct();
        });

    connect(
        removeButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::removeSelectedItem);

    connect(
        changeQuantityButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::changeSelectedQuantity);

    connect(
        cartTable_->selectionModel(),
        &QItemSelectionModel::currentRowChanged,
        this,
        [this](
            const QModelIndex& current,
            const QModelIndex&)
        {
            const int quantity =
                cartModel_->quantityAt(current.row());

            if (quantity > 0)
            {
                cartQuantitySpinBox_->setValue(quantity);
            }
        });

    connect(
    paymentButton_,
    &QPushButton::clicked,
    this,
    &MainWindow::payCurrentSale);
}

void MainWindow::addSelectedProduct()
{
    const QModelIndex index = productTable_->currentIndex();

    const pos::Product* product =
        productModel_->productAt(index.row());

    if (product == nullptr)
    {
        QMessageBox::information(
            this,
            tr("Товар не выбран"),
            tr("Выберите товар из каталога."));
        return;
    }

    sale_.addProduct(
        *product,
        addQuantitySpinBox_->value());

    cartModel_->refresh();
    updateTotal();
}

void MainWindow::removeSelectedItem()
{
    const QModelIndex index = cartTable_->currentIndex();

    const pos::ProductId productId =
        cartModel_->productIdAt(index.row());

    if (productId == 0)
    {
        QMessageBox::information(
            this,
            tr("Позиция не выбрана"),
            tr("Выберите позицию в чеке."));
        return;
    }

    sale_.removeProduct(productId);

    cartModel_->refresh();
    updateTotal();
}

void MainWindow::changeSelectedQuantity()
{
    const QModelIndex index = cartTable_->currentIndex();

    const pos::ProductId productId =
        cartModel_->productIdAt(index.row());

    if (productId == 0)
    {
        QMessageBox::information(
            this,
            tr("Позиция не выбрана"),
            tr("Выберите позицию в чеке."));
        return;
    }

    sale_.setQuantity(
        productId,
        cartQuantitySpinBox_->value());

    cartModel_->refresh();
    updateTotal();
}

void MainWindow::updateTotal()
{
    totalLabel_->setText(
        tr("Итого: %1")
            .arg(pos::ui::formatMoney(sale_.total())));
}

void MainWindow::payCurrentSale()
{
    if (sale_.empty())
    {
        QMessageBox::information(
            this,
            tr("Пустой чек"),
            tr("Добавьте хотя бы один товар."));
        return;
    }

    PaymentDialog dialog(
        sale_,
        paymentService_,
        this);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const auto result = dialog.result();

    QMessageBox::information(
        this,
        tr("Оплата принята"),
        tr(
            "Продажа успешно завершена.\n"
            "Сумма: %1\n"
            "Получено: %2\n"
            "Сдача: %3")
            .arg(pos::ui::formatMoney(result.total))
            .arg(pos::ui::formatMoney(result.received))
            .arg(pos::ui::formatMoney(result.change)));

    try
    {
        const auto checkout =
            checkoutService_.complete(
                sale_,
                dialog.result().received);

        if (!checkout.successful())
        {
            QMessageBox::warning(
                this,
                tr("Оплата не завершена"),
                tr("Не удалось завершить продажу."));
            return;
        }

        QMessageBox::information(
            this,
            tr("Оплата принята"),
            tr(
                "Чек №%1 успешно сохранён.\n"
                "Сумма: %2\n"
                "Получено: %3\n"
                "Сдача: %4")
                .arg(static_cast<qlonglong>(checkout.receiptId))
                .arg(pos::ui::formatMoney(checkout.payment.total))
                .arg(pos::ui::formatMoney(checkout.payment.received))
                .arg(pos::ui::formatMoney(checkout.payment.change))
        );

        cartModel_->refresh();
        cartTable_->clearSelection();

        cartQuantitySpinBox_->setValue(1);
        addQuantitySpinBox_->setValue(1);

        updateTotal();
    }
    catch (const std::exception& error)
    {
        QMessageBox::critical(
            this,
            tr("Ошибка базы данных"),
            tr(
                "Не удалось сохранить чек.\n"
                "Текущий чек не был очищен.\n\n%1")
                .arg(QString::fromUtf8(error.what())));
    }
                
    // cartModel_->refresh();
    // cartTable_->clearSelection();

    // cartQuantitySpinBox_->setValue(1);
    // addQuantitySpinBox_->setValue(1);

    // updateTotal();
}
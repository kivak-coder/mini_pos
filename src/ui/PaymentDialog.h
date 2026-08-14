#pragma once

#include "application/CashPaymentService.h"
#include "domain/Sale.h"

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class PaymentDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(
        const pos::Sale& sale,
        const pos::application::CashPaymentService& service, QWidget* parent = nullptr);

    const pos::application::CashPaymentResult& result() const noexcept;

private:
    void updatePaymentPreview();
    void tryAcceptPayment();

    const pos::Sale& sale_;
    const pos::application::CashPaymentService& service_;

    pos::application::CashPaymentResult result_;

    QLineEdit* receivedEdit_ = nullptr;
    QLabel* changeLabel_ = nullptr;
    QLabel* errorLabel_ = nullptr;
    QPushButton* payButton_ = nullptr;
};
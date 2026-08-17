#include "ui/PaymentDialog.h"
#include "ui/MoneyFormatter.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringList>
#include <optional>

namespace {

std::optional<pos::Money> parseMoney(QString text) {
    text = text.trimmed();
    text.replace(',', '.');

    if (text.isEmpty()) {
        return std::nullopt; // indicate that optional has no value
    }

    const QStringList parts = text.split('.');
    if (parts.size() > 2 || parts.front().isEmpty()) {
        return std::nullopt;
    }

    bool rublesValid = false;
    const qlonglong rubles = parts.front().toLongLong(&rublesValid);
    if (!rublesValid || rubles < 0) {
        return std::nullopt;
    }

    QString fraction = parts.size() == 2 ? parts.back() : QString();

    if (fraction.size() > 2) {
        return std::nullopt;
    }

    while (fraction.size() < 2) {
        fraction.append(QLatin1Char('0'));
    }

    bool kopeksValid = false;

    const qlonglong kopeks = fraction.toLongLong(&kopeksValid);

    if (!kopeksValid || kopeks < 0) {
        return std::nullopt;
    }

    return static_cast<pos::Money>(rubles * 100 + kopeks);
}
}

PaymentDialog::PaymentDialog(const pos::Sale& sale, const pos::application::CashPaymentService& service, QWidget* parent) 
    : QDialog(parent), sale_(sale), service_(service) 
    {
        setWindowTitle("Оплата наличными");
        setModal(true);
        setMinimumWidth(420);

        auto * mainLayout = new QVBoxLayout(this);
        auto* formLayout = new QFormLayout;

        auto * totalLabel = new QLabel(pos::ui::formatMoney(sale_.total()), this);
        QFont totalFont = totalLabel->font();
        totalFont.setBold(true);
        totalFont.setPointSize(14);
        totalLabel->setFont(totalFont);

        receivedEdit_  = new QLineEdit(this);
        receivedEdit_->setPlaceholderText(tr("Например: 1500,00"));

        const QRegularExpression inputPattern(QStringLiteral("^\\d{0,9}([.,]\\d{0,2})?$"));

        receivedEdit_->setValidator(new QRegularExpressionValidator(inputPattern, receivedEdit_));

        changeLabel_ = new QLabel(tr("—"), this);

        errorLabel_ = new QLabel(this);
        errorLabel_->setStyleSheet(QStringLiteral("color: #b00020;"));
        errorLabel_->setWordWrap(true);

        formLayout->addRow(tr("Сумма чека:"), totalLabel);
        formLayout->addRow(tr("Получено:"), receivedEdit_);
        formLayout->addRow(tr("Сдача:"), changeLabel_);

        auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        payButton_ = buttonBox->button(QDialogButtonBox::Ok);
        payButton_->setText(tr("Принять оплату"));
        payButton_->setEnabled(false);
        payButton_->setDefault(true);
        buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(errorLabel_);
        mainLayout->addWidget(buttonBox);

        connect(receivedEdit_, &QLineEdit::textChanged, this, [this] 
        {
            updatePaymentPreview();
        });

        connect(buttonBox, &QDialogButtonBox::accepted, this, &PaymentDialog::tryAcceptPayment);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &PaymentDialog::reject);

        receivedEdit_->setFocus();
    }

    const pos::application::CashPaymentResult& PaymentDialog::result() const noexcept {
        return result_;  // whi this word do not work
    }

    void PaymentDialog::updatePaymentPreview() {
        const std::optional<pos::Money> received = parseMoney(receivedEdit_->text());
        if (!received.has_value()) {
            result_ = {};
            changeLabel_->setText(tr("-"));
            errorLabel_->clear();
            payButton_->setEnabled(false);
            return;
        }

        result_ = service_.evaluate(sale_, *received);
        
        using pos::application::PaymentStatus;

        switch (result_.status)
        {
            case PaymentStatus::Success:
                changeLabel_->setText(pos::ui::formatMoney(result_.change));
                errorLabel_->clear();
                payButton_->setEnabled(true);
                break;

            case PaymentStatus::EmptySale:
                changeLabel_->setText(tr("—"));
                errorLabel_->setText(tr("Нельзя оплатить пустой чек."));
                payButton_->setEnabled(false);
                break;
            
            case PaymentStatus::NonPositiveAmount:
                changeLabel_->setText(tr("—"));
                errorLabel_->setText(tr("Полученная сумма должна быть больше нуля."));
                payButton_->setEnabled(false);
                break;
            
            case PaymentStatus::InsufficientFunds:
                changeLabel_->setText(tr("Не хватает: %1").arg(pos::ui::formatMoney(result_.total - result_.received)));
                errorLabel_->setText(tr("Полученной суммы недостаточно."));
                payButton_->setEnabled(false);
                break;
            
        }
    }

    void PaymentDialog::tryAcceptPayment() {
        updatePaymentPreview();
        if (result_.successful()) {
            accept();
        }
    }

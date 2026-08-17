#include "infrastructure/SqliteReceiptRepository.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <stdexcept>
#include <utility>

namespace
{

std::runtime_error queryError(
    const QString& context,
    const QSqlError& error)
{
    return std::runtime_error(
        QStringLiteral("%1: %2")
            .arg(context, error.text())
            .toStdString());
}

}

namespace pos::infrastructure
{

SqliteReceiptRepository::SqliteReceiptRepository(
    QString connectionName)
    : connectionName_(std::move(connectionName))
{
}

ReceiptId SqliteReceiptRepository::save(
    const Receipt& receipt)
{
    QSqlDatabase database =
        QSqlDatabase::database(connectionName_);

    if (!database.isValid() || !database.isOpen())
    {
        throw std::runtime_error(
            "SQLite database is not open");
    }

    if (!database.transaction())
    {
        throw queryError(
            QStringLiteral(
                "Cannot start database transaction"),
            database.lastError());
    }

    try
    {
        QSqlQuery receiptQuery(database);

        receiptQuery.prepare(QString::fromUtf8(R"SQL(
            INSERT INTO receipts (
                created_at,
                total_kopecks,
                received_kopecks,
                change_kopecks
            )
            VALUES (
                :created_at,
                :total,
                :received,
                :change
            )
        )SQL"));

        receiptQuery.bindValue(
            QStringLiteral(":created_at"),
            static_cast<qlonglong>(
                receipt.createdAtUnixSeconds()));

        receiptQuery.bindValue(
            QStringLiteral(":total"),
            static_cast<qlonglong>(
                receipt.total()));

        receiptQuery.bindValue(
            QStringLiteral(":received"),
            static_cast<qlonglong>(
                receipt.received()));

        receiptQuery.bindValue(
            QStringLiteral(":change"),
            static_cast<qlonglong>(
                receipt.change()));

        if (!receiptQuery.exec())
        {
            throw queryError(
                QStringLiteral(
                    "Cannot insert receipt"),
                receiptQuery.lastError());
        }

        bool receiptIdIsValid = false;

        const qlonglong receiptId =
            receiptQuery
                .lastInsertId()
                .toLongLong(&receiptIdIsValid);

        if (!receiptIdIsValid || receiptId <= 0)
        {
            throw std::runtime_error(
                "SQLite returned invalid receipt id");
        }

        QSqlQuery itemQuery(database);

        itemQuery.prepare(QString::fromUtf8(R"SQL(
            INSERT INTO receipt_items (
                receipt_id,
                line_number,
                product_id,
                product_name,
                unit_price_kopecks,
                quantity,
                line_total_kopecks
            )
            VALUES (
                :receipt_id,
                :line_number,
                :product_id,
                :product_name,
                :unit_price,
                :quantity,
                :line_total
            )
        )SQL"));

        int lineNumber = 0;

        for (const ReceiptItem& item :
             receipt.items())
        {
            itemQuery.bindValue(
                QStringLiteral(":receipt_id"),
                receiptId);

            itemQuery.bindValue(
                QStringLiteral(":line_number"),
                lineNumber);

            itemQuery.bindValue(
                QStringLiteral(":product_id"),
                static_cast<qlonglong>(
                    item.productId()));

            itemQuery.bindValue(
                QStringLiteral(":product_name"),
                QString::fromUtf8(
                    item.productName().c_str()));

            itemQuery.bindValue(
                QStringLiteral(":unit_price"),
                static_cast<qlonglong>(
                    item.unitPrice()));

            itemQuery.bindValue(
                QStringLiteral(":quantity"),
                item.quantity());

            itemQuery.bindValue(
                QStringLiteral(":line_total"),
                static_cast<qlonglong>(
                    item.lineTotal()));

            if (!itemQuery.exec())
            {
                throw queryError(
                    QStringLiteral(
                        "Cannot insert receipt item"),
                    itemQuery.lastError());
            }

            ++lineNumber;
        }

        if (!database.commit())
        {
            throw queryError(
                QStringLiteral(
                    "Cannot commit receipt transaction"),
                database.lastError());
        }

        return static_cast<ReceiptId>(receiptId);
    }
    catch (...)
    {
        database.rollback();
        throw;
    }
}

}
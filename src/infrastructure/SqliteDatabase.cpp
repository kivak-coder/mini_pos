#include "infrastructure/SqliteDatabase.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <stdexcept>
#include <utility>

namespace
{

std::runtime_error databaseError(
    const QString& context,
    const QSqlError& error)
{
    return std::runtime_error(
        QStringLiteral("%1: %2")
            .arg(context, error.text())
            .toStdString());
}

void executeOrThrow(
    QSqlDatabase& database,
    const QString& sql)
{
    QSqlQuery query(database);

    if (!query.exec(sql))
    {
        throw databaseError(
            QStringLiteral("SQL statement failed"),
            query.lastError());
    }
}

}

namespace pos::infrastructure
{

SqliteDatabase::SqliteDatabase(
    QString filePath,
    QString connectionName)
    : connectionName_(std::move(connectionName))
{
    if (!QSqlDatabase::isDriverAvailable(
            QStringLiteral("QSQLITE")))
    {
        throw std::runtime_error(
            "Qt SQLite driver QSQLITE is unavailable");
    }

    if (QSqlDatabase::contains(connectionName_))
    {
        throw std::runtime_error(
            "Database connection name is already used");
    }

    QSqlDatabase database =
        QSqlDatabase::addDatabase(
            QStringLiteral("QSQLITE"),
            connectionName_);

    database.setDatabaseName(filePath);

    if (!database.open())
    {
        const std::runtime_error error =
            databaseError(
                QStringLiteral(
                    "Cannot open SQLite database"),
                database.lastError());

        database = QSqlDatabase{};
        QSqlDatabase::removeDatabase(connectionName_);

        throw error;
    }

    try
    {
        initializeSchema(database);
    }
    catch (...)
    {
        database.close();
        database = QSqlDatabase{};

        QSqlDatabase::removeDatabase(connectionName_);
        throw;
    }
}

SqliteDatabase::~SqliteDatabase()
{
    {
        QSqlDatabase database =
            QSqlDatabase::database(
                connectionName_,
                false);

        if (database.isValid())
        {
            database.close();
        }
    }

    QSqlDatabase::removeDatabase(connectionName_);
}

const QString&
SqliteDatabase::connectionName() const noexcept
{
    return connectionName_;
}

void SqliteDatabase::initializeSchema(
    QSqlDatabase& database)
{
    executeOrThrow(
        database,
        QStringLiteral("PRAGMA foreign_keys = ON"));

    executeOrThrow(
        database,
        QString::fromUtf8(R"SQL(
            CREATE TABLE IF NOT EXISTS receipts (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                created_at INTEGER NOT NULL,
                total_kopecks INTEGER NOT NULL
                    CHECK (total_kopecks > 0),
                received_kopecks INTEGER NOT NULL,
                change_kopecks INTEGER NOT NULL,

                CHECK (
                    received_kopecks >= total_kopecks
                ),

                CHECK (
                    change_kopecks =
                    received_kopecks - total_kopecks
                )
            )
        )SQL"));

    executeOrThrow(
        database,
        QString::fromUtf8(R"SQL(
            CREATE TABLE IF NOT EXISTS receipt_items (
                receipt_id INTEGER NOT NULL,
                line_number INTEGER NOT NULL,
                product_id INTEGER NOT NULL,
                product_name TEXT NOT NULL,
                unit_price_kopecks INTEGER NOT NULL,
                quantity INTEGER NOT NULL,
                line_total_kopecks INTEGER NOT NULL,

                PRIMARY KEY (
                    receipt_id,
                    line_number
                ),

                FOREIGN KEY (receipt_id)
                    REFERENCES receipts(id)
                    ON DELETE CASCADE,

                CHECK (unit_price_kopecks > 0),
                CHECK (quantity > 0),

                CHECK (
                    line_total_kopecks =
                    unit_price_kopecks * quantity
                )
            )
        )SQL"));

    executeOrThrow(
        database,
        QStringLiteral("PRAGMA user_version = 1"));
}

}
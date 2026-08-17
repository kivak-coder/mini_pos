#define BOOST_TEST_MODULE SqliteReceiptRepositoryTests
#include <boost/test/included/unit_test.hpp>

#include "domain/Receipt.h"
#include "infrastructure/SqliteDatabase.h"
#include "infrastructure/SqliteReceiptRepository.h"

#include <QCoreApplication>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QVariant>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

class QtApplicationFixture final
{
public:
    QtApplicationFixture()
    {
        if (QCoreApplication::instance() == nullptr) {
            application_ =
                std::make_unique<QCoreApplication>(argc_, argv_);
        }
    }

private:
    int argc_ = 1;
    char applicationName_[32] = "repository_tests";
    char* argv_[2] = {applicationName_, nullptr};

    std::unique_ptr<QCoreApplication> application_;
};

BOOST_GLOBAL_FIXTURE(QtApplicationFixture);

void executeOrThrow(
    const QSqlDatabase& database,
    const QString& sql)
{
    QSqlQuery query(database);

    if (!query.exec(sql)) {
        throw std::runtime_error(
            query.lastError().text().toStdString());
    }
}

long long queryInteger(
    const QSqlDatabase& database,
    const QString& sql)
{
    QSqlQuery query(database);

    if (!query.exec(sql)) {
        throw std::runtime_error(
            query.lastError().text().toStdString());
    }

    if (!query.next()) {
        throw std::runtime_error(
            "SQL query did not return a row");
    }

    return query.value(0).toLongLong();
}

pos::Receipt makeReceipt()
{
    std::vector<pos::ReceiptItem> items;

    items.emplace_back(
        pos::ProductId{1},
        "Milk",
        pos::Money{8990},
        2);

    items.emplace_back(
        pos::ProductId{2},
        "Bread",
        pos::Money{5990},
        1);

    constexpr std::int64_t createdAtUnixSeconds =
        1'700'000'000;

    return pos::Receipt{
        createdAtUnixSeconds,
        std::move(items),
        pos::Money{30000}
    };
}

class RepositoryFixture
{
public:
    RepositoryFixture()
        : databasePath_(temporaryDirectory_.filePath("test.sqlite3"))
    {
        if (!temporaryDirectory_.isValid()) {
            throw std::runtime_error(
                temporaryDirectory_.errorString().toStdString());
        }

        database_ = std::make_unique<
            pos::infrastructure::SqliteDatabase>(
                databasePath_);

        repository_ = std::make_unique<
            pos::infrastructure::SqliteReceiptRepository>(
                database_->connectionName());
    }

    QSqlDatabase connection() const
    {
        const QSqlDatabase database =
            QSqlDatabase::database(
                database_->connectionName(),
                false);

        if (!database.isValid()) {
            throw std::runtime_error(
                "Test database connection is invalid");
        }

        return database;
    }

    pos::infrastructure::SqliteReceiptRepository& repository()
    {
        return *repository_;
    }

    const QString& databasePath() const noexcept
    {
        return databasePath_;
    }

private:
    // Порядок объявления важен.
    // Поля уничтожаются в обратном порядке:
    // repository -> database -> temporary directory.

    QTemporaryDir temporaryDirectory_;
    QString databasePath_;

    std::unique_ptr<
        pos::infrastructure::SqliteDatabase> database_;

    std::unique_ptr<
        pos::infrastructure::SqliteReceiptRepository> repository_;
};

} // namespace

BOOST_FIXTURE_TEST_SUITE(
    SqliteReceiptRepositorySuite,
    RepositoryFixture)

BOOST_AUTO_TEST_CASE(database_file_is_created)
{
    BOOST_TEST(QFile::exists(databasePath()));
}

BOOST_AUTO_TEST_CASE(database_contains_required_tables)
{
    const QSqlDatabase database = connection();

    const QStringList tables = database.tables();

    BOOST_TEST(tables.contains("receipts"));
    BOOST_TEST(tables.contains("receipt_items"));
}

BOOST_AUTO_TEST_CASE(save_writes_receipt_and_all_items)
{
    const pos::Receipt receipt = makeReceipt();

    const pos::ReceiptId receiptId =
        repository().save(receipt);

    BOOST_TEST(receiptId > 0);

    const QSqlDatabase database = connection();

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT COUNT(*) FROM receipts") == 1);

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT COUNT(*) FROM receipt_items") == 2);

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT total_kopecks FROM receipts LIMIT 1")
        == 23970);

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT received_kopecks FROM receipts LIMIT 1")
        == 30000);

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT change_kopecks FROM receipts LIMIT 1")
        == 6030);
}

BOOST_AUTO_TEST_CASE(
    failed_item_insert_rolls_back_parent_receipt)
{
    const QSqlDatabase database = connection();

    /*
     * Намеренно повреждаем схему.
     *
     * Репозиторий сначала вставит строку в receipts,
     * а потом попытается вставить позиции в receipt_items.
     * Вторая операция завершится ошибкой.
     */
    executeOrThrow(
        database,
        "DROP TABLE receipt_items");

    BOOST_CHECK_THROW(
        repository().save(makeReceipt()),
        std::runtime_error);

    /*
     * Если транзакция реализована правильно,
     * первая вставка тоже должна быть отменена.
     *
     * В receipts не должно остаться половины чека.
     */
    BOOST_TEST(
        queryInteger(
            database,
            "SELECT COUNT(*) FROM receipts") == 0);
}

BOOST_AUTO_TEST_CASE(
    previous_receipts_remain_after_failed_save)
{
    repository().save(makeReceipt());

    const QSqlDatabase database = connection();

    BOOST_TEST(
        queryInteger(
            database,
            "SELECT COUNT(*) FROM receipts") == 1);

    executeOrThrow(
        database,
        "DROP TABLE receipt_items");

    BOOST_CHECK_THROW(
        repository().save(makeReceipt()),
        std::runtime_error);


    BOOST_TEST(
        queryInteger(
            database,
            "SELECT COUNT(*) FROM receipts") == 1);
}

BOOST_AUTO_TEST_SUITE_END()
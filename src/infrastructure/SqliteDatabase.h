#pragma once

#include <QString>

class QSqlDatabase;

namespace pos::infrastructure
{

class SqliteDatabase final
{
public:
    explicit SqliteDatabase(
        QString filePath,
        QString connectionName =
            QStringLiteral("mini_pos_main"));

    ~SqliteDatabase();

    SqliteDatabase(const SqliteDatabase&) = delete;
    SqliteDatabase& operator=(
        const SqliteDatabase&) = delete;

    const QString& connectionName() const noexcept;

private:
    void initializeSchema(QSqlDatabase& database);

    QString connectionName_;
};

}
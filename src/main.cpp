#include "domain/Sale.h"
#include "infrastructure/ProductCatalogDemo.h"
#include "infrastructure/SqliteDatabase.h"
#include "infrastructure/SqliteReceiptRepository.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

#include <exception>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    QCoreApplication::setOrganizationName(
        QStringLiteral("MiniPosDatabase"));

    QCoreApplication::setApplicationName(
        QStringLiteral("MiniPOS"));

    try
    {
        const QString dataDirectory =
            QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);

        if (dataDirectory.isEmpty() ||
            !QDir{}.mkpath(dataDirectory))
        {
            throw std::runtime_error(
                "Cannot create application data directory");
        }

        const QString databasePath =
            dataDirectory +
            QStringLiteral("/mini_pos.sqlite");

        pos::infrastructure::SqliteDatabase database(databasePath);

        pos::infrastructure::SqliteReceiptRepository
            receiptRepository(
                database.connectionName());

        pos::ProductCatalog catalog =
            pos::infrastructure::
                makeDemoProductCatalog();

        pos::Sale currentSale;

        MainWindow window(
            catalog,
            currentSale,
            receiptRepository);

        window.show();

        return application.exec();
    }
    catch (const std::exception& error)
    {
        QMessageBox::critical(nullptr, QStringLiteral("Ошибка запуска"), QString::fromUtf8(error.what()));
        return 1;
    }
}
#include "domain/Sale.h"
#include "infrastructure/ProductCatalogDemo.h"
#include "ui/MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    pos::ProductCatalog catalog =
        pos::infrastructure::makeDemoProductCatalog();

    pos::Sale currentSale;

    MainWindow window(catalog, currentSale);
    window.show();

    return application.exec();
}
#include "ui/MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Mini POS"));
    resize(1000, 650);

    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);
    auto* title = new QLabel(tr("Кассовый модуль"), centralWidget);

    title->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);
    setCentralWidget(centralWidget);
}
#include "mainwindow.h"
#include "dashboard.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyleFactory>
#include <QSettings>
#include <QApplication>
#include <QStatusBar>
#include <QTabWidget>
#include <QIcon>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    try {

        QApplication::setStyle(QStyleFactory::create("Fusion"));

        initializeUI();
        loadWindowState();
        statusBar->showMessage(tr("Ready"), 2000);

    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, tr("Initialization Error"),
                              tr("Failed to initialize application: %1").arg(e.what()));
        exit(EXIT_FAILURE);
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::initializeUI()
{

    setWindowTitle(tr("Academic Pro – Your Daily Learning Tracker"));
    setMinimumSize(800, 600);

     statusBar = std::make_unique<QStatusBar>();
    setStatusBar(statusBar.get());

    tabWidget = std::make_unique<QTabWidget>(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget.get());

    dashboard = std::make_unique<Dashboard>();

    tabWidget->addTab(dashboard.get(), QIcon(":/icons/dashboard.png"), tr("Dashboard"));

    tabWidget->setCurrentIndex(0);
}


void MainWindow::saveWindowState()
{
    QSettings settings("SmartStudent", "MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("currentTab", tabWidget->currentIndex());
}

void MainWindow::loadWindowState()
{
    QSettings settings("SmartStudent", "MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    const int tabIndex = settings.value("currentTab", 0).toInt();
    if (tabIndex >= 0 && tabIndex < tabWidget->count()) {
        tabWidget->setCurrentIndex(tabIndex);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowState();

    QSettings settings("SmartStudent", "ApplicationData");

    if (dashboard) {
        dashboard->saveData(&settings);
    }

    QMainWindow::closeEvent(event);
}

#include "mainwindow.h"
#include "devicetreewidget.h"
#include "performancewidget.h"
#include "devicecontrolwidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // Создаем представления (виджеты) для каждой вкладки:
    DeviceTreeWidget* devicesWidget = new DeviceTreeWidget(this);
    PerformanceWidget* performanceWidget = new PerformanceWidget(this);
    DeviceControlWidget* controlWidget = new DeviceControlWidget(this);

    // Добавляем вкладки:
    m_tabWidget->addTab(devicesWidget, tr("Devices"));
    m_tabWidget->addTab(performanceWidget, tr("Performance"));
    m_tabWidget->addTab(controlWidget, tr("Device Control"));

    setWindowTitle("Device Manager App");
}

MainWindow::~MainWindow()
{
    // Виджеты автоматически удалятся, так как они имеют родительский объект (central widget)
}

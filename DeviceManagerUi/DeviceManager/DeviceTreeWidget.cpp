#include "devicetreewidget.h"
#include "DeviceManagerLib.h" // Публичный API вашей библиотеки
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include <QHeaderView>
#include <iostream>

using namespace dm;

DeviceTreeWidget::DeviceTreeWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels(QStringList() << "Device Name" << "Device ID" << "Manufacturer");
    layout->addWidget(m_treeWidget);

    QPushButton* refreshBtn = new QPushButton("Refresh", this);
    layout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, this, &DeviceTreeWidget::refreshDevices);

    refreshDevices();  // Загрузка устройств при запуске.
}

DeviceTreeWidget::~DeviceTreeWidget() { }

static void addTreeItem(QTreeWidget* tree, const DeviceTreeNode& node, QTreeWidgetItem* parent = nullptr)
{
    QTreeWidgetItem* item = (parent == nullptr)
            ? new QTreeWidgetItem(tree)
            : new QTreeWidgetItem(parent);

    item->setText(0, QString::fromStdString(node.info.name));
    item->setText(1, QString::fromStdString(node.info.deviceId));
    item->setText(2, QString::fromStdString(node.info.manufacturer));

    for (const auto &child : node.children) {
        addTreeItem(tree, child, item);
    }
}

void DeviceTreeWidget::refreshDevices()
{
    m_treeWidget->clear();

    // Используем класс DeviceEnumerator, чтобы получить дерево устройств.
    DeviceEnumerator enumerator;
    DeviceTreeNode tree = enumerator.getDeviceTree();
    addTreeItem(m_treeWidget, tree);

    m_treeWidget->expandAll();
}

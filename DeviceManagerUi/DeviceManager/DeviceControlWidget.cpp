#include "devicecontrolwidget.h"
#include "DeviceManagerLib.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

using namespace dm;

DeviceControlWidget::DeviceControlWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Поле ввода для Device ID.
    m_deviceIdEdit = new QLineEdit(this);
    m_deviceIdEdit->setPlaceholderText("Enter Device ID...");
    mainLayout->addWidget(m_deviceIdEdit);

    // Кнопки управления
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_enableButton = new QPushButton("Enable Device", this);
    m_disableButton = new QPushButton("Disable Device", this);
    btnLayout->addWidget(m_enableButton);
    btnLayout->addWidget(m_disableButton);
    mainLayout->addLayout(btnLayout);

    // Метка для вывода результата операции
    m_resultLabel = new QLabel("Result:", this);
    mainLayout->addWidget(m_resultLabel);

    // Подключаем слоты
    connect(m_enableButton, &QPushButton::clicked, this, &DeviceControlWidget::onEnableClicked);
    connect(m_disableButton, &QPushButton::clicked, this, &DeviceControlWidget::onDisableClicked);
}

DeviceControlWidget::~DeviceControlWidget() { }

void DeviceControlWidget::onEnableClicked()
{
    QString devId = m_deviceIdEdit->text();
    if (devId.isEmpty()) {
        m_resultLabel->setText("Enter Device ID first.");
        return;
    }

    DeviceController controller;
    bool res = controller.enableDevice(devId.toStdString());
    m_resultLabel->setText("Enable returned: " + QString(res ? "true" : "false"));
}

void DeviceControlWidget::onDisableClicked()
{
    QString devId = m_deviceIdEdit->text();
    if (devId.isEmpty()) {
        m_resultLabel->setText("Enter Device ID first.");
        return;
    }

    DeviceController controller;
    bool res = controller.disableDevice(devId.toStdString());
    m_resultLabel->setText("Disable returned: " + QString(res ? "true" : "false"));
}

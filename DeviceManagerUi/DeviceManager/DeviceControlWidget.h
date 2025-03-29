#ifndef DEVICECONTROLWIDGET_H
#define DEVICECONTROLWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class DeviceControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceControlWidget(QWidget *parent = nullptr);
    ~DeviceControlWidget();

private slots:
    void onEnableClicked();
    void onDisableClicked();

private:
    QLineEdit* m_deviceIdEdit;
    QPushButton* m_enableButton;
    QPushButton* m_disableButton;
    QLabel* m_resultLabel;
};

#endif // DEVICECONTROLWIDGET_H

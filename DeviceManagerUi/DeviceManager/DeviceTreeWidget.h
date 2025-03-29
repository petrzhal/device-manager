#ifndef DEVICETREEWIDGET_H
#define DEVICETREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>

class DeviceTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceTreeWidget(QWidget *parent = nullptr);
    ~DeviceTreeWidget();

public slots:
    void refreshDevices();

private:
    QTreeWidget* m_treeWidget;
};

#endif // DEVICETREEWIDGET_H

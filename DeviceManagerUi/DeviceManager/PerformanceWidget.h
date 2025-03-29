#ifndef PERFORMANCEWIDGET_H
#define PERFORMANCEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class PerformanceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PerformanceWidget(QWidget *parent = nullptr);
    ~PerformanceWidget();

private slots:
    void updatePerformance();

private:
    QLabel* m_cpuLabel;
    QLabel* m_memoryLabel;
    QLabel* m_diskLabel;
    QLabel* m_gpuLabel;
    QLabel* m_networkLabel;
    QTimer* m_timer;
};

#endif // PERFORMANCEWIDGET_H

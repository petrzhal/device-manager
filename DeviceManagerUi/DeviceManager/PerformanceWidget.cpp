#include "performancewidget.h"
#include "DeviceManagerLib.h"
#include <QVBoxLayout>
#include <QString>

using namespace dm;

PerformanceWidget::PerformanceWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_cpuLabel = new QLabel("CPU usage: ", this);
    m_memoryLabel = new QLabel("Memory usage: ", this);
    m_diskLabel = new QLabel("Disk usage: ", this);
    m_gpuLabel = new QLabel("GPU usage: ", this);
    m_networkLabel = new QLabel("Network usage: ", this);

    layout->addWidget(m_cpuLabel);
    layout->addWidget(m_memoryLabel);
    layout->addWidget(m_diskLabel);
    layout->addWidget(m_gpuLabel);
    layout->addWidget(m_networkLabel);

    // Обновление производительности каждые 2 секунды
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PerformanceWidget::updatePerformance);
    m_timer->start(2000);
    updatePerformance();
}

PerformanceWidget::~PerformanceWidget() { }

void PerformanceWidget::updatePerformance()
{
    PerformanceMonitor monitor;
    double cpu = monitor.getCPUUsage();
    double mem = monitor.getMemoryUsage();
    double disk = monitor.getDiskUsage();
    double gpu = monitor.getGPUUsage();
    double net = monitor.getNetworkUsage();

    m_cpuLabel->setText(QString("CPU usage: %1%").arg(cpu, 0, 'f', 2));
    m_memoryLabel->setText(QString("Memory usage: %1%").arg(mem, 0, 'f', 2));
    m_diskLabel->setText(QString("Disk usage: %1%").arg(disk, 0, 'f', 2));
    m_gpuLabel->setText(QString("GPU usage: %1%").arg(gpu, 0, 'f', 2));
    m_networkLabel->setText(QString("Network usage: %1 B/sec").arg(net, 0, 'f', 2));
}

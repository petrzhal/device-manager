#include "performancewidget.h"
#include "DeviceManagerLib.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMetaObject>
#include <QStringList>
#include <future>
#include <thread>
#include <tuple>

using namespace dm;

PerformanceWidget::PerformanceWidget(QWidget *parent)
    : QWidget(parent)
{
    // Создаем интерфейс
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

    // Запускаем периодический таймер
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PerformanceWidget::updatePerformance);
    timer->start(2000);

    // Первый запуск обновления
    updatePerformance();
}

PerformanceWidget::~PerformanceWidget() { }

void PerformanceWidget::updatePerformance()
{
    // Асинхронный вызов, возвращающий кортеж:
    // CPU (double), Memory (double), Disk (DisksStorage - std::map<std::string, float>),
    // GPU (double) и Network (int)
    auto future = std::async(std::launch::async, []() -> std::tuple<double, double, PerformanceMonitor::DisksStorage, double, int> {
        PerformanceMonitor monitor;
        double cpuUsage = monitor.getCPUUsage();
        double memUsage = monitor.getMemoryUsage();
        auto disksUsage = monitor.getDiskUsage();
        double gpuUsage = monitor.getGPUUsage();
        int networkUsage = monitor.getNetworkUsage();
        return std::make_tuple(cpuUsage, memUsage, disksUsage, gpuUsage, networkUsage);
    });

    // Создаем отдельный поток для ожидания результата
    std::thread([this, fut = std::move(future)]() mutable {
        // Получаем результат (блокирующий вызов, но не в UI-потоке)
        auto result = fut.get();
        double cpu = std::get<0>(result);
        double mem = std::get<1>(result);
        auto disks = std::get<2>(result);
        double gpu = std::get<3>(result);
        int net = std::get<4>(result);

        // Форматируем информацию по дискам
        QStringList diskInfo;
        for (const auto& disk : disks) {
            diskInfo << QString::fromStdString(disk.first) + ": " +
                        QString::number(disk.second, 'f', 2) + "%";
        }
        QString disksText = diskInfo.join(", ");

        // Сохраняем результаты в локальные переменные, которые затем можно безопасно захватить во вложенной лямбде
        double l_cpu = cpu;
        double l_mem = mem;
        double l_gpu = gpu;
        int l_net = net;
        QString l_disksText = disksText;

        // Обновляем UI в главном потоке
        QMetaObject::invokeMethod(this, [this, l_cpu, l_mem, l_gpu, l_net, l_disksText]() {
            m_cpuLabel->setText(QString("CPU usage: %1%").arg(l_cpu, 0, 'f', 2));
            m_memoryLabel->setText(QString("Memory usage: %1%").arg(l_mem, 0, 'f', 2));
            m_diskLabel->setText(QString("Disk usage: %1").arg(l_disksText));
            m_gpuLabel->setText(QString("GPU usage: %1%").arg(l_gpu, 0, 'f', 2));
            m_networkLabel->setText(QString("Network usage: %1 B/sec").arg(l_net));
        }, Qt::QueuedConnection);
    }).detach();
}

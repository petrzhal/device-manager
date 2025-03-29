#include <iostream>
#include <memory>
#include <string>
#include <cctype>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

// Используем пространство имён boost::asio и tcp для удобства
using boost::asio::ip::tcp;

class MultiThreadedServer {
public:
    // Конструктор принимает ссылку на io_context, порт для прослушивания и размер пула потоков
    MultiThreadedServer(boost::asio::io_context& ioContext, unsigned short port, std::size_t threadPoolSize)
        : ioContext_(ioContext),
        acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)),
        threadPoolSize_(threadPoolSize)
    {
        startAccept();
    }

    // Запуск сервера в пуле потоков
    void run() {
        // Создаем пул потоков
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < threadPoolSize_; ++i) {
            threads.emplace_back([this]() {
                ioContext_.run();
                });
        }
        // Ждем завершения работы всех потоков
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    // Начало асинхронного приёма новых подключений
    void startAccept() {
        // Создаем новую сессию для каждого подключения
        auto newSession = std::make_shared<Session>(ioContext_);
        acceptor_.async_accept(newSession->socket(),
            [this, newSession](const boost::system::error_code& ec) {
                if (!ec) {
                    newSession->start(); // запускаем обработку сессии
                }
                else {
                    std::cerr << "Ошибка при принятии подключения: " << ec.message() << std::endl;
                }
                // Продолжаем принимать следующее подключение
                startAccept();
            });
    }

    // Внутренний класс для обработки сессии (одного подключения)
    class Session : public std::enable_shared_from_this<Session> {
    public:
        explicit Session(boost::asio::io_context& ioContext)
            : socket_(ioContext)
        {
        }

        tcp::socket& socket() {
            return socket_;
        }

        // Запуск обработки сессии: начать считать данные
        void start() {
            doRead();
        }

    private:
        // Асинхронное чтение запроса от клиента
        void doRead() {
            auto self = shared_from_this();
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](const boost::system::error_code& ec, std::size_t length) {
                    if (!ec) {
                        processRequest(length); // обработка данных
                        doWrite(length);        // отправка ответа
                    }
                    else {
                        std::cerr << "Ошибка чтения: " << ec.message() << std::endl;
                    }
                });
        }

        // Простая обработка запроса (например, перевод символов в верхний регистр)
        void processRequest(std::size_t length) {
            // Здесь можно разместить более сложную обработку, возможно, с распределением по другим потокам
            for (std::size_t i = 0; i < length; ++i) {
                data_[i] = std::toupper(data_[i]);
            }
        }

        // Асинхронная отправка ответа клиенту
        void doWrite(std::size_t length) {
            auto self = shared_from_this();
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](const boost::system::error_code& ec, std::size_t /*bytesTransferred*/) {
                    if (!ec) {
                        // Можно сразу продолжить чтение, если требуется постоянное соединение
                        doRead();
                    }
                    else {
                        std::cerr << "Ошибка записи: " << ec.message() << std::endl;
                    }
                });
        }

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    boost::asio::io_context& ioContext_;
    tcp::acceptor acceptor_;
    std::size_t threadPoolSize_;
};

int main() {
    try {
        // Создаем io_context для обработки операций ввода-вывода
        boost::asio::io_context ioContext;
        // Порт, на котором сервер будет принимать подключения
        unsigned short port = 12345;
        // Определяем размер пула потоков (например, по количеству доступных потоков процессора)
        std::size_t threadPoolSize = std::thread::hardware_concurrency();
        if (threadPoolSize == 0) {
            threadPoolSize = 2; // запасной вариант
        }

        MultiThreadedServer server(ioContext, port, threadPoolSize);
        std::cout << "Сервер запущен на порту " << port << ", пул из " << threadPoolSize << " потоков." << std::endl;
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "Исключение: " << e.what() << std::endl;
    }

    return 0;
}

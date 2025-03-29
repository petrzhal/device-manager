#include <iostream>
#include <memory>
#include <string>
#include <cctype>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

// ���������� ������������ ��� boost::asio � tcp ��� ��������
using boost::asio::ip::tcp;

class MultiThreadedServer {
public:
    // ����������� ��������� ������ �� io_context, ���� ��� ������������� � ������ ���� �������
    MultiThreadedServer(boost::asio::io_context& ioContext, unsigned short port, std::size_t threadPoolSize)
        : ioContext_(ioContext),
        acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)),
        threadPoolSize_(threadPoolSize)
    {
        startAccept();
    }

    // ������ ������� � ���� �������
    void run() {
        // ������� ��� �������
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < threadPoolSize_; ++i) {
            threads.emplace_back([this]() {
                ioContext_.run();
                });
        }
        // ���� ���������� ������ ���� �������
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    // ������ ������������ ����� ����� �����������
    void startAccept() {
        // ������� ����� ������ ��� ������� �����������
        auto newSession = std::make_shared<Session>(ioContext_);
        acceptor_.async_accept(newSession->socket(),
            [this, newSession](const boost::system::error_code& ec) {
                if (!ec) {
                    newSession->start(); // ��������� ��������� ������
                }
                else {
                    std::cerr << "������ ��� �������� �����������: " << ec.message() << std::endl;
                }
                // ���������� ��������� ��������� �����������
                startAccept();
            });
    }

    // ���������� ����� ��� ��������� ������ (������ �����������)
    class Session : public std::enable_shared_from_this<Session> {
    public:
        explicit Session(boost::asio::io_context& ioContext)
            : socket_(ioContext)
        {
        }

        tcp::socket& socket() {
            return socket_;
        }

        // ������ ��������� ������: ������ ������� ������
        void start() {
            doRead();
        }

    private:
        // ����������� ������ ������� �� �������
        void doRead() {
            auto self = shared_from_this();
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](const boost::system::error_code& ec, std::size_t length) {
                    if (!ec) {
                        processRequest(length); // ��������� ������
                        doWrite(length);        // �������� ������
                    }
                    else {
                        std::cerr << "������ ������: " << ec.message() << std::endl;
                    }
                });
        }

        // ������� ��������� ������� (��������, ������� �������� � ������� �������)
        void processRequest(std::size_t length) {
            // ����� ����� ���������� ����� ������� ���������, ��������, � �������������� �� ������ �������
            for (std::size_t i = 0; i < length; ++i) {
                data_[i] = std::toupper(data_[i]);
            }
        }

        // ����������� �������� ������ �������
        void doWrite(std::size_t length) {
            auto self = shared_from_this();
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](const boost::system::error_code& ec, std::size_t /*bytesTransferred*/) {
                    if (!ec) {
                        // ����� ����� ���������� ������, ���� ��������� ���������� ����������
                        doRead();
                    }
                    else {
                        std::cerr << "������ ������: " << ec.message() << std::endl;
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
        // ������� io_context ��� ��������� �������� �����-������
        boost::asio::io_context ioContext;
        // ����, �� ������� ������ ����� ��������� �����������
        unsigned short port = 12345;
        // ���������� ������ ���� ������� (��������, �� ���������� ��������� ������� ����������)
        std::size_t threadPoolSize = std::thread::hardware_concurrency();
        if (threadPoolSize == 0) {
            threadPoolSize = 2; // �������� �������
        }

        MultiThreadedServer server(ioContext, port, threadPoolSize);
        std::cout << "������ ������� �� ����� " << port << ", ��� �� " << threadPoolSize << " �������." << std::endl;
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "����������: " << e.what() << std::endl;
    }

    return 0;
}

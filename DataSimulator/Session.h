#include <cstdlib>
#include <iostream>
#include <memory>
#include "asio.hpp"
#include <VAsyncIO.h>

using namespace asio;
using namespace asio::ip;


/*在什么情况下要使类A继承enable_share_from_this?
使用场合:当类A被share_ptr管理, 且在类A的 成员函数 里需要把 当前 类对象 作为参数传给其他函数时, 就需要传递一个指向自身的share_ptr
我们就使类A继承enable_share_from_this, 然后通过其成员函数share_from_this()返回当指向自身的share_ptr*/


class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket) : socket_(std::move(socket))
	{
		std::cout << "Session" << std::endl;
	}

	void start()
	{
		do_read();
	}
	~Session()
	{
		std::cout << "~Session" << std::endl;
	}

	//asio::buffer类, 用来缓存需要收发的数据, buffer相关的类是asio中功能非常独立的部分
private:

	void do_read()
	{
		auto self(shared_from_this());
		//成员函数, 接收一次数据, 收到多少是多少
		//为什么要捕获this?为了直接调用成员函数do_write和变量data_
		//为什么要捕获self?为了延长对象生命周期
		//断开连接,boost::asio内部会将shared_ptr的引用计数降到0

		//异步监听,当有信息发过来,就调用lambda
		socket_.async_read_some(asio::buffer(data_, max_length),
			[this, self](std::error_code ec, std::size_t length)
		{
			if (!ec){
				std::cout << "self.use_count():" << self.use_count() << std::endl;
				std::cout << data_ << std::endl;
				do_write(length);
			}
		});
		std::cout << "async_read_some结束" << std::endl;
	}

	void do_write(std::size_t length)
	{
		//shared_from_this() 返回一个当前类的std::share_ptr
		auto self(shared_from_this());
		//发送指定字节的数据
		asio::async_write(socket_, asio::buffer(data_, length),
			[this, self](std::error_code ec, std::size_t /*length*/){
			if (!ec){
				do_read();
			}
		});
		std::cout << "async_write结束 do_write结束" << std::endl;
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
	//asio::ip::tcp::endpoint: tcp地址+端口，用作参数
public:
	server(asio::io_service& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
	{
		std::string _ip;
		int _port;
		std::string _error_str="";
	//	session_ = new Session(*socket_);
		do_accept();
	}

private:

	
	void do_accept()
	{
		//接受一个连接, 注意只有一个, 如果要接受多个, 在回调函数中再次调用此函数
		//当有连接请求才会调用lambda表达式(回调函数)
		std::string _ip; 
		int _port;
		std::string _error_str="";
		io_service_ = new asio::io_service;
		ep_ = new asio::ip::tcp::endpoint(asio::ip::address::from_string(_ip), _port);
		socket_ = new asio::ip::tcp::socket(*io_service_);
		asio::error_code ec;
		session_ = new Session(*socket_);

		asio::io_service ios;
		asio::ip::tcp::acceptor acceptor(*io_service_, *ep_);
		acceptor.accept(*socket_);
		//acceptor.async_accept(*socket_, ep_,nullptr);
		/*acceptor_.async_accept(*socket_, ep_, [this](std::error_code ec, tcp::socket socket)
		{
		if (!ec)
		{
		std::make_shared<Session>(std::move(socket))->start();
		}
		运行程序可以知道已经超出作用域, 但Session的析构函数并没有被调用,为什么,就是因为上面的延长生命周期,神奇不
		std::cout << "std::make_shared<Session>(std::move(socket))作用域超出" << std::endl;
		do_accept();
		});*/
	}

	//tcp服务器用来接受客户端连接的类
	tcp::socket *socket_;
	tcp::acceptor acceptor_;
	asio::io_service* io_service_;
	asio::ip::basic_endpoint<asio::ip::tcp>* ep_;
	//asio::basic_stream_socket<asio::ip::tcp>* socket_;
	std::string data_buffer_;
	Session *session_;
	
};
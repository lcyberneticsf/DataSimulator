#include <cstdlib>
#include <iostream>
#include <memory>
#include "asio.hpp"
#include <VAsyncIO.h>

using namespace asio;
using namespace asio::ip;


/*��ʲô�����Ҫʹ��A�̳�enable_share_from_this?
ʹ�ó���:����A��share_ptr����, ������A�� ��Ա���� ����Ҫ�� ��ǰ ����� ��Ϊ����������������ʱ, ����Ҫ����һ��ָ�������share_ptr
���Ǿ�ʹ��A�̳�enable_share_from_this, Ȼ��ͨ�����Ա����share_from_this()���ص�ָ�������share_ptr*/


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

	//asio::buffer��, ����������Ҫ�շ�������, buffer��ص�����asio�й��ܷǳ������Ĳ���
private:

	void do_read()
	{
		auto self(shared_from_this());
		//��Ա����, ����һ������, �յ������Ƕ���
		//ΪʲôҪ����this?Ϊ��ֱ�ӵ��ó�Ա����do_write�ͱ���data_
		//ΪʲôҪ����self?Ϊ���ӳ�������������
		//�Ͽ�����,boost::asio�ڲ��Ὣshared_ptr�����ü�������0

		//�첽����,������Ϣ������,�͵���lambda
		socket_.async_read_some(asio::buffer(data_, max_length),
			[this, self](std::error_code ec, std::size_t length)
		{
			if (!ec){
				std::cout << "self.use_count():" << self.use_count() << std::endl;
				std::cout << data_ << std::endl;
				do_write(length);
			}
		});
		std::cout << "async_read_some����" << std::endl;
	}

	void do_write(std::size_t length)
	{
		//shared_from_this() ����һ����ǰ���std::share_ptr
		auto self(shared_from_this());
		//����ָ���ֽڵ�����
		asio::async_write(socket_, asio::buffer(data_, length),
			[this, self](std::error_code ec, std::size_t /*length*/){
			if (!ec){
				do_read();
			}
		});
		std::cout << "async_write���� do_write����" << std::endl;
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
	//asio::ip::tcp::endpoint: tcp��ַ+�˿ڣ���������
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
		//����һ������, ע��ֻ��һ��, ���Ҫ���ܶ��, �ڻص��������ٴε��ô˺���
		//������������Ż����lambda���ʽ(�ص�����)
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
		���г������֪���Ѿ�����������, ��Session������������û�б�����,Ϊʲô,������Ϊ������ӳ���������,���治
		std::cout << "std::make_shared<Session>(std::move(socket))�����򳬳�" << std::endl;
		do_accept();
		});*/
	}

	//tcp�������������ܿͻ������ӵ���
	tcp::socket *socket_;
	tcp::acceptor acceptor_;
	asio::io_service* io_service_;
	asio::ip::basic_endpoint<asio::ip::tcp>* ep_;
	//asio::basic_stream_socket<asio::ip::tcp>* socket_;
	std::string data_buffer_;
	Session *session_;
	
};
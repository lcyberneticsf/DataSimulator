#include "stdafx.h"
#include "communicator.h"
#include "DataSimulator.h"
#include "DataSimulatorDlg.h"
#include "VCameraEnum.h"
//#include "VCameraMessage.h"
//#include "VCameraFrame.h"
#include<io.h>
#include <VAsyncIO.h>

#include <thread>
#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace Vsee;
bool aq::Communicator::image_show = false;
bool aq::Communicator::batch_image_load = true;
bool aq::Communicator::single_image_load = false;
bool aq::Communicator::last_image_load = false;
bool aq::Communicator::m_bServerSendMode=true;
bool aq::Communicator::m_bClientSendMode=false;
bool aq::Communicator::m_show_ok = true;
Vsee::VCameraMessageTrans  aq::Communicator::box_ctrl_msg;       //use to transmit control parameters from server to box;
extern HWND m_hMainWnd;

struct information
{
	std::uint32_t height;
	std::uint32_t width;
	std::uint32_t channels;
	std::uint32_t data_legnth;
	char* m_picture_data;
}Info1;

std::vector<std::string> aq::Communicator::split(std::string str, char seg)
{
    std::vector<std::string> split_strs;
    std::string temp;
    std::istringstream iss(str);
    while (getline(iss, temp, seg))
    {
        split_strs.push_back(temp);
    }
    return split_strs;
}

void aq::Communicator::print_hex(unsigned char *_buf, int _len)
{
    for (int i = 0 ; i < _len ; i++)
    {
        printf("%02x ", _buf[i]);
    }
    std::cout << std::endl;
}

aq::Communicator::Communicator()
{
	ios[0] = new asio::io_service(1);
	ios[1] = new asio::io_service(1);
	ios[2] = new asio::io_service(1);
	ios[3] = new asio::io_service(1);
	ios[4] = new asio::io_service(1);
	ios[5] = new asio::io_service(1);
	ios[6] = new asio::io_service(1);
	ios[7] = new asio::io_service(1);

	sockets[0] = new tcp::socket(*ios[0]);//将socket->io_service变量设置为对应的ios[i]，在VCameraSession<Derived>::sendMessage(VCameraMessage && msg)中要用到io_service来发送信息
	sockets[1] = new tcp::socket(*ios[1]);
	sockets[2] = new tcp::socket(*ios[2]);
	sockets[3] = new tcp::socket(*ios[3]);
	sockets[4] = new tcp::socket(*ios[4]);
	sockets[5] = new tcp::socket(*ios[5]);
	sockets[6] = new tcp::socket(*ios[6]);
	sockets[7] = new tcp::socket(*ios[7]);

	sessions[0] = new Vsee::VTx2Session(*sockets[0]);
	sessions[1] = new Vsee::VTx2Session(*sockets[1]);
	using MutexLock = std::lock_guard<std::mutex>;

	eps[0] = tcp::endpoint(address::from_string("127.0.0.1"), 60000);
	eps[1] = tcp::endpoint(address::from_string("127.0.0.1"), 60000);
	eps[2] = tcp::endpoint(address::from_string("192.168.1.13"), PortNumber::PRC);
	eps[3] = tcp::endpoint(address::from_string("192.168.1.14"), PortNumber::PRC);
	eps[4] = tcp::endpoint(address::from_string("192.168.1.15"), PortNumber::PRC);
	eps[5] = tcp::endpoint(address::from_string("192.168.1.16"), PortNumber::PRC);
	eps[6] = tcp::endpoint(address::from_string("192.168.1.17"), PortNumber::PRC);
	eps[7] = tcp::endpoint(address::from_string("192.168.1.18"), PortNumber::PRC);
	//frame_queue_ = new std::queue<Vsee::VCameraFrame>;
}

bool aq::Communicator::tcp_connect(std::string _ip, int _port, std::string &_error_str)
{
    _error_str.clear();
    // check input
    std::vector<std::string> ip = split(_ip, '.');
    if (ip.size() != 4)
    {
        _error_str = "Invalid Ip. There is a size error.";
        return false;
    }
    for (auto iter : ip)
    {
        int tmp = -1;
        try
        {
            tmp = std::stoi(iter);
        }
        catch (std::invalid_argument e)
        {
            std::cerr << e.what() << std::endl;
            _error_str = std::string("Invalid Ip. There is an invalid argument: ") + iter + ".";
            return false;
        }
        if ((tmp < 0) || (tmp > 255))
        {
            _error_str = std::string("Invalid Ip. There is an argument: ") + iter + " out of range.";
            return false;
        }
    }
    if ((_port < 0) || (_port > 65535))
    {
        _error_str = std::string("Invalid Port. ") + std::to_string(_port) + " is out of range.";
        return false;
    }

	io_service_ = new asio::io_service;
	ep_ = new asio::ip::tcp::endpoint(asio::ip::address::from_string(_ip), _port);
	socket_ = new asio::ip::tcp::socket(*io_service_);
	asio::error_code ec;
	
	socket_->connect(*ep_, ec);
	if (ec)
	{
		_error_str = "The connection could have timed out.";
		return false;
	}
	data_buffer_.clear();
	data_buffer_.resize(1000000);
    _error_str = "Ok.";
    return true;
}

bool aq::Communicator::receive_img(std::string &_error_str)
{
	
	_error_str.clear();
	std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
	VCameraMessageTrans* msg_in = (VCameraMessageTrans*)data_buffer_.data();
	int width = msg_in->width;
	int height = msg_in->height;
	int channels = msg_in->channels;
	int data_size = width*height*channels;

	std::int64_t next = Vsee::VCameraMessageTrans::nextLoad(data_buffer_.data(), data_buffer_.size());
	next = data_size;

	if (next > 0)
	{
		asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), next));
		//std::cout << "next : " << next << std::endl;
		//print_hex((unsigned char *)data_buffer_.c_str() + size, next);
	}
	else
		return true;


	cv::Mat img(height, width, CV_8UC3, (void*)((char*)data_buffer_.data()));
	//cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
	std::string file_path = "f:/save_image/" + std::to_string(counter) + ".bmp";
	cv::imwrite(file_path, img);
	counter++;

	Vsee::VCameraMessageTrans msg = Vsee::VCameraMessageTrans::load(data_buffer_.data(), data_buffer_.size());
	Vsee::VCameraMessageTrans frame(std::move(msg));
	if (msg.empty())
	{
		_error_str = "Empty camera message.";
		return false;
	}
	if (msg.checkCrc())
	{
		switch (msg.token())
		{
		case 51:
		{
				   Vsee::VCameraFrame frame;// (std::move(msg));
				   if (frame.format() == Vsee::FrameFormat::RGB)
				   {
					   m_.lock();
					   frame_queue_->push(frame);
					   m_.unlock();
				   }
				   break;
		}
		default:
			break;
		}
	}
	else
	{
		_error_str = "CRC error.";
		return false;
	}
	
    return true;
}

bool aq::Communicator::receive_img(std::string &_error_str, asio::basic_stream_socket<asio::ip::tcp>* socket_)
{

	_error_str.clear();
	std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
	VCameraMessageTrans* msg_in = (VCameraMessageTrans*)data_buffer_.data();
	//VCameraMessage* msg_in = new VCameraMessage;
	//memcpy(msg_in, data_buffer_.data(), sizeof(VCameraMessage));
	int width = msg_in->width;
	int height = msg_in->height;
	int channels = msg_in->channels;
	int data_size = width*height*channels;

	std::int64_t next = Vsee::VCameraMessageTrans::nextLoad(data_buffer_.data(), data_buffer_.size());
	next = data_size;

	if (next > 0)
	{
		asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), next));
		//std::cout << "next : " << next << std::endl;
		//print_hex((unsigned char *)data_buffer_.c_str() + size, next);
	}
	else
		return true;


	cv::Mat img(height, width, CV_8UC3, (void*)((char*)data_buffer_.data()));
	//cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
	std::string file_path = "f:/save_image/" + std::to_string(counter) + ".bmp";
	cv::imwrite(file_path, img);
	counter++;

	Vsee::VCameraMessageTrans msg = Vsee::VCameraMessageTrans::load(data_buffer_.data(), data_buffer_.size());
	Vsee::VCameraFrame frame;// (std::move(msg));
	if (msg.empty())
	{
		_error_str = "Empty camera message.";
		return false;
	}
	if (msg.checkCrc())
	{
		switch (msg.token())
		{
		case 51:
		{
				   Vsee::VCameraFrame frame;// (std::move(msg));
				   if (frame.format() == Vsee::FrameFormat::RGB)
				   {
					   m_.lock();
					   frame_queue_->push(frame);
					   m_.unlock();
				   }
				   break;
		}
		default:
			break;
		}
	}
	else
	{
		_error_str = "CRC error.";
		return false;
	}

	return true;
}

bool aq::Communicator::send_cmd(aq::CMD _cmd, std::string &_error_str, std::vector<int> _state, int _sequence)
{
	
    _error_str.clear();
    if (_cmd == CMD::SEND_RESULTS)
    {
        std::uint32_t result[2];
        result[0] = _sequence;
		std::uint32_t state = 0;
        for (int i = 0 ; i < 8 ; i++)
        {
            if (i < _state.size())
            {
				state += _state[i] * std::pow(16, 7 - i);
            }
            else
            {
            }
        }
		result[1] = state;

		Vsee::VCameraMessageTrans response((int)_cmd, (const char*)result, sizeof(result));
		std::cout << "send_cmd================================>> : " << _sequence << std::endl;
		print_hex((unsigned char*)response.data(), response.dataSize());
		std::cout << "<<================================send_cmd" << std::endl;
		//const std::string &tmp = response.;
		auto buf = asio::buffer(response.bytes(), response.byteSize());
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		socket_->write_some(buf);
    }
    else
    {
		std::uint32_t result[1];
		result[0] = _sequence;
		//for (int i = 0; i < 8; i++)
		//{
		//	if (i < _state.size())
		//	{
		//		result[i + 1] = _state[i];
		//	}
		//	else
		//	{
		//		result[i + 1] = 0;
		//	}
		//}

		Vsee::VCameraMessageTrans response((int)_cmd, nullptr, 0);
		socket_->write_some(asio::buffer(response.data(), response.dataSize()));
    }
	
    return true;
}

//author :	LF
//time:		2020-9-16
//parameters:
//			std::string _ip : the local server ip adress
//			int _port : the local server  net port for machine server 
//function: To start a Server state machine,to wait for all the net request from client,and when require income,
//			then start a server thread for the net connect;
bool aq::Communicator::server(std::string _ip, int _port, std::string &_error_str)
{
	
	_error_str.clear();
	// check input
	std::vector<std::string> ip = split(_ip, '.');
	if (ip.size() != 4)
	{
		_error_str = "Invalid Ip. There is a size error.";
		return false;
	}
	for (auto iter : ip)
	{
		int tmp = -1;
		try
		{
			tmp = std::stoi(iter);
		}
		catch (std::invalid_argument e)
		{
			std::cerr << e.what() << std::endl;
			_error_str = std::string("Invalid Ip. There is an invalid argument: ") + iter + ".";
			return false;
		}
		if ((tmp < 0) || (tmp > 255))
		{
			_error_str = std::string("Invalid Ip. There is an argument: ") + iter + " out of range.";
			return false;
		}
	}
	if ((_port < 0) || (_port > 65535))
	{
		_error_str = std::string("Invalid Port. ") + std::to_string(_port) + " is out of range.";
		return false;
	}


	asio::error_code ec;
	//asio::io_service ios;
	
	while (true)
	{
		
		io_service_ = new asio::io_service;
		ep_ = new asio::ip::tcp::endpoint(asio::ip::address::from_string(_ip), _port);
		socket_ = new asio::ip::tcp::socket(*io_service_);
		asio::ip::tcp::acceptor acceptor(*io_service_, *ep_);
		
		acceptor.accept(*socket_);            //When a net connect is requseted,then start a server thrread for the connection;
		//acceptor.accept(*sockets[0]);
		{
			char* MsgPost = new char[64];
			strcpy(MsgPost, "AIDI Box Client Connetc in!");
			::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
			//std::thread t(&aq::Communicator::server_run, this, sockets[0], _ip, _port, _error_str);	//Start receive&send serer function
			std::thread t(&aq::Communicator::server_run, this, socket_, _ip, _port, _error_str);	//Start receive&send serer function
			t.detach();
		}
		//服务器端程序里要注意的是自由函数buffer(),它可用包装很多种类的容器成为asio组件可用的缓冲区类型。通常我们不能直接把数组，vector等容器用作asio的读写参数，必须使用buffer()函数包装。
	}

	if (ec)
	{
		_error_str = "The connection could have timed out.";
		return false;
	}
	
	return true;
}
bool aq::Communicator::server_run(asio::basic_stream_socket<asio::ip::tcp>* socket_, std::string _ip, int _port, std::string &_error_str)
{
	size_t size = 10000000;
	data_buffer_.clear();
	data_buffer_.resize(1000000);
	
	if (m_bServerSendMode)  //if the mode is server sending,and client receive image 
	{
		Vsee::VCameraFrame VFrame;
		cv::Mat image;
		image = cv::imread("f:/" + std::to_string(1) + ".bmp");
		int nWidth = image.cols;
		int nHeight = image.rows;
		int nChannnels = image.channels();
		//size_t size = 10000000;
		char* buffer = new char[size];
		//	VCameraMessagePrivate* pre = new VCameraMessagePrivate();
		VCameraMessageTrans msg(buffer, size);
		int n_size = sizeof(VCameraMessageTrans);
		int n_size2 = sizeof(msg);
		msg.width = image.cols;
		msg.height = image.rows;
		msg.channels = image.channels();
		int length = nWidth*nHeight*nChannnels;
		//data_buffer_.clear();
		//data_buffer_.resize(1000000);
		//get_file_list(file_load_path);
		int file_size = file_list.size();
		std::string file_path;


		msg.setData((char*)image.data, nWidth*nHeight*nChannnels, true);
		//sessions[0]->sendMessage(msg);

		//Vsee::VCameraFrame frame;// (std::move(msg));
		//nWidth = frame.width();
		//nHeight = frame.height();
		int file_total = file_list.size();
		int file_countr = 0;
		while (file_countr < file_total - 1)
		{
			if (aq::Communicator::batch_image_load)
			{
				file_path = file_list[file_countr];
				file_countr++;
				image = cv::imread(file_path);
				nWidth = image.cols;
				nHeight = image.rows;
				nChannnels = image.channels();
				msg.width = image.cols;
				msg.height = image.rows;
				msg.channels = image.channels();
				length = nWidth*nHeight*nChannnels;

				//data_buffer_.clear();
				//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
				size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
				VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
				nWidth = msg2->width;
				nHeight = msg2->height;
				int nSignalling = msg2->signalling;

				if (size != 0)
				{
					//sessions[0]->sendMessage(msg);
					{
						auto& io = socket_->get_io_service();
						if (!socket_->is_open())
							return false;
						asio::error_code er;
						auto buf = asio::buffer(&msg, sizeof(msg));
						int nLenght = msg.byteSize();
						socket_->write_some(buf);

						//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
						buf = asio::buffer(image.data, length);

						//asio::async_write(_socket, buf, [keep = std::move(msg)](asio::error_code, std::size_t){});
						//asio::write(*_socket, buf, NULL, &er);
						socket_->write_some(buf);
					}
					std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					
				}
				else
				{
					Sleep(1000);
				}
				int sleep_times = 100 / camera_scan_speed;
				Sleep(sleep_times);
			}
			else if (aq::Communicator::single_image_load)
			{
				aq::Communicator::single_image_load = false;
				file_path = file_list[file_countr];
				file_countr++;
				image = cv::imread(file_path);
				nWidth = image.cols;
				nHeight = image.rows;
				nChannnels = image.channels();
				msg.width = image.cols;
				msg.height = image.rows;
				msg.channels = image.channels();
				length = nWidth*nHeight*nChannnels;

				//data_buffer_.clear();
				//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
				size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
				VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
				nWidth = msg2->width;
				nHeight = msg2->height;
				int nSignalling = msg2->signalling;

				if (size != 0)
				{
					//sessions[0]->sendMessage(msg);
					{
						auto& io = socket_->get_io_service();
						if (!socket_->is_open())
							return false;
						asio::error_code er;
						auto buf = asio::buffer(&msg, sizeof(msg));
						int nLenght = msg.byteSize();
						socket_->write_some(buf);

						//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
						buf = asio::buffer(image.data, length);

						//asio::async_write(_socket, buf, [keep = std::move(msg)](asio::error_code, std::size_t){});
						//asio::write(*_socket, buf, NULL, &er);
						socket_->write_some(buf);
					}
					std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
				}
				else
				{
					Sleep(1000);
				}
			}
			else
			{
				Sleep(300);
			}

		}
	}
	else  //the mode is server receive,and clietn send image;
	{
		VCameraMessageTrans msg;
		std::string error_str;
		msg.width = 115;
		msg.height = 115;
		msg.signalling = 2;
		counter = 0;
//		send_cmd(aq::CMD::START_RECV_IMG, error_str);
		while (1)
		{
			auto buf = asio::buffer(&msg, sizeof(msg));
			socket_->write_some(buf);
			std::string s;
			if (!receive_img(s, socket_))
			{
				std::cerr << s << std::endl;
			}
		}

	}
	
	
	return true;
}
bool aq::Communicator::get_file_list(std::string file_load_path)
{
	CString m_strFolderPath;
	CString m_strFileExt;
	char strFilePath[256];
	memset(strFilePath, 0, 256);
	strcpy(strFilePath, file_load_path.data());
	int nNameLength = strlen(strFilePath);
	char strFileExt[8];
	memset(strFileExt, 0, 8);
	int i = 0;
	int nDotIndex = 0;
	file_amount = 0;
	for (i = nNameLength - 1; i > 0; i--)
	{
		char ch = strFilePath[i];
		if (ch == '.')
			break;
	}
	strncpy(strFileExt, strFilePath + i + 1, nNameLength - i - 1);
	m_strFileExt.Format("%s", strFileExt);
	for (i = nNameLength - 1; i > 0; i--)
	{
		char ch = strFilePath[i];
		if (ch == '\\')
			break;
	}
	strFilePath[i] = '\0';
	m_strFolderPath.Format("%s", strFilePath);
	std::string strFolderPath = m_strFolderPath;
	get_files(strFolderPath, strFileExt, file_amount);
	return true;
}

int aq::Communicator::get_files(std::string fileFolderPath, std::string fileExtension, int& nFileNum)
{
	std::string fileFolder = fileFolderPath + "\\*" + fileExtension;
	std::string fileName;
	struct _finddata_t fileInfo;
	nFileNum = 0;
	long long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
	if (findResult == -1)
	{
		_findclose(findResult);
		return 0;
	}
	bool flag = 0;

	do
	{
		fileName = fileFolderPath + "\\" + fileInfo.name;
		if (fileInfo.attrib == _A_ARCH)
		{
			file_list.push_back(fileName);
			nFileNum++;
		}
	} while (_findnext(findResult, &fileInfo) == 0);

	_findclose(findResult);
}


/*
Autoor: LF
Date :   2020-9-1
Function:
1)Start a thread to connnetc the remote server;
2)Send images to remote server;
3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
bool  aq::Communicator::run_tcp_connect(std::string _ip, int _port)
{
	std::thread t(&aq::Communicator::tcp_connect_thread, this ,_ip, _port);
	//std::thread t(&aq::Communicator::server_run, this, socket_, _ip, _port, _error_str);
	t.detach();
	return true;
}

/*
Autoor: LF
Date :   2020-10-29
Function:
1)Start a thread to connnetc the remote server;
2)Send images to remote server;
3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
bool  aq::Communicator::run_tcp_connect(std::string _ip, int _port, int camera_number)
{
	std::thread t(&aq::Communicator::tcp_connect_thread2, this, _ip, _port, camera_number);
	//std::thread t(&aq::Communicator::server_run, this, socket_, _ip, _port, _error_str);
	t.detach();
	return true;
}


/*
Autoor: LF
Date :   2020-9-1
Function:
1)This is the  thread function to connnetc the remote server;
2)Send images to remote server;
3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
bool aq::Communicator::tcp_connect_thread(std::string _ip, int _port)
{
	
	if (!m_bClientSendMode)  // if mode is client receive,and server send image mode
	{
		while (true)
		{
			try
			{

				std::string error_str;
				if (!tcp_connect(_ip, _port, error_str))
				{
					std::cerr << error_str << std::endl;
					//return false;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
				std::cout << "connected " << _ip << ":" << _port << std::endl;
				VCameraMessageTrans msg;
				msg.width = 115;
				msg.height = 115;
				msg.signalling = 2;
				counter = 0;
				send_cmd(aq::CMD::START_RECV_IMG, error_str);
				while (1)
				{
					auto buf = asio::buffer(&msg, sizeof(msg));
					socket_->write_some(buf);
					std::string s;
					if (!receive_img(s))
					{
						std::cerr << s << std::endl;
					}
				}
			}
			catch (...)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				continue;
			}
		}
	}
	
	else if (m_bClientSendMode)  //if the mode is client sending,and server receive image ,and this is the DateSimulator work mode;
	{	
		try
		{
			std::string error_str;
			bool bConnectServer = false;
			while (!bConnectServer)
			{
				if (!tcp_connect(_ip, _port, error_str))
				{
					std::cerr << error_str << std::endl;
					//return false;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
				char* MsgPost = new char[64];
				strcpy(MsgPost, "Connect AIDI Box Server!");
				::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
				bConnectServer = true;
			}
			Vsee::VCameraFrame VFrame;
			cv::Mat image;
			image = cv::imread("f:/" + std::to_string(1) + ".bmp");
			int nWidth = image.cols;
			int nHeight = image.rows;
			int nChannnels = image.channels();
			size_t size = DATA_BUFFER_LENGTH;
			char* buffer = new char[size];
			//	VCameraMessagePrivate* pre = new VCameraMessagePrivate();
			VCameraMessageTrans msg(buffer, size);
			int n_size = sizeof(VCameraMessageTrans);	//sizeof(VCameraMessageTrans)=512
			int n_size2 = sizeof(msg);
			msg.width = image.cols;
			msg.height = image.rows;
			msg.channels = image.channels();
			msg.signalling = 20;
			int length = nWidth*nHeight*nChannnels;
			data_buffer_.clear();
			data_buffer_.resize(DATA_BUFFER_LENGTH);
			//get_file_list(file_load_path);
			int file_size = file_list.size();
			std::string file_path;

			msg.setData((char*)image.data, nWidth*nHeight*nChannnels, true);
			//sessions[0]->sendMessage(msg);

			Vsee::VCameraMessageTrans frame(std::move(msg));
			/*nWidth = frame.width();
			nHeight = frame.height();*/
			nWidth = frame.width;
			nHeight = frame.height;

			int file_total = file_list.size();
			int file_countr = 0;

			while (file_countr < file_total - 1)
			{
				if (aq::Communicator::batch_image_load)
				{
					
					file_path = file_list[file_countr];
					file_countr++;
					image = cv::imread(file_path);
					nWidth = image.cols;
					nHeight = image.rows;
					nChannnels = image.channels();
					msg.width = image.cols;
					msg.height = image.rows;
					msg.channels = image.channels();
					length = nWidth*nHeight*nChannnels;
					msg.magnetic_valve_num = 4;    //To indicate the camera series number is the 4th;
					int nsize = sizeof(VCameraMessageTrans);

					char* MsgPost = new char[64];
					strcpy(MsgPost, file_path.c_str());
					::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);

					//data_buffer_.clear();
					//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
					size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
					VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
					nWidth = msg2->width;
					nHeight = msg2->height;
					int nSignalling = msg2->signalling;

					if (nSignalling == 20)
					{
						auto& io = socket_->get_io_service();
						if (!socket_->is_open())
							return false;
						asio::error_code er;
						auto buf = asio::buffer(&msg, sizeof(msg));
						int nLenght = msg.byteSize();
						socket_->write_some(buf);

						//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
						buf = asio::buffer(image.data, length);

						socket_->write_some(buf);
						std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					}
					else
					{
						Sleep(1000);
					}
					int sleep_times = 100 / camera_scan_speed;
					//Sleep(sleep_times);
					std::this_thread::sleep_for(std::chrono::milliseconds(sleep_times));
				}
				else if (aq::Communicator::single_image_load)
				{
					aq::Communicator::single_image_load = false;
					file_path = file_list[file_countr];
					file_countr++;
					image = cv::imread(file_path);
					nWidth = image.cols;
					nHeight = image.rows;
					nChannnels = image.channels();
					msg.width = image.cols;
					msg.height = image.rows;
					msg.channels = image.channels();
					length = nWidth*nHeight*nChannnels;

					//data_buffer_.clear();
					//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
					size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
					VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
					nWidth = msg2->width;
					nHeight = msg2->height;
					int nSignalling = msg2->signalling;

					if (size != 0)
					{
						//sessions[0]->sendMessage(msg);
						{
							auto& io = socket_->get_io_service();
							if (!socket_->is_open())
								return false;
							asio::error_code er;
							auto buf = asio::buffer(&msg, sizeof(msg));
							int nLenght = msg.byteSize();
							socket_->write_some(buf);

							//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
							buf = asio::buffer(image.data, length);

							//asio::async_write(_socket, buf, [keep = std::move(msg)](asio::error_code, std::size_t){});
							//asio::write(*_socket, buf, NULL, &er);
							socket_->write_some(buf);
						}
						std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));;
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(300));;
				}
			}
		}
		catch (...)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}				
	}
	else
	{
		;
	}
	
	return true;
}


/*
Autoor: LF
Date :   2020-10-29
Function:
1)This is the  thread function to connnetc the remote server;
2)Send images to remote server;
3)This is the mode to connect TX2/NX box,and this mode is the DataSimulator work mode;
*/
bool aq::Communicator::tcp_connect_thread2(std::string _ip, int _port, int camera_number)
{
	send_ok = false;
	if (!m_bClientSendMode)  // if mode is client receive,and server send image mode
	{
		while (true)
		{
			try
			{

				std::string error_str;
				if (!tcp_connect(_ip, _port, error_str))
				{
					std::cerr << error_str << std::endl;
					//return false;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
				std::cout << "connected " << _ip << ":" << _port << std::endl;
				VCameraMessageTrans msg;
				msg.width = 115;
				msg.height = 115;
				msg.signalling = 2;
				counter = 0;
				send_cmd(aq::CMD::START_RECV_IMG, error_str);
				while (1)
				{
					auto buf = asio::buffer(&msg, sizeof(msg));
					socket_->write_some(buf);
					std::string s;
					if (!receive_img(s))
					{
						std::cerr << s << std::endl;
					}
				}
			}
			catch (...)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				continue;
			}
		}
	}

	else if (m_bClientSendMode)  //if the mode is client sending,and server receive image ,and this is the DateSimulator work mode;
	{
		try
		{
			std::string error_str;
			bool bConnectServer = false;
			while (!bConnectServer)
			{
				if (!tcp_connect(_ip, _port, error_str))
				{
					std::cerr << error_str << std::endl;
					//return false;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
				char* MsgPost = new char[64];
				strcpy(MsgPost, "Connect AIDI Box Server!");
				::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
				bConnectServer = true;
			}
			Vsee::VCameraFrame VFrame;
			cv::Mat image;
			image = cv::imread("f:/" + std::to_string(1) + ".bmp");
			int nWidth = image.cols;
			int nHeight = image.rows;
			int nChannnels = image.channels();
			size_t size = DATA_BUFFER_LENGTH;
			char* buffer = new char[size];
			//	VCameraMessagePrivate* pre = new VCameraMessagePrivate();
			VCameraMessageTrans msg(buffer, size);
			int n_size = sizeof(VCameraMessageTrans);	//sizeof(VCameraMessageTrans)=512
			int n_size2 = sizeof(msg);
			msg.width = image.cols;
			msg.height = image.rows;
			msg.channels = image.channels();
			msg.signalling = 20;
			int length = nWidth*nHeight*nChannnels;
			data_buffer_.clear();
			data_buffer_.resize(DATA_BUFFER_LENGTH);
			//get_file_list(file_load_path);
			int file_size = file_list.size();
			std::string file_path;

			msg.setData((char*)image.data, nWidth*nHeight*nChannnels, true);
			//sessions[0]->sendMessage(msg);

			Vsee::VCameraMessageTrans frame(std::move(msg));
			/*nWidth = frame.width();
			nHeight = frame.height();*/
			nWidth = frame.width;
			nHeight = frame.height;

			int file_total = file_list.size();
			int file_countr = -1;

			while (file_countr < file_total-1)
			{
				if (aq::Communicator::batch_image_load)
				{
					file_countr++;
					file_path = file_list[file_countr];
					image = cv::imread(file_path);
					nWidth = image.cols;
					nHeight = image.rows;
					nChannnels = image.channels();
					msg.width = image.cols;
					msg.height = image.rows;
					msg.channels = image.channels();
					length = nWidth*nHeight*nChannnels;
					msg.data_length = length;
					msg.magnetic_valve_num = camera_number;    //To indicate the camera series number is the 4th;
					int nsize = sizeof(VCameraMessageTrans);

					char* MsgPost = new char[64];
					strcpy(MsgPost, file_path.c_str());
					
					if (image_show)
					{
						//while (!m_show_ok)
						//{
						//	Sleep(10);
						//}
						information Info;
						Info.channels = nChannnels;
						Info.height = nHeight;
						Info.width = nWidth;
						Info.data_legnth = length;
						Info.m_picture_data = (char*)image.data;
						//m_show_ok = false;
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 5, (LPARAM)&Info);
					}
					else
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
					//data_buffer_.clear();
					//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
					size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
					VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
					nWidth = msg2->width;
					nHeight = msg2->height;
					int nSignalling = msg2->signalling;

					if (nSignalling == 20)
					{
						auto& io = socket_->get_io_service();
						if (!socket_->is_open())
							return false;
						asio::error_code er;
						auto buf = asio::buffer(&msg, sizeof(msg));
						int nLenght = msg.byteSize();
						socket_->write_some(buf);

						//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
						buf = asio::buffer(image.data, length);

						socket_->write_some(buf);
						std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					}
					else
					{
						Sleep(1000);
					}
					int sleep_times = int(float(100) / camera_scan_speed);
					//Sleep(sleep_times);
					std::this_thread::sleep_for(std::chrono::milliseconds(sleep_times));
				}
				else if (aq::Communicator::single_image_load)
				{
					aq::Communicator::single_image_load = false;
					file_countr++;
					file_path = file_list[file_countr];		
					image = cv::imread(file_path);
					nWidth = image.cols;
					nHeight = image.rows;
					nChannnels = image.channels();
					msg.width = image.cols;
					msg.height = image.rows;
					msg.channels = image.channels();
					length = nWidth*nHeight*nChannnels;
					msg.magnetic_valve_num = camera_number;    //To indicate the camera series number is the 4th;
					char* MsgPost = new char[64];
					strcpy(MsgPost, file_path.c_str());
					if (image_show)
					{
						Info1.channels = nChannnels;
						Info1.height = nHeight;
						Info1.width = nWidth;
						Info1.data_legnth = length;
						Info1.m_picture_data = (char*)image.data;
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 5, (LPARAM)&Info1);
					}
					else
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
						::PostMessage(m_hMainWnd, ID_STOPSEND, file_countr, 0);
					//data_buffer_.clear();
					//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
					size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
					VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
					nWidth = msg2->width;
					nHeight = msg2->height;
					int nSignalling = msg2->signalling;

					if (size != 0)
					{
						//sessions[0]->sendMessage(msg);
						{
							auto& io = socket_->get_io_service();
							if (!socket_->is_open())
								return false;
							asio::error_code er;
							auto buf = asio::buffer(&msg, sizeof(msg));
							int nLenght = msg.byteSize();
							socket_->write_some(buf);

							//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
							buf = asio::buffer(image.data, length);

							//asio::async_write(_socket, buf, [keep = std::move(msg)](asio::error_code, std::size_t){});
							//asio::write(*_socket, buf, NULL, &er);
							socket_->write_some(buf);
						}
						std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));;
					}
				}
				else if (aq::Communicator::last_image_load)
				{
					aq::Communicator::last_image_load = false;
					if (file_countr >= 1)
					{
						file_countr--;					
					}
					else
					{
					}
					file_path = file_list[file_countr];
					image = cv::imread(file_path);
					nWidth = image.cols;
					nHeight = image.rows;
					nChannnels = image.channels();
					msg.width = image.cols;
					msg.height = image.rows;
					msg.channels = image.channels();
					length = nWidth*nHeight*nChannnels;
					msg.magnetic_valve_num = camera_number;    //To indicate the camera series number is the 4th;
					char* MsgPost = new char[64];
					strcpy(MsgPost, file_path.c_str());
					if (image_show)
					{
						Info1.channels = nChannnels;
						Info1.height = nHeight;
						Info1.width = nWidth;
						Info1.data_legnth = length;
						Info1.m_picture_data = (char*)image.data;
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 5, (LPARAM)&Info1);
					}
					else
						::PostMessage(m_hMainWnd, ID_SHOWCTRLMESSAGE, 10, (LPARAM)MsgPost);
						::PostMessage(m_hMainWnd, ID_STOPSEND, file_countr, 0);
					//data_buffer_.clear();
					//std::int64_t size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), 8));
					size = asio::read(*socket_, asio::buffer((char*)data_buffer_.data(), sizeof(VCameraMessageTrans)));
					VCameraMessageTrans* msg2 = (VCameraMessageTrans*)data_buffer_.data();
					nWidth = msg2->width;
					nHeight = msg2->height;
					int nSignalling = msg2->signalling;

					if (size != 0)
					{
						//sessions[0]->sendMessage(msg);
						{
							auto& io = socket_->get_io_service();
							if (!socket_->is_open())
								return false;
							asio::error_code er;
							auto buf = asio::buffer(&msg, sizeof(msg));
							int nLenght = msg.byteSize();
							socket_->write_some(buf);

							//auto buf = asio::buffer(msg.bytes(), msg.byteSize());
							buf = asio::buffer(image.data, length);

							//asio::async_write(_socket, buf, [keep = std::move(msg)](asio::error_code, std::size_t){});
							//asio::write(*_socket, buf, NULL, &er);
							socket_->write_some(buf);
						}
						std::cout << "client:ip:" << socket_->remote_endpoint().address() << "   port:" << socket_->remote_endpoint().port() << std::endl;
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));;
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(300));;
				}
			}
			send_ok = true;
			::PostMessage(m_hMainWnd, ID_SENDMESSAGE, send_ok, 0);
		}
		catch (...)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
	else
	{
		;
	}

	return true;
}
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H
#include "VAsyncIO.h"
#include <queue>
#include <mutex>
#include "VTx2Session.h"
#include "VCameraMessageTrans.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
//#include <asio\ip\address_v4.hpp>

#define  DATA_BUFFER_LENGTH 5000000
using namespace asio::ip;


namespace asio {
	class io_service;
	class VTx2Session;
	namespace ip {
		class tcp;
		template <typename InternetProtocol>
		class basic_endpoint;
	}
	template <typename Protocol>
	class stream_socket_service;
	template <typename Protocol,
		typename StreamSocketService = stream_socket_service<Protocol> >
	class basic_stream_socket;
}

namespace Vsee{
	class VCameraFrame;
}

namespace aq {

class Communicator;

enum CMD
{
    SEND_RESULTS   = 71,
    START_RECV_IMG = 1,
    STOP_RECV_IMG  = 2
};

enum RESULT_STATE
{
    NO         = 0,
    OK         = 1,
    V_WRINKLED = 2,
    WRINKLED   = 3,
    V_DEFORMED = 4,
    DEFORMED   = 5
};

//***********Protocol,use to driver Leihang Valve electric board,Begin*******************************
/*
Name : typedef struct _time_calib_req_tag_:
Function: Time correction ask frame;
时间校准帧，任意一个Agent都可以向I/O-Server在任意时刻发送时间校准包,该包用于同步Agent和I/O-Server的时间上下文。
Agent向I/O-Server发送如下的包（payload_len=8），其中 type 必须填充 0x80000010 （请求）;src 必须填充 0xCC880000（请求类型：为时间校准）;
*/
typedef struct _time_calib_req_tag_
{
	uint32_t type;
	uint32_t src;
}time_calib_req_t;


/*
Name : typedef struct _ack_tag_:
Function: Time correcting frame response
I/O-Server收到上述包后，会向Agent返回如下的包（payload_len=16）, 其中 type 是 0x80000000（响应）;
其中 ack_src 是 0xCC8800DE（响应源：为时间校准请求），（即之前发送给I/O-Server的时间校准请求类型）,
而ack_val是一个 64-bit的绝对非负整数，代表I/O-Server内部的绝对时间滴答值，每个滴答（也就是每个当量‘1’）代表 0.1us（对应内部10mhz的管理时钟源）
注意：ack_val 是 int64 然而，此处的上下文决定其值必须是非负的, 如果当 ack_src 表示 “为时间校准请求” 的响应源;
而 ack_val 为负数时；那么这是一个未定义的包; Agent应当忽略此包.
*/
typedef struct _ack_tag_
{
	uint32_t type;
	uint32_t ack_src;
	int64_t ack_val;
} ack_t;


/*
Name : typedef struct _io_output_transac_tag_
function: I/O-Server 的驱动帧
Agent 向 I/O-Server发送如下的包（payload_len=20）, 其中 type 必须填充 0x80000010 （请求）;src 必须填充 0xCC880004（请求类型：为I/O驱动请求）;
port对应硬件输出端口号码，合法值为：1 ~ 28；如果值不合法那么该包将丢弃, 同时I/O驱动业务将不被任何一个硬件端口执行, ref - 参考时间，即在第 1 小节中，
通过时间校准获得的 ‘ack_val’, 代表I/O-Server内部的绝对时间滴答值，每个滴答（也就是每个当量‘1’）代表0.1us（对应内部10mhz的管理时钟源）;
delay - 代表延迟值，该值 × 0.1us，即为该I/O 驱动相对于 ref 的延迟值, dura - I/O驱动时间，该值 × 0.1us，即为 I/O 处于驱动状态（ON State）的时间
*/
typedef struct _io_output_transac_tag_
{
	uint32_t type;
	uint32_t src;
	uint32_t port;
	uint32_t ref;
	uint32_t delay;
	uint32_t dura;
}io_output_transac_t;

class Communicator
{
private:
    static std::vector<std::string> split(std::string str, char seg);
    static void print_hex(unsigned char *_buf,int _len);
public:
    Communicator();

    bool tcp_connect(std::string _ip, int _port, std::string& _error_str);

    bool receive_img(std::string& _error_str);
	bool aq::Communicator::receive_img(std::string &_error_str, asio::basic_stream_socket<asio::ip::tcp>* socket_);
    bool send_cmd(CMD _cmd, std::string& _error_str, std::vector<int> _state = std::vector<int>(), int _sequence = -1);

	bool server(std::string _ip, int _port, std::string &_error_str);
	bool server_run(asio::basic_stream_socket<asio::ip::tcp>* socket_, std::string _ip, int _port, std::string &_error_str);
	bool get_file_list(std::string file_load_path);
	int		get_files(std::string fileFolderPath, std::string fileExtension, int& nFileNum);
	bool run_tcp_connect(std::string _ip, int _port);
	bool run_tcp_connect(std::string _ip, int _port, int camera_number);
	bool tcp_connect_thread(std::string _ip, int _port);
	bool tcp_connect_thread2(std::string _ip, int _port, int camera_number);


	std::queue<Vsee::VCameraFrame>* frame_queue_;
	std::mutex m_;

//private:
public:
	 int send_ok=false;
	asio::io_service* io_service_;
	//asio::ip::basic_endpoint<asio::ip::tcp>* ep_;
	tcp::endpoint  * ep_;
	asio::basic_stream_socket<asio::ip::tcp>* socket_;
	std::string data_buffer_;
	asio::ip::tcp::socket* _socket;
	boost::scoped_ptr<asio::ip::tcp::socket> magnetic_valve_socket_;
	//asio::tcp::acceptor acceptor_;
	asio::io_service m_io_magnetic;

	Vsee::VTx2Session* sessions[8];
	tcp::socket* sockets[8];
	asio::io_service* ios[8];
	std::queue<Vsee::InferFrame> infer_queue;
	std::mutex             infer_mutex;
	Vsee::InferFrame infer_frame;
	tcp::endpoint eps[8];
	std::vector<std::string> file_list;
	int file_amount;
	static bool image_show;
	static bool batch_image_load;
	static bool single_image_load;
	static bool last_image_load;
	static bool    m_bServerSendMode;
	static bool    m_bClientSendMode;
	static bool m_show_ok;
	int		counter;
	float		camera_scan_speed;
	static  Vsee::VCameraMessageTrans  box_ctrl_msg;
};

}

#endif // COMMUNICATOR_H

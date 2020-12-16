#ifndef EXECUTE_H
#define EXECUTE_H

#include <string>
#include <vector>
#include <iostream>
#include <io.h>
#include <fstream>
#include "VCameraFrame.h"
#include "VCameraSession.h"
#include "VCameraEnumPrivate.h"
//#include "VTx2Session.h"
using namespace asio::ip;
using namespace asio;
using namespace Vsee;

//namespace Vsee {
//	class VCameraFrame;
//}
namespace cv {
	class Mat;
}
namespace aq {

class Communicator;
class Classify;

class Execute
{
public:
    Execute();

	void run_tcp_connect(std::string _ip, int _port);
	void run_tcp_server(std::string _ip, int _port);
    void run_classify();

	void tcp_connect(std::string _ip, int _port);
	void tcp_server(std::string _ip, int _port);
	int aq::Execute::load_classify(Classify* classify_, Vsee::VCameraFrame frame, std::vector<int> &_states);
	int aq::Execute::load_classify(Classify* classify_, cv::Mat frame, std::vector<std::string> &_states);
	int aq::Execute::load_classify_test(Classify* classify_, cv::Mat frame, std::vector<std::string> &_states);
	void classify();
	std::vector<int> send(std::vector<int> m_blackspot_states, std::vector<int> m_yellowrust_states, std::vector<int> m_breach_states, std::vector<int> m_mildew_states, std::vector<int> m_brokenskin_states);
	std::string get_name();
	void set_name();
private:
    Communicator* communicator_;
    Classify* classify_;//大分类
	Classify* classify__Bmildew;//大霉点
	Classify* segement_blackspot; 
	Classify* segement_mildew;
	Classify* segement_brokenskin;
	Classify* segement_breach;
	Classify* segement_yellowrust;
	asio::io_service* ios[2];
	tcp::socket* sockets[2];	

	
};
}

#endif // EXECUTE_H

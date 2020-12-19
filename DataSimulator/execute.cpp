#include "stdafx.h"
#include "execute.h"
#include "communicator.h"
#include "classify.h"

#include <algorithm>
#include "opencv2/opencv.hpp"
#include "rw_ini.h"
#include <thread>
#include <iostream>
#include <vector>
#include <io.h>
#include <string>
#include <fstream>
#include<windows.h>


//defined variable
std::vector<std::string> blackspot_states;
std::vector<std::string> yellowrust_states;
std::vector<std::string> Bmildew_states;
std::vector<std::string> breach_states;
std::vector<std::string> mildew_states;
std::vector<std::string> brokenskin_states;

std::vector<int> m_blackspot_states;
std::vector<int> m_yellowrust_states;
std::vector<int> m_Bmildew_states;
std::vector<int> m_breach_states;
std::vector<int> m_mildew_states;
std::vector<int> m_brokenskin_states;

std::string s;
std::vector<int> A ;

char model1_path[256];
char model2_path[256];
char model3_path[256];
char model4_path[256];
char model5_path[256];

aq::Execute::Execute() : communicator_(new Communicator), segement_blackspot(new Classify(model1_path, "Segment", "BlackSpot", "V2")),
segement_mildew(new Classify(model2_path, "Segment", "Mildew", "V1")), segement_yellowrust(new Classify(model3_path, "Segment", "YellowRust", "V1")),
segement_brokenskin(new Classify(model4_path, "Segment", "Brokenskin", "V1")), segement_breach(new Classify(model5_path, "Segment", "Breach", "V1"))
{
	cv::Mat mat = cv::Mat::zeros(900, 320, CV_8UC3);
	segement_blackspot->reset();
	segement_blackspot->segment_impl(mat);

	segement_mildew->reset();
	segement_mildew->segment_impl(mat);

	segement_yellowrust->reset();
	segement_yellowrust->segment_impl(mat);

	segement_brokenskin->reset();
	segement_brokenskin->segment_impl(mat);

	segement_breach->reset();
	segement_breach->segment_impl(mat);
	ios[0] = new asio::io_service(1);
	ios[1] = new asio::io_service(1);

}

void aq::Execute::run_tcp_connect(std::string _ip, int _port)
{
	std::thread t(&aq::Execute::tcp_connect, this, _ip, _port);
	t.detach();
}
void aq::Execute::run_tcp_server(std::string _ip, int _port)
{
	std::string file_load_path="";
	std::thread t(&aq::Execute::tcp_server, this, _ip, _port);
	t.detach();
}


void aq::Execute::run_classify()
{
	std::thread t(&aq::Execute::classify, this);
	t.detach();
}

void aq::Execute::tcp_connect(std::string _ip, int _port)
{
	//CWnd::MessageBox("to tcp_connetct!");
	//MessageBox(NULL, "tcp_connect", "ddd", MB_OK);
	std::string error_str;
	if (!communicator_->tcp_connect(_ip, _port, error_str))
	{
		std::cerr << error_str << std::endl;
		return;
	}
	std::cout << "connected " << _ip << ":" << _port << std::endl;
	VCameraMessage msg;
	msg.width = 115;
	msg.height = 115;
	msg.signalling = 2;
	communicator_->counter = 0;
	communicator_->send_cmd(aq::CMD::START_RECV_IMG, error_str);
	while (1)
	{
		auto buf = asio::buffer(&msg, sizeof(msg));
		communicator_->socket_->write_some(buf);
		std::string s;
		if (!communicator_->receive_img(s))
		{
			std::cerr << s << std::endl;
		}
	}
}

void aq::Execute::tcp_server(std::string _ip, int _port)
{
	//CWnd::MessageBox("to tcp_connetct!");
	
	std::string error_str;
	if (!communicator_->server(_ip, _port, error_str))
	{
		std::cerr << error_str << std::endl;
		return;
	}
}


int aq::Execute::load_classify(Classify* classify_, cv::Mat frame, std::vector<std::string> &_states)
{
	clock_t startTime, endTime;
	startTime = clock();
	classify_->reset();
	//classify_->segment_impl(frame);
	startTime = clock();
	classify_->classify(frame);
	endTime = clock();
	std::cout << "The classify time is: " << (endTime - startTime) << "ms" << std::endl;
	classify_->get_classify_state(_states);
	std::cout << "**********模型的_states.size:" << _states.size() << std::endl;
	return _states.size();
}

int aq::Execute::load_classify(Classify* classify_, Vsee::VCameraFrame frame, std::vector<int> &_states)
{
	clock_t startTime, endTime;
	startTime = clock();
	classify_->reset();
	classify_->segment(&frame);
	endTime = clock();
	std::cout << "The segment time is: " << (endTime - startTime) << "ms" << std::endl;
	startTime = clock();
	classify_->classify();
	endTime = clock();
	std::cout << "The classify time is: " << (endTime - startTime) << "ms" << std::endl;
	classify_->get_classify_states(_states);
	std::cout << "**********模型的_states.size:" << _states.size() << std::endl;
	return _states.size();
}

int aq::Execute::load_classify_test(Classify* classify_, cv::Mat frame, std::vector<std::string> &_states)
{
	clock_t startTime, endTime;
	startTime = clock();
	classify_->reset();
	classify_->segment_impl(frame);
	startTime = clock();
	classify_->classify();
	endTime = clock();
	std::cout << "The classify time is: " << (endTime - startTime) << "ms" << std::endl;
	classify_->get_classify_state(_states);
	std::cout << "**********模型的_states.size:" << _states.size() << std::endl;
	return _states.size();
}
std::vector<int> aq::Execute::send(std::vector<int> m_blackspot_states, std::vector<int> m_yellowrust_states, std::vector<int> m_breach_states, std::vector<int> m_mildew_states, std::vector<int> m_brokenskin_states)
{
	//char s[32];
	//char para[] = "net.ini";
	//strcpy(s, "Priority");
	//char max1[64]; char max2[64];
	//char max3[64]; char max4[64];
	//char max5[64];
	//CIni  ini;
	//char *p;
	//ini.OpenFile(para, "r");

	//// load port and ip
	//p = ini.GetStr(s, "max1");
	//strcpy(max1, p);
	//cout << "max1=" << max1  << endl;

	//p = ini.GetStr(s, "max2");
	//strcpy(max2, p);
	//cout << "max2=" << max2 << endl;

	//p = ini.GetStr(s, "max3");
	//strcpy(max3, p);
	//cout << "max3=" << max3 << endl;

	//p = ini.GetStr(s, "max4");
	//strcpy(max4, p);
	//cout << "max4=" << max4 << endl;

	//p = ini.GetStr(s, "max5");
	//strcpy(max5, p);
	//cout << "max5=" << max5 << endl;
	A.clear();
	for (int i = 0; i < 8; i++)
	{
		if (m_blackspot_states[i]>0)
		{
			A.push_back(1);
		}
		else if (m_mildew_states[i] > 0)
		{
			A.push_back(2);
		}
		else if (m_brokenskin_states[i] > 0)
		{
			A.push_back(3);
		}
		else if (m_breach_states[i] > 0)
		{
			A.push_back(4);
		}
		else if (m_yellowrust_states[i] > 0)
		{
			A.push_back(5);
		}
		else
		{
			A.push_back(6);
		}
	}
	return A;
	
}
void aq::Execute::classify()
{
	
	int k1 = 0;
	int k2 = 0;
	int k3 = 0;
	int k4 = 0;
	int k5 = 0;
	while (1)
	{
		if (communicator_->frame_queue_->empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}
		communicator_->m_.lock();
		Vsee::VCameraFrame frame = communicator_->frame_queue_->front();
		communicator_->frame_queue_->pop();
		communicator_->m_.unlock();

		k1 = load_classify(segement_blackspot, frame, m_blackspot_states);
		k2 = load_classify(segement_mildew, frame, m_mildew_states);
		k3 = load_classify(segement_brokenskin, frame, m_brokenskin_states);
		k4 = load_classify(segement_breach, frame, m_breach_states);
		k5 = load_classify(segement_yellowrust, frame,m_yellowrust_states);
		communicator_->send_cmd(aq::CMD::SEND_RESULTS, s, send(m_blackspot_states, m_yellowrust_states, m_breach_states, m_mildew_states, m_brokenskin_states), segement_breach->sequence());
		std::cout <<  "发送完毕" << std::endl;
	}
	
	//文件夹测试程序
	//cv::Mat images;
	//aq::aidi::LabelIO result;
	//int nSize = 0;
	//size_t  number;
	//std::vector<cv::String> fn;
	//std::ofstream oFile;
	//std::string pattern = "D:\\天佑\\新建文件夹\\整体方案设计\\";
	//std::string check = "Mildew";
	//oFile.open("D:/Information5.csv", std::ios::out | std::ios::trunc);
	//oFile << "图片序号" << "," << "原图类型" << "," << "检测类型" << "," << "正误" << std::endl;
	//cv::glob(pattern+check, fn);
	//number= fn.size();
	//for (size_t i = 0; i < number; i++)
	//{
	//	images = (cv::imread(fn[i]));
	//	k1 = load_classify(segement_blackspot, images, blackspot_states);
	//	k2 = load_classify(segement_mildew, images, mildew_states);
	//	k3 = load_classify(segement_brokenskin, images, brokenskin_states);
	//	k4 = load_classify(segement_breach, images, breach_states);
	//	k5 = load_classify(segement_yellowrust, images, yellowrust_states);
	//	for (int j = 0; j < k1; j++){
	//		if (blackspot_states[j] == check){
	//			oFile << "图片" << i << "," << "黑斑" << "," << blackspot_states[j] << "," << "正确" << "," << std::endl;;
	//		}
	//		else{
	//			oFile << "图片" << i << "," << "黑斑" << "," << blackspot_states[j] << ","  << "错误" << std::endl;
	//		}
	//	}

	//	for (int j = 0; j < k2; j++){
	//		if (mildew_states[j] == check){
	//			oFile << "图片" << i << "," << "黑斑" << "," << mildew_states[j] << "," << "正确" << "," << std::endl;;
	//		}
	//		else{
	//			oFile << "图片" << i << "," << "黑斑" << "," << mildew_states[j] << "," << "错误" << std::endl;
	//		}
	//	}


	//	for (int j = 0; j < k3; j++){
	//		if (brokenskin_states[j] == check){
	//			oFile << "图片" << i << "," << "黑斑" << "," << brokenskin_states[j] << "," << "正确" << "," << std::endl;;
	//		}
	//		else{
	//			oFile << "图片" << i << "," << "黑斑" << "," << brokenskin_states[j] << "," << "错误" << std::endl;
	//		}
	//	}


	//	for (int j = 0; j < k4; j++){
	//		if (breach_states[j] == check){
	//			oFile << "图片" << i << "," << "黑斑" << "," << breach_states[j] << "," << "正确" << "," << std::endl;;
	//		}
	//		else{
	//			oFile << "图片" << i << "," << "黑斑" << "," << breach_states[j] << "," << "错误" << std::endl;
	//		}
	//	}

	//	for (int j = 0; j < k5; j++){
	//		if (yellowrust_states[j] == check){
	//			oFile << "图片" << i << "," << "黑斑" << "," << yellowrust_states[j] << "," << "正确" << "," << std::endl;;
	//		}
	//		else{
	//			oFile << "图片" << i << "," << "黑斑" << "," << yellowrust_states[j] << "," << "错误" << std::endl;
	//		}
	//	}
	//	//if (k5 > 0){
	//	//	oFile << "-----" << "," << "-----" << "," << "-----" << "," << k5 << "," << std::endl;
	//	//}
	//}
	//
	//
	//oFile.close();

}
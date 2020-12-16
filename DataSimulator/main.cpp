#include <condition_variable>
#include <iostream>
#include <string> 
#include "VCameraFrame.h"
#include <Windows.h>
#include "execute.h"
#include "rw_ini.h"
#include <opencv2\opencv.hpp>
//using namespace std;
//using namespace cv;

/**
* 依照要求，检测程度为黑斑，霉点，破皮，黄锈，裂口，好枣
*/

int main() {

 //defined variable
	char lpPath[] = "net.ini";
	char section[32];
	char section1[32];
	char strl[64];
	char cfg_path[512];
	extern char model1_path[256];
	extern char model2_path[256];
	extern char model3_path[256];
	extern char model4_path[256];
	extern char model5_path[256];
	char ip1[64]; int port1;
	char ip2[64]; int port2;
	char ip3[64]; int port3;
	char ip4[64]; int port4;
	strcpy(section, "System");
	strcpy(section1, "Model");
	CIni  ini;
	char *pVal1;
	ini.OpenFile(lpPath, "r");

// load port and ip
	pVal1 = ini.GetStr(section, "ip1");
	strcpy(ip1, pVal1);
	std::cout<< "beign get ini  parameter......" << std::endl;
	port1 = ini.GetInt(section, "port1");
	cout << "ip1=" << ip1 << ",  port1=" << port1 << endl;

	pVal1 = ini.GetStr(section, "ip2");
	strcpy(ip2, pVal1);
	port2 = ini.GetInt(section, "port2");
	cout << "ip2=" << ip2 << ",  port2=" << port2 << endl;

	pVal1 = ini.GetStr(section, "ip3");
	strcpy(ip3, pVal1);
	port3 = ini.GetInt(section, "port3");
	cout << "ip3=" << ip3 << ",  port3=" << port3 << endl;

	pVal1 = ini.GetStr(section, "ip4");
	strcpy(ip4, pVal1);
	port4 = ini.GetInt(section, "port4");
	cout << "ip4=" << ip4 << ",  port4=" << port4 << endl;


// load model path
	pVal1 = ini.GetStr(section1, "segement_blackspot");
	strcpy(model1_path, pVal1);
	cout << "model1_path=" << model1_path << endl;

	pVal1 = ini.GetStr(section1, "segement_mildew");
	strcpy(model2_path, pVal1);
	cout << "model2_path=" << model2_path << endl;

	pVal1 = ini.GetStr(section1, "segement_yellowrust");
	strcpy(model3_path, pVal1);
	cout << "model3_path=" << model3_path << endl;

	pVal1 = ini.GetStr(section1, "segement_brokenskin");
	strcpy(model4_path, pVal1);
	cout << "model4_path=" << model4_path << endl;

	pVal1 = ini.GetStr(section1, "segement_breach");
	strcpy(model5_path, pVal1);
	cout << "model5_path=" << model5_path << endl;
	ini.CloseFile();
	aq::Execute execute1;
	aq::Execute execute2;
	aq::Execute execute3;
	aq::Execute execute4;

	execute1.run_tcp_connect(ip1, port1);
	execute1.run_classify();
	//system("pause");
	execute2.run_tcp_connect(ip2, port2);
	execute2.run_classify();
	//system("pause");
	execute3.run_tcp_connect(ip3, port3);
	execute3.run_classify();
	//system("pause");
	execute4.run_tcp_connect(ip4, port4);
	execute4.run_classify();
	system("pause");
	return 0;
}
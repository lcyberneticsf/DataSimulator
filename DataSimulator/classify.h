#ifndef CLASSIFY_H
#define CLASSIFY_H
#define  PARSE_LABELIO
#ifdef PARSE_LABELIO
#include "label.pb.h"
#include "opencv2/opencv.hpp"
#endif
#include <vector>
#include <string>
#include "label.pb.h"
#include "opencv2/opencv.hpp"
#include "aidi_vision.h"
#include <iostream>
#include <io.h>
#include <fstream>
//#include "VCameraFrame.h"
//#include "communicator.h"

namespace cv {
class Mat;
}

namespace Vsee {
	class VCameraFrame;
}

namespace aq {

class AidiFactoryRunnerWrapper;

class Communicator;

class Classify
{
public:
    template<typename T>
    static std::vector<int> argsort(const std::vector<T>& array);
public:
	Classify(std::string _rootpath, std::string _model, std::string _name, std::string _v);
	void reset();
	void segment(Vsee::VCameraFrame* _frame);
	void segment_impl(cv::Mat _mat);
	void classify();
	void classify(cv::Mat _mat);
	std::string get_model();
	std::string get_name();
	int sequence();
	void parse_labelio(const aq::aidi::LabelIO& labelio, int &nSize);
	void get_classify_state(std::vector<std::string> &_classify_state);
	void get_classify_states(std::vector<int> &_classify_state);

private:
	aidi::Client client;
	int sequence_;
	std::vector<std::string> classify_state_;
	std::vector<int> classify_states;
	std::vector<cv::Mat> segment_results_;
	const std::string auth_code = "0807bc31-bb48-11ea-8585-f8f21e399a8c";
	std::string model;
	std::string name;
	int segement_state_[8];
};


}

#endif // CLASSIFY_H

#include "stdafx.h"
#include "classify.h"
#include "communicator.h"
#include "VCameraFrame.h"
#include "label.pb.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

aq::Classify::Classify(std::string _rootpath ,std::string _model, std::string _name,std::string _v) :
	model(_model),
	name(_name)
{
	std::string img_path = _rootpath + "/source";
	std::string model_path = _rootpath + "/model";
	client.add_model_engine(_model, model_path + "/" + _v);
	std::cout << "启动唯嵩" + _name + "检测模型" << std::endl;
}

void aq::Classify::parse_labelio(const aq::aidi::LabelIO& labelio, int &nSize)
{
#ifdef PARSE_LABELIO
	aq::aidi::Label label;
	aq::aidi::LabelIO result = labelio;// = client.wait_get_result(id);
	label.ParseFromArray(result.data(), result.size());

	//结果解析，可以使用Opencv或其他几何计算库，本示例以Opencv为例
	nSize = label.regions_size();
	std::cout << "nSize=" << nSize << std::endl;
	for (int j = 0; j < label.regions_size(); j++)
	{
		aq::aidi::Polygon s_polygon = label.regions(j).polygon();
		aq::aidi::Ring outer = s_polygon.outer();
		std::vector<cv::Point> s_contour;
		for (int k = 0; k < outer.points_size(); k++)
		{
			s_contour.push_back(cv::Point(outer.points(k).x(), outer.points(k).y()));
		}
		double s_area = 0.0f;
		//double s_area = cv::contourArea(s_contour);
		//cv::RotatedRect s_rotate_rect = cv::minAreaRect(s_contour);//最小外接矩形
		//cv::Rect s_bounding_rect = s_rotate_rect.boundingRect();//最小包围盒
		//cv::Point2f s_center = s_rotate_rect.center;
		//std::cout << "area : " << s_area << " size(w, h) : " << s_rotate_rect.size << " center : " << s_center.x << "," << s_center.y << std::endl;;
	}
#endif
}

void aq::Classify::reset()
{
	segment_results_.clear();
	classify_state_.clear();
	classify_states.clear();
	//memset(segement_state_, 0, sizeof(segement_state_));
	sequence_ = -1;
}
void aq::Classify::segment(Vsee::VCameraFrame * _frame)
{
	cv::Mat img(_frame->height(), _frame->width(), CV_8UC3, (void*)(_frame->data()));
	cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
	//cv::imshow("test", img);
	//cv::waitKey(0);
	sequence_ = _frame->sequence();
	segment_impl(img);
}
int aq::Classify::sequence()
{
	return sequence_;
}

void aq::Classify::segment_impl(cv::Mat _mat)
{
	cv::Mat img_roi, sig_img;
	int roi_x = 50;
	int roi_y = 0;
	int roi_width = _mat.cols - (roi_x * 2);
	int roi_height = _mat.rows;
	cv::Rect roi_rect(roi_x, roi_y, roi_width, roi_height);
	img_roi = _mat(roi_rect);
	float sig_height = _mat.rows / 8.0;

	for (int i = 0; i < 8; i++)
	{
		cv::Rect rect(int(0), int(sig_height*i), img_roi.cols, int(sig_height));
		cv::Mat dst = img_roi(rect);
		segment_results_.push_back(dst);
	}
}

std::string aq::Classify::get_model(){
	return aq::Classify::model;
}

std::string aq::Classify::get_name(){
	return aq::Classify::name;
}

void aq::Classify::get_classify_state(std::vector<std::string> &_classify_state)
{
	_classify_state = classify_state_;

}
void aq::Classify::get_classify_states(std::vector<int> &_classify_state)
{
	_classify_state = classify_states;

}
void aq::Classify::classify()
{
	int nSize = 0;
	int flag = 0;
	aq::aidi::Image image;
	aq::aidi::BatchImage batch_images;
	for (int i = 0; i < segment_results_.size(); i++)
	{
		cv::Mat img = segment_results_[i];
		image.from_chars((char*)img.data, img.rows, img.cols, img.channels());
		batch_images.push_back(image);
		
	}
	uint64_t id_1 = client.add_images(batch_images);
	aq::aidi::BatchLabelIO results_1 = client.wait_get_result(id_1);
	

	if (get_model() == "Segment")
	{
		//classify_states.push_back(0);
		for (int i = 0; i < results_1.size(); i++)
		{
			aq::aidi::LabelIO result = results_1[i];
			parse_labelio(result, nSize);
			if (nSize > 0)
			{
				//std::cout << result.to_json() << std::endl;
				classify_state_.push_back(get_name());
				flag = 1;
				classify_states.push_back(1);
				//DrawImage(nSize, results_1, image, result, images, Mildew);
			}
			else{
				classify_states.push_back(0);
			}

		}
	}

}


void aq::Classify::classify(cv::Mat _mat)
{
	int nSize = 0;
	aq::aidi::Image image;
	aq::aidi::BatchImage batch_images;
	cv::Mat img = _mat;
	image.from_chars((char*)img.data, img.rows, img.cols, img.channels());
	batch_images.push_back(image);
	uint64_t id_1 = client.add_images(batch_images);
	aq::aidi::BatchLabelIO results_1 = client.wait_get_result(id_1);
	if (get_model() == "Segment")
	{
		aq::aidi::LabelIO result = results_1[0];
		parse_labelio(result, nSize);
		//std::cout << "nsize大小为："<<nSize << std::endl;
		if (nSize > 0)
		{
			std::cout << result.to_json() << std::endl;
			classify_state_.push_back(get_name());
			//DrawImage(nSize, results_1, image, result, images, Mildew);
		}
	}

	//}

}
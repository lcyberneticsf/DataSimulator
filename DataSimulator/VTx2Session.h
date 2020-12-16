#ifndef VTX2SESSION_H
#define VTX2SESSION_H

#include "VCameraEnum.h"
#include "VCameraFrame.h"
#include "VCameraMessage.h"
#include "VCameraProcessorTypes.h"
#include "VCameraSession.h"
#include "VCameraEnumPrivate.h"

namespace Vsee
{
	
	struct InferFrame;
	class VTx2Session : public VCameraSession<VTx2Session>
	{
	public:
		using Base = VCameraSession<VTx2Session>;
		using ProcessFrameFunc = std::function<void(VCameraFrame&&)>;
		using SessionAbortedFunc = std::function<void()>;


		VTx2Session(tcp::socket& socket) :  //利用socket来初始化VTx2Session
			Base(socket),
			_process_frame_func(),
			_session_aborted_func()
		{}

		void startSession() //包含一个完整的接收、发送流程
		{
			static std::array<char, 1> mode = { TransMode::Up };

			//Base::(VCameraMessage(Token::SetTransMode, mode.data(), mode.size()));			// 在 VCameraSession<VTx2Session>->startSession()中执行asyncRecv()进行数据的接收
			//Base::startSession2(VCameraMessage(Token::SetTransMode, mode.data(), mode.size()));
			if (_socket.is_open())
			{
				_started = true;

				asyncRecv();
				//asyncSend();
			}
			//startSession2(VCameraMessage(Token::SetTransMode, mode.data(), mode.size()));
			sendMessage(VCameraMessage(Vsee::Token::SetTransMode, mode.data(), mode.size()));      //发送请求图像数据指令
		}

		void setFrameResult(const VFrameResult& res)
		{
			sendMessage(VCameraMessage(Token::PushProcessResult, res.bytes(), res.byteSize()));
		}

		void setProcessFrameFunc(ProcessFrameFunc&& func) { _process_frame_func = std::move(func); }
		void setProcessFrameFunc(const ProcessFrameFunc& func) { _process_frame_func = func; }

		void setSessionAbortedFunc(SessionAbortedFunc&& func) { _session_aborted_func = std::move(func); }
		void setSessionAbortedFunc(const SessionAbortedFunc& func) { _session_aborted_func = func; }

	private:
		friend class VCameraSession<VTx2Session>;

		void processMessage(VCameraMessage&& msg) /* override */
		{
			//MessageBox(NULL, "VCameraSession::processMessage", "ddd", MB_OK);
			if (msg.empty())
				return;

			if (msg.token() != Token::RetFrame)
				return;

			if (!_process_frame_func)
				return;

		//	_process_frame_func(VCameraFrame(std::move(msg)));//执行192行代码
			Vsee::VCameraFrame frame(std::move(msg));
			int nWidth = frame.width();
			int nHeight = frame.height();
			if (nWidth != 0)
			{
				cv::Mat img(frame.height(), frame.width(), CV_8UC3, (void*)(frame.data()));
				cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
				cv::imwrite("f:/lenna_gray.vmp", img);
			}
			
		}

		void sessionAborted() /* override */
		{
			if (_session_aborted_func)
				_session_aborted_func();
		}

	private:
		ProcessFrameFunc _process_frame_func;
		SessionAbortedFunc _session_aborted_func;
		//InferFrame*		infer_queue;
		std::queue<InferFrame>		*infer_queue;

	};

	struct InferFrame
	{
		InferFrame() :
		_session(nullptr), _frame() {}

		InferFrame(VTx2Session* s, VCameraFrame&& f) :
			_session(s), _frame(std::move(f)) {}

		InferFrame(InferFrame&& other) :
			_session(other._session), _frame(std::move(other._frame)) {
			other._session = nullptr;
		}

		InferFrame& operator=(InferFrame&& rhs)
		{
			if (&rhs != this)
			{
				_session = rhs._session;
				rhs._session = nullptr;

				_frame = std::move(rhs._frame);
			}

			return *this;
		}

		VTx2Session* _session;
		VCameraFrame _frame;
	};
}
#endif
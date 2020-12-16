
#include "stdafx.h"
#include "VCameraMessage.h"
#include "VCameraEnumPrivate.h"
#include "VCameraMessagePrivate.h"


namespace Vsee
{
	VCameraMessage::VCameraMessage() : _private(nullptr) {}
	VCameraMessage::VCameraMessage(VCameraMessagePrivate* pri) : _private(pri)
	{
	}
	VCameraMessage::~VCameraMessage()
	{
		if (_private)
			delete _private;
	}

	VCameraMessage::VCameraMessage(const VCameraMessage & other) :
		_private(new VCameraMessagePrivate(*other._private)),
		_string(other.data(), other.length())
	{
		//_string(other.data(), other.length());
		//char* buf = other.bytes();
		//_string.
		//_string.clear();
		//_string.length() = other.length();
		
	}
	VCameraMessage::operator bool() const { return !empty(); }

	VCameraMessage & VCameraMessage::operator=(const VCameraMessage & rhs)
	{
		if (this != &rhs)
		{
			if (rhs._private)
			{
				if (_private)
					*_private = *rhs._private;
				else
					_private = new VCameraMessagePrivate(*rhs._private);
			}
			else
			{
				delete _private;
				_private = nullptr;
			}
		}

		return *this;
	}

	VCameraMessage::VCameraMessage(VCameraMessage&& other) :
		_private(other._private)
	{
		other._private = nullptr;
	}

	VCameraMessage & VCameraMessage::operator=(VCameraMessage&& rhs)
	{
		if (this != &rhs)
		{
			_private = rhs._private;
			rhs._private = nullptr;
		}

		return *this;
	}

	VCameraMessage::VCameraMessage(const char * raw, std::size_t size) :
		_private(new VCameraMessagePrivate(raw, size)), _string(raw,size)
	{
		

	}

	VCameraMessage::VCameraMessage(std::uint32_t token, const char * data, size_t len, bool gen_crc) :
		_private(new VCameraMessagePrivate(token, data, len, gen_crc)), _string()
	{
		std::uint32_t length = 4 + len;
		std::uint32_t crc = 0;

		_string.reserve(length + 4);
		_string.append(LeadCode, 4);
		_string.append((const char*)&length, 4);
		_string.append((const char*)&token, 4);
		_string.append(data, len);

		if (gen_crc)
			crc = crc32(bytes() + 4, byteSize() - 4);

		_string.append((const char*)&crc, 4);
	}

	const char * VCameraMessage::findLeadCode(const char * buf, std::size_t len)
	{
		//return VCameraMessagePrivate::findLeadCode(buf, len);
		static const char lead_code[] = "\xE7\xB2\xC7\xA4";
		return safe_strchr(buf, len, lead_code, 4);
	}

	bool VCameraMessage::empty() const
	{
		if (!_private)
			return true;
		return _private->empty();
	}

	std::size_t VCameraMessage::nextLoad(const char* buf, std::size_t buf_len)
	{
		//return VCameraMessagePrivate::nextLoad(buf, buf_len);
		if (!buf || !buf_len)
			return 8;         //空缓冲区，至少需要接收8字节

		if (buf_len < 8)
			return 8 - buf_len;      //不足8字节继续接收剩余字节数

		if (
			buf[0] != LeadCode[0] &&
			buf[1] != LeadCode[1] &&
			buf[2] != LeadCode[2] &&
			buf[3] != LeadCode[3]
			)
			return 0;

		//返回长度字段值加上CRC字段长度
		return bytesToValue(buf[4], buf[5], buf[6], buf[7]) + 4;
	}

	VCameraMessage VCameraMessage::load(const char* buf, std::size_t size)
	{
		//return VCameraMessage(VCameraMessagePrivate::load(buf, size));
		if (!buf || !size)
			return{};


		do   //开始提取报文流程
		{
			if (
				buf[0] != LeadCode[0] &&
				buf[1] != LeadCode[1] &&
				buf[2] != LeadCode[2] &&
				buf[3] != LeadCode[3]
				)
				break;

			std::uint32_t length = 8 + bytesToValue(buf[4], buf[5], buf[6], buf[7]) + 4;
			if (size < length)
				break;

			return  VCameraMessage(buf, length);

		} while (0);

		return{};
	}

	bool VCameraMessage::checkCrc() const
	{
		/*if (!_private)
			return false;
			return _private->checkCrc();*/
		if (empty())
			return false;

		auto tmp_crc = crc32(bytes() + 4, byteSize() - 8);
		return crc() == tmp_crc;
	}

	std::uint32_t VCameraMessage::length() const
	{
		/*if (!_private)
			return 0;

			return _private->length();*/
		if (empty())
			return 0;

		return *(std::uint32_t*)(bytes() + 4);
	}

	void VCameraMessage::setLength(std::uint32_t len, bool up)
	{
		/*if (_private)
			_private->setLength(len, up);*/
		if (empty())
			return;

		*(std::uint32_t*)(bytes() + 4) = len;

		if (up)
			updateCrc();
	}

	std::uint32_t VCameraMessage::token() const
	{
		/*if (!_private)
			return 0;
			return _private->token();*/
		if (empty())
			return 0;
		return *(std::uint32_t*)(bytes() + 8);

	}

	void VCameraMessage::setToken(std::uint32_t tok, bool up)
	{
		/*if (_private)
			_private->setToken(tok, up);*/
		if (empty())
			return;
		*(std::uint32_t*)(bytes() + 8) = tok;
		if (up)
			updateCrc();

	}

	const char * VCameraMessage::data() const
	{
		/*if (!_private)
			return nullptr;
			return _private->data();*/
		if (empty())
			return nullptr;

		return bytes() + 12;
	}

	void VCameraMessage::setData(const char * d, std::size_t len, bool up)
	{
		/*if (_private)
			_private->setData(d, len, up);*/
		_string.resize(12);
		_string.reserve(len + 16);
		_string.append(d, len);
		_string.append("\x0\x0\x0\x0", 4);

		setLength(len + 4, up);
	}

	std::size_t VCameraMessage::dataSize() const
	{
		/*if (!_private)
			return 0;
		return _private->dataSize();*/
		if (empty())
			return 0;

		return length() - 4;
	}

	std::uint32_t VCameraMessage::crc() const
	{
		/*if (!_private)
			return 0;
			return _private->crc();*/
		if (empty())
			return 0;

		return *(std::uint32_t*)(bytes() + length() + 8);
	}

	void VCameraMessage::setCrc(std::uint32_t crc)
	{
		/*if (_private)
			_private->setCrc(crc);*/
		if (empty())
			return;

		*(std::uint32_t*)(bytes() + length() + 8) = crc;
	}

	void VCameraMessage::updateCrc()
	{
		/*if (_private)
			_private->updateCrc();*/
		if (empty())
			return;

		std::uint32_t new_crc = crc32(bytes() + 4, byteSize() - 8);
		*(std::uint32_t*)(bytes() + length() + 8) = new_crc;
	}

	const char * VCameraMessage::bytes() const
	{
		/*if(!_private)
			return nullptr;
			return _private->bytes();*/
		return _string.data();
	}

	std::size_t VCameraMessage::byteSize() const
	{
		/*if (!_private)
			return 0;
			return _private->byteSize();*/
		return _string.size();
	}
}

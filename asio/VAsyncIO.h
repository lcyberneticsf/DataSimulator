#pragma once

#ifndef _V_ASYNC_IO_H_
#define _V_ASYNC_IO_H_

#ifndef ASIO_STANDALONE
#	define ASIO_STANDALONE
#endif

//#include "../include/common/ignore_warning.h"

#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#	include <sdkddkver.h>
#	include "asio.hpp"
#endif

#if defined linux || defined __linux__ || defined __GNUC__ || defined __GNUG__ || defined __clang__
	WARNING_PUSH
	WARNING_DISABLE("-Wconversion")
#	include "asio.hpp"
	WARNING_POP
#endif

namespace Vsee
{
	using namespace asio::ip;
}

#endif
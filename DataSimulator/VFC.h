#pragma once

#ifndef VSEE_VFC_H_
#define VSEE_VFC_H_

#if defined(_MSC_VER)
#	if defined(VFC_DLL)
#		define VFC_API __declspec(dllexport)
#	else
#		define VFC_API __declspec(dllimport)
#	endif
#else
#	define VFC_API
#endif

#endif
//
//  backtrace.cpp
//  dispatch_noexcept
//
//  Created by Jeremy Agostino on 6/23/17.
//  Copyright © 2017 Tekserve. All rights reserved.
//

#include <cstdlib>
#include <string>
#include <array>

#if __APPLE__
#include <execinfo.h>
#endif

#ifdef __WIN32__
// We don't have platform support for backtrace() on win32
static inline int backtrace(void** buffer, int size) { return 0; }
static inline char** backtrace_symbols(void* const buffer, int size) { return NULL; }
#endif

std::string _gc_dispatch_get_backtrace()
{
	std::array<void*, 128> callstack;
	auto frames = backtrace(callstack.data(), callstack.size());
	if (frames <= 0)
	{
		return "";
	}
	
	char** strs = backtrace_symbols(callstack.data(), frames);
	std::string trace;
	for (int i = 0; i < frames; ++i)
	{
		trace += strs[i] + std::string{"\n"};
	}
	trace.pop_back();
	free(strs);
	
	return trace;
}

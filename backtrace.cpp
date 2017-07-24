//
//  backtrace.cpp
//  dispatch_noexcept
//
//  Created by Jeremy Agostino on 6/23/17.
//  Copyright © 2017 GroundControl Solutions Inc.
//  See accompanying LICENSE file.
//

#include <cstdlib>
#include <string>
#include <vector>

#if __APPLE__
#include <execinfo.h>
#endif

#ifdef __WIN32__
// We don't have platform support for backtrace() on win32
static inline int backtrace(void** buffer, int size) { return 0; }
static inline char** backtrace_symbols(void* const buffer, int size) { return NULL; }
#endif

std::vector<void *> _gc_dispatch_get_backtrace()
{
	std::vector<void *> callstack{128};
	auto frames = backtrace(callstack.data(), static_cast<int>(callstack.size()));
	if (frames <= 0)
	{
		return {};
	}
	callstack.resize(frames);
	
	return callstack;
}

std::string _gc_dispatch_get_backtrace_symbols(const std::vector<void *> & callstack)
{
	char ** strs = backtrace_symbols(callstack.data(), static_cast<int>(callstack.size()));
	std::string trace;
	for (size_t i = 0; i < callstack.size(); ++i)
	{
		trace += strs[i] + std::string{"\n"};
	}
	// Last entry is just a newline, remove it
	trace.pop_back();
	free(strs);
	
	return trace;
}

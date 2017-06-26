//
//  dispatch.cpp
//  GGSharedSupport
//
//  Created by Jeremy Agostino on 6/23/17.
//  Copyright © 2017 Tekserve. All rights reserved.
//

#include "dispatch.h"
#include <string>
#include <exception>
#include <thread>
#include <atomic>

// Remove the override macros from the header
#undef dispatch_sync
#undef dispatch_sync_f
#undef dispatch_async
#undef dispatch_async_f
#undef dispatch_apply
#undef dispatch_apply_f
#undef dispatch_after
#undef dispatch_after_f

#pragma mark - Options

static std::atomic_bool log_backtrace{false};
extern "C" void gc_dispatch_except_log_backtrace(bool enable)
{
	log_backtrace.store(enable, std::memory_order_relaxed);
}

static std::atomic<gc_dispatch_except_log_func_ptr> logging_function{nullptr};
void gc_dispatch_except_log_func(gc_dispatch_except_log_func_ptr ptr)
{
	logging_function.store(ptr, std::memory_order_relaxed);
}

#pragma mark - Logging

void CF_FORMAT_FUNCTION(1,2) LogWarningMessage(CFStringRef format, ...)
{
	va_list va;
	va_start(va, format);
	CFStringRef log = CFStringCreateWithFormatAndArguments(kCFAllocatorDefault, nullptr, format, va);
	va_end(va);
	
	auto func = logging_function.load(std::memory_order_relaxed);
	if (func) {
		func(log);
	}
	else {
		CFShow(log);
	}
	
	if (log) {
		CFRelease(log);
	}
}

#pragma mark - Block wrapping helpers

extern std::string _gc_dispatch_get_backtrace();

#ifdef _WIN32
// On Windows we don't get callee backtraces, so displaying those by forcing an unwind is not helpful
static constexpr bool force_unwind = false;
#endif
#ifdef __APPLE__
// On Mac we do get callee backtraces, and since clang unwinds throws from noexcept functions these are better than the useless throw backtraces.  Once clang gets its shit together we can choose between throw backtraces and calle backtraces.
static constexpr bool force_unwind = true;
#endif

template <class Block, class ...Args>
static void noexcept_call_block(const std::string &bt_str, Block bl, Args ...args) noexcept
{
	if (force_unwind) {
		try {
			bl(args...);
		}
		// Explicitly catch exception to force a stack unwind
		catch (...) {
			LogWarningMessage(CFSTR("FATAL APPLICATION ERROR -- An uncaught exception was intercepted before it could hit libdispatch and trigger an undefined behavior fault. Exception will be rethrown in C++ context. Block callee backtrace: %s"), bt_str.c_str());
			throw;
		}
	}
	else {
		// If this throws an exception it will be handled by the enclosing function's noexcept specifier
		bl(args...);
	}
}

template <class Ret, class ...TrailingArgs>
static auto _dispatchBlockFromFunc(void * context, Ret (*work)(void *, TrailingArgs...))
{
	auto block = ^(TrailingArgs ...args){
		work(context, args...);
	};
	return Block_copy(block);
}

template <class ...Args>
static auto _dispatchRethrowBlock(void (^block)(Args...)) -> decltype(block)
{
	// Retain the user-provided block
	__block auto blockRetained = Block_copy(block);
	
	// Generate a backtrace string, if required
	std::string bt_str;
	auto do_backtrace = log_backtrace.load(std::memory_order_relaxed);
	if (do_backtrace) {
		bt_str = _gc_dispatch_get_backtrace();
	}
	
	auto blockWithExHandler = ^(Args...args){
		// Ensure no exception escapes this block
		noexcept_call_block(bt_str, blockRetained, args...);
		Block_release(blockRetained);
	};
	return Block_copy(blockWithExHandler);
}

#pragma mark - Noexcept dispatch method wrappers

#ifdef DNE_SUPPORT_DISPATCH_SYNC
extern "C" void dispatch_sync_noexcept(dispatch_queue_t queue, dispatch_block_t block)
{
	dispatch_sync(queue, block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_SYNC_F
extern "C" void dispatch_sync_f_noexcept(dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	dispatch_sync_f(queue, context, work);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC
extern "C" void dispatch_async_noexcept(dispatch_queue_t queue, dispatch_block_t block)
{
	dispatch_async(queue, block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC_F
extern "C" void dispatch_async_f_noexcept(dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	dispatch_async_f(queue, context, work);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY
extern "C" void dispatch_apply_noexcept(size_t iterations, dispatch_queue_t queue, void (^block)(size_t))
{
	dispatch_apply(iterations, queue, block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
extern "C" void dispatch_apply_f_noexcept(size_t iterations, dispatch_queue_t queue, void * context, void (*work)(void *, size_t))
{
	dispatch_apply_f(iterations, queue, context, work);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY
extern "C" void dispatch_after_noexcept(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block)
{
	dispatch_after(when, queue, block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
extern "C" void dispatch_after_f_noexcept(dispatch_time_t when, dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	dispatch_after_f(when, queue, context, work);
}
#endif

#pragma mark - Exception-safe dispatch method wrappers

#ifdef DNE_SUPPORT_DISPATCH_SYNC
extern "C" void _gc_dispatch_sync_with_exception_handler(dispatch_queue_t queue, dispatch_block_t block)
{
	auto blockWithExHandler = _dispatchRethrowBlock(block);
	dispatch_sync(queue, blockWithExHandler);
	Block_release(blockWithExHandler);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_SYNC_F
extern "C" void _gc_dispatch_sync_f_with_exception_handler(dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	auto block = _dispatchBlockFromFunc(context, work);
	_gc_dispatch_sync_with_exception_handler(queue, block);
	Block_release(block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC
extern "C" void _gc_dispatch_async_with_exception_handler(dispatch_queue_t queue, dispatch_block_t block)
{
	auto blockWithExHandler = _dispatchRethrowBlock(block);
	dispatch_async(queue, blockWithExHandler);
	Block_release(blockWithExHandler);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC_F
extern "C" void _gc_dispatch_async_f_with_exception_handler(dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	auto block = _dispatchBlockFromFunc(context, work);
	_gc_dispatch_async_with_exception_handler(queue, block);
	Block_release(block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY
extern "C" void _gc_dispatch_apply_with_exception_handler(size_t iterations, dispatch_queue_t queue, void (^block)(size_t))
{
	auto blockWithExHandler = _dispatchRethrowBlock(block);
	dispatch_apply(iterations, queue, blockWithExHandler);
	Block_release(blockWithExHandler);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
extern "C" void _gc_dispatch_apply_f_with_exception_handler(size_t iterations, dispatch_queue_t queue, void * context, void (*work)(void *, size_t))
{
	auto block = _dispatchBlockFromFunc(context, work);
	_gc_dispatch_apply_with_exception_handler(iterations, queue, block);
	Block_release(block);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_AFTER
extern "C" void _gc_dispatch_after_with_exception_handler(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block)
{
	auto blockWithExHandler = _dispatchRethrowBlock(block);
	dispatch_after(when, queue, blockWithExHandler);
	Block_release(blockWithExHandler);
}
#endif

#ifdef DNE_SUPPORT_DISPATCH_AFTER_F
extern "C" void _gc_dispatch_after_f_with_exception_handler(dispatch_time_t when, dispatch_queue_t queue, void * context, dispatch_function_t work)
{
	auto block = _dispatchBlockFromFunc(context, work);
	_gc_dispatch_after_with_exception_handler(when, queue, block);
	Block_release(block);
}
#endif

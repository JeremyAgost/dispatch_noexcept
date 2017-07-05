//
//  dispatch.hpp
//  dispatch_noexcept
//
//  Created by Jeremy Agostino on 6/23/17.
//  Copyright © 2017 Tekserve. All rights reserved.
//

#ifndef dispatch_hpp
#define dispatch_hpp

#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>

#ifdef __cplusplus
extern "C" {
#endif
	
#pragma mark - Configuration
	
/**
 * @function gc_dispatch_except_log_backtrace
 * @param enable If true, an exception fault in dispatch will have the dispatch caller backtrace logged as well as the exception.
 */
extern void gc_dispatch_except_log_backtrace(bool enable);
	
typedef void(*gc_dispatch_except_log_func_ptr)(CFStringRef);
/**
 * @function gc_dispatch_except_log_func
 * @param ptr Function pointer that will be called with exception log messages. Pass NULL to use default.
 */
extern void gc_dispatch_except_log_func(gc_dispatch_except_log_func_ptr ptr);
	
#ifdef __APPLE__
	#define DNE_SUPPORT_DISPATCH_SYNC
	#define DNE_SUPPORT_DISPATCH_SYNC_F
	#define DNE_SUPPORT_DISPATCH_ASYNC
	#define DNE_SUPPORT_DISPATCH_ASYNC_F
	#define DNE_SUPPORT_DISPATCH_APPLY
	#define DNE_SUPPORT_DISPATCH_APPLY_F
	#define DNE_SUPPORT_DISPATCH_AFTER
	#define DNE_SUPPORT_DISPATCH_AFTER_F
#endif
	
#ifdef __WIN32__
	#define DNE_SUPPORT_DISPATCH_SYNC
	#define DNE_SUPPORT_DISPATCH_SYNC_F
	#define DNE_SUPPORT_DISPATCH_ASYNC
	#define DNE_SUPPORT_DISPATCH_ASYNC_F
	#define DNE_SUPPORT_DISPATCH_APPLY
//	#define DNE_SUPPORT_DISPATCH_APPLY_F	// This method is missing from libdispatch.dll
	#define DNE_SUPPORT_DISPATCH_AFTER
//	#define DNE_SUPPORT_DISPATCH_AFTER_F	// This method is missing from libdispatch.dll
#endif
	
#pragma mark - Noexcept marked prototypes of dispatch queue methods
	
#ifdef DNE_SUPPORT_DISPATCH_SYNC
extern void dispatch_sync_noexcept(dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_SYNC_F
extern void dispatch_sync_f_noexcept(dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_ASYNC
extern void dispatch_async_noexcept(dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_ASYNC_F
extern void dispatch_async_f_noexcept(dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_APPLY
extern void dispatch_apply_noexcept(size_t iterations, dispatch_queue_t queue, void (^block)(size_t));
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
extern void dispatch_apply_f_noexcept(size_t iterations, dispatch_queue_t queue, void * context, void (*work)(void *, size_t));
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_AFTER
extern void dispatch_after_noexcept(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_AFTER_F
extern void dispatch_after_f_noexcept(dispatch_time_t when, dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#pragma mark - Unsafe redirect prototypes of dispatch queue methods
	
#ifdef DNE_SUPPORT_DISPATCH_SYNC
extern void _gc_dispatch_sync_with_exception_handler(dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_SYNC_F
extern void _gc_dispatch_sync_f_with_exception_handler(dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_ASYNC
extern void _gc_dispatch_async_with_exception_handler(dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_ASYNC_F
extern void _gc_dispatch_async_f_with_exception_handler(dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_APPLY
extern void _gc_dispatch_apply_with_exception_handler(size_t iterations, dispatch_queue_t queue, void (^block)(size_t));
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
extern void _gc_dispatch_apply_f_with_exception_handler(size_t iterations, dispatch_queue_t queue, void * context, void (*work)(void *, size_t));
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_AFTER
extern void _gc_dispatch_after_with_exception_handler(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block);
#endif
	
#ifdef DNE_SUPPORT_DISPATCH_AFTER_F
extern void _gc_dispatch_after_f_with_exception_handler(dispatch_time_t when, dispatch_queue_t queue, void * context, dispatch_function_t work);
#endif
	
#ifdef __cplusplus
}
#endif

#ifdef GCMKSTR
	#error GCMKSTR already defined, you'll need to fix that
#endif

#pragma mark - Macros to override original dispatch queue methods with the exception handler variants

#define GCMKSTR(X) #X

#ifdef DNE_SUPPORT_DISPATCH_SYNC
#define dispatch_sync(queue, block) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_sync is being rewritten with a basic exception handler. Replace with dispatch_sync_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_sync_with_exception_handler(queue, block); \
}
#else
#define dispatch_sync(queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_sync is not supported." )))
#define dispatch_sync_noexcept(queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_sync_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_SYNC_F
#define dispatch_sync_f(queue, context, work) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_sync_f is being rewritten with a basic exception handler. Replace with dispatch_sync_f_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_sync_f_with_exception_handler(queue, context, work); \
}
#else
#define dispatch_sync_f(queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_sync_f is not supported." )))
#define dispatch_sync_f_noexcept(queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_sync_f_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC
#define dispatch_async(queue, block) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_async is being rewritten with a basic exception handler. Replace with dispatch_async_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_async_with_exception_handler(queue, block); \
}
#else
#define dispatch_async(queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_async is not supported." )))
#define dispatch_async_noexcept(queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_async_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_ASYNC_F
#define dispatch_async_f(queue, context, work) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_async_f is being rewritten with a basic exception handler. Replace with dispatch_async_f_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_async_f_with_exception_handler(queue, context, work); \
}
#else
#define dispatch_async_f(queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_async_f is not supported." )))
#define dispatch_async_f_noexcept(queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_async_f_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY
#define dispatch_apply(iterations, queue, block) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_apply is being rewritten with a basic exception handler. Replace with dispatch_apply_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_apply_with_exception_handler(queue, block); \
}
#else
#define dispatch_apply(iterations, queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_apply is not supported." )))
#define dispatch_apply_noexcept(iterations, queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_apply_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_APPLY_F
#define dispatch_apply_f(iterations, queue, context, work) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_apply_f is being rewritten with a basic exception handler. Replace with dispatch_apply_f_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_apply_f_with_exception_handler(queue, block); \
}
#else
#define dispatch_apply_f(iterations, queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_apply_f is not supported." )))
#define dispatch_apply_f_noexcept(iterations, queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_apply_f_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_AFTER
#define dispatch_after(when, queue, block) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_after is being rewritten with a basic exception handler. Replace with dispatch_after_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_after_with_exception_handler(when, queue, block); \
}
#else
#define dispatch_after(when, queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_after is not supported." )))
#define dispatch_after_noexcept(when, queue, block) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_after_noexcept is not supported." )))
#endif

#ifdef DNE_SUPPORT_DISPATCH_AFTER_F
#define dispatch_after_f(when, queue, context, work) \
{ \
	_Pragma(GCMKSTR(GCC warning( "Usage of dispatch_after_f is being rewritten with a basic exception handler. Replace with dispatch_after_f_noexcept if you can guarantee no exceptions will hit libdispatch." ))) \
	_gc_dispatch_after_f_with_exception_handler(when, queue, context, work); \
}
#else
#define dispatch_after_f(when, queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_after_f is not supported." )))
#define dispatch_after_f_noexcept(when, queue, context, work) \
	_Pragma(GCMKSTR(GCC error( "Method dispatch_after_f_noexcept is not supported." )))
#endif

#endif /* dispatch_hpp */

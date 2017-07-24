//
//  main.cpp
//  test_without_dispatch_noexcept
//
//  Created by Jeremy Agostino on 7/23/17.
//  Copyright © 2017 GroundControl Solutions Inc.
//  See accompanying LICENSE file.
//

#include <iostream>
#include <exception>
#include <dispatch/dispatch.h>

#ifdef INCLUDE_DISPATCH_NOEXCEPT
#include "dispatch.h"
#endif

void throw_error() {
	throw std::runtime_error("A nasty error has occurred");
}

void dispatch_thrower() {
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_global_queue(0,0), ^{
		throw_error();
	});
}

int main(int argc, const char * argv[]) {
	
#ifdef INCLUDE_DISPATCH_NOEXCEPT
	gc_dispatch_except_log_backtrace(true);
#endif
	
	dispatch_thrower();
	
	dispatch_main();
	
	return 0;
}

cmake_minimum_required(VERSION 3.1)
project(dispatch_noexcept)

set(DISPATCH_NOEXCEPT_SOURCE_FILES
	${version_rc}
	backtrace.cpp
	dispatch.cpp
	dispatch.h
)

set_source_files_properties(dispatch_noexcept.cpp COMPILE_FLAGS "-Wno-ignored-attributes -Wno-trigraphs")

add_library(dispatch_noexcept ${DISPATCH_NOEXCEPT_SOURCE_FILES})

set_target_properties(dispatch_noexcept PROPERTIES DEFINE_SYMBOL DNE_BUILDING_DNE)
target_compile_options(dispatch_noexcept PRIVATE  ${GC_COMPILER_ERRORS})

target_link_libraries(dispatch_noexcept
	GCLog
	${dispatch}
	${CoreFoundation}
)

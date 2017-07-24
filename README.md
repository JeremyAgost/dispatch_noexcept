# dispatch_noexcept

This is a library intended to make it easier to debug uncaught exceptions in code that makes use of libdispatch. Any blocks excuted with dispatch\_client\_callout are enclosed in an Objective-C try/catch block that invokes objc_terminate() if an exception isn't caught. While the end result is roughly the same as a typical unhandled exception program termination, it can make it somewhat harder to debug the uncaught exception. This library wraps the block in a new try/catch that will record and print the stack trace of the dispatch invocation.

## How To Use

The Xcode project is configured to build a shared library target. Include the "dispatch.h" header and use dispatch functions as normal. Every instance of an overriden dispatch function will add a compiler warning message indicating that the function is being overriden. The original functions are also redeclared as \*\_noexcept (e.g. dispatch\_async\_noexcept) if you want to use them in a context where you know exceptions won't be thrown.

### Stack Logging

Call `gc_dispatch_except_log_backtrace(true)` to enable logging of backtraces when an uncaught exception blows up. Of course there's some overhead. When traces are enabled, you'll get something like this printed to stderr:

```
FATAL APPLICATION ERROR -- An uncaught exception was intercepted before it could hit libdispatch and trigger an undefined behavior fault. Exception will be rethrown in C++ context. Block callee backtrace:
0   libdispatch_noexcept.dylib          0x00000001000acb1d _Z26_gc_dispatch_get_backtracev + 125
1   libdispatch_noexcept.dylib          0x00000001000af643 _ZL21_dispatchRethrowBlockIJEEDtfp_EU13block_pointerFvDpT_E + 403
2   libdispatch_noexcept.dylib          0x00000001000afd8d _gc_dispatch_after_with_exception_handler + 29
3   test_with_dispatch_noexcept         0x0000000100000e43 _Z16dispatch_throwerv + 67
4   test_with_dispatch_noexcept         0x0000000100000e97 main + 39
5   libdyld.dylib                       0x00007fff9c71f235 start + 1
libc++abi.dylib: terminating with uncaught exception of type std::runtime_error: A nasty error has occurred
```

### Testing

There are two binary targets, _test\_with\_dispatch\_noexcept_ and _test\_without\_dispatch\_noexcept_, that can be used to demonstrate the different exception handling behavior.

### Platform Support

An Xcode project is provided for building the shared library on macOS. A CMakeList file is provided for building on Win32 or Linux, if that's helpful for you.

## Authors

* **Jeremy Agostino** - [JeremyAgost](https://github.com/JeremyAgost)
* **Marc Aldorasi** - [m42a](https://github.com/m42a)
* **Mitchell Wong** - [mitchellwong](https://github.com/mitchellwong)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

"""

singleton library hook

makes a c++ node of the library

"""
@tool
extends Node
class_name CPPLibrary

static var _cpp_static_library: CPPStaticLibrary


static func get_lib() -> CPPStaticLibrary:
    if _cpp_static_library == null:
        _cpp_static_library = CPPStaticLibrary.new()    
    return _cpp_static_library
    

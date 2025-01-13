"""

static library hook (because c++ cannot get a static object through)


add me as CPPStaticLibrary

"""
@tool
extends Node

var cpp_static_library: CPPStaticLibrary

#func _ready():
    #if _singleton == null:
        #_singleton = self
        #set_physics_process(false) 


func test():
    
    print(cpp_static_library.my_static_function(34, 77))


func _ready():
    if cpp_static_library == null:
        set_physics_process(false) 
        cpp_static_library = CPPStaticLibrary.new()
        

## Example static-like function
#func my_static_function(a, b):
    #return a + b

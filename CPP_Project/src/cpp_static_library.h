/*

a singleton helper for Godot


it seems we cannot just bind a static class, we need to create an autoload in order to use from gdscript (the same as gdsript)




*/
#ifndef CPP_STATIC_LIBRARY_H
#define CPP_STATIC_LIBRARY_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties


class CPPStaticLibrary : public Node {
    GDCLASS(CPPStaticLibrary, Object);

   public:
    static void _bind_methods() {
        // Register methods for the library
        ClassDB::bind_method(D_METHOD("my_static_function", "a", "b"), &CPPStaticLibrary::my_static_function);


    }

    // we cannot actually make this function static and bind it!
    // we need to use autoloads same as gdscript
    int my_static_function(int a, int b) {
        return a + b;
    }
};






#endif
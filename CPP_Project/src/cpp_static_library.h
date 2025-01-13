/*

a singleton helper for Godot


it seems we cannot just bind a static class, we need to create an autoload in order to use from gdscript (the same as gdsript)




*/
#ifndef CPP_STATIC_LIBRARY_H
#define CPP_STATIC_LIBRARY_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties



// NOTE... i tried to make this an GodotObject or an Object, doesn't seem to work
class CPPStaticLibrary : public Node {
    GDCLASS(CPPStaticLibrary, Node)

   public:
    static void _bind_methods() {
        // WARNING I HAVE COMPILE ISSUES WITH THIS AREA  (STACK TRACE UNCLEAR)

        // Register methods for the library
        ClassDB::bind_method(D_METHOD("my_static_function", "a", "b"), &CPPStaticLibrary::my_static_function);

        ClassDB::bind_method(D_METHOD("floats_to_image", "image", "image_size", "format"), &CPPStaticLibrary::floats_to_image);
        ClassDB::bind_method(D_METHOD("blur_image", "input_image", "image_size", "radius"), &CPPStaticLibrary::blur_image);
    }

    // we cannot actually make this function static and bind it!
    // we need to use autoloads same as gdscript
    int my_static_function(int a, int b) {
        return a + b;
    }

    PackedFloat32Array image_channel_to_floats(const Ref<Image> image, int channel) {
        return ImageHelper::image_channel_to_floats(image, channel);
    }

    Ref<Image> floats_to_image(PackedFloat32Array image, Vector2i image_size, Image::Format image_format) {
        return ImageHelper::floats_to_image(image, image_size, image_format);
    }

    PackedFloat32Array blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius) {
        return ImageHelper::blur_image(input_image, image_size, radius);
    }
};

#endif
/*

static library for binding to godot,




*/
#ifndef C_STATIC_LIBRARY_H
#define C_STATIC_LIBRARY_H

#include <helper.h>
#include <macros.h>

// bind method macro (specific to CStaticLibrary)
#define BIND_METHOD(name, ...) ClassDB::bind_method(D_METHOD(#name, __VA_ARGS__), &CStaticLibrary::name);

// CStaticLibrary inherits Object, so try not to create over and over

// to use in gdscript:
//     var clib := CStaticLibrary.new()

class CStaticLibrary : public Object {
    GDCLASS(CStaticLibrary, Object)

   public:
    static void _bind_methods() {
        // WARNING I HAVE COMPILE ISSUES WITH THIS AREA  (STACK TRACE UNCLEAR)

        // Register methods for the library (using local macro)

        // WARNING: we can not use refs like PackedFloat32Array& without them being constant
        // it appears Godot needs them protected like this, it forces us to copy the data at some point so the hooks here are different sometimes to the c++ function
        BIND_METHOD(my_static_function, "a", "b")
        BIND_METHOD(floats_to_image, "image", "image_size", "format")
        BIND_METHOD(image_channel_to_floats, "image", "channel")
        BIND_METHOD(blur_image, "input_image", "image_size", "radius", "wrap")
        BIND_METHOD(overlay_image, "target_image", "overlay_image", "mode")
        BIND_METHOD(offset_image, "image", "image_size", "offset", "wrap")
        BIND_METHOD(colors_to_image, "color_array", "image_size")
        BIND_METHOD(generate_normal_map, "image", "image_size", "normal_strength", "wrap")
        BIND_METHOD(generate_normal_map_old, "image", "image_size", "normal_strength")
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

    PackedFloat32Array blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius, bool wrap) {
        return ImageHelper::blur_image(input_image, image_size, radius, wrap);
    }

    PackedFloat32Array overlay_image(const PackedFloat32Array& target_image, const PackedFloat32Array& overlay_image, int mode = 0) {
        PackedFloat32Array ret = target_image;  // we cannot use the reference in gdscript, so we will make a copy and return this
        ImageHelper::overlay_image(ret, overlay_image, mode);
        return ret;
    }

    PackedFloat32Array offset_image(const PackedFloat32Array& image, Vector2i image_size, Vector2i offset, bool wrap = true) {
        return ImageHelper::offset_image(image, image_size, offset, wrap);
    }

    PackedColorArray generate_normal_map(const PackedFloat32Array& image, Vector2i image_size, float normal_strength, bool wrap) {
        return ImageHelper::generate_normal_map(image, image_size, normal_strength, wrap);
    }

    PackedColorArray generate_normal_map_old(const PackedFloat32Array& image, Vector2i image_size, float normal_strength) {
        return ImageHelper::generate_normal_map_old(image, image_size, normal_strength);
    }

    Ref<Image> colors_to_image(const PackedColorArray& color_array, Vector2i image_size) {
        return ImageHelper::colors_to_image(color_array, image_size, Image::FORMAT_RGB8);
    }
};

#endif
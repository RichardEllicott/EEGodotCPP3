/*

Helper Functions



*/
#ifndef HELPER_H  // header guard
#define HELPER_H

#include <cstddef>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node.hpp>  // Node
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>  // godot::UtilityFunctions::print(input);
#include <godot_cpp/variant/vector2.hpp>
#include <stdexcept>
#include <vector>


// #include "core/object.h"
// #include "core/class_db.h"
// #include "core/math/vector2.h"
#include <random>


// #include <cmath>
// #include <algorithm>

// #include <godot_cpp/variant/packed_vector2_array.hpp>

using namespace godot;

// VIP functions
#pragma region GLOBALS

static const double DEG_TO_RAD = Math_PI / 180.0;  // multiply by this to convert
static const double RAD_TO_DEG = 180.0 / Math_PI;  // multiply by this to convert

// print shortcut
#ifndef PRINT
#define PRINT
template <typename... Args>
void print(Args... args) {
    UtilityFunctions::print(args...);
}
#endif

// lerp template function, works with c++ types
// see also:
// UtilityFunctions::lerp and UtilityFunctions::lerpf
#ifndef LERP
#define LERP
template <typename T>
T lerp(T a, T b, T alpha) {
    return a + (b - a) * alpha;
}
#endif

// positive mod (as c++ mod is actually division remainder)
// works the same as the python one, so never returns negative, allows seamless wrapping
#ifndef POS_MOD
#define POS_MOD
inline int pos_mod(int a, int b) {
    const int result = a % b;
    return result >= 0 ? result : result + b;
}
#endif

#pragma endregion

// get any node (staticly)
// usage:
// auto node2d = get_node_as<Node2D>(this, "Node2D");
//
#ifndef GET_NODE_AS
#define GET_NODE_AS
template <typename T>
T* get_node_as(Node* current_node, const NodePath& path) {
    if (!current_node) {
        UtilityFunctions::printerr("Current node is null!");
        return nullptr;
    }

    Node* node_ptr = current_node->get_node_or_null(path);
    return Object::cast_to<T>(node_ptr);
}
#endif

// less important functions in a library
class ImageHelper {
   public:
    // Static method to sample an image at UV coordinates using bilinear interpolation
    static Color sample_image(const Image& image, const Vector2& uv);      // for & ref
    static Color sample_image(const Ref<Image> image, const Vector2& uv);  // for Godot ref. i think the normal way

    // convert an image channel to float data
    static PackedFloat32Array image_channel_to_floats(const Ref<Image> image, int channel);

    static Ref<Image> floats_to_image(PackedFloat32Array image, Vector2i image_size, Image::Format image_format);

    // worked from:
    // https://gemini.google.com/app/2eb57cf690ac3d2e
    static PackedFloat32Array blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius);

    // Helper function to create a 1D Gaussian kernel
    static void create_gaussian_kernel(std::vector<float>& kernel, float radius);



    static PackedFloat32Array erode(const PackedFloat32Array& heightmap, Vector2i image_size, int iterations, float deposition, float erosion);
};

// // Simple Frequency Modulation function
//
// double fm_wave(double x, double carrier_freq, double modulating_freq, double modulation_index)
// {
// 	// FM Wave equation: y = sin(2 * pi * carrier_freq * x + modulation_index * sin(2 * pi * modulating_freq * x))
// 	return sin(2 * Math_PI * carrier_freq * x + modulation_index * sin(2 * Math_PI * modulating_freq * x));
// }

#pragma region HISTORY_BUFFER
// trouble using templates for this pattern as it involves two types, PackedVector2Array and Vector2
class PackedVector2ArrayBuffer {
   public:
    PackedVector2Array buffer = PackedVector2Array();

    int buffer_position = 0;

    PackedVector2ArrayBuffer(int size) {
        buffer.resize(size);
    }

    void resize(int size) {
        buffer.resize(size);
    }
    void add(Vector2 value) {
        buffer[buffer_position] = value;
        buffer_position = (buffer_position + 1) % buffer.size();
    }

    int size() {
        return buffer.size();
    }

    Vector2 get(int position) {
        position = pos_mod(buffer_position + position - 1, buffer.size());
        return buffer[position];
    }

    // PackedVector2Array get_frame(int position, int size) {
    //     position = pos_mod(buffer_position + position, buffer.size());

    //     PackedVector2Array return_buffer = PackedVector2Array();
    //     return_buffer.resize(size);
    //     for (int i = 0; i < size; i++) {
    //         int j = (position + i) % buffer.size();
    //         return_buffer[i] = buffer[j];
    //     }
    //     return return_buffer;
    // }

    // PackedVector2Array get_last_frame(int size) {
    //     return get_frame(size - 1, size);
    // }
};

#pragma endregion

#endif
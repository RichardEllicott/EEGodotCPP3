/*

Helper Functions



*/
#ifndef HELPER_H  // header guard
#define HELPER_H



#include <godot_cpp/variant/utility_functions.hpp>  // godot::UtilityFunctions::print(input);
#include <godot_cpp/classes/node.hpp> // Node


// #include <godot_cpp/variant/packed_vector2_array.hpp>

using namespace godot;

// // chat gp generated circular array pool (for audio), with resizing (uses a vector)
// #pragma region CIRCULAR_VECTOR_POOL

#include <vector>
#include <stdexcept>
#include <cstddef>


// working positive mod (as c++ mod is actually remainder)
// works the same as the python one, so never returns negative, allows seamless wrapping
inline int pos_mod(int a, int b) {
    const int result = a % b;
    return result >= 0 ? result : result + b;
}



#pragma region GET_NODE_AS_OR_NULL

// get any node (staticly)
// usage:
// auto node2d = get_node_as<Node2D>(this, "Node2D");
//
template <typename T>
T* get_node_as(Node* current_node, const NodePath& path) {
    if (!current_node) {
        UtilityFunctions::printerr("Current node is null!");
        return nullptr;
    }

    Node* node_ptr = current_node->get_node_or_null(path);
    return Object::cast_to<T>(node_ptr);
}

#pragma endregion




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

static const double DEG_TO_RAD = Math_PI / 180.0;  // multiply by this to convert
static const double RAD_TO_DEG = 180.0 / Math_PI;  // multiply by this to convert

// // Simple Frequency Modulation function
// double fm_wave(double x, double carrier_freq, double modulating_freq, double modulation_index)
// {
// 	// FM Wave equation: y = sin(2 * pi * carrier_freq * x + modulation_index * sin(2 * pi * modulating_freq * x))
// 	return sin(2 * Math_PI * carrier_freq * x + modulation_index * sin(2 * Math_PI * modulating_freq * x));
// }

// add my lerp function to global namespace
#ifndef LERP
#define LERP
template <typename T>
T lerp(T a, T b, T alpha) {
    return a + (b - a) * alpha;
}
#endif


// print template, makes print() a global function 
template <typename... Args>
void print(Args... args) {
    UtilityFunctions::print(args...);
}



// OLD PRINT FUNCTIONS

// static void print(String input)  // string (normal)
// {
//     UtilityFunctions::print(input);
// }

// static void print(int input)  // int
// {
//     String s = String::num(input);
//     print(s);
// }
// static void print(float input)  // float
// {
//     String s = String::num(input);
//     print(s);
// }
// static void print(double input)  // double
// {
//     String s = String::num(input);
//     print(s);
// }




// allows getting a node as the type like:
// auto audio_gen2 = get_node_as<NodeType>(node_path);
//
// should return nullptr if no valid node
//

// static void format(String input){
// 	String::format();
// }

// static void format2(String input, Array[String] pars)
// {
// 	Dictionary placeholders;
// 	placeholders["name"] = "Player";
// 	placeholders["score"] = 100;

// 	String formatted_string = String("Hello {name}, your score is {score}!").format(placeholders);

// 	// godot::String s = in
// }

class Helper {
   public:
    // static void print(float input) /// float
    // {
    // 	print(godot::String("{}").format(Array::make(input)));
    // }
    // static void print(double input) /// double
    // {
    // 	print(godot::String("{}").format(Array::make(input)));
    // }

    // static const double PI = 3.141592653589793;
    // const double DEG_TO_RAD = PI / 180.0;
    // const double RAD_TO_DEG = 180.0 / PI;

    // static double trichoidal_wave(double x)
    // {
    // 	double A1 = 1.0, A2 = 0.5, A3 = 0.3;
    // 	double k1 = 1.0, k2 = 2.0, k3 = 3.0;
    // 	return A1 * sin(k1 * x) + A2 * sin(k2 * x) + A3 * sin(k3 * x);
    // }

    // static double trichoidal_wave(double x)
    // {
    // 	// // You can use a sum of sine waves at harmonic frequencies to approximate the trichoidal pattern
    // 	// double A1 = 1.0, A2 = 0.5, A3 = 0.25;

    // 	// double k1 = 2.0 * Math_PI / period, k2 = 4.0 * Math_PI / period, k3 = 6.0 * Math_PI / period;

    // 	// return A1 * sin(k1 * x) + A2 * sin(k2 * x) + A3 * sin(k3 * x);

    // 	x = sin(x);
    // 	bool neg = x < 0.0;
    // 	if (neg)
    // 		x *= -1.0;

    // 	x = x * x;
    // 	if (neg)
    // 		x *= -1.0;

    // 	return x;
    // }
};

#endif
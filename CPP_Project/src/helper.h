#ifndef HELPER_H // header guard
#define HELPER_H

/*
Static Helper Class
*/

#include <godot_cpp/variant/utility_functions.hpp> // godot::UtilityFunctions::print(input);

// for format??? have not checked yet
// #include <godot_cpp/classes/global_constants.hpp>
// #include <godot_cpp/classes/string.hpp>
// #include <godot_cpp/core/class_db.hpp>
// #include <godot_cpp/variant/dictionary.hpp>
// #include <godot_cpp/variant/variant.hpp>



using namespace godot;

// static helper class to remeber print etc

static const double DEG_TO_RAD = Math_PI / 180.0;
static const double RAD_TO_DEG = 180.0 / Math_PI;

// // Simple Frequency Modulation function
// double fm_wave(double x, double carrier_freq, double modulating_freq, double modulation_index)
// {
// 	// FM Wave equation: y = sin(2 * pi * carrier_freq * x + modulation_index * sin(2 * pi * modulating_freq * x))
// 	return sin(2 * Math_PI * carrier_freq * x + modulation_index * sin(2 * Math_PI * modulating_freq * x));
// }

static void print(String input) // string (normal)
{
	godot::UtilityFunctions::print(input);
}

static void print(int input) // int
{
	godot::String s = godot::String::num(input);
	print(s);
}
static void print(float input) // float
{
	godot::String s = godot::String::num(input);
	print(s);
}
static void print(double input) // double
{
	godot::String s = godot::String::num(input);
	print(s);
}

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

class Helper
{
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
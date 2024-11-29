#ifndef WATERSIM_CPP // header guard
#define WATERSIM_CPP

#include <macros.h>
#include <helper.h>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "watersim.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

// string formating
#include <iostream>
#include <format>
// std::format("Hello {}!\n", "world");

// #include <godot_cpp/classes/logger.hpp>

using namespace godot;

WaterSim::WaterSim()
{

	// Initialize any variables here.
	time_passed = 0.0;
	// amplitude = 10.0;

	// array_data = PackedFloat32Array();

	// parameters = Dictionary();

	// test_value = 123.0;

	speed = 1.0;

	frame_count = 0;

	grid_size = Vector2i(32, 32);

	size = Vector2(1.0, 1.0);

	_init_grid_data(); // required to set grids up

	sine_strength = 1.0;
	sine_frequency = 8.0;

	// displacement_grid[4] = 1.0;
	// displacement_grid[7] = 1.0;

	// rng = GDCLASS(RandomNumberGenerator, );
	// rng = RandomNumberGenerator();

	// data = memnew(PackedFloat32Array);
}

WaterSim::~WaterSim()
{
	// Add your cleanup here.
}

// int WaterSim::coor_to_ref(Vector2i coor){

// 	return coor.x + coor.y * coor.x;
// }

void WaterSim::_draw()
{
	// godot::UtilityFunctions::print("foo!");

	// Helper::print("hello!");
	// print("hello1111");

	// draw_circle(Vector2(), 16.0, Color(1, 1, 1), true);

	if (!enabled)
		return; // skip drawing if lock

	for (int y = 0; y < grid_size.y; y++)
	{
		for (int x = 0; x < grid_size.x; x++)
		{

			// float data_value = (*displacement_grid)[coor_to_ref(x, y)];
			float data_value = 0.5;

			int ref = coor_to_ref(x, y);
			if (ref < displacement_grid.size())
			{
				data_value = displacement_grid[coor_to_ref(x, y)];
				data_value *= 0.85; // optional scale
				data_value = data_value / 2.0 + 0.5;

				draw_rect(Rect2(Vector2(x, y), Vector2(1, 1)), Color(data_value, 0, 0), true);
			}
		}
	}
}

void WaterSim::run_simulation(double delta)
{

	// print("loop test 1...");

	print("helelel");

	
	_check_grid_data();

	int displacement_grid_size = displacement_grid.size();
	int velocity_grid_size = velocity_grid.size();

	// print("loop test 2...");
	// if the grids are 0 in size

	// print(godot::String::format("{}", displacement_grid_size));

	// String tt = godot::String::num(displacement_grid_size);

	// tt = godot::String::format("{}", displacement_grid_size);
	// String tt = godot::String::format("{}", displacement_grid_size);

	// // Create a godot::String instance
	// godot::String tt = godot::String("{}").format(Array::make(displacement_grid_size));

	// print(tt);

	String tt2 = godot::String::num(velocity_grid_size);
	// print(tt2);

	if (displacement_grid_size != velocity_grid_size || displacement_grid_size == 0)
		return;

	// print("loop test 3...");
	int expected_size = grid_size.x * grid_size.y;

	if (displacement_grid_size != expected_size)
		return;

	// Helper::print(rng.randf()); // WORKING TEST

	// print("loop test 4...");

	for (int y = 0; y < grid_size.y; y++)
	{
		double y_lerp = float(y) / (grid_size.y - 1.0); // position from 0 to 1

		for (int x = 0; x < grid_size.x; x++)
		{
			int ref = coor_to_ref(x, y);
			Vector2i _position = Vector2i(x, y);

			double x_lerp = float(x) / (grid_size.x - 1.0); // position from 0 to 1

			int n_ref = coor_to_ref(x, y - 1);
			int e_ref = coor_to_ref(x + 1, y);
			int s_ref = coor_to_ref(x, y + 1);
			int w_ref = coor_to_ref(x - 1, y);

			float h = displacement_grid[ref % displacement_grid_size]; // note the modulo protects from errors (slightly dirty)
			float hn = displacement_grid[n_ref % displacement_grid_size];
			float he = displacement_grid[e_ref % displacement_grid_size];
			float hs = displacement_grid[s_ref % displacement_grid_size];
			float hw = displacement_grid[w_ref % displacement_grid_size];

			// (*displacement_grid).get
			// force is ~ to (n+e+s+w)/4 - h

			float f = (hn + he + hs + hw) / 4.0 - h; // calculate our force it is based on our displacement vs the other 4 averaged

			f *= delta; // apply delta
			f *= speed; // apply speed factor

			float v = velocity_grid[ref % velocity_grid_size]; // update our velocity
			v += f;
			// v = MAX(v, -1.0);
			v = CLAMP(v, -1.0, 1.0);

			// velocity_grid[ref % velocity_grid_size] = v; // CRASH 2

			// continue;

			h += v * delta; // apply the actual velocity moving the height

			h = CLAMP(v, 0.0, 1.0);

			// displacement_grid[ref % displacement_grid_size] = h; // CRASH2
			// set_displacement(Vector2i(x,y), h); // CRASH

			// set_displacement(_position, rng.randf());

			// Helper::print(h); // CAUSES CRASHES!!!
			// Helper::print(f); // TEST ME

			
			float sin_pos = x_lerp * sine_frequency * 360.0 * DEG_TO_RAD + time_passed * speed;
			float sin_height = sin(sin_pos);
			// float sin_height = Helper::trichoidal_wave(sin_pos);


			set_displacement(_position, sin_height * sine_strength); // NOISE OKAY

			// set_displacement(_position, h); // THiS CRASHES WITH HH!!!
			// set_displacement(_position, rng.randf()); // NOISE OKAY
			// displacement_grid[ref % displacement_grid_size] = rng.randf(); // NOISE OKAY
		}
	}
}

void WaterSim::_physics_process(double delta)
{
	frame_count++;

	if (!enabled)
		return; // if not enabled don't run further

	run_simulation(delta);
}

void WaterSim::_process(double delta)
{

	time_passed += delta;

	// Vector2 new_position = Vector2(
	// 	amplitude + (amplitude * sin(time_passed * 2.0)),
	// 	amplitude + (amplitude * cos(time_passed * 1.5))
	// );
	// set_position(new_position);

	queue_redraw();
}

// macros from macros.h
CREATE_GETTER_SETTER(WaterSim, bool, enabled)
CREATE_GETTER_SETTER(WaterSim, float, speed)
CREATE_GETTER_SETTER(WaterSim, Vector2i, grid_size)
CREATE_GETTER_SETTER(WaterSim, Vector2, size)
CREATE_GETTER_SETTER(WaterSim, float, sine_strength)
CREATE_GETTER_SETTER(WaterSim, float, sine_frequency)

void WaterSim::_bind_methods()
{
	// macros from macros.h
	CREATE_CLASSDB_BINDINGS(WaterSim, BOOL, enabled)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, speed)
	CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2I, grid_size)
	CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2, size)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_strength)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_frequency)
}

#endif

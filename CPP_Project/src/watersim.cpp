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
				// data_value *= 0.85; // optional scale
				// data_value = data_value / 2.0 + 0.5;

				draw_rect(Rect2(Vector2(x, y), Vector2(1, 1)), Color(data_value, data_value, data_value), true);
			}
		}
	}
}

void WaterSim::run_simulation(double delta)
{

	frame_count += 1;

	grid_size.x = MAX(grid_size.x, 0);
	grid_size.y = MAX(grid_size.y, 0);
	int expected_array_size = grid_size.x * grid_size.y;

	_check_grid_data();

	#define DRIP1
	#ifdef DRIP1
	if (frame_count % 32 == 0){
		
		int rand_ref = rng->randi_range(0, expected_array_size - 1);
		displacement_grid[rand_ref] += drip_volume;
	}
	#endif



	// int displacement_grid_size = displacement_grid.size();

	// int velocity_grid_size = velocity_grid.size();

	// String tt2 = godot::String::num(velocity_grid_size);
	// // print(tt2);

	// if (displacement_grid_size != velocity_grid_size || displacement_grid_size == 0)
	// 	return;

	// int expected_size = grid_size.x * grid_size.y;

	// if (displacement_grid_size != expected_size)
	// 	return;

	for (int y = 0; y < grid_size.y; y++)
	{
		float y_lerp = float(y) / (grid_size.y - 1.0); // position from 0 to 1

		for (int x = 0; x < grid_size.x; x++)
		{
			int ref = coor_to_ref(x, y);
			Vector2i _position = Vector2i(x, y);

			float x_lerp = float(x) / (grid_size.x - 1.0); // position from 0 to 1

			int n_ref = coor_to_ref(x, y - 1); // warning still issues with this function if the values are set too negative!
			int e_ref = coor_to_ref(x + 1, y);
			int s_ref = coor_to_ref(x, y + 1);
			int w_ref = coor_to_ref(x - 1, y);

			float height = displacement_grid[ref]; // note the modulo protects from errors (slightly dirty)

			float velocity = velocity_grid[ref]; // get  our velocity

			float hn = displacement_grid[n_ref];
			float he = displacement_grid[e_ref];
			float hs = displacement_grid[s_ref];
			float hw = displacement_grid[w_ref];

			// force is proportional to surrounding cells
			float force = ((hn + he + hs + hw) / 4.0) - height; // calculate our force it is based on our displacement vs the other 4 averaged
			force *= speed * water_pressure;										// apply speed factor (this scales the force it would be based on the liquids weight i guess)

			// add our force
			velocity += force;

			// adjust our height by the velocity (use the delta)
			height += velocity * delta;

			// apply drag
			velocity = velocity * (1.0 - delta * drag); // lerp to 0 drag

#ifdef OCEAN_WAVES
			// add ocean wave extra, sort of an extra simulation
			float sin_pos = x_lerp * sine_frequency * 360.0 * DEG_TO_RAD + time_passed * speed;
			float sin_height = sin(sin_pos);
// float sin_height = Helper::trichoidal_wave(sin_pos);
#endif		

			# ifdef DRIP2
			if (drip_chance > 0.0){
				if (rng->randf() <= drip_chance){
					height += drip_volume;
				}


			}
			#endif

			velocity_grid[ref] = velocity;	 // save the velocity back
			displacement_grid[ref] = height; // add back

#ifdef DEBUG
			if (y == 0 && x == 0 && frame_count % 16 == 0)
			{ // print less regular
				print("out");
				print(height);
			}
#endif
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
CREATE_GETTER_SETTER(WaterSim, float, water_pressure)

CREATE_GETTER_SETTER(WaterSim, Vector2i, grid_size)
CREATE_GETTER_SETTER(WaterSim, Vector2, size)
CREATE_GETTER_SETTER(WaterSim, float, sine_strength)
CREATE_GETTER_SETTER(WaterSim, float, sine_frequency)

CREATE_GETTER_SETTER(WaterSim, float, drag)
CREATE_GETTER_SETTER(WaterSim, float, drip_chance)
CREATE_GETTER_SETTER(WaterSim, float, drip_volume)





void WaterSim::_bind_methods()
{
	// macros from macros.h
	CREATE_CLASSDB_BINDINGS(WaterSim, BOOL, enabled)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, speed)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, water_pressure)

	CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2I, grid_size)
	CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2, size)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_strength)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_frequency)

	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, drag)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, drip_chance)
	CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, drip_volume)

}

#endif

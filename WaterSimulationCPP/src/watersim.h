#ifndef WATERSIM_H
#define WATERSIM_H

#include <macros.h>
#include <helper.h>
// #include <register_types.h>
// #include <all_includes.h>

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

// #include <gdextension_interface.h> // needed for extension i think

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <godot_cpp/variant/utility_functions.hpp> // godot::UtilityFunctions::print(input);

using namespace godot;

// //
// //

class WaterSim : public Sprite2D
{
	GDCLASS(WaterSim, Sprite2D)

	// WARNING: MACROS (they need corrosponding macros in the cpp file)

	DECLARE_PROPERTY(bool, enabled)
	DECLARE_PROPERTY(float, speed)
	DECLARE_PROPERTY(Vector2i, grid_size)
	DECLARE_PROPERTY(Vector2, size)
	DECLARE_PROPERTY(float, sine_strength)
	DECLARE_PROPERTY(float, sine_frequency)

private:
	double time_passed;

	int frame_count;

	// PackedFloat32Array *data = memnew(PackedFloat32Array); // seems the only way to prevent crash (not having * crashes)

	// displacement
	// velocity

	// FOR SOME REASON I THOUGHT I NEEDED THIS WORKAROUND????
	// PackedFloat32Array* _displacement_grid = memnew(PackedFloat32Array);
	// PackedFloat32Array displacement_grid = (*_displacement_grid);

	PackedFloat32Array displacement_grid = PackedFloat32Array(); // but this works okay? are we supposed to need memnew??

	// PackedFloat32Array* velocity_grid = memnew(PackedFloat32Array);

	PackedFloat32Array velocity_grid = PackedFloat32Array();

	// PackedFloat32Array& data = *_data;
	// WARING, needs to be used as a pointer
	// (*data)
	// data->size();
	// (*data).size();

	bool data_lock = false; // block the data to stop crash

protected:
	static void _bind_methods();

public:
	WaterSim();
	~WaterSim();

	void _process(double delta) override;
	void _draw() override;

	void _physics_process(double delta) override;

	void _check_grid_data()
	{

		int array_size = grid_size.x * grid_size.y;

		bool check = true;
		if (check && displacement_grid.size() != array_size)
			check = false;

		if (check && displacement_grid.size() != array_size)
			check = false;

		if (!check)
		{
			_init_grid_data();
		}
	}

	void _init_grid_data()
	{

		print("init grid data...");

		data_lock = true;

		int array_size = grid_size.x * grid_size.y;

		// (*data).resize(array_size);
		displacement_grid.resize(array_size);
		velocity_grid.resize(array_size);

		for (int i = 0; i < array_size; i++)
		{
			displacement_grid[i] = rng.randf();
		}

		data_lock = false;
	}

	void run_simulation(double delta);

	// int coor_to_ref(Vector2i coor);

	int coor_to_ref(Vector2i coor)
	{
		coor.x %= grid_size.x; // make the grid wrap around
		coor.y %= grid_size.y;
		return coor.x + grid_size.x * coor.y;
	}

	int coor_to_ref(int x, int y)
	{
		// x %= grid_size.x; // make the grid wrap around
		// y %= grid_size.y;
		// return x + grid_size.x * y;
		return coor_to_ref(Vector2i(x, y));
	}

	Vector2i ref_to_coor(int ref)
	{
		return Vector2i(ref % grid_size.x, ref / grid_size.x);
	}

	float get_displacement(Vector2i coor)
	{
		_check_grid_data();

		if (displacement_grid.is_empty())
			return 0.0;

		int ref = coor_to_ref(coor);
		ref %= displacement_grid.size();
		return displacement_grid[ref];
	}

	void set_displacement(Vector2i coor, float displacement)
	{

		if (displacement_grid.is_empty())
			return;

		_check_grid_data();

		int ref = coor_to_ref(coor);
		displacement_grid[ref] = displacement;
	}

	// WARNING THIS ISSUE CAUSES IDE CRASHES
	// this workaround
	// https://github.com/godotengine/godot/issues/95745
	//
	// A plain:
	// RandomNumberGenerator rng = RandomNumberGenerator(); // causes crash!
	//
	RandomNumberGenerator *_rng = memnew(RandomNumberGenerator);
	RandomNumberGenerator rng = (*_rng);

	int test_int()
	{

		// new(RandomNumberGenerator);

		return 123;
	}
};

#endif
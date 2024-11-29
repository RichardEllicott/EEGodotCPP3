/*

18 - How to write a FLIP water / fluid simulation running in your browser 
https://www.youtube.com/watch?v=XmzBREkK8kY&ab_channel=TenMinutePhysics




*/
#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/sprite2d.hpp>
#include <vector>

namespace godot
{

	class WaterSimulation : public Sprite2D
	{
		GDCLASS(WaterSimulation, Sprite2D)

	private:
		double time_passed;
		double amplitude;

		


	protected:
		static void _bind_methods();

	public:
		WaterSimulation();
		~WaterSimulation();

		PackedFloat32Array data;

		void _process(double delta) override;

		void set_amplitude(const double p_amplitude);
		double get_amplitude() const;
	};

}

#endif
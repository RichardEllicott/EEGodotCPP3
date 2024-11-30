#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <macros.h>
#include <helper.h>


// #include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/sprite2d.hpp>


using namespace godot;





class Template : public Sprite2D
{
	GDCLASS(Template, Sprite2D)

    // MACROS from macros.h
	DECLARE_PROPERTY(bool, enabled)
	DECLARE_PROPERTY(float, speed)

private:


protected:
	static void _bind_methods();

public:
	Template();
	~Template();

	void _process(double delta) override;
	void _draw() override;
	void _physics_process(double delta) override;

};

#endif
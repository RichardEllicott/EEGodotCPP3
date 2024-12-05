#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>

using namespace godot;

class Template : public Sprite2D
{
    GDCLASS(Template, Sprite2D)

    // MACROS from macros.h
    DECLARE_PROPERTY(bool, enabled) // we need to also add two more lines to the cpp file per a property we want to @export
    DECLARE_PROPERTY(float, speed)

private:
protected:
    static void _bind_methods();

public:
    Template();
    ~Template();

    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
    void _draw() override;
};

#endif
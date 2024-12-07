#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

class Template : public Sprite2D
{
    GDCLASS(Template, Sprite2D)

    // we need to also add two more lines to the cpp file per a property we want to @export

    DECLARE_PROPERTY(bool, enabled) // variants don't need the Ref<> syntax
    DECLARE_PROPERTY(float, speed)
    DECLARE_PROPERTY(Vector2i, grid_size)

    DECLARE_PROPERTY(Ref<Texture2D>, texture2d) // note the Ref type is correct for this pattern, the ref will delete it's pointer automaticly

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
#ifndef TEMPLATE_CPP
#define TEMPLATE_CPP

#include <template.h>

#include <macros.h>
#include <helper.h>

using namespace godot;

Template::Template()
{
    enabled = false;
    speed = 35.7;

    print("hello from template!");
}

Template::~Template()
{
}

void Template::_process(double delta)
{
}

void Template::_draw()
{
}

void Template::_physics_process(double delta)
{
}

void Template::_ready()
{
    emit_signal("test_signal", this, Vector2(3, 7));
}

// macros from macros.h
CREATE_GETTER_SETTER(Template, bool, enabled)
CREATE_GETTER_SETTER(Template, float, speed)
CREATE_GETTER_SETTER(Template, Vector2i, grid_size)

CREATE_GETTER_SETTER(Template, Ref<Texture2D>, texture2d)

void Template::_bind_methods()
{
    // macros from macros.h
    CREATE_CLASSDB_BINDINGS(Template, BOOL, enabled)
    // CREATE_CLASSDB_BINDINGS2(Template, "bool", enabled) // tested alt macro? CRASHED!!!
    CREATE_CLASSDB_BINDINGS(Template, FLOAT, speed)
    CREATE_CLASSDB_BINDINGS(Template, VECTOR2I, grid_size)

    CREATE_CLASSDB_BINDINGS2(Template, "Texture2D", texture2d)

    ADD_SIGNAL(MethodInfo("test_signal", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos"))); // signal still testing
}

#endif
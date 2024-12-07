/*

single file template!

WARNING: using header files is a good idea!

however this template using macros can allow declaring @export's in one file

so it's a good idea for prototyping quickly though, you could port the code to a neater form later

C++ is very easy if you just start typing it!

*/
#ifndef TEMPLATE_SFILE_H
#define TEMPLATE_SFILE_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;

class TemplateSFile : public Sprite2D
{
    GDCLASS(TemplateSFile, Sprite2D)

    // // these macros create the variable and also get/set functions
    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    // DECLARE_PROPERTY_SINGLE_FILE(float, speed)
    // DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

    // DECLARE_PROPERTY_SINGLE_FILE(Ref<Texture2D>, texture2d)
    // DECLARE_PROPERTY_SINGLE_FILE(Ref<RandomNumberGenerator>, rng)

private:
    // declare normals vars
    int test_int = 123;
    String test_string = "test_string";
    // but declare Godot objects with a Ref<>
    Ref<RandomNumberGenerator> rng2; // make sure call instantiate() in the constructor!

protected:
    static void _bind_methods()
    {
        // // these macros create the bindings for the properties
        CREATE_CLASSDB_BINDINGS(TemplateSFile, BOOL, enabled);        // just crashes with signal or not
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, Variant::FLOAT, speed) // note Variant::FLOAT is also valid
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, VECTOR2I, grid_size)   // note Variant::FLOAT is also valid

        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "Texture2D", texture2d)
        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "RandomNumberGenerator", rng)

        // // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
        // ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
    }

public:
    TemplateSFile()
    {
        print("hello from single file template!");

        rng2.instantiate(); // ensure the ref is created, or linked to something

        // rng->randomize(); // the use a ref, use the pointer syntax

        // // if you do not set these the properties will not have a default value!
        // enabled = false;
        // speed = 123.0;
        // grid_size = Vector2i(64, 64);
    };
    ~TemplateSFile()
    {
        print("bye bye from single file template!");
    };

    void _ready() override
    {
        print("ready from single file template!");
    };
    void _process(double delta) override
    {
        queue_redraw();
    };
    void _physics_process(double delta) override {

    };
    void _draw() override
    {

        // for (int y = 0; y < grid_size.y; y++)
        // {

        //     float y_lerp = y / (grid_size.y - 1.0); // the -1.0 makes a cast (could use float())

        //     for (int x = 0; x < grid_size.x; x++)
        //     {
        //         float x_lerp = x / (grid_size.x - 1.0);

        //         Color color = Color(x_lerp, 0.5, y_lerp); // x/y normal

        //         // Color color = Color::from_hsv(fmod(data_value, 1.0), 1.0, 1.0);

        //         draw_rect(Rect2(Vector2(x, y), Vector2(1, 1)), color, true);
        //     }
        // }
    };
};

#endif
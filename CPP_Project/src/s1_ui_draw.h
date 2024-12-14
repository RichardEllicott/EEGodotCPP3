/*

single file template!

WARNING: using header files is a good idea! while this file is named as a header file (for compiler consistency) it is a plain script file

however this template using macros can allow declaring @export's in one file

so it's a good idea for prototyping quickly though, you could port the code to a neater form later

C++ is very easy if you just start typing it!

*/
#ifndef S1_UI_DRAW_H
#define S1_UI_DRAW_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

// #include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

class S1_UI_Draw : public Control {
    GDCLASS(S1_UI_Draw, Control)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, enabled, false)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2i, grid_size, Vector2i(16, 16))
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2, cell_size, Vector2(8, 8))

    DECLARE_PROPERTY_SINGLE_FILE(Ref<Texture2D>, texture2d)

   private:
    RID canvas_rid;

   public:
    // subroutine example (can be called from GDScript)
    void macro_test() {
        UtilityFunctions::print("macro_test from c++!!!");
    }

    // function with input example (can be called from GDScript)
    String macro_test2(const String &input, int number) {  // a String doesn't need to be a %reference

        return input + String::num(number);
    }

   protected:
    static void _bind_methods() {
        // // these macros create the bindings for the properties
        CREATE_VAR_BINDINGS(S1_UI_Draw, BOOL, enabled);  // just crashes with signal or not
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, Variant::FLOAT, speed) // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1_UI_Draw, VECTOR2I, grid_size)  // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1_UI_Draw, VECTOR2, cell_size)   // note Variant::FLOAT is also valid

        CREATE_CLASS_BINDINGS(S1_UI_Draw, "Texture2D", texture2d)  // maybe the texture causes crashing?????? (i have ide crashes)
        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "RandomNumberGenerator", rng)

        // // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
        // ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));

        // manual method binding (so the methods can be called from GDScript or other languages)
        ClassDB::bind_method(D_METHOD("macro_test"), &S1_UI_Draw::macro_test);
        ClassDB::bind_method(D_METHOD("macro_test2", "input", "number"), &S1_UI_Draw::macro_test2);  // method bind with pars
    }

   public:
    // call only once
    void _create_canvas() {
        auto rs = RenderingServer::get_singleton();
        canvas_rid = rs->canvas_item_create();  // create a canvas

        rs->canvas_item_set_parent(canvas_rid, get_canvas_item());
    }

    // call on exit
    void _delete_canvas() {
        auto rs = RenderingServer::get_singleton();
        rs->canvas_item_clear(canvas_rid);
    }

    void draw_grid() {
        auto rs = RenderingServer::get_singleton();

        for (int y = 0; y < grid_size.y; y++) {
            // float y_lerp = y / (grid_size.y - 1.0); // the -1.0 makes a cast (could use float())

            for (int x = 0; x < grid_size.x; x++) {
                // float x_lerp = x / (grid_size.x - 1.0);

                auto position = Vector2(x, y) * cell_size;

                auto rect = Rect2(position, cell_size);

                auto color = Color(1, 0, 0);

                rs->canvas_item_add_rect(canvas_rid, rect, color, false);
            }
        }
    }

    // void draw_rect(Rect2 rect, Color color = Color(1, 1, 0))
    // {
    //     auto rs = RenderingServer::get_singleton();
    //     rs->canvas_item_add_rect(canvas_rid, rect, color, true);
    // }

    // void draw_circle(Vector2 position, float radius, Color color)
    // {
    //     auto rs = RenderingServer::get_singleton();
    //     rs->canvas_item_add_circle(canvas_rid, position, radius, color, true);
    // }

    S1_UI_Draw() {
        print("S1_UI_Draw()...");

        _create_canvas();

        auto rs = RenderingServer::get_singleton();

        // rng2.instantiate(); // ensure the ref is created, or linked to something (note the . as we access the value type Ref<>)

        // if (rng2.is_valid())   // we don't need to check here after instantiate, but this is the memory safe pattern
        //     rng2->randomize(); // the use a ref, use the pointer syntax (warning this would crash if the rng2 didn't exist, sometimes you should check)

        auto rect = Rect2(Vector2(0, 0), Vector2(32, 32));

        auto color = Color(1, 0, 1);

        // RenderingServer::get_singleton()->canvas_item_add_texture_rect(canvas_rid, rect, texture2d, false, color,false);

        rs->canvas_item_add_rect(canvas_rid, rect, color, true);

        rs->canvas_item_add_circle(canvas_rid, Vector2(), 16.0, Color(1, 0.5, 0), true);

        // RenderingServer.canvas_item_add_circle(ci_rid, Vector2.ZERO, 16.0, Color.AQUAMARINE, true) # works but not rect

        // RenderingServer.canvas_item_add_rect(ci_rid, Rect2(Vector2(), Vector2(32, 16) ) , Color.DARK_RED, true   )

        // draw_grid();
    };
    ~S1_UI_Draw() {
        print("~S1_UI_Draw()...");
        _delete_canvas();
    };

    void _ready() override {
    };
    void _process(double delta) override {
        queue_redraw();
    };
    void _physics_process(double delta) override {

    };
    void _draw() override {

        // // draw a grid of colors as a visual demo
        // for (int y = 0; y < grid_size.y; y++)
        // {
        //     float y_lerp = y / (grid_size.y - 1.0); // the -1.0 makes a cast (could use float())

        //     for (int x = 0; x < grid_size.x; x++)
        //     {
        //         float x_lerp = x / (grid_size.x - 1.0);

        //         Color color = Color(x_lerp, 0.5, y_lerp); // x/y normal
        //         //         // Color color = Color::from_hsv(fmod(data_value, 1.0), 1.0, 1.0);
        //         draw_rect(Rect2(Vector2(x, y), Vector2(1, 1)), color, true, 0.0, true);

        //         // draw_texture()
        //     }
        // }
    };
};

#endif
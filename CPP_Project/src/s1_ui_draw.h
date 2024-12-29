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
// #include <s1_audio_generator.h>
#include <s1_audio/poly_synth.h>




#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

class S1_UI_Draw : public Control {
    GDCLASS(S1_UI_Draw, Control)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, enabled, false)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2i, grid_size, Vector2i(16, 16))
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2, cell_size, Vector2(8, 8))

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, cell_border, 1.0f)

    DECLARE_PROPERTY_SINGLE_FILE(Ref<Texture2D>, texture2d)

    DECLARE_PROPERTY_SINGLE_FILE(NodePath, s1_audio_generator)  // we are not sure how to link this as an export

   private:
    RID canvas_rid;

    const std::vector<int> key_pattern = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};

    const std::vector<String> note_pattern = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

   public:
    // allows getting a node as the type like:
    // auto audio_gen2 = get_node_as<NodeType>(node_path);
    // // return nullptr if no node
    template <typename T>
    T* get_node_as(const NodePath& path) {
        Node* node_ptr = get_node_or_null(path);
        return Object::cast_to<T>(node_ptr);
    }

    // S1AudioGenerator* _s1_audio_generator = nullptr;

    auto get_s1_audio_generator_ptr() {
        auto node_ptr = get_node_or_null(s1_audio_generator);
        auto ret_ptr = Object::cast_to<PolySynth>(node_ptr);
        return ret_ptr;
    }

    // subroutine example (can be called from GDScript)
    void macro_test() {
        UtilityFunctions::print("macro_test from c++!!!");


        auto audio_gen2 = get_node_as<PolySynth>(s1_audio_generator);
        // auto audio_gen2 = get_node_as<Ref<S1AudioGenerator>>(s1_audio_generator); // doesn't work

        // // Check if the pointer is not null
        if (audio_gen2) {
            print("test cast sucess!!");

            //     print("test ref cast sucess!!");
        }

        // Ref<S1AudioGenerator> = test;
    }

    // function with input example (can be called from GDScript)
    String macro_test2(const String& input, int number) {  // a String doesn't need to be a %reference

        return input + String::num(number);
    }

   protected:
    static void _bind_methods() {
        // // these macros create the bindings for the properties
        CREATE_VAR_BINDINGS(S1_UI_Draw, BOOL, enabled);  // just crashes with signal or not
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, Variant::FLOAT, speed) // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1_UI_Draw, VECTOR2I, grid_size)  // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1_UI_Draw, VECTOR2, cell_size)   // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1_UI_Draw, FLOAT, cell_border)   // note Variant::FLOAT is also valid

        CREATE_CLASS_BINDINGS(S1_UI_Draw, "Texture2D", texture2d)  // maybe the texture causes crashing?????? (i have ide crashes)

        CREATE_VAR_BINDINGS(S1_UI_Draw, NODE_PATH, s1_audio_generator)

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

    void draw_grid_cell(Vector2i position) {
        auto rs = RenderingServer::get_singleton();

        float y_lerp = position.y / (grid_size.y - 1.0);  // the -1.0 makes a cast (could use float())
        float x_lerp = position.x / (grid_size.x - 1.0);

        auto position2 = Vector2(position) * cell_size;

        auto rect = Rect2(position2, cell_size);

        rect.position += Vector2(cell_border, cell_border);
        rect.size -= Vector2(cell_border, cell_border) * 2.0f;

        auto color = Color(x_lerp, 0, y_lerp);

        rs->canvas_item_add_rect(canvas_rid, rect, color, false);
    }

    void draw_grid() {
        auto rs = RenderingServer::get_singleton();

        for (int y = 0; y < grid_size.y; y++) {
            //

            for (int x = 0; x < grid_size.x; x++) {
                //

                draw_grid_cell(Vector2i(x, y));
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
        // print("S1_UI_Draw()...");

        _create_canvas();

        draw_grid();
    };
    ~S1_UI_Draw() {
        // print("~S1_UI_Draw()...");
        _delete_canvas();
    };

    void _ready() override {
        queue_redraw();
    };
    void _process(double delta) override {
        queue_redraw();

        // draw_grid();
    };
    void _physics_process(double delta) override {

    };
    void _draw() override {

    };
};

#endif
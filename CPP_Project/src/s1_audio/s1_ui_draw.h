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

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/method_bind.hpp>

using namespace godot;

class S1_UI_Draw : public Control {
    GDCLASS(S1_UI_Draw, Control)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, enabled, false)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2i, grid_size, Vector2i(16, 16))
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2, cell_size, Vector2(8, 8))

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, cell_border, 1.0f)

    DECLARE_PROPERTY_SINGLE_FILE(Ref<ImageTexture>, image_texture)

    DECLARE_PROPERTY_SINGLE_FILE(NodePath, poly_synth)  // we are not sure how to link this as an export

    // wave draw
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, draw_wave, true)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, draw_wave_frames, 1024)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2, draw_wave_size, Vector2(1, 1))

   private:
    RID canvas_rid;

    const std::vector<int> key_pattern = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};

    const std::vector<String> note_pattern = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

   public:
    Ref<Image> image;  // Ref to store the image

    int texture_width = 512;   // Width of the texture
    int texture_height = 128;  // Height of the texture

    // i tried using refs, but the pointer worked better

    // REF cannot be used!!
    // Ref<PolySynth> poly_synth_ref; // breaks

    // void _update_poly_synth_ref() {
    //     if (!poly_synth_ref.is_valid()) {
    //         auto _poly_synth = get_node_as<PolySynth>(this, poly_synth);
    //         if (_poly_synth)
    //             poly_synth_ref = Ref<Node>(_poly_synth);
    //     }
    // }

    // subroutine example (can be called from GDScript)
    void macro_test() {
        print("macro_test from c++!!!");

        // ref doesn't work!
        // _update_poly_synth_ref();
        // if (poly_synth_ref.is_valid()) {
        //     print("valid ref!!!");
        //     print("poly_synth_ref->get_attack() ", poly_synth_ref->get_attack());
        // }

        auto _poly_synth = get_node_as<PolySynth>(this, poly_synth);
        if (_poly_synth) {
            print("_poly_synth->get_attack() ", _poly_synth->get_attack());
        }

        print("hello", 123);

        print("test image gen:");

        // Create an image object with a width and height
        image.instantiate();                                 // Correctly initialize the Image
        image->create(256, 256, false, Image::FORMAT_RGB8);  // Create a 256x256 image

        // Fill the image with a solid color (e.g., red)
        image->fill(Color(1, 0, 0));  // RGB color (1,0,0) = red

        // Create the texture from the image
        // texture2d.instantiate();  // Initialize the ImageTexture
        // texture2d->

        image_texture = ImageTexture::create_from_image(image);
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

        CREATE_CLASS_BINDINGS(S1_UI_Draw, "ImageTexture", image_texture)  // maybe the texture causes crashing?????? (i have ide crashes)

        CREATE_VAR_BINDINGS(S1_UI_Draw, NODE_PATH, poly_synth)

        // wave draw
        CREATE_VAR_BINDINGS(S1_UI_Draw, BOOL, draw_wave)
        CREATE_VAR_BINDINGS(S1_UI_Draw, INT, draw_wave_frames)
        CREATE_VAR_BINDINGS(S1_UI_Draw, VECTOR2, draw_wave_size)

        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "RandomNumberGenerator", rng)

        // // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
        // ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));

        // manual method binding (so the methods can be called from GDScript or other languages)
        ClassDB::bind_method(D_METHOD("macro_test"), &S1_UI_Draw::macro_test);
        ClassDB::bind_method(D_METHOD("macro_test2", "input", "number"), &S1_UI_Draw::macro_test2);  // method bind with pars
    }

   public:
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

    void _draw_waveform() {
        if (!draw_wave) return;

        auto rs = RenderingServer::get_singleton();

        rs->canvas_item_clear(canvas_rid);  // clear the existing image

        auto _poly_synth = get_node_as<PolySynth>(this, poly_synth);
        if (_poly_synth) {
            // print("poly_synth_ref->get_attack() ", poly_synth_ref->get_attack());

            // print("HOOK2!!");

            for (int x = 0; x < draw_wave_frames; x++) {
                Vector2 val1 = _poly_synth->history_buffer.get(-x);
                // Vector2 val2 = _poly_synth->history_buffer.get(-x + 1);

                Vector2 pos1 = Vector2(x, val1.x);
                pos1 *= draw_wave_size;

                // Vector2 pos2 = Vector2(x + 1, val2.x);

                // rs->canvas_item_add_line(canvas_rid, pos1, pos2, Color(1, 0, 0), 1.0);

                rs->canvas_item_add_circle(canvas_rid, pos1, 1.0, Color(1, 0, 0), false);
            }

            // _poly_synth->history_buffer.get
        }
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

   private:
    // call only once
    void _create_canvas() {
        auto rs = RenderingServer::get_singleton();
        canvas_rid = rs->canvas_item_create();  // create a canvas
        rs->canvas_item_set_parent(canvas_rid, get_canvas_item());
    }

    // call on exit
    void _delete_canvas() {
        auto rs = RenderingServer::get_singleton();
        rs->free_rid(canvas_rid);  // the propper render server delete!
    }

   public:
    S1_UI_Draw() {
        _create_canvas();

        // draw_grid();
    };
    ~S1_UI_Draw() {
        // print("~S1_UI_Draw()...");
        _delete_canvas();
    };

    void _ready() override {
        queue_redraw();

        print("us _ready_ready_ready");
    };
    void _process(double delta) override {
        if (image_texture.is_valid()) {
            // texture2d.
        }

        _draw_waveform();  // doesn't need a redraw i don't think?
    };
    void _physics_process(double delta) override {

    };

    void _draw() override {

    };
};

#endif
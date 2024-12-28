/*

single file audio generator


setup to take a child... kept before refactor


*/
#ifndef S1_AUDIO_GENERATOR_H
#define S1_AUDIO_GENERATOR_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

#include <godot_cpp/classes/audio_stream_player.hpp>             // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_playback.hpp>           // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_generator.hpp>          // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp> // AudioStreamGeneratorPlayback
// #include <godot_cpp/classes/audio_stream.hpp> // AudioStream

using namespace godot;

class S1AudioGenerator : public AudioStreamPlayer
{
    GDCLASS(S1AudioGenerator, AudioStreamPlayer)

    // // these macros create the variable and also get/set functions
    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled) // WARNING  ... i worry about default values... maybe we should make the macro do this

    DECLARE_PROPERTY_SINGLE_FILE(float, pulse_hz)
    DECLARE_PROPERTY_SINGLE_FILE(float, sample_hz)

    // DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

    // DECLARE_PROPERTY_SINGLE_FILE(Ref<Texture2D>, texture2d)
    // DECLARE_PROPERTY_SINGLE_FILE(Ref<RandomNumberGenerator>, rng)

private:
    // declare normals vars
    int counter = 0;
    String test_string = "test_string";
    // but declare Godot objects with a Ref<>
    Ref<RandomNumberGenerator> rng2; // make sure call instantiate() in the constructor!

protected:
    static void _bind_methods()
    {
        // // these macros create the bindings for the properties
        CREATE_VAR_BINDINGS(S1AudioGenerator, BOOL, enabled); // just crashes with signal or not

        CREATE_VAR_BINDINGS(S1AudioGenerator, Variant::FLOAT, sample_hz) // note Variant::FLOAT is also valid
        CREATE_VAR_BINDINGS(S1AudioGenerator, Variant::FLOAT, pulse_hz)  // note Variant::FLOAT is also valid

        // CREATE_VAR_BINDINGS(S1AudioGenerator, VECTOR2I, grid_size) // note Variant::FLOAT is also valid

        // CREATE_CLASS_BINDINGS(S1AudioGenerator, "Texture2D", texture2d) // maybe the texture causes crashing?????? (i have ide crashes)
        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "RandomNumberGenerator", rng)

        // // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
        // ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
    }

public:
    S1AudioGenerator()
    {

        sample_hz = 44100;
        pulse_hz = 220;

        // print("hello from single file template!");

        rng2.instantiate(); // ensure the ref is created, or linked to something (note the . as we access the value type Ref<>)

        if (rng2.is_valid())   // we don't need to check here after instantiate, but this is the memory safe pattern
            rng2->randomize(); // the use a ref, use the pointer syntax (warning this would crash if the rng2 didn't exist, sometimes you should check)

        // // if you do not set these the properties will not have a default value!
        // enabled = false;
        // speed = 123.0;
        // grid_size = Vector2i(64, 64);
    };
    ~S1AudioGenerator() {
    };

    void _ready() override {
    };

    void _process(double delta) override
    {

        int print_cycle = 60;

        counter++;

        AudioStreamPlayer *audio_stream_player = get_node<AudioStreamPlayer>("AudioStreamPlayer");
        // Ref<AudioStreamPlayer> audio_stream_player = get_node<AudioStreamPlayer>("AudioStreamPlayer");

        if (audio_stream_player != nullptr)
        {
            if (counter % print_cycle == 0)
                print("test2");

            if (audio_stream_player->is_playing()) // make sure playing before getting playback
            {
                Ref<AudioStreamPlayback> playback = audio_stream_player->get_stream_playback();

                if (playback.is_valid())
                {
                    Ref<AudioStreamGeneratorPlayback> generator_playback = playback; // attempt cast with ref system

                    if (generator_playback.is_valid()) // if valid
                    {
                        // return generator_playback; // finally return the ref

                        if (counter % print_cycle == 0)
                            print("generator_playback.is_valid()");
                    }
                }
            }
        }
    };
    void _physics_process(double delta) override {

    };
};

#endif
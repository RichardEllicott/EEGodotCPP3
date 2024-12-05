/*

WARNING... probabally to delete

this was an experiment with all the code in the header file!

*/


#ifndef AUDIO_GEN_H
#define AUDIO_GEN_H

#include <macros.h>
#include <helper.h>

#include <godot_cpp/classes/audio_stream_generator.hpp> // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_player.hpp>    // AudioStreamPlayer

#include <godot_cpp/classes/audio_stream_generator_playback.hpp> // AudioStreamGeneratorPlayback
#include <godot_cpp/classes/audio_stream_playback.hpp>           // AudioStreamPlayback

// trying to make Godot recognise me!
// #include <godot_cpp/core/defs.hpp>
// #include <godot_cpp/godot.hpp>

// #include <godot_cpp/classes/sprite2d.hpp>
// #include <godot_cpp/classes/random_number_generator.hpp>
// // #include <gdextension_interface.h> // needed for extension i think
// #include <godot_cpp/variant/utility_functions.hpp> // godot::UtilityFunctions::print(input);

// #include <godot_cpp/classes/node.hpp>

// #include <godot_cpp/classes/node2d.hpp>
// #include <godot_cpp/classes/sprite2d.hpp>

// #include <godot_cpp/core/class_db.hpp> // GDCLASS

// #include <godot_cpp/classes/audio_stream_generator_playback.hpp>
// #include <godot_cpp/classes/audio_server.hpp>
// #include <godot_cpp/classes/node.hpp>
// #include <godot_cpp/variant/utility_functions.hpp>
// #include <cmath>

using namespace godot;

class AudioGen : public Node
{

    GDCLASS(AudioGen, Node)

    // DECLARE_PROPERTY(bool, enabled)

private:
    bool enabled;

    Ref<AudioStreamGeneratorPlayback> playback_ref;

    float phase = 0.0;

public:
    void set_enabled(const bool p_enabled) { enabled = p_enabled; };
    bool get_enabled() const { return enabled; };

    DECLARE_PROPERTY(AudioStreamPlayer, player)

    DECLARE_PROPERTY(AudioStreamPlayback, playback)

    DECLARE_PROPERTY(float, sample_hz)
    DECLARE_PROPERTY(float, pulse_hz)

    // header file macro, to declare a property

    AudioGen()
    {

        pulse_hz = 440.0;
    }

    ~AudioGen()
    {
    }

    void _ready() override
    {
        player.play();
        // playback = player.get_stream_playback();

        // Ref<AudioStreamPlayback> playback = player.get_stream_playback();

        playback_ref = player.get_stream_playback();

        if (playback_ref.is_valid())
        {
        }
    }

    void fill_buffer()
    {
        if (playback_ref.is_valid())
        {
            float increment = pulse_hz / sample_hz;
            int frames_available = playback_ref->get_frames_available();

            for (int i = 0; i < frames_available; i++)
            {

                playback_ref->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
                phase = fmod(phase + increment, 1.0);
            }
        }
    }

protected:
    static void _bind_methods()
    {
        // macros from macros.h
        // CREATE_CLASSDB_BINDINGS(WaterSim, BOOL, enabled)
        // CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, speed)
        // CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2I, grid_size)
        // CREATE_CLASSDB_BINDINGS(WaterSim, VECTOR2, size)
        // CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_strength)
        // CREATE_CLASSDB_BINDINGS(WaterSim, FLOAT, sine_frequency)

        // CREATE_CLASSDB_BINDINGS(AudioGen, BOOL, enabled)
    }
};

#endif
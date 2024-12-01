#ifndef SOUND_GENERATOR_H
#define SOUND_GENERATOR_H

/*

WARNING NOTE


i need to make sure i save this as sin_example!!!


*/

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>

// #include <godot_cpp/classes/audio_stream_generator.hpp> // AudioStreamGenerator

#include <godot_cpp/classes/audio_stream_player.hpp>             // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_playback.hpp>           // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_generator_playback.hpp> // AudioStreamGeneratorPlayback

using namespace godot;

class SoundGenerator : public Sprite2D
{
    GDCLASS(SoundGenerator, Sprite2D)

    // MACROS from macros.h
    DECLARE_PROPERTY(bool, enabled) // we need to also add two more lines to the cpp file per a property we want to @export
    DECLARE_PROPERTY(float, speed)

    // DECLARE_PROPERTY(Ref<AudioStreamPlayer>, player)

    // DECLARE_PROPERTY(AudioStreamPlayback, playback)

    DECLARE_PROPERTY(float, sample_hz)
    DECLARE_PROPERTY(float, pulse_hz)

private:
    double timer = 0.0;
    double delay = 0.125;

    double phase = 0.0;


    AudioStreamPlayer *audio_player_ptr;
    AudioStreamPlayer *get_audio_player_ptr();

    Ref<AudioStreamGeneratorPlayback> get_audio_generator_playback_ref();
    AudioStreamGeneratorPlayback* get_audio_generator_playback_ptr();



protected:
    static void _bind_methods();

public:
    SoundGenerator();
    ~SoundGenerator();

    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
    void _draw() override;

    void fill_buffer();

    


    // Ref<AudioStreamGeneratorPlayback> audio_generator_playback_ref;



    // AudioStreamPlayer* player;
    // AudioStreamPlayback* playback;

    // Ref<AudioStreamGeneratorPlayback> playback_ref;
};

#endif
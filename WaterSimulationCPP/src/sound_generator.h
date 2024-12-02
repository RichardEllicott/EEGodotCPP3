#ifndef SOUND_GENERATOR_H
#define SOUND_GENERATOR_H

/*

simple sine generator example


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
    DECLARE_PROPERTY(int, buffer_size)
    DECLARE_PROPERTY(float, sample_hz)
    DECLARE_PROPERTY(float, pulse_hz)


private:

    // i am not 100% sure but it seems wise to update the buffer at a slower rate? i get crashes sometimes
    // double timer = 0.0;
    // double timer_delay = 1.0 / 16.0;

    double phase = 0.0;


    // these functions assist in finding the AudioStreamPlayer child and it's AudioStreamGeneratorPlayback
    // we need to interact with these using pointers and quite a few scenarios just crash!
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

    

};

#endif
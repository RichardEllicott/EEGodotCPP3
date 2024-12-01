#ifndef SOUND_GENERATOR_CPP
#define SOUND_GENERATOR_CPP

#include <sound_generator.h>

#include <macros.h>
#include <helper.h>

#include <godot_cpp/classes/audio_stream_player.hpp>             // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_playback.hpp>           // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_generator.hpp>          // AudioStreamGeneratorPlayback
#include <godot_cpp/classes/audio_stream_generator_playback.hpp> // AudioStreamGeneratorPlayback

using namespace godot;

SoundGenerator::SoundGenerator()
{
    enabled = false;
    speed = 35.7;

    sample_hz = 44100;
}

SoundGenerator::~SoundGenerator()
{
}

void SoundGenerator::_draw()
{
}

void SoundGenerator::_physics_process(double delta)
{
}

void SoundGenerator::_process(double delta)
{
    timer += delta;
    if (timer >= delay)
    {
        timer -= delay;
        fill_buffer();
    }
}

void SoundGenerator::fill_buffer()
{

    AudioStreamPlayer *audio_player = get_node<AudioStreamPlayer>("AudioStreamPlayer");

    if (!audio_player->is_playing()) // not playing, do not call get_stream_playback
        return;

    Ref<AudioStreamPlayback> playback = audio_player->get_stream_playback(); // use ptr()??

    if (playback.is_valid())
    {

        Ref<AudioStreamGeneratorPlayback> generator_playback = playback;

        if (generator_playback.is_valid() && !generator_playback.is_null() && generator_playback != nullptr)
        {
            print("Successfully casted to AudioStreamGeneratorPlayback.");

            float increment = pulse_hz / sample_hz;

            // int frames_available2 = generator_playback->get_frames_available();


            AudioStreamGeneratorPlayback *playback_ptr = generator_playback.ptr(); // for some reason we need the pointer not the ref (which crashes)
            if (playback_ptr)
            {
                // Now you can access the plain object
                int frames_available = playback_ptr->get_frames_available();


                for (int i = 0; i < frames_available; i++){
                    playback_ptr->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
                    phase = fmod(phase + increment, 1.0);

                }

                print(frames_available);
            }
            else
            {
                print("Invalid playback reference!");
            }
        }
    };

    // AudioStreamPlayback* playback_ptr = playback_ref.ptr();

    // AudioStreamPlayback _final = *playback_ptr;

    // int frames_available = playback_ref.ptr()->get_frames_available();
}

void SoundGenerator::_ready()
{
    Node *child = get_node_or_null("AudioStreamPlayer");

    if (child == nullptr)
    {
        UtilityFunctions::print("Child not found!");

        return;
    }
    else
    {
        AudioStreamPlayer *_player = Object::cast_to<AudioStreamPlayer>(child);
        if (_player != nullptr)
        {
            UtilityFunctions::print("Found a AudioStreamPlayer node!");
            player = _player;

            playback_ref = _player->get_stream_playback();
        }
        else
        {
            UtilityFunctions::print("Child is not a AudioStreamPlayer node.");
        }
    }
}

// macros from macros.h
CREATE_GETTER_SETTER(SoundGenerator, bool, enabled)
CREATE_GETTER_SETTER(SoundGenerator, float, speed)

CREATE_GETTER_SETTER(SoundGenerator, float, sample_hz)
CREATE_GETTER_SETTER(SoundGenerator, float, pulse_hz)

// CREATE_GETTER_SETTER(SoundGenerator, AudioStreamPlayer, player)
// CREATE_GETTER_SETTER(SoundGenerator, AudioStreamPlayback, playback)

void SoundGenerator::_bind_methods()
{
    // macros from macros.h
    CREATE_CLASSDB_BINDINGS(SoundGenerator, BOOL, enabled)
    CREATE_CLASSDB_BINDINGS(SoundGenerator, FLOAT, speed)

    CREATE_CLASSDB_BINDINGS(SoundGenerator, FLOAT, sample_hz)
    CREATE_CLASSDB_BINDINGS(SoundGenerator, FLOAT, pulse_hz)
}

#endif
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

    sample_hz = 44100.0;
    pulse_hz = 220.0;
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

AudioStreamPlayer *SoundGenerator::get_audio_player_ptr()
{
    if (audio_player_ptr == nullptr) // still testing pattern (maybe unstable)
        audio_player_ptr = get_node<AudioStreamPlayer>("AudioStreamPlayer");

    return audio_player_ptr;
}

// REFACTOR
Ref<AudioStreamGeneratorPlayback> SoundGenerator::get_audio_generator_playback_ref()
{
    // we seem to need to do this casting in steps
    Ref<AudioStreamPlayback> playback = get_audio_player_ptr()->get_stream_playback(); // needs to return the plain AudioStreamPlayback
    if (playback.is_valid())                                                           // if valid try to cast
    {
        Ref<AudioStreamGeneratorPlayback> generator_playback = playback; // casting occurs here

        if (generator_playback.is_valid()) // if valid
        {
            return generator_playback; // finally return the ref
        }
    }

    // // probabally no!?! we will check
    // Ref<AudioStreamGeneratorPlayback> playback = get_audio_player_ptr()->get_stream_playback();
    // if (playback.is_valid())
    // {
    //     return playback; // return valid ref
    // }

    return nullptr;
}

AudioStreamGeneratorPlayback *SoundGenerator::get_audio_generator_playback_ptr()
{
    Ref<AudioStreamGeneratorPlayback> ref = get_audio_generator_playback_ref();
    if (ref.is_valid())
    {
        return ref.ptr(); // return valid pointer
    }
    return nullptr;
}

void SoundGenerator::fill_buffer()
{
    // CHECKS, prevents crashes!
    if (get_audio_player_ptr() == nullptr) // check the AudioStreamPlayer
    {
        print("AudioStreamPlayer not found!");
        return;
    }

    if (!audio_player_ptr->is_playing()) // not playing, do not call get_stream_playback (which gives a warning)
        return;

    AudioStreamGeneratorPlayback *playback_ptr = get_audio_generator_playback_ptr(); // we need a pointer to the playback generator

    if (playback_ptr && playback_ptr != NULL && playback_ptr != nullptr) // THESE EXTRA CHECKS REQUIRED!!! lol
    {
        // print("AudioStreamGeneratorPlayback not found!");

        float increment = pulse_hz / sample_hz;

        int frames_available = playback_ptr->get_frames_available();

        for (int i = 0; i < frames_available; i++)
        {
            playback_ptr->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
            phase = fmod(phase + increment, 1.0);
        }

        print(frames_available);
    }
}

void SoundGenerator::_ready()
{
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
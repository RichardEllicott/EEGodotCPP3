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
    buffer_size = 1024;

    sample_hz = 44100.0;
    pulse_hz = 220.0;

    // timer_delay = 1.0 / 16.0;
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
    if (enabled)
    {
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

    if (get_audio_player_ptr() != nullptr) // will be null if no AudioStreamPlayer
    {

        if (audio_player_ptr->is_playing()) // check if playing
        {

            AudioStreamGeneratorPlayback *playback_ptr = get_audio_generator_playback_ptr(); // we need a pointer to the playback generator

            // WARNING.. it seems to be critical here to use:
            // (playback_ptr != NULL)
            //
            // but this contradicts c++ style guides that advise "nullptr"

            if (playback_ptr != NULL)
            { // note i changed this from NULL!?! (still not sure maybe check all)

                float increment = pulse_hz / sample_hz;

                int frames_available = playback_ptr->get_frames_available();
                frames_available = MIN(frames_available, buffer_size);

                for (int i = 0; i < frames_available; i++)
                {
                    playback_ptr->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
                    phase = fmod(phase + increment, 1.0);
                }

                print(frames_available);
            }
        }
    }
    else
    {
        print("AudioStreamPlayer child not found!");
    }

    // if (!audio_player_ptr->is_playing()) // not playing, do not call get_stream_playback (which gives a warning)
    //     return;

    // AudioStreamGeneratorPlayback *playback_ptr = get_audio_generator_playback_ptr(); // we need a pointer to the playback generator

    // MIX RATE MAY CRASH???

    // // getting the mix rate
    // Ref<AudioStream> audio_stream_ref = audio_player_ptr->get_stream();
    // if (audio_stream_ref.is_valid())
    // {
    //     Ref<AudioStreamGenerator> audio_stream_generator_ref = audio_stream_ref;
    //     if (audio_stream_generator_ref.is_valid())
    //     {
    //         AudioStreamGenerator *audio_stream_generator_ptr = audio_stream_generator_ref.ptr();

    //         if (audio_stream_generator_ptr != NULL) // check to avoid crash
    //         {
    //             sample_hz = audio_stream_generator_ptr->get_mix_rate(); // WARNING MAYBE SHOULD CHECK
    //         }
    //     }
    // }

    // WARNING
    // doing this crashes:
    //
    // if (playback_ptr == NULL) return
    //
    // i sometimes use this check pattern but it seems to be dangerous with pointers

    // i also tried:
    // if (playback) // still crashes
    // if (playback_ptr != nullptr)

    // SHOULD BE CRASH!?!
    // if (playback_ptr != NULL) // WARNING this is a golden check (even despite the previous pattern)
    // {
    //     // if (playback_ptr != nullptr)
    //     // {
    //     float increment = pulse_hz / sample_hz;

    //     int frames_available = playback_ptr->get_frames_available();

    //     frames_available = MIN(frames_available, buffer_size);

    //     for (int i = 0; i < frames_available; i++)
    //     {
    //         playback_ptr->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
    //         phase = fmod(phase + increment, 1.0);
    //     }

    //     print(frames_available);
    // }
}

void SoundGenerator::_ready()
{
}

// macros from macros.h
CREATE_GETTER_SETTER(SoundGenerator, bool, enabled)
CREATE_GETTER_SETTER(SoundGenerator, int, buffer_size)
CREATE_GETTER_SETTER(SoundGenerator, float, sample_hz)
CREATE_GETTER_SETTER(SoundGenerator, float, pulse_hz)

void SoundGenerator::_bind_methods()
{
    // macros from macros.h
    CREATE_CLASSDB_BINDINGS(SoundGenerator, BOOL, enabled)
    CREATE_CLASSDB_BINDINGS(SoundGenerator, INT, buffer_size)
    CREATE_CLASSDB_BINDINGS(SoundGenerator, FLOAT, sample_hz)
    CREATE_CLASSDB_BINDINGS(SoundGenerator, FLOAT, pulse_hz)
}

#endif
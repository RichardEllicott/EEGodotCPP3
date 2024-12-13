#ifndef SOUND_GENERATOR_CPP
#define SOUND_GENERATOR_CPP

#include <sound_generator.h>

#include <macros.h>
#include <helper.h>

#include <godot_cpp/classes/audio_stream_player.hpp>             // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_playback.hpp>           // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_generator.hpp>          // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp> // AudioStreamGeneratorPlayback

#include <godot_cpp/classes/audio_stream.hpp> // AudioStream

using namespace godot;

SoundGenerator::SoundGenerator()
{
    enabled = false;

    sample_rate = 44100.0;
    frequency = 220.0;

    // timer_delay = 1.0 / 16.0;
}

SoundGenerator::~SoundGenerator()
{
   audio_player_ptr = nullptr; // important for preventing memory crash
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

#define DEBUG_FILL_BUFFER // prints out messsages to find the point of crash! (KEEP)

#ifdef DEBUG_FILL_BUFFER
    print("DEBUG_FILL_BUFFER 0");
#endif

    if (get_audio_player_ptr() != NULL) // will be NULL if no AudioStreamPlayer
    {
#ifdef DEBUG_FILL_BUFFER
        print("DEBUG_FILL_BUFFER 1");
#endif

        if (audio_player_ptr->is_playing()) // check if playing
        {
#ifdef DEBUG_FILL_BUFFER
            print("DEBUG_FILL_BUFFER 2");
#endif

            AudioStreamGeneratorPlayback *playback_ptr = get_audio_generator_playback_ptr(); // we need a pointer to the playback generator

            // WARNING.. it seems to be critical here to use:
            // (playback_ptr != NULL)
            //
            // but this contradicts c++ style guides that advise "nullptr"

            if (playback_ptr != NULL) // very weird nullptr fails, using NULL
            {
#ifdef DEBUG_FILL_BUFFER
                print("DEBUG_FILL_BUFFER 3");
#endif

                float increment = frequency / sample_rate;

                // THIS IS WHERE MOST CRASHES WOULD START!!!

                int frames_available = playback_ptr->get_frames_available();

#ifdef DEBUG_FILL_BUFFER
                print("DEBUG_FILL_BUFFER 4");
#endif

                for (int i = 0; i < frames_available; i++)
                {
                    // playback_ptr->push_frame(Vector2(1.0, 1.0) * sin(phase * Math_TAU));
                    // phase = fmod(phase + increment, 1.0);

                    playback_ptr->push_frame(_get_frame());
                }

                print(frames_available);
            }
        }
    }
    else
    {
        print("AudioStreamPlayer child not found!");
    }
}

float lowPassFilter(float currentInput, float previousOutput, float alpha)
{
    return alpha * currentInput + (1.0f - alpha) * previousOutput;
}

Vector2 SoundGenerator::_get_frame()
{
    float increment = frequency / sample_rate;

    float pulse_width = 0.25;

    // float signal = sin(phase * Math_TAU); // sin
    // float signal = phase - UtilityFunctions::floorf(phase + 0.5f); //saw
    float signal = (phase < pulse_width) ? 1.0f : -1.0f; // square

    Vector2 frame = Vector2(1.0, 1.0) * signal; // testing "auto"
    phase = fmod(phase + increment, 1.0);
    return frame;
}

void SoundGenerator::_ready()
{
    enabled = false;
    // _update_sample_rate();
}

// gets the sample rate from the AudioStreamGenerator
//
// GDScript (one line!):
// sample_rate = $AudioStreamPlayer.stream.mix_rate
//
// i have this optional, as i think it might crash sometimes
//
//
void SoundGenerator::_update_sample_rate()
{
    if (get_audio_player_ptr() != nullptr) // will be null if no AudioStreamPlayer
    {
        // get sample rate
        Ref<AudioStream> audio_stream_ref = audio_player_ptr->get_stream();
        if (audio_stream_ref.is_valid())
        {
            Ref<AudioStreamGenerator> audio_stream_generator_ref = audio_stream_ref; // attempt cast
            if (audio_stream_generator_ref.is_valid())
            {
                AudioStreamGenerator *audio_stream_generator_ptr = audio_stream_generator_ref.ptr(); // get pointer from reference
                if (audio_stream_generator_ptr != NULL)                                              // check to avoid crash (note nullptr doesn't work)
                {
                    sample_rate = audio_stream_generator_ptr->get_mix_rate();
                }
            }
        }
    }
}

// macros from macros.h
CREATE_GETTER_SETTER(SoundGenerator, bool, enabled)
CREATE_GETTER_SETTER(SoundGenerator, float, sample_rate)
CREATE_GETTER_SETTER(SoundGenerator, float, frequency)


// CREATE_GETTER_SETTER(SoundGenerator, AudioStream*, audio_stream)

// AudioStream SoundGenerator::get_audio_stream() const { return audio_stream; }
// void SoundGenerator::set_audio_stream(const AudioStream p_audio_stream) { audio_stream = p_audio_stream; }

void SoundGenerator::_bind_methods()
{
    // macros from macros.h
    CREATE_VAR_BINDINGS(SoundGenerator, BOOL, enabled)
    CREATE_VAR_BINDINGS(SoundGenerator, FLOAT, sample_rate)
    CREATE_VAR_BINDINGS(SoundGenerator, FLOAT, frequency)

#pragma region AudioStreamExport // generated this with chat gp!

    // ClassDB::bind_method(D_METHOD("set_audio_stream", "audio_stream"), &SoundGenerator::set_audio_stream);
    // ClassDB::bind_method(D_METHOD("get_audio_stream"), &SoundGenerator::get_audio_stream);
    // ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "audio_stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_audio_stream", "get_audio_stream");

    CREATE_CLASS_BINDINGS(SoundGenerator, "AudioStream", audio_stream)



#pragma endregion

    // CREATE_CLASSDB_BINDINGS(SoundGenerator, GODOT_CPP_AUDIO_STREAM_HPP, audio_stream)
}

#pragma region AudioStreamExport
void SoundGenerator::set_audio_stream(const Ref<AudioStream> &p_audio_stream)
{
    audio_stream = p_audio_stream;
}

Ref<AudioStream> SoundGenerator::get_audio_stream() const
{
    return audio_stream;
}
#pragma endregion

#endif
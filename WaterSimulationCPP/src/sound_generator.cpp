#ifndef SOUND_GENERATOR_CPP
#define SOUND_GENERATOR_CPP

#include <sound_generator.h>

#include <macros.h>
#include <helper.h>

#include <godot_cpp/classes/audio_stream_player.hpp>   // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_playback.hpp> // AudioStreamPlayback

using namespace godot;

SoundGenerator::SoundGenerator()
{
    enabled = false;
    speed = 35.7;
}

SoundGenerator::~SoundGenerator()
{
}

void SoundGenerator::_process(double delta)
{
}

void SoundGenerator::_draw()
{
}

void SoundGenerator::_physics_process(double delta)
{
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
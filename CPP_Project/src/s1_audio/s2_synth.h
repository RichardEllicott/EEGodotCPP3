/*

new synth code design idea

-use traditional style (we still have one file, but it is split into regions)
-use dictionaries for many variables
-allow the channel to access a pointer to it's parent to get default values, or for these to be overridden
-allow commands to set things

in theory it could be set up by GDScript easily, encompass different designs


again the synth works like:

S2SynthChannel (channel childs) ... we read the signal with _get_signal()
S2Synth                         ... we read an audio buffer that gives us a PackedVector2Array, as is standard for the stereo sound in Godot



music standards to obey:

concert pitch A4 is 440Hz
middle C, C4 is 9 semitones lower
many synths default to C3 as "neutral"

so our standard 0 pitch will be C3



*/
#ifndef S2_SYNTH_H
#define S2_SYNTH_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

#include <s1_audio/s1_audio_filter.h>

#include <godot_cpp/classes/random_number_generator.hpp>

// #include <stdlib.h>
#include <unordered_map>  // strange i have to add this?
#include <map>            // strange i have to add this?

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer
// #include <godot_cpp/classes/audio_stream_generator.hpp>           // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>  // AudioStreamGeneratorPlayback
// #include <godot_cpp/classes/audio_stream_playback.hpp>            // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_player.hpp>  // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_wav.hpp>     // AudioStreamPlayer
#include <godot_cpp/classes/random_number_generator.hpp>

#include <iostream>

// #include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer

// this would allow using unordered_map's on Variants???
#pragma region VARIANT_HASH_FUNCTION
#ifdef VARIANT_HASH_FUNCTION

namespace std {

template <>
struct hash<Variant> {
    size_t operator()(const Variant& v) const {
        switch (v.get_type()) {
            case Variant::NIL:
                return 0;
            case Variant::BOOL:
                return std::hash<bool>()(v);
            case Variant::INT:
                return std::hash<int>()(v);
            case Variant::FLOAT:
                return std::hash<float>()(v);
            case Variant::STRING:
                return std::hash<String>()(v);
            case Variant::VECTOR2:
                return std::hash<Vector2>()(v);
            case Variant::VECTOR3:
                return std::hash<Vector3>()(v);
            case Variant::TRANSFORM2D:
                return std::hash<Transform2D>()(v);
            case Variant::RECT2:
                return std::hash<Rect2>()(v);
            case Variant::COLOR:
                return std::hash<Color>()(v);
            case Variant::DICTIONARY:
                return std::hash<Dictionary>()(v);
            case Variant::ARRAY:
                return std::hash<Array>()(v);
            // case Variant::RAW_ARRAY:
            //     return std::hash<Vector<uint8_t>>()({v});
            default:
                return std::hash<int>()(v.get_type());  // For unsupported types
        }
    }
};
}  // namespace std
#endif
#pragma endregion

using namespace godot;

class S2NoiseCache {
   public:
    Ref<RandomNumberGenerator> rng;

    std::vector<float> noise_buffer;

    float noise(int position) {
        position = pos_mod(position, noise_buffer.size());
        return noise_buffer[position];
    };

    S2NoiseCache(int cache_size = 44100) {
        rng.instantiate();
        rng->set_seed(0);

        noise_buffer.resize(cache_size);
        for (int i = 0; i < cache_size; i++) {
            noise_buffer[i] = rng->randf_range(-1.0f, 1.0f);
        }
    }
    // ~S2NoiseCache() {
    // }
};

#pragma region EXOTIC_WAVES
class S2ExoticWaves {
   public:
    // basic sine
    static float sine(float phase) {
        return sin(phase * Math_TAU);
    }

    // basic saw
    static float saw(float phase) {
        return fmod(phase, 1.0f) * 2.0f - 1.0f;
    }

    // square with pwm
    static float square(float phase, float pulse_width) {
        return (fmod(phase, 1.0f) < pulse_width) ? 1.0f : -1.0f;
    }

    // SAW with PWM (using two saws)
    static float saw2(float phase, float pulse_width) {
        float mod_x = fmod(phase / 2.0f, 1.0f);  // divide by 2 to make same pitch as normal saw

        pulse_width = CLAMP(pulse_width, 0.0f, 1.0f);  // clamp pulse width to avoid bad behaviour

        float val;  // Determine the position within the current cycle
        if (mod_x < pulse_width) {
            val = mod_x / pulse_width;
        } else {
            val = (mod_x - pulse_width) / (1.0f - pulse_width);
        }

        val = val * 2.0f - 1.0f;  // Scale the value to the range of -1 to 1

        return val;
    }

    // SAW with PWM using two different waveforms, passed as function pointers
    static float pwm_template(float phase, float pulse_width, float (*wave1_function)(float), float (*wave2_function)(float)) {
        float mod_x = fmod(phase / 2.0f, 1.0f);        // Divide by 2 to make same pitch as normal saw
        pulse_width = CLAMP(pulse_width, 0.0f, 1.0f);  // Clamp pulse width to avoid bad behaviour

        float val;  // Determine the position within the current cycle
        if (mod_x < pulse_width) {
            // Use wave1_function for the first half (mod_x < pw)
            val = wave1_function(mod_x / pulse_width);  // Normalize to 0-1 range
        } else {
            // Use wave2_function for the second half (mod_x >= pw)
            val = wave2_function((mod_x - pulse_width) / (1.0f - pulse_width));  // Normalize to 0-1 range
        }

        val = val * 2.0f - 1.0f;  // Scale the value to the range of -1 to 1

        return val;
    }

    static float sin2(float phase, float pulse_width) {
        return pwm_template(phase, pulse_width, sine, sine);
    }
};
#pragma endregion

#pragma region HEADER_FILES

// forward declaration (allows the objects to reference each other)
class S2Synth;         // the synth
class S2SynthChannel;  // the channels, used by the synth

// synth channel
class S2SynthChannel {
   private:
    S2Synth* parent;

   public:
    float frequency;  // occilator frequency

    float pulse_width;

    enum Waveform {
        SINE,
        SQUARE,
        SAW2,
        SSAW,
        NOISE,
    };

    //     enum Waveform {
    //     SINE,     // sine
    //     SAW,     // saw
    //     SQUARE,  // square with PWM (normal at 0.5)
    //     SAW2,    // saw with PWM (normal at 0.5)
    //     SIN2,    // sine with PWM (normal at 0.5)
    //     SSAW,    // super saw (saw multiplied with 5th)
    //     NOISE,   // random noise
    // };

    int waveform;  // occilator frequency

    float timer;  // occilator position

    enum class State {
        DISABLED,
        HOLD,
        RELEASE
    };

    State state;  // state marks if we are playing hold or relase, or inactive

    float get_envelope();  // normal envelope (maybe this will become abstract?)

    float get_signal();  // get the signal at the current timer position

    void _increment_timer();  // increment by the correct amount based on our frequency

    S2SynthChannel(S2Synth* _parent);  // created with a reference to parent
    ~S2SynthChannel();
};

class S2Synth {
   private:
   public:
    std::vector<S2SynthChannel> channels;
    int max_channels;  // -1 infinite

    // std::unordered_map<String, Variant> float_vars; // doesn't work without a custom hash
    // std::map<String, Variant> variables;  // doesn't work without a custom hash
    // Dictionary<String, Variant> vars_dict;

    // DYNAMIC VARS
    std::map<String, float> float_vars;  // doesn't work without a custom hash
    void set_float_var(String name, float value);
    float get_float_var(String name);
    Variant run_command(String command);

    Dictionary vars;  // godot dictionary (flexible)
    void set_var(String name, Variant value);
    Variant get_var(String name);

    float mix_rate;

    float timer;

    int add_channel();
    void clear_channel(int channel);
    void clear_channels();


    S2NoiseCache noise_cache;



    PackedVector2Array _get_audio_buffer(int frames);

    S2Synth();
    ~S2Synth();
};

#pragma endregion

#pragma region CHANNEL_CPP

float S2SynthChannel::get_envelope() {
    return 1.0f;
}

float S2SynthChannel::get_signal() {
    float signal = 0.0f;

    switch (waveform) {
        case SINE:
            signal += S2ExoticWaves::sine(timer);
            break;
        case SQUARE:
            signal += S2ExoticWaves::square(timer, pulse_width);
            break;
        case SAW2:
            signal += S2ExoticWaves::saw2(timer, pulse_width);
            break;
        case NOISE:
            signal += parent->noise_cache.noise(timer * 44100.0);
            break;
    }

    // float pulse_width = parent->get_float_var("pulse_width");

    signal *= get_envelope();

    return signal;
}

void S2SynthChannel::_increment_timer() {
}

// S2SynthChannel::S2SynthChannel() {
// }

// S2SynthChannel::S2SynthChannel(const S2Synth* _parent){
//     parent = _parent;
// }

S2SynthChannel::S2SynthChannel(S2Synth* _parent)
    : parent(_parent) {}

S2SynthChannel::~S2SynthChannel() {
}

#pragma endregion

#pragma region SYNTH_CPP

int S2Synth::add_channel() {
    auto channel = S2SynthChannel(this);

    channels.push_back(channel);
    return channels.size() - 1;

    // return channel;
}

void S2Synth::clear_channels() {
    channels.clear();
}

void S2Synth::clear_channel(int channel_id) {
    channels.clear();
}

// DYNAMIC VARS
void S2Synth::set_float_var(String name, float value) {
    float_vars[name] = value;
}

float S2Synth::get_float_var(String name) {
    if (float_vars.find(name) != float_vars.end()) {  // if key
        return float_vars[name];
    }
    return -FLT_MAX;
}

void S2Synth::set_var(String name, Variant value) {
    float_vars[name] = value;
}

Variant S2Synth::get_var(String name) {
    if (float_vars.find(name) != float_vars.end()) {  // if key
        return float_vars[name];
    }
    return nullptr;
}

Variant S2Synth::run_command(String command_string) {
    // split the string
    Array split = command_string.split(" ", false);
    while (split.size() < 3)  // ensure length is 3
        split.append("");

    String command = split[0];
    String par1 = split[1];
    String par2 = split[2];
    String type = split.size() > 3 ? split[3] : "string";  // Default to string if no type provided

    if (command == "test") {
        return "testing 123...";
    } else if (command == "set_float") {
        set_float_var(par1, par2.to_float());

    } else if (command == "get_float") {
        return get_float_var(par1);
    } else if (command == "get") {
        return get_var(par1);
    } else if (command == "set") {
        // return set_var(par1, par2);
    }

    return "error";
}

PackedVector2Array S2Synth::_get_audio_buffer(int frames) {
    PackedVector2Array audio_buffer;
    audio_buffer.resize(frames);

    for (int i = 0; i < frames; i++) {
        float signal = 0.0f;

        for (S2SynthChannel& channel : channels) {  // iterate (can modify objects)
            signal += channel.get_signal();
        }

        audio_buffer[i] = Vector2(1.0, 1.0) * signal;
    }

    return audio_buffer;
}

S2Synth::S2Synth() {
    max_channels = 32;

    noise_cache = S2NoiseCache(44100);

    print("S2Synth LAUNCHED!!!....");
}
S2Synth::~S2Synth() {
}

#pragma endregion

#endif
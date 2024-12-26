/*

single file, multipurpose filter

only usable from c++

*/
#ifndef S1_POLY_SYNTH_H
#define S1_POLY_SYNTH_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

#include <s1_audio/s1_audio_filter.h>

// #include <stdlib.h>
#include <unordered_map>  // strange i have to add this?

// #include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer

using namespace godot;

// struct S1PolySynthNote {
//     float pitch;   // note value where 0 is middle C (440hz)
//     float volume;  // Volume or intensity, typically in the range [0, 1]
//     float duration;

//     float start_time;

//     // A D S R

//     float attack = 1.0f;  // larger longer
//     float decay = 0.0f;   // use small fractions for long decay

//     float sustain = 1.0f;
//     float release = 1.0f;

//     bool playing_tail = false;  // when true we will do the tail

//     // return pitch val to chromatic scale
//     float get_frequency() {
//         return 2.0f * pow(2.0f, pitch / 12.0f);
//     }
// };

// one mono synth, that is used by S1Viroid2
class S1PolySynthChannel {
   private:
    float _mix_rate = 44100;

    S1AudioFilter filter = S1AudioFilter(10.0f, _mix_rate, S1AudioFilter::LOW);

   public:
    float frequency = 55.0;  // this is in hz, it should be set when we trigger the channel
    float pulse_width = 0.5;

    float pulse_width_modulation = 1.0 / 16.0;

    float volume = 1.0;  // multiply volume, set 0-1
    // float gain_db = 0.0; // apply after volume, db gain

    // ADSR
    float attack = 0.125f;

    float attack_level = 1.0f;

    float decay = 1.0f;
    float decay_level = 0.5f;

    // float sustain = 0.5f;
    float release = 1.0f;

    // Massive has

    // attack
    // level

    // decay
    // level

    //sloop
    //morph
    //level

    // release


    bool hold = true;  // is holding (not released)

    enum Mode {
        SINE,
        SQUARE,
        SAW,
    };

    // lerp template function
    template <typename T>
    T lerp(T a, T b, T alpha) {
        return a + (b - a) * alpha;
    }

    Mode mode = SINE;

    float timer = 0.0;

    float start_time = 0.0f;

    float _get_signal() {
        float signal = 0.0f;

        switch (mode) {
            case SINE:
                signal += _SIN(timer * frequency);
                break;
            case SQUARE:
                signal += _SQR(timer * frequency, pulse_width);
                break;
            case SAW:
                signal += _SAW(timer * frequency);
                break;
        }

        // decay from start of note
        float decay_env = 1.0f;
        decay_env -= timer / decay;       // decay
        decay_env = MAX(decay_env, 0.0);  // cannot go below 0.0f

        decay_env = lerp(attack_level, decay_level, decay_env); // the envelop has heighs like the massive one

        float attack_gate = timer / attack;
        attack_gate = MIN(attack_gate, 1.0f);  // can't be higher than 1

        signal *= decay_env;
        signal *= attack_gate;

        // signal -= timer * decay; // decay
        //

        // signal *= volume;

        return signal;
    }

    static float _SAW(float x) {
        return fmod(x, 1.0f) * 2.0f - 1.0f;
    }

    // square, pulse width 0 to 1.0
    static float _SQR(float x, float pw) {
        return (fmod(x, 1.0f) < pw) ? 1.0f : -1.0f;
    }

    static float _SIN(float x) {
        return sin(x * Math_TAU);
    }

    void set_mix_rate(float mix_rate) {
        _mix_rate = mix_rate;
        filter.set_sample_rate(mix_rate);
    }
    // void set_frequency(float frequency) {
    //     _frequency = frequency;
    // }

    // void set_pulse_width(float pulse_width) {
    //     _pulse_width = pulse_width;
    // }

    void set_note(float note_value) {
        frequency = 440.0f * pow(2.0f, note_value / 12.0f);  // 0 is middle C (440hz)
    }

    PackedVector2Array _get_audio_buffer(int frames_available) {
        PackedVector2Array buffer;  // create an stereo audio buffer

        buffer.resize(frames_available);  // set it's size in one (faster than appending)

        float increment = 1.0f / _mix_rate;  // the increment is the time in seconds of one frame

        for (int i = 0; i < frames_available; i++) {
            float signal = _get_signal();
            timer += increment;

            signal = CLAMP(signal, -1.0, 1.0);  // final hard clip

            buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
        }
    }
};

// tackling poly different to allow filters
class S1PolySynth {
   public:
    float mix_rate = 44100;  // in hz
    float timer = 0.0f;      // in seconds

    float add_level = 0.5f;  // poly add level

    float gain_db = 0.0f;  // linear volume (not db)

    float pulse_width = 0.5;

    float filter_pitch_tracking = 1.0;

    bool test_signal = false;

    // ADSR
    // float attack = 1.0f;
    // float decay = 1.0f;
    // float sustain = 1.0f;
    // float release = 1.0f;

    // std::unordered_map<float, Note> notes;  // current notes

    std::unordered_map<float, S1PolySynthChannel> channels;  // current notes

    int max_channels = 32;

    enum Mode {
        MONO,
        POLY
    };

    Mode mode = Mode::POLY;

    void add_note(float note, float volume = 1.0f) {
        // auto x = channels.size();

        // Key is not present
        if (channels.find(note) == channels.end()) {
            print("S1PolySynth add_note: " + godot::String::num(note));

            auto channel = S1PolySynthChannel();

            // channel.attack = attack;
            // channel.decay = decay;
            // channel.sustain = sustain;
            // channel.release = release;

            channel.set_note(note);
            channels[note] = channel;
            print("channels.size: " + godot::String::num(channels.size()));
        } else {
            // print("S1PolySynth FAILED add_note: " + godot::String::num(note));
        }
    }

    void clear_note(float note) {
        if (channels.find(note) != channels.end()) {
            print("S1PolySynth clear_note: " + godot::String::num(note));
            print("channels.size: " + godot::String::num(channels.size()));
        }

        channels.erase(note);  // no exception is called
    }

    void clear_notes() {
        channels.clear();
    }

    bool debug_print = false;
    int print_modulo = 1024 * 16;
    int print_count = 0;

    // get an audio buffer array, stero signal ready to push
    PackedVector2Array _get_audio_buffer(int frames_available) {
        PackedVector2Array buffer;  // create an stereo audio buffer

        buffer.resize(frames_available);  // set it's size in one (faster than appending)

        float increment = 1.0f / mix_rate;  // the increment is the time in seconds of one frame

        for (int i = 0; i < frames_available; i++) {
            float signal = 0.0f;

            if (test_signal) {  // test signal for debug
                signal += sin(timer * Math_TAU * 440.0f);
                timer += increment;
            }

            for (int j = 0; j < channels.size(); j++) {
                // hook here works!!
            }

            // VERY IMPORTANT (to me)
            // C++ always want to copy the classes, so this pattern ensures we do not make copies and change the orginals
            for (auto& pair : channels) {
                auto& channel = pair.second;  // Use reference to modify the original object
                signal += channel._get_signal() * add_level;
                channel.timer += increment;  // Now you can modify the original channel
            }

            signal *= pow(10.0, gain_db / 20.0);  // apply volume as decibels (like -12 db for example, 0 is neutral)

            // signal = high_pass_filter.process(signal);  // high pass to stop bottom outs

            signal = CLAMP(signal, -1.0, 1.0);  // final hard clip

            if (debug_print) {  // useful to see the output numbers to check for output
                print_count++;
                if (print_count % print_modulo == 0) {
                    print("signal: " + godot::String::num(signal));
                }
            }

            buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
        }

        return buffer;
    }
};

#endif
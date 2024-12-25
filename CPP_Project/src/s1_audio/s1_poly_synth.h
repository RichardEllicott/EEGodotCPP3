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
    float _frequency = 55.0;
    float _mix_rate = 44100;

    float _pulse_width = 0.5;



    S1AudioFilter filter = S1AudioFilter(10.0f, _mix_rate, S1AudioFilter::LOW);

   public:
    enum Mode {
        SINE,
        SQUARE,
        SAW,
    };

    Mode mode = SINE;


    float start_time = 0.0f;


    float _get_signal(float time_position) {
        switch (mode) {
            case SINE:
                return _SIN(time_position);
                break;
            case SQUARE:
                return _SQR(time_position, _pulse_width);
                break;
            case SAW:
                return _SAW(time_position);
                break;

            default:
                break;
        }
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
    void set_frequency(float frequency) {
        _frequency = frequency;
    }

    void set_frequency(float frequency) {
        _frequency = frequency;
    }
    void set_pulse_width(float pulse_width) {
        _pulse_width = pulse_width;
    }

    void set_pitch(float note_value) {
        440.0f * pow(2.0f, note_value / 12.0f);  // 0 is middle C (440hz)
    }
};

// tackling poly different to allow filters
class S1PolySynth {
   public:
    float mix_rate = 44100;  // in hz
    float timer = 0.0f;      // in seconds

    float volume = 1.0f;     // linear volume (not db)
    float add_level = 0.5f;  // poly add level

    float volume_db = -12.0f;  // linear volume (not db)

    float pulse_width = 0.5;


    float timer = 0.0;

    // std::unordered_map<float, Note> notes;  // current notes

    std::unordered_map<float, S1PolySynthChannel> channels;  // current notes

    int max_channels = 32;

    enum Mode {
        MONO,
        POLY
    };

    Mode mode = Mode::MONO;

    void add_note(float pitch, float volume = 1.0f, float duration = -1) {
        auto x = channels.size();

        if (channels.find(pitch) == channels.end()) {  // if present do nothing (add aftertouch code here)
        } else { // we need a new channel
            auto channel = S1PolySynthChannel();
            channel.set_pitch(pitch);
            channel.start_time = timer;
        }

        // if (notes.find(pitch) != notes.end()) {  // only add note if not present
        //     // Note note = Note();
        //     // note.pitch = pitch;
        //     // note.volume = volume;
        //     // note.duration = duration;
        //     // note.start_time = timer;

        //     // notes[pitch] = note;
        // }
    }

    void clear_note(float pitch) {

        channels.erase(pitch); // no exception is called

        // if (channels.find(pitch) != channels.end()) {  // if note present
        //     channels.erase(pitch)
        // }

        
    }



    // signal to generate, you can replace this
    float _test_signal() {
        return sin(timer * Math_TAU * 440.0f);
    }

    // get an audio buffer array, stero signal ready to push
    PackedVector2Array _get_audio_buffer(int frames_available) {
        PackedVector2Array buffer;  // create an stereo audio buffer

        buffer.resize(frames_available);  // set it's size in one (faster than appending)

        float increment = 1.0 / mix_rate;  // the increment is the time in seconds of one frame

        for (int i = 0; i < frames_available; i++) {
            
            float signal = _test_signal();



            timer += increment;

            signal *= pow(10.0, volume_db / 20.0);  // apply volume as decibels (like -12 db for example, 0 is neutral)

            // signal = high_pass_filter.process(signal);  // high pass to stop bottom outs

            signal = CLAMP(signal, -1.0, 1.0);  // final hard clip

            buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
        }

        return buffer;
    }



};

#endif
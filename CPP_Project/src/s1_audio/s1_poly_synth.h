/*

single file style (header and source together)

contains the synthesiser as more portable c++

S1PolySynthChannel // we make a channel for each key on the keyboard, contains a synth, enevelope and filters etc

S1PolySynth // builds the waveform from all the channels


to keep complexity down, we have a seperate wrapper for Godot that will link this object


*/
#ifndef S1_POLY_SYNTH_H
#define S1_POLY_SYNTH_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

#include <s1_audio/s1_audio_filter.h>

#include <godot_cpp/classes/random_number_generator.hpp>

// #include <stdlib.h>
#include <unordered_map>  // strange i have to add this?

// #include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer

using namespace godot;

// add my lerp function to global namespace
#ifndef LERP
#define LERP
template <typename T>
T lerp(T a, T b, T alpha) {
    return a + (b - a) * alpha;
}
#endif

// forward references
class _S1PolySynth;
class _S1PolySynthChannel;




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

// copied
class S2WaveHelper {
   public:
    // Ref<AudioStreamWAV> audio_stream_wav;  // a godot ref, note this might keep the thing in memory
    // AudioStreamWAV &audio_stream;  // C++ ref?? or pointer are choices

    static const int wave_lerp_mode = 0;

    static const int print_mod2 = 1024 * 4;
    static const int print_count2 = 0;

   protected:
    // Helper to decode 16-bit PCM sample from PackedByteArray
    static int16_t decode_sample(const PackedByteArray& data, int index) {
        if (index + 1 >= data.size()) {
            return 0;
        }
        // Combine two bytes into a signed 16-bit value
        return static_cast<int16_t>((data[index + 1] << 8) | (data[index]));
    }

   public:
    static float read_audio_stream_wav(Ref<AudioStreamWAV> audio_stream_wav, float sample_pos_f) {
        float signal = 0.0;

        if (audio_stream_wav.is_valid()) {
            int format = audio_stream_wav->get_format();

            if (format != AudioStreamWAV::FORMAT_16_BITS) {
                // UtilityFunctions::print("Unsupported format. Only PCM16 is supported.");
                return 0.0f;
            }

            PackedByteArray audio_bytes = audio_stream_wav->get_data();

            int channels = audio_stream_wav->is_stereo() ? 2 : 1;  // one or two channels (we don't yet support 2)

            int bytes_per_sample = 2;  // 16-bit PCM is 2 bytes per sample

            int total_samples = audio_bytes.size() / (channels * bytes_per_sample);

            // Calculate the sample position
            int sample_pos = sample_pos_f;  // cast to in for actual sample pos

            float lerp_pos = sample_pos_f - sample_pos;

            int sample_pos2 = (sample_pos + 1);

            sample_pos %= total_samples;
            sample_pos2 %= total_samples;

            // if (sample_pos < 0 || sample_pos >= total_samples - 1) {
            //     return 0.0f;  // Out of bounds
            // }

            // Retrieve the two samples for interpolation
            int sample1_index = sample_pos * channels * bytes_per_sample;
            int sample2_index = sample_pos2 * channels * bytes_per_sample;

            int16_t sample1 = decode_sample(audio_bytes, sample1_index);
            int16_t sample2 = decode_sample(audio_bytes, sample2_index);

            float sample1f = static_cast<float>(sample1) / 32768.0f;
            float sample2f = static_cast<float>(sample2) / 32768.0f;

            switch (wave_lerp_mode) {
                case 0:
                    signal = sample1f;
                    break;

                case 1:

                    // float lerped_sample = sample2f * lerp_pos + sample1f * (1.0f - lerp_pos); // reduces to:
                    float lerped_sample = sample1f + lerp_pos * (sample2f - sample1f);  // reduced (note i avoid the library to avoid doubles)
                    signal = lerped_sample;

                    break;
            }
        }

        if (print_count2 % print_mod2 == 0) {
            // print("samples:");
            // print(sample1f);
            // print(sample2f);
        }

        return signal;
    };
};


// one mono synth, that is used by S1Viroid2
class _S1PolySynthChannel {
#pragma region CONSTRUCTOR

   public:
    // std::weak_ptr<S1PolySynth> synth;
    // explicit S1PolySynthChannel(std::shared_ptr<S1PolySynth> parent_synth) : synth(parent_synth) {}

    //    private:
    //     Ref<S1PolySynth> _parent;
    //     // warning this Ref pattern would only work in Godot itself
    //     // would need to change toa weakref system, Gemeni had details

    //    public:
    //     S1PolySynthChannel(Ref<S1PolySynth> parent) {
    //         _parent = parent;
    //     }

    // ~S1PolySynthChannel() {
    // }
    // #pragma endregion

    _S1PolySynth* parent;
    // _S1PolySynthChannel(_S1PolySynth* _parent) {
    //     parent = _parent;
    // }

    //      _S1PolySynth& parent;
    // _S1PolySynthChannel( _S1PolySynth& _parent) {
    //     parent = _parent;
    // }

    // trying even putting refs here causes errors

    // Ref<_S1PolySynth> parent; // no compile
    // _S1PolySynth* parent; // no cpmpile either i think

    // _S1PolySynth parent; // even this no compile! trying order of classes??

    // _S1PolySynthChannel() {
    // }

#pragma endregion

   private:
    float _mix_rate = 44100;

    // S1AudioFilter filter = S1AudioFilter(220.0f, _mix_rate, S1AudioFilter::LOW);
    LowPassFilter filter = LowPassFilter( _mix_rate, 220.0f, 0.5f);


   public:
    float frequency = 55.0;  // this is in hz, it should be set when we trigger the channel
    float pulse_width = 1.0f / 3.0f;

    float volume = 1.0;  // multiply volume, set 0-1
    // float gain_db = 0.0; // apply after volume, db gain

    // ADSR
    float attack = 0.125f;      // attack time
    float attack_level = 1.0f;  // normally 1.0, the level the attack reaches up to
    float decay = 1.0f;         // time to decay from attack_level to decay_level
    float sustain = 1.0f;       // final level while held
    float release = 0.5f;       // release

    int envelope_stage = 0;  // 0 = holding, 1 = released, -1 to delete

    bool filter_enabled = true;

    // Pulse Width Modulation
    float pulse_width_mod = 0.0f;
    float pulse_width_mod_freq = 1.0f;

    void set_filter_frequency(float filter_frequency) {
        filter.set_cutoff(filter_frequency);
    }

    void set_filter_resonance(float filter_resonance) {
        filter.set_resonance(filter_resonance);
    }

    enum Waveform {
        SIN,
        SAW,
        SQUARE,
        SAW2,  // normal SAW at pulsewidth 0.5,
        SIN2
    };

    int waveform = SQUARE;

    float timer = 0.0f;

    float start_time = 0.0f;

    float release_time = 0.0f;

    float envelope_release_height = 1.0;  // set when we release, to make it release from the last height (preventing pop)

    // Basic SAW
    static float _SAW(float x) {
        return fmod(x, 1.0f) * 2.0f - 1.0f;
    }

    // SAW with PWM using two different waveforms, passed as function pointers
    static float _PWM_TEMPLATE(float x, float pw, float (*wave1_function)(float), float (*wave2_function)(float)) {
        float mod_x = fmod(x / 2.0f, 1.0f);  // Divide by 2 to make same pitch as normal saw
        pw = CLAMP(pw, 0.0f, 1.0f);          // Clamp pulse width to avoid bad behaviour

        float val;  // Determine the position within the current cycle
        if (mod_x < pw) {
            // Use wave1_function for the first half (mod_x < pw)
            val = wave1_function(mod_x / pw);  // Normalize to 0-1 range
        } else {
            // Use wave2_function for the second half (mod_x >= pw)
            val = wave2_function((mod_x - pw) / (1.0f - pw));  // Normalize to 0-1 range
        }

        val = val * 2.0f - 1.0f;  // Scale the value to the range of -1 to 1

        return val;
    }

    // SAW with PWM (using two saws)
    static float _SAW2(float x, float pw) {
        float mod_x = fmod(x / 2.0f, 1.0f);  // divide by 2 to make same pitch as normal saw

        pw = CLAMP(pw, 0.0f, 1.0f);  // clamp pulse width to avoid bad behaviour

        float val;  // Determine the position within the current cycle
        if (mod_x < pw) {
            val = mod_x / pw;
        } else {
            val = (mod_x - pw) / (1.0f - pw);
        }

        val = val * 2.0f - 1.0f;  // Scale the value to the range of -1 to 1

        return val;
    }

    // Square Wave
    static float _SQR(float x, float pw) {
        return (fmod(x, 1.0f) < pw) ? 1.0f : -1.0f;
    }

    static float _SIN2(float x, float pw) {
        return _PWM_TEMPLATE(x, pw, _SIN, _SIN);
    }

    // Sin Wave
    static float _SIN(float x) {
        return sin(x * Math_TAU);
    }

    void set_mix_rate(float mix_rate) {
        _mix_rate = mix_rate;
        filter.set_sample_rate(mix_rate);
    }

    // set note where 0 = A4, 1 = A#4, 2 = B4, 3 = C5
    // see:
    // https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
    //
    void set_note(float note_value) {
        frequency = 440.0f * pow(2.0f, note_value / 12.0f);  // 0 is A4 (440 Hz), the A above middle C
    }

    float _get_envelope1() {
        if (envelope_stage == 0) {  // stage lone when we hold the note
            // decay from start of note

            float position = timer - start_time;

            float decay_env = 1.0f - CLAMP(position / decay, 0.0f, 1.0f);  // decay envelope (decay from start of note)
            decay_env = lerp(attack_level, sustain, decay_env);

            float attack_env = CLAMP(position / attack, 0.0f, 1.0f);  // attack envelope
            decay_env = lerp(0.0f, attack_level, attack_env);

            return decay_env * attack_env;

        } else {  // stage 1 release

            float position = timer - release_time;

            float release_env = envelope_release_height;
            // float release_env = 1.0f;

            release_env -= position / release;
            release_env = MAX(release_env, 0.0);  // cannot go below 0.0f

            release_env *= sustain;  // should drop from the decay level ?

            if (release_env == 0.0f) {
                envelope_stage = -1;
            }

            return release_env;
        }
    }

    float _get_signal() {
        float signal = 0.0f;

        float position = timer - start_time;  // timer is synced with parent

        switch (waveform) {
            case SIN:
                signal += _SIN(position * frequency);
                break;
            case SAW:
                signal += _SAW(position * frequency);
                break;
            case SQUARE:
                signal += _SQR(position * frequency, pulse_width);
                break;
            case SAW2:
                signal += _SAW2(position * frequency, pulse_width);
                break;
            case SIN2:
                signal += _SIN2(position * frequency, pulse_width);
                break;
        }

        if (filter_enabled) {
            signal = filter.process(signal);
        }

        signal *= _get_envelope1();

        return signal;
    }

    // not yet used! might be faster in the end to get chunks
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
// using node to allow Ref<>
class _S1PolySynth {
   public:
    float mix_rate = 44100;  // in hz

    float timer = 0.0f;  // in seconds

    // float start_time = 0.0f;

    float add_level = 0.5f;  // poly add level

    float gain_db = 0.0f;  // linear volume (not db)

    float pulse_width = 0.5f;

    float filter_pitch_tracking = 1.0f;

    bool filter_enabled = true;
    float filter_frequency = 440.0f;
    float filter_resonance = 1.0f;

    int test_signal = 0;

    // ADSR
    float attack = 0.125f;  // attack time
    float attack_level = 1.0f;
    float decay = 1.0f;
    float sustain = 0.5f;  // sustain level
    float release = 1.0f;

    int waveform = 0;

    // std::unordered_map<float, Note> notes;  // current notes

    std::unordered_map<float, _S1PolySynthChannel> channels;  // current notes

    int max_channels = 32;

    enum Mode {
        MONO,
        POLY
    };

    Mode mode = Mode::POLY;

    Ref<RandomNumberGenerator> rng;

    _S1PolySynth() {
        rng.instantiate();
        rng->set_seed(0);
    }

    // the release enevelope pattern looks a bit complicated
    void add_note(float note, float volume = 1.0f) {
        // auto x = channels.size();

        // Key is not present
        if (channels.find(note) == channels.end()) {
            print("S1PolySynth add_note: " + String::num(note));

            _S1PolySynthChannel channel = _S1PolySynthChannel();  // create a new channel

            // channel.parent = this;

            channel.set_note(note);
            channel.start_time = timer;

            // channels[note] = channel;
            channels.insert_or_assign(note, channel);

            // print("channels.size: " + godot::String::num(channels.size()));

        } else {
            // print("S1PolySynth FAILED add_note: " + godot::String::num(note));

            _S1PolySynthChannel& channel = channels[note];  // channel already present

            if (channel.envelope_stage != 0) {  // released or finish, so retrigger
                channel.set_note(note);
                channel.start_time = timer;
                channel.envelope_stage = 0;
            }
        }
    }

    void clear_note(float note) {
        if (channels.find(note) != channels.end()) {
            // print("S1PolySynth clear_note: " + godot::String::num(note));
            // print("channels.size: " + godot::String::num(channels.size()));

            _S1PolySynthChannel& channel = channels[note];

            // channel.envelope_release_height = 1.0;

            if (channel.envelope_stage == 0) {  // if we have a playing note

                channel.envelope_release_height = channel._get_envelope1();

                channel.envelope_stage = 1;
                // channel.start_time = timer;
                channel.release_time = timer;
            }

            // channels.erase(note);  // no exception is called
        }
    }

    void clear_notes() {
        channels.clear();
    }

    bool debug_print = false;
    int print_modulo = 1024 * 16;
    int print_count = 0;

    void _sync_channel_vars() {
        for (auto& pair : channels) {
            auto& channel = pair.second;  // Use reference to modify the original object

            // SYNC VARS, may be ennefiencient??
            channel.attack = attack;
            channel.attack_level = attack_level;
            channel.decay = decay;
            channel.sustain = sustain;
            channel.release = release;
            channel.pulse_width = pulse_width;
            channel.waveform = waveform;

            channel.filter_enabled = filter_enabled;

            channel.set_filter_frequency(filter_frequency);
            channel.set_filter_resonance(filter_resonance);

            // channel.
        }
    }

    // get an audio buffer array, stero signal ready to push
    PackedVector2Array _get_audio_buffer(int frames_available) {
        PackedVector2Array buffer;  // create an stereo audio buffer

        buffer.resize(frames_available);  // set it's size in one (faster than appending)

        float increment = 1.0f / mix_rate;  // the increment is the time in seconds of one frame

        _sync_channel_vars();  // sync the variables, we might try to eliminate this pattern

        for (int i = 0; i < frames_available; i++) {
            float signal = 0.0f;

            // test signal for debug
            switch (test_signal) {
                case 1:
                    signal += sin(timer * Math_TAU * 440.0f);
                    break;

                case 2:
                    signal += rng->randf() * 2.0f - 1.0f;
                    break;
            }

#ifdef DONOTSHOW
            // this allows iterating and chanding the target, note the auto& refs
            for (auto& pair : channels) {
                auto& channel = pair.second;  // Use reference to modify the original object
            }
#endif

            // iterate all the channels, adding the signals
            // this iteration pattern keeps a ref, allows deleting the channels where the release has ended
            for (auto it = channels.begin(); it != channels.end();) {
                auto& channel = it->second;

                // Check if the channel should be deleted
                if (channel.envelope_stage == -1) {
                    it = channels.erase(it);  // Erase and move to the next element
                } else {
                    channel.timer = timer;  // Sync the timer
                    signal += channel._get_signal() * add_level;
                    ++it;  // Move to the next element
                }
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

            timer += increment;
        }

        return buffer;
    }
};

#endif
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

#ifndef LERP
#define LERP
template <typename T>
T lerp(T a, T b, T alpha) {
    return a + (b - a) * alpha;
}
#endif

#ifndef MIDI_PITCH_FUNCT
#define MIDI_PITCH_FUNCT

// https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
float note_to_frequency(int midi_note = 69) {
    return 440.0f * pow(2.0f, (midi_note - 69) / 12.0f);
}

int frequency_to_note(float frequency) {
    return round(69 + 12 * log2(frequency / 440.0f));
}

#endif

// forward references .. they don't fully solve the issue in c++ where i might need to seperate this file to h and cpp
class S1PolySynth;
class S1PolySynthChannel;

// a solution to our problems would be header files
// class S1PolySynth {
//     // ADSR
//     float attack;
//     float attack_level;
//     float decay;
//     float sustain;
//     float release;
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
class S1PolySynthChannel {
#pragma region CONSTRUCTOR

   public:
    S1PolySynth* parent;

    S1PolySynthChannel() {
        rng.instantiate();  // not yet used
        rng->randomize();
    }

#pragma endregion

   private:
    float _mix_rate = 44100;

    S1AudioFilter filter = S1AudioFilter(220.0f, _mix_rate, S1AudioFilter::LOW); // my first one
    // LowPassFilter filter = LowPassFilter(_mix_rate, 220.0f, 0.5f); // trying to improve, seems to make the sound go though

    Ref<RandomNumberGenerator> rng;

   public:
    float frequency = 55.0;  // this is in hz, it should be set when we trigger the channel
    float pulse_width = 1.0f / 3.0f;

    float volume = 1.0;  // multiply volume, set 0-1
    float volume_db = 0.0; // apply after volume, db gain

    // ADSR
    float attack = 0.125f;      // attack time
    float attack_level = 1.0f;  // normally 1.0, the level the attack reaches up to
    float decay = 1.0f;         // time to decay from attack_level to decay_level
    float sustain = 1.0f;       // final level while held
    float release = 0.5f;       // release

    int envelope_stage = 0;  // 0 = holding, 1 = released, -1 to delete

    bool filter_enabled = false;

    // Pulse Width Modulation (in sync with frequency)
    float pwm = 0.0f;
    float pwm_frequency = 1.0f;

    float phase_modulation = 0.0f;
    float phase_modulation_frequency = 1.0f;

    void set_filter_frequency(float filter_frequency) {
        filter.set_cutoff(filter_frequency);
    }

    void set_filter_resonance(float filter_resonance) {
        filter.set_resonance(filter_resonance);
    }

    enum Waveform {
        SIN,     // sine
        SAW,     // saw
        SQUARE,  // square with PWM (normal at 0.5)
        SAW2,    // saw with PWM (normal at 0.5)
        SIN2,    // sine with PWM (normal at 0.5)
        SSAW,    // super saw (saw multiplied with 5th)
        NOISE,   // random noise
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

    // set note where 0 = C4 (9 semitones below A4)
    void set_note(float note_value) {
        // https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
        // (69) 440hz is A4
        // (60) C4 is 9 semitones down

        note_value -= 9;                                     // move the A4 down to C4 .... (so 0 = C4)
        frequency = 440.0f * pow(2.0f, note_value / 12.0f);  // 0 is A4 (440 Hz), the A above middle C
    }

    float _get_envelope1() {
        if (envelope_stage == 0) {  // stage lone when we hold the note
            // decay from start of note

            float position = timer - start_time;

            float decay_env = 1.0f - CLAMP(position / decay, 0.0f, 1.0f);  // decay envelope (decay from start of note)
            decay_env = lerp(attack_level, sustain, decay_env);

            // float attack = parent->attack;
            // float& attack = parent->attack;  // Create a reference to parent's attack

            // attack = parent->attack; // test explode??

            float attack_env = CLAMP(position / attack, 0.0f, 1.0f);  // Use the reference

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

        position *= frequency;

        if (phase_modulation > 0.0) {
            position += phase_modulation * sin(position * Math_TAU * phase_modulation_frequency);  // phase modulation
        }
        float _pulse_width = pulse_width;

        if (pwm > 0.0) {
            _pulse_width += pwm * sin(position * Math_TAU * pwm_frequency);  // pulse width modulation
        }

        switch (waveform) {
            case SIN:
                signal += _SIN(position);
                break;
            case SAW:
                signal += _SAW(position);
                break;
            case SQUARE:
                signal += _SQR(position, _pulse_width);
                break;
            case SAW2:
                signal += _SAW2(position, _pulse_width);
                break;
            case SIN2:
                signal += _SIN2(position, _pulse_width);
                break;
            case SSAW:
                signal += _SAW(position);
                signal *= _SAW(position * 1.5);
                break;
            case NOISE:
                signal += rng->randf() * 2.0 - 1.0;
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
class S1PolySynth {
   public:
    float mix_rate = 44100;  // in hz

    float timer = 0.0f;  // in seconds

    // float start_time = 0.0f;

    float add_level = 0.5f;  // poly add level

    // volume is handled here, not on the channels
    float volume = 1.0f;
    float voume_db = 0.0f;  // linear volume (not db)

    // seperate filters for each channel get these values
    bool filter_enabled = false;
    float filter_frequency = 440.0f;
    float filter_resonance = 1.0f;
    float filter_tracking = 0.0f;

    int test_signal = 0;  // 0 = none, 1 = sine, 2 = noise

    // ADSR
    float attack = 0.125f;  // attack time
    float attack_level = 1.0f;
    float decay = 1.0f;
    float sustain = 0.5f;  // sustain level
    float release = 1.0f;

    float pulse_width = 0.5f;
    int waveform = 0;
    float pwm = 0.0f;
    float pwm_frequency = 1.0f;

    float phase_modulation = 0.0f;
    float phase_modulation_frequency = 1.0f;

    // copy all the vars to the channels
    // i might change this later to have channels reference parent
    // but for now we have circular reference issues that need a different file structre
    void _sync_channel_vars() {
        for (auto& pair : channels) {
            auto& channel = pair.second;  // Use reference to modify the original object

            // ADSR
            channel.attack = attack;
            channel.attack_level = attack_level;
            channel.decay = decay;
            channel.sustain = sustain;
            channel.release = release;

            // pulse width
            channel.pulse_width = pulse_width;
            channel.waveform = waveform;
            channel.pwm = pwm;
            channel.pwm_frequency = pwm_frequency;

            channel.phase_modulation = phase_modulation;
            channel.phase_modulation_frequency = phase_modulation_frequency;

            // filter tracking
            float _filter_frequency = filter_frequency;
            if (filter_tracking > 0.0) {
                float ratio = channel.frequency / 440.0f;
                float _filter_frequency = _filter_frequency * pow(ratio, filter_tracking);
            }
            // filter
            channel.filter_enabled = filter_enabled;
            channel.set_filter_frequency(_filter_frequency);
            channel.set_filter_resonance(filter_resonance);

            // channel.
        }
    }


    // WARNING mix rate just set once
    S1AudioFilter high_pass_filter = S1AudioFilter(10.0f, 44100, S1AudioFilter::HIGH); // my first one
    // S1AudioFilter high_pass_filter = S1AudioFilter(5000.0f, 44100, S1AudioFilter::LOW); // my first one


    // std::unordered_map<float, Note> notes;  // current notes

    std::unordered_map<float, S1PolySynthChannel> channels;  // current notes

    int max_channels = 32;  // -1 infinite

    enum Mode {
        MONO,
        POLY
    };

    Mode mode = Mode::POLY;

    Ref<RandomNumberGenerator> rng;

    S1PolySynth() {
        rng.instantiate();
        rng->set_seed(0);
    }

    // the release enevelope pattern looks a bit complicated
    void add_note(float note, float volume = 1.0f) {
        // auto x = channels.size();

        // Key is not present
        if (channels.find(note) == channels.end()) {
            // print("S1PolySynth add_note: " + String::num(note));

            S1PolySynthChannel channel = S1PolySynthChannel();  // create a new channel
            // _S1PolySynthChannel channel = _S1PolySynthChannel(this);  // create a new channel
            channel.parent = this;  // I WANTED THIS TO BE ENFORCED IN CONSTRUCTOR

            // channel.parent = this;

            channel.set_note(note);
            channel.start_time = timer;

            // channels[note] = channel;
            channels.insert_or_assign(note, channel);

            // print("channels.size: " + godot::String::num(channels.size()));

        } else {
            // print("S1PolySynth FAILED add_note: " + godot::String::num(note));

            S1PolySynthChannel& channel = channels[note];  // channel already present

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

            S1PolySynthChannel& channel = channels[note];

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

             // high pass master to try and stop bottom outs
             // SEEMS TO BE A PROBLEM?? SILENT??
            signal = high_pass_filter.process(signal);

            signal *= volume;
            signal *= pow(10.0, voume_db / 20.0);  // apply volume as decibels (like -12 db for example, 0 is neutral)


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
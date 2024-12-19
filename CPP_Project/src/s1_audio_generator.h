/*

single file audio generator

AudioStreamPlayer based
make an AudioStreamGenerator for this to work


*/
#ifndef S1_AUDIO_GENERATOR_H
#define S1_AUDIO_GENERATOR_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

// #include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/audio_stream.hpp>                     // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_generator.hpp>           // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>  // AudioStreamGeneratorPlayback
#include <godot_cpp/classes/audio_stream_playback.hpp>            // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_player.hpp>              // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_wav.hpp>                 // AudioStreamPlayer
#include <godot_cpp/classes/random_number_generator.hpp>

// #include <godot_cpp/classes/array.hpp> // Array

using namespace godot;

// abstract note structure
// i choose to use a structure with vector's (c++'s version of Lists)
//
// they are lighter than objects it's appropiate apparently but would make little difference
//
struct Note {
    float pitch;   // note value where 0 is middle C (440hz)
    float volume;  // Volume or intensity, typically in the range [0, 1]
    float duration;

    float start_time;

    float get_frequency() {
        return 2.0f * pow(2.0f, pitch / 12.0f);
    }

    // // Constructor
    // Note(float pitch = 440.0f, float volume = 1.0f, float duration = 1.0)
    //     : pitch(pitch), volume(volume),
    // {
    // }
};

// RC-like envelope follower
class AnalogPeakSimulator {
   private:
    float peak;       // Current peak value
    float decayRate;  // How quickly the peak falls

   public:
    AnalogPeakSimulator(float initialPeak = 0.0f, float decay = 0.01f)
        : peak(initialPeak), decayRate(decay) {}

    float process(float input) {
        // Simulate voltage stability (smooth peak-following)
        if (input > peak) {
            peak = input;  // Instant peak rise
        } else {
            peak -= decayRate * (peak - input);  // Smooth decay
        }
        return peak;
    }
};


class ResonantFilter {
   private:
    float cutoff_frequency;    // Cutoff frequency in Hz
    float sample_rate;         // Sample rate in Hz
    float alpha;               // Filter coefficient
    float resonance = 0.0f;    // Resonance amount (0 = no resonance)
    float prev_input = 0.0f;   // Previous input sample
    float prev_output = 0.0f;  // Previous output sample
    bool is_high_pass = true;  // Default to high-pass mode

   public:
    ResonantFilter(float cutoff, float sampleRate, bool highPass = true)
        : cutoff_frequency(cutoff), sample_rate(sampleRate), is_high_pass(highPass) {
        updateAlpha();
    }

    void set_cutoff(float cutoff) {
        cutoff_frequency = cutoff;
        updateAlpha();
    }

    void set_sample_rate(float _sample_rate){
        sample_rate = _sample_rate;
    }

    void set_resonance(float res) {
        resonance = res;
    }

    void set_mode(bool highPass) {
        is_high_pass = highPass;
    }

    float process(float input) {
        float output;
        if (is_high_pass) {
            // High-pass filter with resonance
            output = alpha * (prev_output + input - prev_input) - resonance * prev_output;
        } else {
            // Low-pass filter with resonance
            output = prev_output + alpha * (input - prev_output) - resonance * prev_output;
        }

        // Update state
        prev_input = input;
        prev_output = output;
        return output;
    }

   private:
    void updateAlpha() {
        float rc = 1.0f / (2.0f * Math_PI * cutoff_frequency);
        alpha = rc / (rc + (1.0f / sample_rate));
    }
};

class S1AudioGenerator : public AudioStreamPlayer {
    GDCLASS(S1AudioGenerator, AudioStreamPlayer)

    // // these macros create the variable and also get/set functions
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, enabled, false)     // unused
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mix_rate, 44100)     // samples per a second (hz)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, frequency, 220)    // frequency of tone generator (hz) (default A3)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, pulse_width, 0.5)  // pulse widh of some waves like square
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mode, 4)             // 0=sin 1=square 2=saw 3=wave

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, volume_dB, -24)  // decibels (only for the generator stream, not render)

    // DECLARE_PROPERTY_SINGLE_FILE(Ref<AudioStream>, audio_stream)
    DECLARE_PROPERTY_SINGLE_FILE(Ref<AudioStreamWAV>, audio_stream_wav)  // read and write to this wav file

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, render_length, 1.0)  // length (seconds) of sample to render to wav

    // DECLARE_PROPERTY_SINGLE_FILE(Array, poly_notes)
    // read and write to this wav file .... I THINK MADE A CRASH WHEN WE ADDED IT BUT OKAY!?
    // not sure about the memory saftey as not initilized
    DECLARE_PROPERTY_SINGLE_FILE(PackedFloat32Array, poly_notes)

   private:
    ResonantFilter high_pass_filter = ResonantFilter(10.0f, mix_rate);  // passing above 10Hz prevents bottom outs

   public:
    void macro_test() {
        UtilityFunctions::print("My method called!");

        add_note(0, 1.0, 1.0);
    }

    String macro_test2(const String &input, int number)  // a String doesn't need to be a %reference
    {
        return input + String::num(number);
    }

   protected:
    static void _bind_methods() {
        // // these macros create the bindings for the properties
        CREATE_VAR_BINDINGS(S1AudioGenerator, BOOL, enabled)
        CREATE_VAR_BINDINGS(S1AudioGenerator, INT, mix_rate)
        CREATE_VAR_BINDINGS(S1AudioGenerator, FLOAT, frequency)
        CREATE_VAR_BINDINGS(S1AudioGenerator, FLOAT, pulse_width)
        CREATE_VAR_BINDINGS(S1AudioGenerator, INT, mode)

        CREATE_VAR_BINDINGS(S1AudioGenerator, FLOAT, volume_dB)

        // CREATE_CLASS_BINDINGS(S1AudioGenerator, "AudioStream", audio_stream)
        CREATE_CLASS_BINDINGS(S1AudioGenerator, "AudioStreamWAV", audio_stream_wav)

        CREATE_VAR_BINDINGS(S1AudioGenerator, FLOAT, render_length)

        // CREATE_VAR_BINDINGS(S1AudioGenerator, ARRAY, poly_notes)
        CREATE_VAR_BINDINGS(S1AudioGenerator, Variant::PACKED_FLOAT32_ARRAY, poly_notes)

        // manual binding of methods
        // note the macro pattern is recognised by my gdscript plugins
        ClassDB::bind_method(D_METHOD("macro_test"), &S1AudioGenerator::macro_test);
        ClassDB::bind_method(D_METHOD("macro_generate_wav"), &S1AudioGenerator::macro_generate_wav);

        ClassDB::bind_method(D_METHOD("macro_test2", "input", "number"), &S1AudioGenerator::macro_test2);  // method bind with pars

        // add notes for poly mode
        ClassDB::bind_method(D_METHOD("add_note", "pitch", "volume", "duration"), &S1AudioGenerator::add_note);
        ClassDB::bind_method(D_METHOD("clear_note", "pitch"), &S1AudioGenerator::clear_note);
        ClassDB::bind_method(D_METHOD("clear_notes"), &S1AudioGenerator::clear_notes);

        // this binding pattern seems to crash the compile, still working this out
        // BIND_ENUM_CONSTANT(WAVE::SINE);
        // BIND_ENUM_CONSTANT(WAVE::SQUARE);
        // BIND_ENUM_CONSTANT(WAVE::SAW);
        // BIND_ENUM_CONSTANT(WAVE::WAVE);
    }

   private:
    // declare normals vars
    int counter = 0;
    String test_string = "test_string";
    // but declare Godot objects with a Ref<>
    Ref<RandomNumberGenerator> rng;  // make sure call instantiate() in the constructor!

    // this timer is actually not updated by the process loop exactly, but by each individual frame loaded in
    // "_update_generator_buffer" so it is also moved by the sample saving output
    float timer = 0.0;

    float increment;  // calculate as (frequency / mix_rate)

   public:
    // store the previous sound
    std::vector<float> history_buffer;         // Store the past frames (values)
    int history_buffer_size = mix_rate * 4.0;  // 4 seconds
    int history_buffer_position = 0;

    std::unordered_map<float, Note> notes;

   public:
    // functions for adding notes, which we'll add from gdscript (to handle the controls)
    void add_note(float _note, float volume = 1.0f, float duration = -1) {
        // print("add note " + String::num_real(frequency));

        Note note = Note();
        note.pitch = _note;
        note.volume = volume;

        notes[_note] = note;
    }

    void clear_note(float pitch) {
        notes.erase(pitch);
    }

    void clear_notes() {
        notes.clear();
    }

   public:
    S1AudioGenerator() {
        // high_pass_filter(10.0f, mix_rate);

        // ensure the ref is created, or linked to something (note the . as we access the value type Ref<>)
        rng.instantiate();

        if (rng.is_valid())  // we don't need to check here
            rng->randomize();

        _update_sample_rate();  // trys to copy the sample rate to this object fro the streams

        start_audio_thread();  // start the thread
    };
    ~S1AudioGenerator() {
        stop_audio_thread();
    };

    // void _ready() override {
    // };

    enum WAVE {
        SINE,
        SQUARE,
        SAW,
        WAVE
    };

    // Warmth function using soft clipping
    float apply_warmth(float sample, float drive) {
        // Apply a tanh-based saturation
        return std::tanh(drive * sample);
    }

    // set the mix_rate to the same as the AudioStreamGenerator
    void _update_sample_rate() {
        // get sample rate
        Ref<AudioStream> stream = get_stream();
        if (stream.is_valid()) {
            Ref<AudioStreamGenerator> stream_generator = stream;  // attempt cast
            if (stream_generator.is_valid())
                mix_rate = stream_generator->get_mix_rate();
        }

        high_pass_filter.set_sample_rate(mix_rate);
    }

    void _process(double delta) override {};

    int print_cycle = 60;  // slows the printing down from loops, printing only every 60 frames

    // feed the AudioStreamGeneratorPlayback buffer if we are is_playing()
    void _update_generator_buffer() {
        if (is_playing()) {  // this method needs no cache as we use this node
            counter++;

            Ref<AudioStreamPlayback> playback = get_stream_playback();

            if (playback.is_valid()) {  // if playback is valid

                Ref<AudioStreamGeneratorPlayback> generator_playback = playback;  // attempt cast with ref system
                if (generator_playback.is_valid())                                // if valid
                {
                    int frames_available = generator_playback->get_frames_available();

                    if (frames_available > 0) {
                        // fastest in c++ to pack a buffer at once
                        PackedVector2Array buffer;        // create an stereo audio buffer
                        buffer.resize(frames_available);  // set it's size in one (faster than appending)
                        for (int i = 0; i < frames_available; i++) {
                            auto signal = _get_signal();
                            signal = powf(10.0f, signal / 20.0f);  // apply volume as decibels

                            signal = high_pass_filter.process(signal);  // high pass to stop bottom outs

                            signal = CLAMP(signal, -1.0, 1.0);  // final hard clip

                            buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
                        }
                        generator_playback->push_buffer(buffer);
                    }
                }
            }
        }
    }

    void _physics_process(double delta) override {

        // _update_sample_rate();

        // _update_generator_buffer();
    };

    float SIGNAL_sine()  // 0
    {
        return sin(timer * Math_TAU);
    }

    float SIGNAL_square()  // 1
    {
        float phase = fmod(timer, 1.0f);
        return (phase < pulse_width) ? 1.0f : -1.0f;
    }

    float SIGNAL_saw()  // 2
    {
        // float signal =  UtilityFunctions::floorf(fmod(position, 1.0) + 0.5f);

        float phase = fmod(timer, 1.0f);
        float signal = phase * 2.0f - 1.0f;
        return signal;
    }

    float SIGNAL_special()  // 3
    {
        float signal = SIGNAL_sine();
        signal = apply_warmth(signal, 2.0f);
        return signal;
    }

    float SIGNAL_poly()  // 4
    {
        // float signal = SIGNAL_sine();
        // signal = apply_warmth(signal, 2.0f);
        // return signal;

        float signal = 0.0f;
        float add_level = 1.0f;

        // for (int i = 0; i < poly_notes.size(); i++)
        // {
        //     float note = poly_notes[i];
        // }

        // iterate the map
        for (const auto &pair : notes) {
            float key = pair.first;
            Note note = pair.second;

            float _frequency = note.get_frequency();

            signal += sin(timer * Math_TAU * _frequency) * add_level * note.volume;
        }

        // signal = CLAMP(signal, 0.0f, 1.0f);

        // for (int i = 0; i < notes.size(); i++)
        // {
        //     Note note = notes[i];
        // }

        return signal;
    }

   protected:
    // Helper to decode 16-bit PCM sample from PackedByteArray
    int16_t decode_sample(const PackedByteArray &data, int index) {
        if (index + 1 >= data.size()) {
            return 0;
        }
        // Combine two bytes into a signed 16-bit value
        return static_cast<int16_t>((data[index + 1] << 8) | (data[index]));
    }

   public:
    // read the wav file as a signal
    float SIGNAL_wave() {
        float signal = 0.0;

        if (audio_stream_wav.is_valid()) {
            PackedByteArray data = audio_stream_wav->get_data();

            int format = audio_stream_wav->get_format();

            // int sample_rate = audio_stream_wav->get_mix_rate();

            if (format != AudioStreamWAV::FORMAT_16_BITS) {
                // UtilityFunctions::print("Unsupported format. Only PCM16 is supported.");
                return 0.0f;
            }

            // bool stereo = audio_stream_wav->get_stereo();

            // int channels = audio_stream_wav->get_channels();

            int channels = audio_stream_wav->is_stereo() ? 2 : 1;  // one or two channels

            int bytes_per_sample = 2;  // 16-bit PCM is 2 bytes per sample

            int total_samples = data.size() / (channels * bytes_per_sample);

            // Calculate the sample position
            int sample_pos = static_cast<int>(timer * mix_rate);

            sample_pos %= total_samples;  // should cause a loop?? my bit

            if (sample_pos < 0 || sample_pos >= total_samples - 1) {
                return 0.0f;  // Out of bounds
            }

            // Retrieve the two samples for interpolation
            int sample1_index = sample_pos * channels * bytes_per_sample;
            int sample2_index = (sample_pos + 1) * channels * bytes_per_sample;

            int16_t sample1 = decode_sample(data, sample1_index);
            int16_t sample2 = decode_sample(data, sample2_index);

            // Perform linear interpolation
            float fraction = (timer * mix_rate) - sample_pos;

            return sample1 + (sample2 - sample1) * fraction;

            // int sample_rate = audio_stream_wav->get_sample_rate();
            // int channels = audio_stream_wav->get_channels();
            // int format = audio_stream_wav->get_format();

            // }
        }

        return signal;
    }

    // get current signal (note only call once per cycle as it moves the timer)
    float _get_signal() {
        float signal;

        switch (mode) {
            case -1:
                signal = SIGNAL_wave();
                break;
            case 0:
                signal = SIGNAL_sine();
                break;
            case 1:
                signal = SIGNAL_square();
                break;
            case 2:
                signal = SIGNAL_saw();
                break;
            case 3:
                signal = SIGNAL_special();
                break;
            case 4:
                signal = SIGNAL_poly();
                break;
            default:
                signal = 0.0;
        }

        timer += frequency / mix_rate;

        // history buffer
        if (history_buffer.size() != history_buffer_size) {
            history_buffer.resize(history_buffer_size);
            history_buffer_position = 0;
        }
        history_buffer[history_buffer_position] = signal;
        history_buffer_position += 1;
        history_buffer_position %= history_buffer_size;

        return signal;
    }

    // pack a float to bytes on the wav data (from signal to bytes)
    // used to generate wav
    void pack_float_to_pcm(PackedByteArray &data, float amplitude) {
        // Clamp amplitude to the range [-1.0, 1.0]
        amplitude = std::max(-1.0f, std::min(1.0f, amplitude));

        // Scale to 16-bit signed integer range
        int16_t pcm_sample = static_cast<int16_t>(amplitude * 32767);

        // Append to PackedByteArray in little-endian order
        data.append(pcm_sample & 0xFF);         // Low byte
        data.append((pcm_sample >> 8) & 0xFF);  // High byte
    }

    // array of floats, can remaim overdriven etc
    PackedFloat32Array _generate_audio_wave(int samples) {
        PackedFloat32Array wave;
        wave.resize(samples);
        timer = 0.0;  // WARNING resets the timer
        for (int i = 0; i < samples; i++) {
            float signal = _get_signal();
            wave[i] = signal;
        }
        return wave;
    }

    // 16 bit audio file, 2 bytes per sample, single channel
    PackedByteArray _generate_audio_bytes(int samples) {
        PackedByteArray data;
        data.resize(samples * 2);
        timer = 0.0;  // WARNING resets the timer
        for (int i = 0; i < samples; i++) {
            float signal = _get_signal();
            signal = CLAMP(signal, -1.0f, 1.0f);

            int16_t pcm_sample = static_cast<int16_t>(signal * 32767);  // Scale to 16-bit signed integer range

            // Append to PackedByteArray in little-endian order
            int i2 = i * 2;
            data[i2] = pcm_sample & 0xFF;             // Low byte
            data[i2 + 1] = (pcm_sample >> 8) & 0xFF;  // High byte
        }
        return data;
    }

    // save out a sample to a wav file
    // will delete the existing AudioStreamWAV resource
    void _generate_wav() {
        // increment = frequency / mix_rate;  // calculate increment

        audio_stream_wav.instantiate();  // clears and creates a new wav

        // int samples = mix_rate / 4.0;
        int samples = render_length * mix_rate;

        if (audio_stream_wav.is_valid()) {
            print("audio_stream_wav.is_valid()");

            audio_stream_wav->set_format(AudioStreamWAV::FORMAT_16_BITS);
            audio_stream_wav->set_loop_end(samples);
            audio_stream_wav->set_loop_mode(AudioStreamWAV::LOOP_FORWARD);

            // PackedByteArray data = audio_stream_wav->get_data();

            // data.resize(0);

            // timer = 0.0;

            // for (int i = 0; i < samples; i++) {
            //     float signal = _get_signal();

            //     // INLINED (for possible speed)
            //     signal = std::max(-1.0f, std::min(1.0f, signal));           // Clamp amplitude to the range [-1.0, 1.0]
            //     int16_t pcm_sample = static_cast<int16_t>(signal * 32767);  // Scale to 16-bit signed integer range

            //     // Append to PackedByteArray in little-endian order
            //     data.append(pcm_sample & 0xFF);         // Low byte
            //     data.append((pcm_sample >> 8) & 0xFF);  // High byte

            //     // pack_float_to_pcm(data, signal); // old method

            //     // timer += increment;  // note the timer needs to be moved for all the signals to work
            // }

            audio_stream_wav->set_stereo(false);

            PackedByteArray bytes = _generate_audio_bytes(samples);

            audio_stream_wav->set_data(bytes);
        }
    }

    void macro_generate_wav() {
        render_length = 1.0 / frequency * 8.0;

        _generate_wav();
    }

#pragma region THREADS

   private:
    std::thread audio_thread;
    std::atomic<bool> audio_thread_running = false;
    // Ref<AudioStreamGeneratorPlayback> playback;

   public:
    void start_audio_thread() {
        // print("start_audio_thread...");

        if (audio_thread_running)
            return;  // Avoid starting multiple threads.

        audio_thread_running = true;

        // Start audio thread.
        audio_thread = std::thread([this]() {
            while (audio_thread_running)
            {

                _update_generator_buffer();

                // int frames_available = playback->get_frames_available();

                // for (int i = 0; i < frames_available; i++)
                // {

                //     float signal = _get_signal();
                //     signal = std::pow(10.0f, signal / 20.0f);   // apply volume as decibels
                //     Vector2 frame = Vector2(1.0, 1.0) * signal; // make a stero frame from a mono signal
                //     playback->push_frame(frame);                // push the frame to the buffer
                //     timer += increment;
                // }
            } });
    }

    void stop_audio_thread() {
        print("stop_audio_thread...");
        audio_thread_running = false;
        if (audio_thread.joinable()) {
            audio_thread.join();
        }
    }

#pragma endregion
};

#endif
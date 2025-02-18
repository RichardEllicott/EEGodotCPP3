/*

single file (cpp is inside the header, can cause issues but is easier to edit)


this file contains some code to load the audio buffer, and boilerplate to join to the S1PolySynth (contains the synth itself)

also has a feature to save the audio as a wave file


this object serves as a hook from the C++ to Godot, whereby the UI is coded in GDScript


-make sure to set the stream as an AudioStreamGenerator... this is not done automaticly
-set the buffer on this to a small size like 0.02 to 0.04 ms for a fast response time (too small you might get weird sounds)
-usually you want to set autoplay to true so playback will start on launch

*/
#ifndef POLY_SYNTH_H
#define POLY_SYNTH_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

#include <s1_audio/s1_audio_filter.h>

#include <s1_audio/s1_poly_synth.h>

#include <s1_audio/s2_synth.h>

// #include <s1_audio_filter.h>

#include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer
// #include <godot_cpp/classes/audio_stream_generator.hpp>           // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>  // AudioStreamGeneratorPlayback
// #include <godot_cpp/classes/audio_stream_playback.hpp>            // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_player.hpp>  // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_wav.hpp>     // AudioStreamPlayer
// #include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;

// godot wrapper object
class PolySynth : public AudioStreamPlayer {
    GDCLASS(PolySynth, AudioStreamPlayer)

    // note these are my custom macros (look in macros.h)

    enum Mode {
        SINE_TEST,
        POLY_SYNTH,
    };

#pragma region PROPERTY_MACROS

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mode, 0)  // timer for the signal position

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mix_rate, 44100)           // samples per a second (hz)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, frequency, 220)          // frequency of tone generator (hz) (default A3)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, volume, 1.0f)            // volume db
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, volume_db, 0.0)          // volume db
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, timer, 0.0)              // timer for the signal position
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, render_length, 1.0)      // for rendering to wave example
    DECLARE_PROPERTY_SINGLE_FILE(Ref<AudioStreamWAV>, audio_stream_wav)  // read and write to this wav file

    // ADSR
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, attack, 0.125f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, attack_level, 1.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, decay, 1.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, sustain, 0.5f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, release, 0.125f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, pitch_bend, 1.0f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, pulse_width, 0.5f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, pwm, 0.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, pwm_frequency, 1.0f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, phase_modulation, 0.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, phase_modulation_frequency, 0.0f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, filter_enabled, true)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, filter_frequency, 440.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, filter_resonance, 1.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, filter_tracking, 0.0f)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, waveform, 0)

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, low_pass, 440.0f)

#pragma endregion

   public:
    S1PolySynth poly_synth = S1PolySynth();  // my new synth

    S2Synth synth2 = S2Synth(); // TESTING NEW SYNTH

    // hooks to the synth
    void add_note(float pitch, float volume = 1.0f) {
        // print("S2AudioGenerator add_note: " + godot::String::num(pitch));
        poly_synth.add_note(pitch, volume);
    }

    void clear_note(float pitch) {
        // print("S2AudioGenerator clear_note: " + godot::String::num(pitch));
        poly_synth.clear_note(pitch);
    }

    void clear_notes() {
        // print("clear_notes...");
        poly_synth.clear_notes();
    }

    // send a command, new pattern to try and reduce variables and allow setting up routing
    String send_command(String command) {
        return poly_synth.send_command(command);
    }

   protected:
    static void _bind_methods() {
        // note these are my custom macros (look in macros.h)

#pragma region BINDING_MACROS

        CREATE_VAR_BINDINGS(PolySynth, INT, mode)

        CREATE_VAR_BINDINGS(PolySynth, FLOAT, mix_rate);
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, frequency);
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, volume);
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, volume_db);
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, timer);
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, render_length)

        // ADSR
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, attack)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, attack_level)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, decay)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, sustain)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, release)

        CREATE_VAR_BINDINGS(PolySynth, FLOAT, pitch_bend)

        CREATE_VAR_BINDINGS(PolySynth, FLOAT, pulse_width)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, pwm)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, pwm_frequency)

        CREATE_VAR_BINDINGS(PolySynth, FLOAT, phase_modulation)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, phase_modulation_frequency)

        CREATE_VAR_BINDINGS(PolySynth, BOOL, filter_enabled)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, filter_frequency)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, filter_resonance)
        CREATE_VAR_BINDINGS(PolySynth, FLOAT, filter_tracking)

        CREATE_VAR_BINDINGS(PolySynth, INT, waveform)

        CREATE_CLASS_BINDINGS(PolySynth, "AudioStreamWAV", audio_stream_wav)

#pragma endregion

        ClassDB::bind_method(D_METHOD("macro_generate_wav"), &PolySynth::macro_generate_wav);

        // add notes for poly mode
        ClassDB::bind_method(D_METHOD("add_note", "pitch", "volume"), &PolySynth::add_note);
        ClassDB::bind_method(D_METHOD("clear_note", "pitch"), &PolySynth::clear_note);
        ClassDB::bind_method(D_METHOD("clear_notes"), &PolySynth::clear_notes);

        ClassDB::bind_method(D_METHOD("send_command", "command"), &PolySynth::add_note);

        // Bind the enum constants .... we tried and failed to add enumerator export!
        // just crash??
        // BIND_ENUM_CONSTANT(Mode::MODE1);
        // BIND_ENUM_CONSTANT(Mode::MODE2);
        // BIND_ENUM_CONSTANT(Mode::MODE3);

        // // Expose the property to the editor
        // godot::ClassDB::bind_method(godot::D_METHOD("set_mode", "mode"), &S2AudioGenerator::set_mode);
        // godot::ClassDB::bind_method(godot::D_METHOD("get_mode"), &S2AudioGenerator::get_mode);

        // godot::ClassDB::add_property(
        //     "MyNode",
        //     godot::PropertyInfo(godot::Variant::INT, "mode", godot::PROPERTY_HINT_ENUM, "Low,High,Band,Notch"),
        //     "set_mode", "get_mode");
    }

   public:
    PolySynth() {
        _update_sample_rate();
        start_audio_thread();

        print("test");

        int x = 44;
        if (x = 33){
            print("shit");
        };



    };
    ~PolySynth() {
        stop_audio_thread();
    };

    // set the mix_rate to the same as the AudioStreamGenerator
    // i only do this once, so careful of changing the sample rate
    void _update_sample_rate() {
        // get sample rate
        Ref<AudioStream> stream = get_stream();
        if (stream.is_valid()) {
            Ref<AudioStreamGenerator> stream_generator = stream;  // attempt cast
            if (stream_generator.is_valid())
                mix_rate = stream_generator->get_mix_rate();
        }

        // high_pass_filter.set_sample_rate(mix_rate);
    }

    // we might try and use a reference again later/
    // but it's not too bad doing this as we do it once per a buffer not each frame
    void _sync_vars() {
        // SYNC VARS
        poly_synth.attack = attack;
        poly_synth.attack_level = attack_level;
        poly_synth.decay = decay;
        poly_synth.sustain = sustain;
        poly_synth.release = release;
        poly_synth.pulse_width = pulse_width;
        poly_synth.waveform = waveform;

        poly_synth.pitch_bend = pitch_bend;

        poly_synth.pwm = pwm;
        poly_synth.pwm_frequency = pwm_frequency;

        poly_synth.phase_modulation = phase_modulation;
        poly_synth.phase_modulation_frequency = phase_modulation_frequency;

        poly_synth.filter_enabled = filter_enabled;
        poly_synth.filter_frequency = filter_frequency;
        poly_synth.filter_resonance = filter_resonance;
        poly_synth.filter_tracking = filter_tracking;

        poly_synth.voume_db = volume_db;
        poly_synth.volume = volume;
    }

    // not working yet
    // CircularPackedArrayPool<PackedVector2Array> history_buffer2(44100); // should work but doesn't for some reason
    // CircularPackedArrayPool<PackedVector2Array> history_buffer2 = CircularPackedArrayPool<PackedVector2Array>(44100); // use template??

// contains a recording of the sound for visualisation purposes
// we store the data in a wrapped fasion
#pragma region HISTORY_BUFFER

    int history_buffer_size = mix_rate * 1.0;  // 4 seconds
    PackedVector2ArrayBuffer history_buffer = PackedVector2ArrayBuffer(history_buffer_size);

#pragma endregion

    // get an audio buffer array, stero signal ready to push
    // THIS IS THE MAIN FUNCTION NOW, so using arrays instead of single values (for speed)
    PackedVector2Array _get_audio_buffer(int frames_available) {
        // TODO
        if (history_buffer.size() != history_buffer_size) {
            history_buffer.resize(history_buffer_size);
        }

        _sync_vars();

        PackedVector2Array buffer;  // create an stereo audio buffer

        if (mode == -1) {  // sine wave test

            buffer.resize(frames_available);  // set it's size in one (faster than appending)

            float increment = 1.0 / mix_rate;  // the increment is the time in seconds of one frame

            for (int i = 0; i < frames_available; i++) {
                float signal = sin(timer * Math_TAU * frequency);
                timer += increment;
                signal *= pow(10.0, volume_db / 20.0);   // apply volume as decibels (like -12 db for example, 0 is neutral)
                signal = CLAMP(signal, -1.0, 1.0);       // final hard clip
                buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
            }
        } else if (mode == 0) {  // synth mode

            if (frames_available) {
                // poly_synth.timer = timer;  // we sync these variables
                poly_synth.mix_rate = mix_rate;

                buffer = poly_synth._get_audio_buffer(frames_available);

                for (int i = 0; i < buffer.size(); i++) {
                    history_buffer.add(buffer[i]);
                }

                // timer = poly_synth.timer;  // ensure the timer here matches the synth
            }
        }

        return buffer;
    }
#pragma region UPDATE_BUFFER

    // we save a reference to speedup the lookup, we must always clear this when the playing = false
    // but we take advantage of the fact it stays the same while the object is playing
    Ref<AudioStreamGeneratorPlayback> generator_playback;
    void _update_generator_buffer() {
        if (is_playing()) {  // only run if playing (avoids errors)

            // we only check is_valid once, per buffer update
            if (generator_playback.is_valid()) {
                int frames_available = generator_playback->get_frames_available();  // only pack buffer if frames available

                if (frames_available > 0) {
                    PackedVector2Array buffer = _get_audio_buffer(frames_available);
                    generator_playback->push_buffer(buffer);
                }
            } else {
                // if generator not vaid, use this frame to try and find it
                Ref<AudioStreamPlayback> playback = get_stream_playback();  // attempt to get playback (only do this while playing)
                if (playback.is_valid()) {
                    generator_playback = playback;  // this will cause an implicit cast, if it is an AudioStreamGeneratorPlayback
                }
            }

        } else {
            generator_playback.unref();
        }
    }

#pragma endregion

// threads are better than using _process or _physics_process
#pragma region THREADS

   private:
    std::thread audio_thread;
    std::atomic<bool> audio_thread_running = false;

   public:
    void start_audio_thread() {
        if (audio_thread_running)  // only run one at once
            return;

        audio_thread_running = true;

        // start thread
        audio_thread = std::thread([this]() {
            while (audio_thread_running)
            {
                _update_generator_buffer(); // this thread runs at aboiut 100Hz
            } });
    }

    void stop_audio_thread() {
        audio_thread_running = false;
        if (audio_thread.joinable()) {
            audio_thread.join();
        }
    }

#pragma endregion

#pragma region GENERATE_WAVE

    // generate audio as a PackedByteArray (16byte, single channel, audio file at sample_rate)
    //
    // now deliberatly reusing "_get_audio_buffer", for less errors
    //
    PackedByteArray _generate_audio_bytes(int samples) {
        PackedVector2Array audio_buffer = _get_audio_buffer(samples);  // get from the same route as the playback

        PackedByteArray data;
        data.resize(samples * 2);

        for (int i = 0; i < samples; i++) {
            auto stereo = audio_buffer[i];
            float signal = (stereo.x + stereo.y) / 2.0f;

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
        timer = 0.0;

        int sample_count = render_length * mix_rate;  // calculate total sample count

        audio_stream_wav.instantiate();  // clears and creates a new wav

        if (audio_stream_wav.is_valid()) {  // should be valid anyway (just created)

            audio_stream_wav->set_format(AudioStreamWAV::FORMAT_16_BITS);  // only supporting 16 bit mono
            audio_stream_wav->set_loop_end(sample_count);
            audio_stream_wav->set_loop_mode(AudioStreamWAV::LOOP_FORWARD);
            audio_stream_wav->set_stereo(false);
            audio_stream_wav->set_data(_generate_audio_bytes(sample_count));
        }
    }

    void macro_generate_wav() {
        // render_length = 1.0 / frequency * 8.0;  // 8 cycles

        // render_length = 0.25;

        poly_synth.clear_notes();
        poly_synth.add_note(0);

        _generate_wav();
    }

#pragma endregion
};

#endif
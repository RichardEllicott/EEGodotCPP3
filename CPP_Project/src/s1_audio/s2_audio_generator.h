/*


*/
#ifndef S2_AUDIO_GENERATOR_H
#define S2_AUDIO_GENERATOR_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

#include <s1_audio/s1_audio_filter.h>
// #include <s1_audio_filter.h>

#include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer
// #include <godot_cpp/classes/audio_stream_generator.hpp>           // AudioStreamGenerator
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>  // AudioStreamGeneratorPlayback
// #include <godot_cpp/classes/audio_stream_playback.hpp>            // AudioStreamPlayback
#include <godot_cpp/classes/audio_stream_player.hpp>  // AudioStreamPlayer
#include <godot_cpp/classes/audio_stream_wav.hpp>     // AudioStreamPlayer
// #include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;


class S2AudioGenerator : public AudioStreamPlayer {
    GDCLASS(S2AudioGenerator, AudioStreamPlayer)

    // note these are my custom macros (look in macros.h)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mix_rate, 44100)           // samples per a second (hz)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, frequency, 220)          // frequency of tone generator (hz) (default A3)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, volume_db, -12)          // volume db
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, timer, 0.0)              // timer for the signal position
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, render_length, 1.0)      // for rendering to wave example
    DECLARE_PROPERTY_SINGLE_FILE(Ref<AudioStreamWAV>, audio_stream_wav)  // read and write to this wav file

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, mode, 0)  // timer for the signal position

   public:
    enum Mode {
        MODE1,
        MODE2,
        MODE3
    };

   protected:
    static void _bind_methods() {
        // note these are my custom macros (look in macros.h)
        CREATE_VAR_BINDINGS(S2AudioGenerator, FLOAT, mix_rate);
        CREATE_VAR_BINDINGS(S2AudioGenerator, FLOAT, frequency);
        CREATE_VAR_BINDINGS(S2AudioGenerator, FLOAT, volume_db);
        CREATE_VAR_BINDINGS(S2AudioGenerator, FLOAT, timer);
        CREATE_VAR_BINDINGS(S2AudioGenerator, FLOAT, render_length)
        CREATE_CLASS_BINDINGS(S2AudioGenerator, "AudioStreamWAV", audio_stream_wav)

        ClassDB::bind_method(D_METHOD("macro_generate_wav"), &S2AudioGenerator::macro_generate_wav);

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
    S2AudioGenerator() {
        start_audio_thread();
    };
    ~S2AudioGenerator() {
        stop_audio_thread();
    };

    // signal to generate, you can replace this
    float _get_signal() {
        return sin(timer * Math_TAU * frequency);
    }

    // get an audio buffer array, stero signal ready to push
    PackedVector2Array _get_audio_buffer(int frames_available) {
        PackedVector2Array buffer;  // create an stereo audio buffer

        buffer.resize(frames_available);  // set it's size in one (faster than appending)

        float increment = 1.0 / mix_rate;  // the increment is the time in seconds of one frame

        for (int i = 0; i < frames_available; i++) {
            float signal = _get_signal();
            timer += increment;

            signal *= pow(10.0, volume_db / 20.0);  // apply volume as decibels (like -12 db for example, 0 is neutral)

            // signal = high_pass_filter.process(signal);  // high pass to stop bottom outs

            signal = CLAMP(signal, -1.0, 1.0);  // final hard clip

            buffer[i] = Vector2(1.0, 1.0) * signal;  // set the buffer value
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
        PackedVector2Array audio_buffer = _get_audio_buffer(samples);

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
        render_length = 1.0 / frequency * 8.0;  // 8 cycles

        _generate_wav();
    }

#pragma endregion
};

#endif
/*

simple reverb implemented to test audio effect

reverb is a freeverb from chatgp


*/

#ifndef S1_REVERB_H
#define S1_REVERB_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>
#include <godot_cpp/core/class_db.hpp>
// #include <godot_cpp/core/method_bind_ext.hpp>
#include <vector>

// #include <godot_cpp/core/extension_api.hpp>
#include <gdextension_interface.h>

// C++ random (for portability)
#include <random>

using namespace godot;

#define FREEVERB_ORGINAL  // the working orginal

// class FreeverbInstance

// i was testing this, not using maybe?
class S1AudioBuffer {
   private:
    PackedFloat32Array buffer;

    int add_position = 0;

   public:
    void resize(int size) {
        size = MAX(size, 1);  // size cannot be less than 1
        buffer.resize(size);
    }

    void add_sample(float sample) {
        buffer[add_position] = sample;

        add_position = (add_position + 1) % buffer.size();  // update pointer
    }

    // position 0 would be the last stored sample
    // -1 previous, then -2, -3 etc
    float get_sample(int position = 0) {
        position += add_position - 1;  // makes 0 the last sample (note the -1 matches with add sample)

        position += buffer.size() * 4;  // ensure we don't send negative values through mod

        position %= buffer.size();

        return buffer[position];
    }

    // chat gp suggests this, i'm trying to understand this syntax still, it is required for constants
    // chat gp's actual suggestion doesn't work
    // S1AudioBuffer(int size = 0) : buffer(size) {}
    //
    S1AudioBuffer(int size = 256) {
        resize(size);
    }
};

class S1ReverbInstance : public AudioEffectInstance {
    GDCLASS(S1ReverbInstance, AudioEffectInstance)  // test remove??? we need??

   public:
    float mix_rate = 44100;

    // std::mt19937 rng;  // Mersenne Twister random engine

    Ref<RandomNumberGenerator> rng;

    // test vars
    // DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    // DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

   public:
    float spread = 1.0f;  // i'm adding this

    float feedback = 0.7f;
    float damp = 0.5f;

    float mix = 0.5f;

   private:
    struct CombFilter {
        std::vector<float> buffer;
        int buffer_pos = 0;
        float feedback = 0.7f;
        float damp = 0.5f;
        float damp_prev = 0.0f;
    };

    struct AllPassFilter {
        std::vector<float> buffer;
        int buffer_pos = 0;
        float feedback = 0.5f;
    };

    std::vector<CombFilter> comb_filters;
    std::vector<AllPassFilter> allpass_filters;

    void process_comb_filter(CombFilter &comb, float input, float &output) {
        float delayed = comb.buffer[comb.buffer_pos];
        output += delayed;
        float new_sample = input + delayed * comb.feedback;

        // Damping
        comb.damp_prev = new_sample * (1.0f - comb.damp) + comb.damp_prev * comb.damp;
        comb.buffer[comb.buffer_pos] = comb.damp_prev;

        comb.buffer_pos = (comb.buffer_pos + 1) % comb.buffer.size();
    }

    void process_allpass_filter(AllPassFilter &allpass, float &input) {
        float delayed = allpass.buffer[allpass.buffer_pos];
        float new_sample = input + delayed * allpass.feedback;
        allpass.buffer[allpass.buffer_pos] = new_sample;

        input = delayed - new_sample * allpass.feedback;
        allpass.buffer_pos = (allpass.buffer_pos + 1) % allpass.buffer.size();
    }

   public:
    int mode = 0;

    void update_parameters() {
        // orginal_sizes = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};

        if (!rng.is_valid()) {
            rng.instantiate();
            rng->set_seed(0);
        }

        // print();

        // rng;
        for (int i = 0; i < 8; i++) {
            // auto ran_num = std::uniform_real_distribution<float> dist(0.0, 1.0);
            auto tt = rand();
            print(tt);
        }

        std::vector<float> comb_sizes;  // Declare an empty array

        std::vector<float> allpass_sizes;  // Declare an empty array

        switch (mode) {
            case 1:
                break;
            default:
                comb_sizes = {
                    0.025306122448979593f,
                    0.026938775510204082f,
                    0.028956916099773241f,
                    0.03074829931972789f,
                    0.032244897959183672f,
                    0.03380952380952381f,
                    0.035306122448979592f,
                    0.036666666666666667f};

                allpass_sizes = {556, 441, 341, 225};
        }

        // these are based on 44100hz, copied to seconds delay

        for (float size : comb_sizes) {
            CombFilter comb;
            comb.buffer.resize(lrint(size * mix_rate * spread), 0.0f);  // lrint was used in the godot code, it returns a rounded double int
            comb_filters.push_back(comb);
            comb.feedback = feedback;
            comb.damp = damp;
        }

        for (int size : allpass_sizes) {
            AllPassFilter allpass;
            allpass.buffer.resize(size, 0.0f);
            allpass_filters.push_back(allpass);
        }
    }

    S1ReverbInstance() {
        update_parameters();
    }

   public:
    int comb_sizes[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};

    ~S1ReverbInstance() {
    };

    // the effect encapsulated for reuse by my own mixer as well as Godot's
    float process(float sample) {
        auto wet = 0.0f;

        // Process comb filters in parallel
        for (auto &comb : comb_filters) {
            process_comb_filter(comb, sample, wet);
        }
        // Process all-pass filters in series
        for (auto &allpass : allpass_filters) {
            process_allpass_filter(allpass, wet);
        }

        // Mix dry and wet signals
        float out = sample * (1.0f - mix) + wet * mix;

        return out;
    }

    virtual void _process(const void *src_buffer, AudioFrame *dst_buffer, int frame_count) override {
        const AudioFrame *p_src_frames = static_cast<const AudioFrame *>(src_buffer);

        for (int i = 0; i < frame_count; i++) {
            float dry = (p_src_frames[i].left + p_src_frames[i].right) / 2.0f;  // Mono input

            auto out = process(dry);

            dst_buffer[i].left = out;  // back out
            dst_buffer[i].right = out;
        }
    }

   protected:
    static void _bind_methods() {
        // // these macros create the bindings for the properties
        // CREATE_VAR_BINDINGS(S1ReverbInstance, BOOL, enabled);
        // CREATE_VAR_BINDINGS(S1ReverbInstance, VECTOR2I, grid_size)
    }
};

// the wrapper for the effect
class S1Reverb : public AudioEffect {
    GDCLASS(S1Reverb, AudioEffect)

    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

    // having trouble linking this to the wrapper here
    // DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, spread, 1.0f) // EXPANDED BELOW

   public:
    float get_spread() { return spread; };
    void set_spread(const float p_spread) {
        spread = p_spread;
    };

   private:
    float spread = 1.0f;

   protected:
    static void _bind_methods() {
        CREATE_VAR_BINDINGS(S1Reverb, BOOL, enabled);

        // ADD_GROUP("TestGroup", "predelay_");

        CREATE_VAR_BINDINGS(S1Reverb, VECTOR2I, grid_size)

        CREATE_VAR_BINDINGS(S1Reverb, FLOAT, spread)
    }

   public:
    virtual Ref<AudioEffectInstance> _instantiate() override {
        return Ref<S1ReverbInstance>(memnew(S1ReverbInstance));
    }
};

#endif
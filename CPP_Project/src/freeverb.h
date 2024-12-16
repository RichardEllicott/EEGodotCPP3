/*

simple reverb implemented to test audio effect

reverb is a freeverb from chatgp


*/

#ifndef FREEVERB_H
#define FREEVERB_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>
#include <godot_cpp/core/class_db.hpp>
// #include <godot_cpp/core/method_bind_ext.hpp>
#include <vector>

// #include <godot_cpp/core/extension_api.hpp>
#include <gdextension_interface.h>

using namespace godot;

// class FreeverbInstance

class FreeverbInstance : public AudioEffectInstance {
    GDCLASS(FreeverbInstance, AudioEffectInstance)

    // test vars
    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

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
    float mix = 0.5f;

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
    FreeverbInstance() {
        // Initialize comb filters
        int comb_sizes[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
        for (int size : comb_sizes) {
            CombFilter comb;
            comb.buffer.resize(size, 0.0f);
            comb_filters.push_back(comb);
        }

        // Initialize all-pass filters
        int allpass_sizes[4] = {556, 441, 341, 225};
        for (int size : allpass_sizes) {
            AllPassFilter allpass;
            allpass.buffer.resize(size, 0.0f);
            allpass_filters.push_back(allpass);
        }
    }

   public:
    ~FreeverbInstance() {
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
        CREATE_VAR_BINDINGS(FreeverbInstance, BOOL, enabled);
        CREATE_VAR_BINDINGS(FreeverbInstance, VECTOR2I, grid_size)
    }
};

// the wrapper for the effect
class Freeverb : public AudioEffect {
    GDCLASS(Freeverb, AudioEffect)

    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

   protected:
    static void _bind_methods() {
        CREATE_VAR_BINDINGS(Freeverb, BOOL, enabled);
        CREATE_VAR_BINDINGS(Freeverb, VECTOR2I, grid_size)
    }

   public:
    virtual Ref<AudioEffectInstance> _instantiate() override {
        return Ref<FreeverbInstance>(memnew(FreeverbInstance));
    }
};

#endif
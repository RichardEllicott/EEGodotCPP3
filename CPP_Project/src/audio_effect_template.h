/*

single file audio effect template

*/
#ifndef AUDIO_EFFECT_TEMPLATE_H
#define AUDIO_EFFECT_TEMPLATE_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

// #include <godot_cpp/classes/sprite2d.hpp>
// #include <godot_cpp/classes/random_number_generator.hpp>

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>

using namespace godot;

class AudioEffectTemplateInstance : public AudioEffectInstance {
    GDCLASS(AudioEffectTemplateInstance, AudioEffectInstance)

    // test vars
    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)

   public:
    virtual void _process(const void *src_buffer, AudioFrame *dst_buffer, int frame_count) override {
        const AudioFrame *p_src_frames = static_cast<const AudioFrame *>(src_buffer);

        for (int i = 0; i < frame_count; i++) {

            float dry = p_src_frames[i].left + p_src_frames[i].right;  // Mono input

            float out = dry;

            dst_buffer[i].left = out; // back out
            dst_buffer[i].right = out;
        }
    }

   protected:
    static void _bind_methods() {
        // // these macros create the bindings for the properties
        CREATE_VAR_BINDINGS(AudioEffectTemplateInstance, BOOL, enabled);
        CREATE_VAR_BINDINGS(AudioEffectTemplateInstance, VECTOR2I, grid_size)
    }

   public:
    AudioEffectTemplateInstance() {
    };
    ~AudioEffectTemplateInstance() {
    };
};

// the wrapper for the effect
class AudioEffectTemplate : public AudioEffect {
    GDCLASS(AudioEffectTemplate, AudioEffect)

    DECLARE_PROPERTY_SINGLE_FILE(bool, enabled)
    DECLARE_PROPERTY_SINGLE_FILE(Vector2i, grid_size)


   protected:
    static void _bind_methods() {
        CREATE_VAR_BINDINGS(AudioEffectTemplateInstance, BOOL, enabled);
        CREATE_VAR_BINDINGS(AudioEffectTemplateInstance, VECTOR2I, grid_size)
    }

   public:
    virtual Ref<AudioEffectInstance> _instantiate() override {
        return Ref<AudioEffectTemplateInstance>(memnew(AudioEffectTemplateInstance));
    }
};

#endif
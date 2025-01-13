#ifndef REGISTER_TYPES_CPP
#define REGISTER_TYPES_CPP

#include "register_types.h"

// new modules need and entry here, and the other matching region
#pragma region MODULES

#include "watersim.h"        // water sim example (Eulerian Fluid Simulator)
#include "template.h"        // template, do not modify but copy, included for debug
#include "template_sfile.h"  // testing one file ideas

#include "s1_audio_generator.h"  // single file new one
#include "s1_reverb.h"
#include "sine_example.h"  // single file sine example for testing

#include "s1_audio/poly_synth.h"                // NEW FINAL
#include "s1_audio/s1_poly_synth_visualizer.h"  // visualizer

#include "mesh_generator.h"
// #include "mesh_generator/mesh_generator.h" // can't seem to get it here, probabally the h file pattern


#include "mesh_generator2.h" // trying with cut down one
// #include "mesh_generator/mesh_generator2.h" // CANT GET SUBFOLDER WORKING

#include "cpp_static_library.h"




#pragma endregion



// i think all these are required, but the IDE will not complain
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

// new modules need and entry here, and the other matching region
#pragma region MODULES

    GDREGISTER_CLASS(WaterSim);
    GDREGISTER_CLASS(Template);
    GDREGISTER_CLASS(TemplateSFile);

    GDREGISTER_CLASS(S1AudioGenerator);  // sort in file

    GDREGISTER_CLASS(MeshGenerator);
    GDREGISTER_CLASS(MeshGenerator2); // broken in subfolder


    GDREGISTER_CLASS(SineExample);  // Sine example

    GDREGISTER_CLASS(PolySynth);
    GDREGISTER_CLASS(S1PolySynthVisualizer);

    // my audio effect
    // GDREGISTER_CLASS(Freeverb);
    // GDREGISTER_CLASS(FreeverbInstance);

    GDREGISTER_CLASS(S1Reverb);
    GDREGISTER_CLASS(S1ReverbInstance);

    GDREGISTER_CLASS(CPPStaticLibrary);


    // GDREGISTER_CLASS(AudioEffectTemplate); // seems to be appearing as template
    // GDREGISTER_CLASS(AudioEffectTemplateInstance); // seems to be appearing as template

#pragma endregion
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_example_module);
    init_obj.register_terminator(uninitialize_example_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}

#endif
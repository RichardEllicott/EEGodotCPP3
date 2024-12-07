#ifndef REGISTER_TYPES_CPP
#define REGISTER_TYPES_CPP


#include "register_types.h"

// new modules need and entry here, and the other matching region
#pragma region MODULES 
#include "watersim.h" // water sim example (Eulerian Fluid Simulator)
#include "template.h" // template, do not modify but copy, included for debug
#include "template_sfile.h" // testing one file ideas
#include "sound_generator.h" // sound generator example
#include "mesh_generator.h"
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
	GDREGISTER_CLASS(SoundGenerator);
	GDREGISTER_CLASS(MeshGenerator);
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
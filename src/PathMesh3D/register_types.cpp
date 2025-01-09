#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "path_mesh_3d.hpp"
#include "path_extrude_3d.hpp"

void initialize_path_mesh_3d(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

    GDREGISTER_CLASS(PathMesh3D);
    GDREGISTER_CLASS(PathExtrude3D);
}

void uninitialize_path_mesh_3d(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
    // Initialization.
    GDExtensionBool GDE_EXPORT path_mesh_3d_init(
            GDExtensionInterfaceGetProcAddress p_get_proc_address, 
            const GDExtensionClassLibraryPtr p_library, 
            GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_path_mesh_3d);
        init_obj.register_terminator(uninitialize_path_mesh_3d);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }

}
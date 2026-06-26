use godot::prelude::*;

struct PathMesh3D;

#[gdextension]
unsafe impl ExtensionLibrary for PathMesh3D {}

mod path_tool_3d;
mod path_modifier_3d;

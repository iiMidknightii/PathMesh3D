#include "path_tools/path_static_body_3d.hpp"

using namespace godot;

void PathStaticBody3D::_bind_methods() {
    PathTool3D::_bind_path_tool_3d_methods();
    PathPhysicsTool3D::_bind_path_physics_tool_3d_methods();
}

void PathStaticBody3D::_notification(int p_what) { 
    PathTool3D::_notification_path_tool_3d(p_what);
} 

void PathStaticBody3D::_validate_property(PropertyInfo &p_property) const { 
    PathPhysicsTool3D::_validate_path_physics_tool_3d_property(p_property);
} 

void PathStaticBody3D::_rebuild_mesh() { 
    PathPhysicsTool3D::_rebuild_shape(); 
}
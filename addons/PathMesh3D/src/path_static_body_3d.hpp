#pragma once

#include <godot_cpp/classes/static_body3d.hpp>

#include "path_tool_3d.hpp"
#include "path_physics_tool_3d.hpp"

namespace godot {

class MultiMeshInstance3D;

class PathStaticBody3D : public StaticBody3D, public PathPhysicsTool3D<PathStaticBody3D> {
    GDCLASS(PathStaticBody3D, StaticBody3D)

public:
    PathStaticBody3D *_bake_static_body() const {
        return _bake();
    }
    
protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _validate_property(PropertyInfo &p_property) const;
    virtual void _rebuild_mesh() override final;
};

}

VARIANT_ENUM_CAST(PathStaticBody3D::Distribution)
VARIANT_ENUM_CAST(PathStaticBody3D::Alignment)
VARIANT_ENUM_CAST(PathStaticBody3D::Rotation)
VARIANT_ENUM_CAST(PathStaticBody3D::RelativeTransform)
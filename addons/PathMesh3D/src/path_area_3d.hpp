#pragma once

#include <godot_cpp/classes/area3d.hpp>

#include "path_tool_3d.hpp"
#include "path_physics_tool_3d.hpp"

namespace godot {

class MultiMeshInstance3D;

class PathArea3D : public Area3D, public PathPhysicsTool3D<PathArea3D> {
    GDCLASS(PathArea3D, Area3D)

public:
    PathArea3D *_bake_area() const {
        return _bake();
    }

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _validate_property(PropertyInfo &p_property) const;
    virtual void _rebuild_mesh() override final;
};

} // namespace godot

VARIANT_ENUM_CAST(PathArea3D::Distribution)
VARIANT_ENUM_CAST(PathArea3D::Alignment)
VARIANT_ENUM_CAST(PathArea3D::Rotation)
VARIANT_ENUM_CAST(PathArea3D::RelativeTransform)
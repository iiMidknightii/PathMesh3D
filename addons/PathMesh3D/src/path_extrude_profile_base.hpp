#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

namespace godot {

class PathExtrudeProfileBase : public Resource {
    GDCLASS(PathExtrudeProfileBase, Resource)

public:
    Array get_mesh_arrays() const;
    Array get_end_cap_arrays() const;
    PackedVector2Array get_cross_section() const;
    PackedVector3Array get_end_cap() const;

    void set_flip_normals(const bool p_flip_normals);
    bool get_flip_normals() const;

    void queue_update();
    bool regen_if_dirty();

    GDVIRTUAL0R(Array, _generate_cross_section)
    GDVIRTUAL0R(Array, _generate_end_cap);

protected:
    virtual Array _generate_cross_section();
    virtual Array _generate_end_cap();

    static void _bind_methods();

    PackedFloat64Array _generate_v(const PackedVector2Array &p_vertices);

private:
    bool dirty = true;
    bool flip_normals = false;
    Array mesh_array {PackedVector2Array()};
    Array end_cap_array {PackedVector3Array()};
};

}
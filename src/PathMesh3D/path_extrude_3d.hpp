#ifndef PATH_EXTRUDE_3D_H
#define PATH_EXTRUDE_3D_H

#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

namespace godot {

class PathExtrude3D : public GeometryInstance3D {
    GDCLASS(PathExtrude3D, GeometryInstance3D)

public:
    enum EndCaps {
        END_CAPS_NONE = 0, 
        END_CAPS_START = 1 << 0, 
        END_CAPS_END = 1 << 1, 
        END_CAPS_BOTH = END_CAPS_START | END_CAPS_END
    };

    void set_path_3d(Path3D *p_path);
    Path3D *get_path_3d() const;

    void set_cross_section(PackedVector2Array p_cross_section);
    PackedVector2Array get_cross_section() const;

    void set_smooth(const bool p_smooth);
    bool get_smooth() const;

    void set_tessellation_max_stages(const int32_t p_tessellation_max_stages);
    int32_t get_tessellation_max_stages() const;

    void set_tessellation_tolerance_degrees(const double p_tessellation_tolerance_degrees);
    double get_tessellation_tolerance_degrees() const;

    void set_end_cap_mode(const BitField<EndCaps> p_end_cap_mode);
    BitField<EndCaps> get_end_cap_mode() const;

    void set_offset(const Vector2 p_offset);
    Vector2 get_offset() const;

    void set_sample_cubic(const bool p_cubic);
    bool get_sample_cubic() const;

    void set_tilt(const bool p_tilt);
    bool get_tilt() const;

    void queue_rebuild();

    Ref<ArrayMesh> get_baked_mesh() const;

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    PackedVector2Array cross_section;
    Ref<ArrayMesh> generated_mesh;
    Path3D *path3d = nullptr;

    bool smooth = false;
    int32_t tessellation_max_stages = 5;
    double tessellation_tolerance_degrees = 4.0;
    Vector2 offset = Vector2();
    bool sample_cubic = false;
    bool tilt = true;
    EndCaps end_cap_mode = END_CAPS_BOTH;
    bool dirty = true;

    void _rebuild_mesh();
    void _on_curve_changed();
};
}

VARIANT_BITFIELD_CAST(PathExtrude3D::EndCaps);

#endif
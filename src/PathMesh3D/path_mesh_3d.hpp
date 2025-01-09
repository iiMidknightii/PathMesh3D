#ifndef PATH_MESH_3D_H
#define PATH_MESH_3D_H

#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/pair.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

namespace godot {

class PathMesh3D : public GeometryInstance3D {
    GDCLASS(PathMesh3D, GeometryInstance3D)

public:
    enum Distribution {
        DISTRIBUTE_BY_MODEL_LENGTH,
        DISTRIBUTE_BY_COUNT,
        DISTRIBUTE_MAX
    };
    enum Alignment {
        ALIGN_STRETCH,
        ALIGN_FROM_START,
        ALIGN_CENTERED,
        ALIGN_FROM_END,
        ALIGN_MAX
    };

    void set_path_3d(Path3D *p_path);
    Path3D *get_path_3d() const;

    void set_mesh(const Ref<Mesh> &p_mesh);
    Ref<Mesh> get_mesh() const;

    void set_distribution(uint64_t p_surface_idx, Distribution p_distribution);
    Distribution get_distribution(uint64_t p_surface_idx) const;

    void set_alignment(uint64_t p_surface_idx, Alignment p_alignment);
    Alignment get_alignment(uint64_t p_surface_idx) const;

    void set_count(uint64_t p_surface_idx, uint64_t p_count);
    uint64_t get_count(uint64_t p_surface_idx) const;

    void set_warp_along_curve(uint64_t p_surface_idx, bool p_warp);
    bool get_warp_along_curve(uint64_t p_surface_idx) const;

    void set_sample_cubic(uint64_t p_surface_idx, bool p_cubic);
    bool get_sample_cubic(uint64_t p_surface_idx) const;

    void set_tilt(uint64_t p_surface_idx, bool p_tilt);
    bool get_tilt(uint64_t p_surface_idx) const;

    void set_offset(uint64_t p_surface_idx, Vector2 p_offset);
    Vector2 get_offset(uint64_t p_surface_idx) const;

    void queue_rebuild();
    
    Ref<ArrayMesh> get_baked_mesh() const;

protected:
    static void _bind_methods();
    void _notification(int p_what);
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    bool _set(const StringName &p_name, const Variant &p_property);
	bool _get(const StringName &p_name, Variant &r_property) const;

private:
    Ref<Mesh> source_mesh;
    Ref<ArrayMesh> generated_mesh;
    Path3D *path3d = nullptr;

    struct SurfaceData {
        Distribution distribution = Distribution::DISTRIBUTE_BY_MODEL_LENGTH;
        Alignment alignment = Alignment::ALIGN_STRETCH;
        uint64_t count = 2;
        bool warp_along_curve = true;
        bool cubic = false;
        bool tilt = true;
        Vector2 offset = Vector2();
        bool dirty = true;
    };
    Vector<SurfaceData> surfaces;

    void _queue_surface(uint64_t p_surface_idx);
    void _rebuild_mesh();
    bool _are_any_dirty() const;
    Pair<uint64_t, String> _decode_dynamic_propname(const StringName &p_name) const;
    uint64_t _get_max_count() const;
    void _on_mesh_changed();
    void _on_curve_changed();
};

}

VARIANT_ENUM_CAST(PathMesh3D::Distribution);
VARIANT_ENUM_CAST(PathMesh3D::Alignment);

#endif
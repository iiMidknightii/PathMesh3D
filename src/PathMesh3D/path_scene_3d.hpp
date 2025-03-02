#ifndef PATH_SCENE_3D_HPP
#define PATH_SCENE_3D_HPP

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

namespace godot {

class PathScene3D : public Node3D {
    GDCLASS(PathScene3D, Node3D)

public:
    enum Distribution {
        DISTRIBUTE_BY_COUNT = 0,
        DISTRIBUTE_BY_DISTANCE = 1,
        DISTRIBUTE_MAX = 2,
    };
    enum Rotation {
        ROTATE_FIXED = 0,
        ROTATE_PATH = 1,
        ROTATE_RANDOM = 2,
        ROTATE_MAX = 3,
    };
    enum Alignment {
        ALIGN_FROM_START = 0,
        ALIGN_CENTERED = 1,
        ALIGN_FROM_END = 2,
        ALIGN_MAX = 3,
    };

    _ALWAYS_INLINE_ void set_scene(const Ref<PackedScene> &p_scene) { 
        if (p_scene != scene) {
            if (scene.is_valid() && scene->is_connected("changed", callable_mp(this, &PathScene3D::_on_scene_changed))) {
                scene->disconnect("changed", callable_mp(this, &PathScene3D::_on_scene_changed));
            }
            scene = p_scene;
            if (scene.is_valid()) {
                scene->connect("changed", callable_mp(this, &PathScene3D::_on_scene_changed));
                _on_scene_changed();
            }
        }
    }
    _ALWAYS_INLINE_ Ref<PackedScene> get_scene() const { return scene; }

    _ALWAYS_INLINE_ void set_path_3d(Path3D *p_path) { 
        if (p_path != path3d) {
            if (path3d != nullptr && path3d->is_connected("curve_changed", callable_mp(this, &PathScene3D::_on_curve_changed))) {
                path3d->disconnect("curve_changed", callable_mp(this, &PathScene3D::_on_curve_changed));
            }
            path3d = p_path;
            if (path3d != nullptr) {
                path3d->connect("curve_changed", callable_mp(this, &PathScene3D::_on_curve_changed));
            }
            _on_curve_changed();
        }
    }
    _ALWAYS_INLINE_ Path3D *get_path_3d() const { return path3d; }

    _ALWAYS_INLINE_ void set_distribution(Distribution p_distribution) { 
        if (distribution != p_distribution) {
            distribution = p_distribution;
            queue_rebuild();
            notify_property_list_changed();
        }
    }
    _ALWAYS_INLINE_ Distribution get_distribution() const { return distribution; }

    _ALWAYS_INLINE_ void set_alignment(Alignment p_alignment) { 
        if (alignment != p_alignment) {
            alignment = p_alignment;
            queue_rebuild();
        }
    }
    _ALWAYS_INLINE_ Alignment get_alignment() const { return alignment; }

    _ALWAYS_INLINE_ void set_count(uint64_t p_count) { 
        if (count != p_count) {
            count = p_count;
            queue_rebuild();
        }
    }
    _ALWAYS_INLINE_ uint64_t get_count() const { return count; }

    _ALWAYS_INLINE_ void set_distance(double p_distance) { 
        if (distance != p_distance) {
            distance = p_distance;
            queue_rebuild();
        }
    }
    _ALWAYS_INLINE_ double get_distance() const { return distance; }

    _ALWAYS_INLINE_ void set_rotation_mode(Rotation p_rotation_mode) { 
        if (rotation_mode != p_rotation_mode) {
            rotation_mode = p_rotation_mode;
            queue_rebuild();
            notify_property_list_changed();
        }
    }
    _ALWAYS_INLINE_ Rotation get_rotation_mode() const { return rotation_mode; }

    _ALWAYS_INLINE_ void set_rotation(const Vector3 &p_rotation) { 
        if (rotation != p_rotation) {
            rotation = p_rotation;
            queue_rebuild();
        }
    }
    _ALWAYS_INLINE_ Vector3 get_rotation() const { return rotation; }

    _ALWAYS_INLINE_ void set_sample_cubic(bool p_cubic) { 
        if (sample_cubic != p_cubic) {
            sample_cubic = p_cubic;
            queue_rebuild();
        }
    }
    _ALWAYS_INLINE_ bool get_sample_cubic() const { return sample_cubic; }

    _ALWAYS_INLINE_ void queue_rebuild() { 
        dirty = true;
        callable_mp(this, &PathScene3D::_rebuild_scene).call_deferred();
    }

    TypedArray<Node3D> bake_instances();

    PackedStringArray _get_configuration_warnings() const override;

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _validate_property(PropertyInfo &property) const;

private:
    Ref<PackedScene> scene;
    Path3D *path3d = nullptr;
    Distribution distribution = DISTRIBUTE_BY_COUNT;
    Alignment alignment = ALIGN_FROM_START;
    uint64_t count = 1;
    double distance = 1.0;
    Rotation rotation_mode = ROTATE_PATH;
    Vector3 rotation = Vector3();
    bool sample_cubic = false;
    bool dirty = true;

    Vector<Node3D *> instances;
    Transform3D path_transform = Transform3D();

    void _on_scene_changed();
    void _on_curve_changed();
    void _rebuild_scene();

};

}

VARIANT_ENUM_CAST(godot::PathScene3D::Distribution);
VARIANT_ENUM_CAST(godot::PathScene3D::Rotation);
VARIANT_ENUM_CAST(godot::PathScene3D::Alignment);

#endif
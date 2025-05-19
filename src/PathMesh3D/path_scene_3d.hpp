#ifndef PATH_SCENE_3D_HPP
#define PATH_SCENE_3D_HPP

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

namespace godot {

class PathScene3D : public Node3D {
    GDCLASS(PathScene3D, Node3D)

public:
    enum SceneTransform {
        TRANSFORM_SCENE_LOCAL,
        TRANSFORM_SCENE_PATH_NODE,
        TRANSFORM_SCENE_MAX,
    };
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

    void set_scene(const Ref<PackedScene> &p_scene);
    Ref<PackedScene> get_scene() const;

    void set_path_3d(Path3D *p_path);
    Path3D *get_path_3d() const;

    void set_scene_transform(const SceneTransform p_transform);

    SceneTransform get_scene_transform() const;

    void set_distribution(Distribution p_distribution);
    Distribution get_distribution() const;

    void set_alignment(Alignment p_alignment);
    Alignment get_alignment() const;

    void set_count(uint64_t p_count);
    uint64_t get_count() const;

    void set_distance(double p_distance);
    double get_distance() const;

    void set_rotation_mode(Rotation p_rotation_mode);
    Rotation get_rotation_mode() const;

    void set_rotation(const Vector3 &p_rotation);
    Vector3 get_rotation() const;

    void set_sample_cubic(bool p_cubic);
    bool get_sample_cubic() const;

    void queue_rebuild();

    TypedArray<Node3D> bake_instances();

    PackedStringArray _get_configuration_warnings() const override;

    ~PathScene3D() override;

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _validate_property(PropertyInfo &property) const;

private:
    Ref<PackedScene> scene;
    Path3D *path3d = nullptr;
    SceneTransform scene_transform = TRANSFORM_SCENE_LOCAL;
    Distribution distribution = DISTRIBUTE_BY_COUNT;
    Alignment alignment = ALIGN_FROM_START;
    uint64_t count = 1;
    double distance = 1.0;
    Rotation rotation_mode = ROTATE_PATH;
    Vector3 rotation = Vector3();
    bool sample_cubic = false;
    bool dirty = true;

    Vector<Node3D *> instances;
    Transform3D local_transform = Transform3D();
    Transform3D path_transform = Transform3D();

    void _on_scene_changed();
    void _on_curve_changed();
    void _rebuild_scene();

};

}

VARIANT_ENUM_CAST(PathScene3D::SceneTransform);
VARIANT_ENUM_CAST(PathScene3D::Distribution);
VARIANT_ENUM_CAST(PathScene3D::Rotation);
VARIANT_ENUM_CAST(PathScene3D::Alignment);

#endif
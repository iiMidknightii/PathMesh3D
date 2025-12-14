#pragma once

#include "path_tool_3d.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/defs.hpp"
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh.hpp>

namespace godot {

template <typename T>
class PathPhysicsTool3D  : public PathTool3D<T> {
public:
    enum Distribution {
        DISTRIBUTE_BY_COUNT,
        DISTRIBUTE_BY_DISTANCE,
        DISTRIBUTE_MAX,
    };
    enum Rotation {
        ROTATE_FIXED,
        ROTATE_PATH,
        ROTATE_MAX,
    };
    enum Alignment {
        ALIGN_FROM_START,
        ALIGN_CENTERED,
        ALIGN_FROM_END,
        ALIGN_MAX,
    };

    virtual ~PathPhysicsTool3D() {
        if (shape.is_valid()) {
            if (shape->is_connected("changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathPhysicsTool3D<T>::_on_shape_changed)))) {
                shape->disconnect("changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathPhysicsTool3D<T>::_on_shape_changed)));
            }
            shape.unref();
        }
    }

    void set_shape(const Ref<Shape3D> &p_shape) {
        if (shape != p_shape) {
            if (shape.is_valid()) {
                if (shape->is_connected("changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathPhysicsTool3D<T>::_on_shape_changed)))) {
                    shape->disconnect("changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathPhysicsTool3D<T>::_on_shape_changed)));
                }
            }

            shape = p_shape;

            if (shape.is_valid()) {
                shape->connect("changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathPhysicsTool3D<T>::_on_shape_changed)));
            }

            _on_shape_changed();
        }
    }

    Ref<Shape3D> get_shape() const {
        return shape;
    }

    void set_distribution(Distribution p_distribution) {
        if (distribution != p_distribution) {
            distribution = p_distribution;
            _self()->queue_rebuild();
            _self()->notify_property_list_changed();
        }
    }

    auto get_distribution() const -> Distribution {
        return distribution;
    }

    void set_alignment(Alignment p_alignment) {
        if (alignment != p_alignment) {
            alignment = p_alignment;
            _self()->queue_rebuild();
        }
    }

    auto get_alignment() const -> Alignment{
        return alignment;
    }

    void set_count(uint64_t p_count) {
        if (count != p_count) {
            count = p_count;
            if (distribution == DISTRIBUTE_BY_COUNT) {
                _self()->queue_rebuild();
            }
        }
    }

    uint64_t get_count() const {
        return count;
    }

    void set_distance(double p_distance) {
        if (distance != p_distance) {
            distance = p_distance;
            if (distribution == DISTRIBUTE_BY_DISTANCE) {
                _self()->queue_rebuild();
            }
        }
    }

    double get_distance() const {
        return distance;
    }

    void set_rotation_mode(Rotation p_rotation_mode) {
        if (rotation_mode != p_rotation_mode) {
            rotation_mode = p_rotation_mode;
            _self()->queue_rebuild();
        }
    }

    auto get_rotation_mode() const -> Rotation {
        return rotation_mode;
    }

    void set_rotation(const Vector3 &p_rotation) {
        if (rotation != p_rotation) {
            rotation = p_rotation;
            _self()->queue_rebuild();
        }
    }

    Vector3 get_rotation() const {
        return rotation;
    }

    void set_sample_cubic(bool p_cubic) {
        if (sample_cubic != p_cubic) {
            sample_cubic = p_cubic;
            _self()->queue_rebuild();
        }
    }

    bool get_sample_cubic() const {
        return sample_cubic;
    }

protected:
    static void _bind_path_physics_tool_3d_methods() {
        ClassDB::bind_method(D_METHOD("set_shape", "shape"), static_cast<void (T::*)(const Ref<Shape3D>&)>(&PathPhysicsTool3D::set_shape));
        ClassDB::bind_method(D_METHOD("get_shape"), static_cast<Ref<Shape3D> (T::*)() const>(&PathPhysicsTool3D::get_shape));
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape3D"), "set_shape", "get_shape"); 
        
        ClassDB::bind_method(D_METHOD("set_distribution", "distribution"), static_cast<void (T::*)(Distribution)>(&PathPhysicsTool3D::set_distribution)); 
        ClassDB::bind_method(D_METHOD("get_distribution"), static_cast<Distribution (T::*)() const>(&PathPhysicsTool3D::get_distribution)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "distribution", PROPERTY_HINT_ENUM, "By Count,By Distance"), "set_distribution", "get_distribution"); 
        
        ClassDB::bind_method(D_METHOD("set_alignment", "alignment"), static_cast<void (T::*)(Alignment)>(&PathPhysicsTool3D::set_alignment)); 
        ClassDB::bind_method(D_METHOD("get_alignment"), static_cast<Alignment (T::*)() const>(&PathPhysicsTool3D::get_alignment)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "alignment", PROPERTY_HINT_ENUM, "From Start,Centered,From End"), "set_alignment", "get_alignment"); 
        
        ClassDB::bind_method(D_METHOD("set_count", "count"), static_cast<void (T::*)(uint64_t)>(&PathPhysicsTool3D::set_count)); 
        ClassDB::bind_method(D_METHOD("get_count"), static_cast<uint64_t (T::*)() const>(&PathPhysicsTool3D::get_count)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "count", PROPERTY_HINT_RANGE, "0,1000,1,or_greater"), "set_count", "get_count"); 
        
        ClassDB::bind_method(D_METHOD("set_distance", "distance"), static_cast<void (T::*)(double)>(&PathPhysicsTool3D::set_distance)); 
        ClassDB::bind_method(D_METHOD("get_distance"), static_cast<double (T::*)() const>(&PathPhysicsTool3D::get_distance)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::FLOAT, "distance", PROPERTY_HINT_RANGE, "0.01,1000.0,0.01,or_greater"), "set_distance", "get_distance"); 
        
        ClassDB::bind_method(D_METHOD("set_rotation_mode", "rotation_mode"), static_cast<void (T::*)(Rotation)>(&PathPhysicsTool3D::set_rotation_mode)); 
        ClassDB::bind_method(D_METHOD("get_rotation_mode"), static_cast<Rotation (T::*)() const>(&PathPhysicsTool3D::get_rotation_mode)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "rotation_mode", PROPERTY_HINT_ENUM, "Fixed,Path"), "set_rotation_mode", "get_rotation_mode"); 
        
        ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), static_cast<void (T::*)(const Vector3&)>(&PathPhysicsTool3D::set_rotation)); 
        ClassDB::bind_method(D_METHOD("get_rotation"), static_cast<Vector3 (T::*)() const>(&PathPhysicsTool3D::get_rotation)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::VECTOR3, "rotation"), "set_rotation", "get_rotation"); 
        
        ClassDB::bind_method(D_METHOD("set_sample_cubic", "cubic"), static_cast<void (T::*)(bool)>(&PathPhysicsTool3D::set_sample_cubic)); 
        ClassDB::bind_method(D_METHOD("get_sample_cubic"), static_cast<bool (T::*)() const>(&PathPhysicsTool3D::get_sample_cubic)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::BOOL, "sample_cubic"), "set_sample_cubic", "get_sample_cubic"); 
        
        ClassDB::add_signal(T::get_class_static(), MethodInfo("shape_changed")); 
        
        ClassDB::bind_integer_constant(T::get_class_static(), "Distribution", "DISTRIBUTE_BY_COUNT", DISTRIBUTE_BY_COUNT); 
        ClassDB::bind_integer_constant(T::get_class_static(), "Distribution", "DISTRIBUTE_BY_DISTANCE", DISTRIBUTE_BY_DISTANCE); 
        ClassDB::bind_integer_constant(T::get_class_static(), "Alignment", "ALIGN_FROM_START", ALIGN_FROM_START); 
        ClassDB::bind_integer_constant(T::get_class_static(), "Alignment", "ALIGN_CENTERED", ALIGN_CENTERED); 
        ClassDB::bind_integer_constant(T::get_class_static(), "Alignment", "ALIGN_FROM_END", ALIGN_FROM_END); 
        ClassDB::bind_integer_constant(T::get_class_static(), "Rotation", "ROTATE_FIXED", ROTATE_FIXED);
        ClassDB::bind_integer_constant(T::get_class_static(), "Rotation", "ROTATE_PATH", ROTATE_PATH);
    }

    void _validate_path_physics_tool_3d_property(PropertyInfo &p_property) const {
        if (p_property.name == StringName("count") && get_distribution() == DISTRIBUTE_BY_DISTANCE) { 
            p_property.usage = PROPERTY_USAGE_NONE; 
        } else if (p_property.name == StringName("distance") && get_distribution() == DISTRIBUTE_BY_COUNT) { 
            p_property.usage = PROPERTY_USAGE_NONE; 
        } 
    }

    T *_bake() const {
        T *out = memnew(T);
        Ref<Shape3D> new_shape = shape->duplicate();

        for (const uint32_t owner_id : owners) {
            CollisionShape3D *new_obj = memnew(CollisionShape3D);
            new_obj->set_shape(new_shape);
            out->add_child(new_obj);
            new_obj->set_transform(_self()->shape_owner_get_transform(owner_id));
            if (out->get_owner() != nullptr) {
                new_obj->set_owner(out->get_owner());
            }
        }

        return out;
    }

    void _rebuild_shape() { 
        for (const uint32_t owner_id : owners) { 
            _self()->shape_owner_clear_shapes(owner_id); 
            _self()->remove_shape_owner(owner_id); 
        } 
        owners.clear(); 
        
        MultiMeshInstance3D *collision_debug = _get_collision_debug();
        if (collision_debug != nullptr) { 
            _self()->remove_child(collision_debug); 
            collision_debug->queue_free(); 
            collision_debug_id = 0;
        } 
        
        Path3D *path3d = _self()->get_path_3d(); 
        if (path3d == nullptr || path3d->get_curve().is_null() || !path3d->is_inside_tree() || !shape.is_valid()) { 
            return; 
        } 
        
        Transform3D mod_transform = _self()->_get_final_transform(); 
        
        Ref<Curve3D> curve = path3d->get_curve(); 
        if (curve->get_point_count() < 2) { 
            return; 
        } 
        
        double baked_l = curve->get_baked_length(); 
        if (baked_l == 0.0) { 
            return; 
        } 
        
        uint64_t n_instances = 0; 
        double separation = 0.0; 
        switch (distribution) { 
            case DISTRIBUTE_BY_COUNT: { 
                n_instances = count; 
                separation = baked_l / (count - 1); 
            } break; 
            case DISTRIBUTE_BY_DISTANCE: { 
                separation = distance; 
                n_instances = Math::floor(baked_l / separation) + 1; 
            } break; 
            default: 
                ERR_FAIL(); 
        } 
        
        double offset = 0.0; 
        if (distribution != DISTRIBUTE_BY_COUNT) { 
            switch (alignment) { 
                case ALIGN_FROM_START: { 
                    offset = 0.0; 
                } break; 
                case ALIGN_CENTERED: { 
                    offset = (baked_l - separation * (n_instances - 1)) * 0.5; 
                } break; 
                case ALIGN_FROM_END: { 
                    offset = baked_l - separation * (n_instances - 1); 
                } break; 
                default: 
                    ERR_FAIL(); 
            } 
        } 
        
        bool debugging = Engine::get_singleton()->is_editor_hint() || _self()->get_tree()->is_debugging_collisions_hint(); 
        
        if (debugging) { 
            MultiMeshInstance3D *collision_debug = memnew(MultiMeshInstance3D); 
            collision_debug_id = collision_debug->get_instance_id(); 
            collision_debug->set_multimesh(memnew(MultiMesh)); 
            collision_debug->get_multimesh()->set_transform_format(MultiMesh::TRANSFORM_3D); 
            collision_debug->get_multimesh()->set_instance_count(n_instances); 
            collision_debug->get_multimesh()->set_mesh(shape->get_debug_mesh()); 
            _self()->add_child(collision_debug, false, _self()->INTERNAL_MODE_BACK); 
        } 
        
        owners.resize(n_instances); 
        for (uint64_t i = 0; i < n_instances; ++i) { 
            Transform3D transform; 
            switch (rotation_mode) { 
                case ROTATE_FIXED: { 
                    transform.origin = curve->sample_baked(offset, sample_cubic); 
                    transform.basis = Basis::from_euler(rotation); 
                } break; 
                case ROTATE_PATH: { 
                    transform = curve->sample_baked_with_rotation(offset, sample_cubic, true); 
                    transform.basis.rotate(rotation); 
                } break; 
                default: 
                    ERR_FAIL(); 
            } 
            
            transform *= _self()->_sample_3d_modifiers_at(offset / baked_l); 
            
            if (_self()->_get_relative_transform() == _self()->TRANSFORM_PATH_NODE) { 
                transform = mod_transform * transform; 
            } 
            
            if (debugging) { 
                MultiMeshInstance3D *collision_debug = _get_collision_debug();
                if (collision_debug != nullptr) { 
                    collision_debug->get_multimesh()->set_instance_transform(i, transform); 
                } 
            } 
            
            uint32_t owner_id = _self()->create_shape_owner(_self()); 
            _self()->shape_owner_add_shape(owner_id, shape); 
            _self()->shape_owner_set_transform(owner_id, transform); 
            owners[i] = owner_id; 
            offset += separation; 
        } 
    } 

    void _on_shape_changed() { 
        _self()->queue_rebuild(); 
        _self()->emit_signal("shape_changed"); 
    }

private:
    Ref<Shape3D> shape;
    Distribution distribution = DISTRIBUTE_BY_COUNT;
    Alignment alignment = ALIGN_FROM_START;
    uint64_t count = 1;
    double distance = 1.0;
    Rotation rotation_mode = ROTATE_FIXED;
    Vector3 rotation = Vector3();
    bool sample_cubic = false;

    LocalVector<uint32_t> owners;
    uint64_t collision_debug_id = 0;

    _FORCE_INLINE_ T *_self() {
        return static_cast<T *>(this);
    }
    _FORCE_INLINE_ const T *_self() const {
        return static_cast<const T *>(this);
    }

    _FORCE_INLINE_ MultiMeshInstance3D *_get_collision_debug() const {
        return Object::cast_to<MultiMeshInstance3D>(ObjectDB::get_instance(collision_debug_id));
    }
};

}
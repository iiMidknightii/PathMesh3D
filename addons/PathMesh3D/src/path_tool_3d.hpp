#pragma once

#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

#include "godot_cpp/classes/curve3d.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "path_modifier_3d.hpp"

namespace godot {

class PathToolInterface {

public:
    virtual void set_path_3d(Path3D *p_path) = 0;
    virtual Path3D *get_path_3d() const = 0;

    Ref<Curve3D> get_curve_3d() const {
        Path3D *path3d_node = get_path_3d();
        if (path3d_node != nullptr) {
            return path3d_node->get_curve();
        } else {
            return Ref<Curve3D>();
        }
    }

    virtual void register_modifier(PathModifier3D *p_modifier) = 0;
    virtual void unregister_modifier(PathModifier3D *p_modifier) = 0;
};

template <typename T>
class PathTool3D : public PathToolInterface {

public:
    enum RelativeTransform { 
        TRANSFORM_LOCAL, 
        TRANSFORM_PATH_NODE,
        TRANSFORM_MAX,
    };

    virtual ~PathTool3D() {
        Path3D *path3d_node = get_path_3d(); 
        if (path3d_node != nullptr && path3d_node->is_connected("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed)))) { 
            path3d_node->disconnect("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed))); 
        }
        modifiers.clear();
    }

    _FORCE_INLINE_ void set_path_3d(Path3D *p_path) {
        ERR_FAIL_NULL(p_path);

        if (p_path->get_instance_id() != path3d_id) {
            Path3D *old_node = get_path_3d();
            if (old_node != nullptr && old_node->is_connected("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed)))) { 
                old_node->disconnect("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed))); 
            } 
            
            path3d_id = p_path->get_instance_id(); 
            
            if (p_path != nullptr && !p_path->is_connected("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed)))) { 
                p_path->connect("curve_changed", callable_mp(_self(), static_cast<void (T::*)()>(&PathTool3D<T>::_on_curve_changed))); 
            }
            
            _on_curve_changed();
        } 
    } 
    
    _FORCE_INLINE_ Path3D *get_path_3d() const {
        return Object::cast_to<Path3D>(ObjectDB::get_instance(path3d_id));
    }
    
    _FORCE_INLINE_ void set_relative_transform(const RelativeTransform p_transform) { 
        if (relative_transform != p_transform) { 
            relative_transform = p_transform; 
            queue_rebuild(); 
        } 
    } 
    
    _FORCE_INLINE_ RelativeTransform get_relative_transform() const {
        return relative_transform;
    }
    
    _FORCE_INLINE_ void register_modifier(PathModifier3D *p_modifier) { 
        if (!modifiers.has(p_modifier)) { 
            modifiers.push_back(p_modifier); 
            queue_rebuild(); 
        } 
    } 
    
    _FORCE_INLINE_ void unregister_modifier(PathModifier3D *p_modifier) { 
        if (modifiers.has(p_modifier)) { 
            modifiers.erase(p_modifier); 
            queue_rebuild(); 
        } 
    } 
    
    _FORCE_INLINE_ void queue_rebuild() {
        dirty = true;
    }

protected:
    static void _bind_path_tool_3d_methods() {
        ClassDB::bind_method(D_METHOD("queue_rebuild"), static_cast<void (T::*)()>(&PathTool3D::queue_rebuild)); 

        ClassDB::bind_method(D_METHOD("set_path_3d", "path"), static_cast<void (T::*)(Path3D *)>(&PathTool3D::set_path_3d));
        ClassDB::bind_method(D_METHOD("get_path_3d"), static_cast<Path3D *(T::*)() const>(&PathTool3D::get_path_3d));
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::OBJECT, "path_3d", PROPERTY_HINT_NODE_TYPE, "Path3D"), "set_path_3d", "get_path_3d");
        
        ClassDB::bind_method(D_METHOD("set_relative_transform", "transform"), static_cast<void (T::*)(RelativeTransform)>(&PathTool3D::set_relative_transform)); 
        ClassDB::bind_method(D_METHOD("get_relative_transform"), static_cast<RelativeTransform (T::*)() const>(&PathTool3D::get_relative_transform)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "relative_transform", PROPERTY_HINT_ENUM, "Transform to Self,Transform to Path Node"), "set_relative_transform", "get_relative_transform"); 
        
        ClassDB::add_signal(T::get_class_static(), MethodInfo("curve_changed")); 
        
        ClassDB::bind_integer_constant(T::get_class_static(), "RelativeTransform", "TRANSFORM_LOCAL", TRANSFORM_LOCAL); 
        ClassDB::bind_integer_constant(T::get_class_static(), "RelativeTransform", "TRANSFORM_PATH_NODE", TRANSFORM_PATH_NODE);
    }

    void _notification_path_tool_3d(int p_what) {
        switch (p_what) {
            case Node::NOTIFICATION_READY: {
                _self()->set_process_internal(true);
                _rebuild_mesh();
            } break;

            case Node::NOTIFICATION_INTERNAL_PROCESS: {
                if (_pop_should_rebuild()) {
                    _rebuild_mesh();
                }
            } break;
        }
    }

    virtual void _rebuild_mesh() = 0;

    virtual bool _pop_should_rebuild() {
        return _pop_is_dirty();
    }

    _FORCE_INLINE_ bool _pop_is_dirty() { 
        bool is_dirty = dirty; 
        if (!is_dirty && relative_transform == TRANSFORM_PATH_NODE) { 
            Transform3D tmp = _self()->get_global_transform(); 
            is_dirty = is_dirty || self_transform != tmp; 
            self_transform = tmp; 
            Path3D *path3d_node = get_path_3d(); 
            if (path3d_node != nullptr) { 
                tmp = path3d_node->get_global_transform(); 
                is_dirty = is_dirty || path_transform != tmp; 
                path_transform = tmp; 
            } 
        } 
    
        for (PathModifier3D *modifier : modifiers) { 
            is_dirty = is_dirty || modifier->pop_is_dirty(); 
        } 
    
        dirty = false; 
        return is_dirty; 
    }

    _FORCE_INLINE_ RelativeTransform _get_relative_transform() const {
        return relative_transform;
    }
    
    _FORCE_INLINE_ Transform3D _get_final_transform() const { 
        return self_transform.affine_inverse() * path_transform; 
    } 
    
    _FORCE_INLINE_ Transform3D _sample_3d_modifiers_at(real_t p_offset_ratio) const { 
        Transform3D out; 
        for (PathModifier3D *modifier : modifiers) { 
            out *= modifier->sample_3d_modifier_at(p_offset_ratio); 
        } 
        return out; 
    }
    
    _FORCE_INLINE_ Transform2D _sample_uv_modifiers_at(real_t p_offset_ratio) const { 
        Transform2D out; 
        for (PathModifier3D *modifier : modifiers) { 
            out *= modifier->sample_uv_modifier_at(p_offset_ratio); 
        } 
        return out; 
    }
    
private:
    uint64_t path3d_id = 0; 
    RelativeTransform relative_transform = TRANSFORM_LOCAL; 
    Transform3D self_transform; 
    Transform3D path_transform; 
    LocalVector<PathModifier3D *> modifiers; 
    bool dirty = true; 

private:
    _FORCE_INLINE_ void _on_curve_changed() { 
        queue_rebuild(); 
        _self()->emit_signal("curve_changed"); 
    } 

    _FORCE_INLINE_ T *_self() {
        return static_cast<T *>(this);
    }
    _FORCE_INLINE_ const T *_self() const {
        return static_cast<const T *>(this);
    }
};

}
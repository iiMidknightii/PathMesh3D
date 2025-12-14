#pragma once

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/defs.hpp"
#include <godot_cpp/classes/engine.hpp>
#include "path_tool_3d.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>
#include <godot_cpp/classes/mesh_convex_decomposition_settings.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/templates/local_vector.hpp>

namespace godot {

template <typename T>
class PathCollisionTool3D : public PathTool3D<T> {
public:
    enum CollisionMode {
        COLLISION_MODE_NONE = 0,
        COLLISION_MODE_TRIMESH,
        COLLISION_MODE_CONVEX,
        COLLISION_MODE_MULTIPLE_CONVEX,
    };

    _FORCE_INLINE_ void set_generate_collision(const CollisionMode p_mode) {
        if (collision_mode != p_mode) {
            collision_mode = p_mode;
            queue_rebuild_collision();
            _self()->notify_property_list_changed();
        }
    }

    _FORCE_INLINE_ CollisionMode get_generate_collision() const {
        return collision_mode;
    }

    _FORCE_INLINE_ void set_convex_collision_clean(bool p_clean) {
        if (convex_collision_clean != p_clean) {
            convex_collision_clean = p_clean;
            queue_rebuild_collision();
        }
    }

    _FORCE_INLINE_ bool get_convex_collision_clean() const {
        return convex_collision_clean;
    }

    _FORCE_INLINE_ void set_convex_collision_simplify(bool p_simplify) {
        if (convex_collision_simplify != p_simplify) {
            convex_collision_simplify = p_simplify;
            queue_rebuild_collision();
        }
    }

    _FORCE_INLINE_ bool get_convex_collision_simplify() const {
        return convex_collision_simplify;
    }

    _FORCE_INLINE_ Node *get_collision_node() const {
        return Object::cast_to<Node>(ObjectDB::get_instance(collision_node_id));
    }

    _FORCE_INLINE_ Node *get_collision_debug_node() const {
        return Object::cast_to<Node>(ObjectDB::get_instance(collision_debug_id));
    }

    _FORCE_INLINE_ void queue_rebuild_collision() {
        collision_dirty = true;
    }

    _FORCE_INLINE_ void set_collision_layer(uint32_t p_layer) {
        Node *collision_node = get_collision_node();
        if (collision_layer != p_layer && collision_node != nullptr) {
            PhysicsBody3D *body = Object::cast_to<PhysicsBody3D>(collision_node);
            if (body != nullptr) {
                collision_layer = p_layer;
                body->set_collision_layer(collision_layer);
            }
        }
    }

    _FORCE_INLINE_ uint32_t get_collision_layer() const {
        return collision_layer;
    }

    _FORCE_INLINE_ void set_collision_mask(uint32_t p_mask) {
        Node *collision_node = get_collision_node();
        if (collision_mask != p_mask && collision_node != nullptr) {
            PhysicsBody3D *body = Object::cast_to<PhysicsBody3D>(collision_node);
            if (body != nullptr) {
                collision_mask = p_mask;
                body->set_collision_mask(collision_mask);
            }
        }
    }

    _FORCE_INLINE_ uint32_t get_collision_mask() const {
        return collision_mask;
    }

    _FORCE_INLINE_ Node *create_trimesh_collision_node() {
        return _setup_collision_node(_get_mesh()->create_trimesh_shape());
    }

    _FORCE_INLINE_ void create_trimesh_collision() {
        _add_child_collision_node(create_trimesh_collision_node());
    }

    _FORCE_INLINE_ Node *create_convex_collision_node(bool p_clean = true, bool p_simplify = false) {
        return _setup_collision_node(_get_mesh()->create_convex_shape(p_clean, p_simplify));
    }

    _FORCE_INLINE_ void create_convex_collision(bool p_clean = true, bool p_simplify = false) {
        _add_child_collision_node(create_convex_collision_node(p_clean, p_simplify));
    }

    _FORCE_INLINE_ Node *create_multiple_convex_collision_node(const Ref<MeshConvexDecompositionSettings> &p_settings = nullptr) {
        Ref<MeshConvexDecompositionSettings> settings = p_settings;
        if (!settings.is_valid()) {
            settings.instantiate();
        }

        /* TODO: GDExtension doesn't have API parity here... */
        LocalVector<Ref<Shape3D>> shapes; // = _self()->_get_mesh()->convex_decompose(settings);
        if (shapes.is_empty()) {
            return nullptr;
        }

        StaticBody3D *static_body = memnew(StaticBody3D);
        for (int i = 0; i < shapes.size(); ++i) {
            CollisionShape3D *cshape = memnew(CollisionShape3D);
            cshape->set_shape(shapes[i]);
            static_body->add_child(cshape, true);
        }
        return static_body;
    }

    _FORCE_INLINE_ void create_multiple_convex_collision(const Ref<MeshConvexDecompositionSettings> &p_settings = nullptr) {
        _add_child_collision_node(create_multiple_convex_collision_node(p_settings));
    }

protected:
    static void _bind_path_collision_tool_3d_methods() {
        ClassDB::add_property_group(T::get_class_static(), "Collision", "");    
        ClassDB::bind_method(D_METHOD("set_generate_collision", "mode"), static_cast<void (T::*)(CollisionMode)>(&PathCollisionTool3D::set_generate_collision));   
        ClassDB::bind_method(D_METHOD("get_generate_collision"), static_cast<CollisionMode (T::*)() const>(&PathCollisionTool3D::get_generate_collision));   
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "collision_mode", PROPERTY_HINT_ENUM, "None,Trimesh,Convex,Multiple Convex"), "set_generate_collision", "get_generate_collision");  
        
        ClassDB::bind_method(D_METHOD("set_convex_collision_clean", "clean"), static_cast<void (T::*)(bool)>(&PathCollisionTool3D::set_convex_collision_clean));  
        ClassDB::bind_method(D_METHOD("get_convex_collision_clean"), static_cast<bool (T::*)() const>(&PathCollisionTool3D::get_convex_collision_clean));   
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::BOOL, "convex_collision_clean"), "set_convex_collision_clean", "get_convex_collision_clean");    
        
        ClassDB::bind_method(D_METHOD("set_convex_collision_simplify", "simplify"), static_cast<void (T::*)(bool)>(&PathCollisionTool3D::set_convex_collision_simplify)); 
        ClassDB::bind_method(D_METHOD("get_convex_collision_simplify"), static_cast<bool (T::*)() const>(&PathCollisionTool3D::get_convex_collision_simplify)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::BOOL, "convex_collision_simplify"), "set_convex_collision_simplify", "get_convex_collision_simplify");   
        
        ClassDB::bind_method(D_METHOD("set_collision_layer", "layer"), static_cast<void (T::*)(uint32_t)>(&PathCollisionTool3D::set_collision_layer));    
        ClassDB::bind_method(D_METHOD("get_collision_layer"), static_cast<uint32_t (T::*)() const>(&PathCollisionTool3D::get_collision_layer)); 
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_layer", "get_collision_layer"); 
        
        ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), static_cast<void (T::*)(uint32_t)>(&PathCollisionTool3D::set_collision_mask));   
        ClassDB::bind_method(D_METHOD("get_collision_mask"), static_cast<uint32_t (T::*)() const>(&PathCollisionTool3D::get_collision_mask));   
        ClassDB::add_property(T::get_class_static(), PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");    
        
        ClassDB::bind_method(D_METHOD("create_trimesh_collision"), static_cast<void (T::*)()>(&PathCollisionTool3D::create_trimesh_collision));   
        ClassDB::bind_method(D_METHOD("create_convex_collision", "clean", "simplify"), static_cast<void (T::*)(bool, bool)>(&PathCollisionTool3D::create_convex_collision), DEFVAL(true), DEFVAL(false));   
        ClassDB::bind_method(D_METHOD("create_multiple_convex_collision", "settings"), static_cast<void (T::*)(const Ref<MeshConvexDecompositionSettings>&)>(&PathCollisionTool3D::create_multiple_convex_collision), DEFVAL(nullptr));  
        
        ClassDB::bind_integer_constant(T::get_class_static(), "CollisionMode", "COLLISION_MODE_NONE", COLLISION_MODE_NONE); 
        ClassDB::bind_integer_constant(T::get_class_static(), "CollisionMode", "COLLISION_MODE_TRIMESH", COLLISION_MODE_TRIMESH);   
        ClassDB::bind_integer_constant(T::get_class_static(), "CollisionMode", "COLLISION_MODE_CONVEX", COLLISION_MODE_CONVEX); 
        ClassDB::bind_integer_constant(T::get_class_static(), "CollisionMode", "COLLISION_MODE_MULTIPLE_CONVEX", COLLISION_MODE_MULTIPLE_CONVEX);
    }

    void _notification_path_collision_tool_3d(int p_what) {
        switch (p_what) {
            case Node::NOTIFICATION_READY: {
                _self()->set_process_internal(true);
                _rebuild_collision_node();
            } break;

            case Node::NOTIFICATION_INTERNAL_PROCESS: {
                if (_self()->_pop_is_collision_dirty()) {
                    _rebuild_collision_node();
                }
            } break;
        }
    }

    void _validate_path_collision_tool_3d_property(PropertyInfo &p_property) const {
        if (p_property.name == StringName("convex_collision_clean") || p_property.name == StringName("convex_collision_simplify")) {
            p_property.usage = get_generate_collision() == COLLISION_MODE_CONVEX ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
        } else if (p_property.name == StringName("collision_layer") || p_property.name == StringName("collision_mask")) {
            p_property.usage = get_generate_collision() == COLLISION_MODE_NONE ? PROPERTY_USAGE_NONE : PROPERTY_USAGE_DEFAULT;
        }
    }

    virtual Ref<ArrayMesh> _get_mesh() const = 0;

    _FORCE_INLINE_ bool _pop_is_collision_dirty() {
        bool is_dirty = collision_dirty;
        collision_dirty = false;
        return is_dirty;
    }

    _FORCE_INLINE_ void _clear_collision_node() {
        Node *collision_node = get_collision_node();
        if (collision_node != nullptr) {
            _self()->remove_child(collision_node);
            collision_node->queue_free();
            collision_node = nullptr;
            collision_node_id = 0;
        }

        Node *collision_debug = get_collision_debug_node();
        if (collision_debug != nullptr) {
            _self()->remove_child(collision_debug);
            collision_debug->queue_free();
            collision_debug = nullptr;
            collision_debug_id = 0;
        }
    }

    _FORCE_INLINE_ void _rebuild_collision_node() {
        _clear_collision_node();

        Node *collision_node = nullptr;

        switch (collision_mode) {
            case COLLISION_MODE_TRIMESH:
                collision_node = create_trimesh_collision_node();
                break;
            case COLLISION_MODE_CONVEX:
                collision_node = create_convex_collision_node(convex_collision_clean, convex_collision_simplify);
                break;
            case COLLISION_MODE_MULTIPLE_CONVEX:
                collision_node = create_multiple_convex_collision_node();
                break;
            default:
                break;
        }

        if (collision_node != nullptr) {
            collision_node_id = collision_node->get_instance_id();

            _self()->add_child(collision_node, false, _self()->INTERNAL_MODE_BACK);

            if (_self()->get_tree()->is_debugging_collisions_hint()) {
                MeshInstance3D *collision_debug = memnew(MeshInstance3D);
                collision_debug_id = collision_debug->get_instance_id();

                CollisionShape3D *cshape = Object::cast_to<CollisionShape3D>(collision_node->get_child(0));
                if (cshape != nullptr) {
                    Ref<Shape3D> shape = cshape->get_shape();
                    if (shape.is_valid()) {
                        collision_debug->set_mesh(shape->get_debug_mesh());
                        _self()->add_child(collision_debug, false, _self()->INTERNAL_MODE_BACK);
                    } else {
                        collision_debug->queue_free();
                        collision_debug_id = 0;
                    }
                } else {
                    collision_debug->queue_free();
                    collision_debug_id = 0;
                }
            }
        }
    }

private:
    CollisionMode collision_mode = COLLISION_MODE_NONE;
    bool convex_collision_clean = true;
    bool convex_collision_simplify = false;
    bool collision_dirty = false;
    uint64_t collision_node_id = 0;
    uint64_t collision_debug_id = 0;
    uint32_t collision_layer = 1;
    uint32_t collision_mask = 1;

    _FORCE_INLINE_ Node *_setup_collision_node(const Ref<Shape3D> &shape) {
        StaticBody3D *static_body = memnew(StaticBody3D);
        CollisionShape3D *cshape = memnew(CollisionShape3D);
        cshape->set_shape(shape);
        static_body->add_child(cshape, true);
        return static_body;
    }

    _FORCE_INLINE_ void _add_child_collision_node(Node *p_node) {
        if (p_node != nullptr) {
            _self()->add_child(p_node, true);
            PhysicsBody3D *body = Object::cast_to<PhysicsBody3D>(p_node);
            if (body != nullptr) {
                body->set_collision_layer(collision_layer);
                body->set_collision_mask(collision_mask);
            }
            if (_self()->get_owner() != nullptr) {
                p_node->set_owner(_self()->get_owner());
                for (int i = 0; i < p_node->get_child_count(); ++i) {
                    Node *c = p_node->get_child(i);
                    c->set_owner(_self()->get_owner());
                }
            }
        }
    }

    _FORCE_INLINE_ T *_self() {
        return static_cast<T *>(this);
    }
    _FORCE_INLINE_ const T *_self() const {
        return static_cast<const T *>(this);
    }
};

}
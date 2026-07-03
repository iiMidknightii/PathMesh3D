#include <cstdint>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/material.hpp>

#include "path_tools/path_mesh_3d.hpp"
#include "path_collision_tool_3d.hpp"
#include "path_tool_3d.hpp"

using namespace godot;

#define CHECK_SURFACE_IDX(m_idx) ERR_FAIL_COND(m_idx < 0 || m_idx >= surfaces.size())
#define CHECK_SURFACE_IDX_V(m_idx, m_ret) ERR_FAIL_COND_V(m_idx < 0 || m_idx >= surfaces.size(), m_ret)

void PathMesh3D::set_surface_rotation(uint64_t p_surface_idx, Vector3 p_rotation) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].rotation != p_rotation) {
        surfaces[p_surface_idx].rotation = p_rotation;
        queue_rebuild();
    }
}

Vector3 PathMesh3D::get_surface_rotation(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, Vector3());

    return surfaces[p_surface_idx].rotation;
}

void PathMesh3D::set_surface_rotation_order(uint64_t p_surface_idx, EulerOrder p_order) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].rotation_order != p_order) {
        surfaces[p_surface_idx].rotation_order = p_order;
        queue_rebuild();
    }
}

EulerOrder PathMesh3D::get_surface_rotation_order(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, EulerOrder::EULER_ORDER_YXZ);

    return surfaces[p_surface_idx].rotation_order;
}

void PathMesh3D::set_surface_distribution(uint64_t p_surface_idx, Distribution p_distribution) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].distribution != p_distribution) {
        surfaces[p_surface_idx].distribution = p_distribution;
        queue_rebuild();
        notify_property_list_changed();
    }
}

auto PathMesh3D::get_surface_distribution(uint64_t p_surface_idx) const -> Distribution {
    CHECK_SURFACE_IDX_V(p_surface_idx, Distribution::DISTRIBUTE_MAX);

    return surfaces[p_surface_idx].distribution;
}

void PathMesh3D::set_surface_alignment(uint64_t p_surface_idx, Alignment p_alignment) {
    CHECK_SURFACE_IDX(p_surface_idx);
    
    if (surfaces[p_surface_idx].alignment != p_alignment) {
        surfaces[p_surface_idx].alignment = p_alignment;
        queue_rebuild();
    }
}

auto PathMesh3D::get_surface_alignment(uint64_t p_surface_idx) const -> Alignment {
    CHECK_SURFACE_IDX_V(p_surface_idx, Alignment::ALIGN_MAX);

    return surfaces[p_surface_idx].alignment;
}

void PathMesh3D::set_surface_count(uint64_t p_surface_idx, uint64_t p_count) {
    CHECK_SURFACE_IDX(p_surface_idx);
    p_count = Math::max(p_count, uint64_t(0));

    if (surfaces[p_surface_idx].count != p_count) {
        surfaces[p_surface_idx].count = p_count;
        if (surfaces[p_surface_idx].distribution == DISTRIBUTE_BY_COUNT) {
            update_configuration_warnings();
            queue_rebuild();
        }
    }
}

uint64_t PathMesh3D::get_surface_count(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, 0);

    return surfaces[p_surface_idx].distribution == Distribution::DISTRIBUTE_BY_COUNT ?
        surfaces[p_surface_idx].count : 0;
}

void PathMesh3D::set_surface_warp_along_curve(uint64_t p_surface_idx, bool p_warp) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].warp_along_curve != p_warp) {
        surfaces[p_surface_idx].warp_along_curve = p_warp;
        queue_rebuild();
    }
}

bool PathMesh3D::get_surface_warp_along_curve(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, false);
    
    return surfaces[p_surface_idx].warp_along_curve;
}

void PathMesh3D::set_surface_mesh_length_override(uint64_t p_surface_idx, real_t p_mesh_length_override) {
    CHECK_SURFACE_IDX(p_surface_idx);

    p_mesh_length_override = Math::max(p_mesh_length_override, real_t(0.0));

    if (surfaces[p_surface_idx].mesh_length_override != p_mesh_length_override) {
        surfaces[p_surface_idx].mesh_length_override = p_mesh_length_override;
        queue_rebuild();
    }
}

real_t PathMesh3D::get_surface_mesh_length_override(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, 0.0);

    return surfaces[p_surface_idx].mesh_length_override;
}

void PathMesh3D::set_surface_sample_cubic(uint64_t p_surface_idx, bool p_cubic) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].cubic != p_cubic) {
        surfaces[p_surface_idx].cubic = p_cubic;
        queue_rebuild();
    }
}

bool PathMesh3D::get_surface_sample_cubic(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, false);

    return surfaces[p_surface_idx].cubic;
}

void PathMesh3D::set_surface_tilt(uint64_t p_surface_idx, bool p_tilt) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].tilt != p_tilt) {
        surfaces[p_surface_idx].tilt = p_tilt;
        queue_rebuild();
    }
}

bool PathMesh3D::get_surface_tilt(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, false);

    return surfaces[p_surface_idx].tilt;
}

void PathMesh3D::set_surface_offset(uint64_t p_surface_idx, Vector2 p_offset) {
    CHECK_SURFACE_IDX(p_surface_idx);

    if (surfaces[p_surface_idx].offset != p_offset) {
        surfaces[p_surface_idx].offset = p_offset;
        queue_rebuild();
    }
}

Vector2 PathMesh3D::get_surface_offset(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, Vector2());

    return surfaces[p_surface_idx].offset;
}

void PathMesh3D::set_surface_positions(uint64_t p_surface_idx, const TypedArray<real_t> &p_positions) {
    CHECK_SURFACE_IDX(p_surface_idx);

    Vector<double> new_positions;
    real_t min_val = _get_surf_length(p_surface_idx);
    real_t max_val = _get_path_length();
    for (real_t pos : p_positions) {
        pos = Math::clamp(pos, min_val, max_val);
        new_positions.push_back(pos);
    }

    if (surfaces[p_surface_idx].positions != new_positions) {
        surfaces[p_surface_idx].positions = new_positions;
        if (surfaces[p_surface_idx].distribution == DISTRIBUTE_MANUAL) {
            queue_rebuild();
        }
    }
}

TypedArray<real_t> PathMesh3D::get_surface_positions(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, TypedArray<real_t>());

    TypedArray<real_t> positions;
    for (const double &pos : surfaces[p_surface_idx].positions) {
        positions.push_back(pos);
    }

    return positions;
}


void PathMesh3D::set_mesh(const Ref<Mesh> &p_mesh) {
    if (p_mesh != source_mesh) {
        if (source_mesh.is_valid() && source_mesh->is_connected("changed", callable_mp(this, &PathMesh3D::_on_mesh_changed))) {
            source_mesh->disconnect("changed", callable_mp(this, &PathMesh3D::_on_mesh_changed));
        }

        source_mesh = p_mesh;

        if (source_mesh.is_valid()) {
            source_mesh->connect("changed", callable_mp(this, &PathMesh3D::_on_mesh_changed));
        }

        _on_mesh_changed();

        notify_property_list_changed();
    }
}

Ref<Mesh> PathMesh3D::get_mesh() const {
    return source_mesh;
}

Ref<ArrayMesh> PathMesh3D::get_baked_mesh() const {
    return generated_mesh->duplicate();
}

uint64_t PathMesh3D::get_surface_triangle_count(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, 0);

    return surfaces[p_surface_idx].n_tris;
}

uint64_t PathMesh3D::get_total_triangle_count() const {
    uint64_t r_value = 0;
    for (const SurfaceData &surf : surfaces) {
        r_value += surf.n_tris;
    }

    return r_value;
}

void PathMesh3D::_bind_methods() {
    PathTool3D::_bind_path_tool_3d_methods();
    PathCollisionTool3D::_bind_path_collision_tool_3d_methods();

    ClassDB::bind_method(D_METHOD("get_baked_mesh"), &PathMesh3D::get_baked_mesh);

    ClassDB::bind_method(D_METHOD("set_surface_offset", "surface_index", "offset"), &PathMesh3D::set_surface_offset);
    ClassDB::bind_method(D_METHOD("get_surface_offset", "surface_index"), &PathMesh3D::get_surface_offset);
    ClassDB::bind_method(D_METHOD("set_surface_rotation", "surface_index", "rotation"), &PathMesh3D::set_surface_rotation);
    ClassDB::bind_method(D_METHOD("get_surface_rotation", "surface_index"), &PathMesh3D::get_surface_rotation);
    ClassDB::bind_method(D_METHOD("set_surface_rotation_order", "surface_index", "order"), &PathMesh3D::set_surface_rotation_order);
    ClassDB::bind_method(D_METHOD("get_surface_rotation_order", "surface_index"), &PathMesh3D::get_surface_rotation_order);
    ClassDB::bind_method(D_METHOD("set_surface_distribution", "surface_index", "distribution"), &PathMesh3D::set_surface_distribution);
    ClassDB::bind_method(D_METHOD("get_surface_distribution", "surface_index"), &PathMesh3D::get_surface_distribution);
    ClassDB::bind_method(D_METHOD("set_surface_alignment", "surface_index", "alignment"), &PathMesh3D::set_surface_alignment);
    ClassDB::bind_method(D_METHOD("get_surface_alignment", "surface_index"), &PathMesh3D::get_surface_alignment);
    ClassDB::bind_method(D_METHOD("set_surface_count", "surface_index", "count"), &PathMesh3D::set_surface_count);
    ClassDB::bind_method(D_METHOD("get_surface_count", "surface_index"), &PathMesh3D::get_surface_count);
    ClassDB::bind_method(D_METHOD("set_surface_positions", "surface_index", "positions"), &PathMesh3D::set_surface_positions);
    ClassDB::bind_method(D_METHOD("get_surface_positions", "surface_index"), &PathMesh3D::get_surface_positions);
    ClassDB::bind_method(D_METHOD("set_surface_warp_along_curve", "surface_index", "warp"), &PathMesh3D::set_surface_warp_along_curve);
    ClassDB::bind_method(D_METHOD("get_surface_warp_along_curve", "surface_index"), &PathMesh3D::get_surface_warp_along_curve);
    ClassDB::bind_method(D_METHOD("set_surface_mesh_length_override", "surface_index", "override"), &PathMesh3D::set_surface_mesh_length_override);
    ClassDB::bind_method(D_METHOD("get_surface_mesh_length_override", "surface_index"), &PathMesh3D::get_surface_mesh_length_override);
    ClassDB::bind_method(D_METHOD("set_surface_sample_cubic", "surface_index", "cubic"), &PathMesh3D::set_surface_sample_cubic);
    ClassDB::bind_method(D_METHOD("get_surface_sample_cubic", "surface_index"), &PathMesh3D::get_surface_sample_cubic);
    ClassDB::bind_method(D_METHOD("set_surface_tilt", "surface_index", "tilt"), &PathMesh3D::set_surface_tilt);
    ClassDB::bind_method(D_METHOD("get_surface_tilt", "surface_index"), &PathMesh3D::get_surface_tilt);

    ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &PathMesh3D::set_mesh);
    ClassDB::bind_method(D_METHOD("get_mesh"), &PathMesh3D::get_mesh);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh", "get_mesh");

    ClassDB::bind_method(D_METHOD("get_total_triangle_count"), &PathMesh3D::get_total_triangle_count);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "total_triangle_count", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_total_triangle_count");
    
    ADD_SIGNAL(MethodInfo("mesh_changed"));

    BIND_ENUM_CONSTANT(DISTRIBUTE_BY_COUNT);
    BIND_ENUM_CONSTANT(DISTRIBUTE_BY_MODEL_LENGTH);
    BIND_ENUM_CONSTANT(DISTRIBUTE_MANUAL);
    BIND_ENUM_CONSTANT(ALIGN_STRETCH);
    BIND_ENUM_CONSTANT(ALIGN_FROM_START);
    BIND_ENUM_CONSTANT(ALIGN_CENTERED);
    BIND_ENUM_CONSTANT(ALIGN_FROM_END);
}

void PathMesh3D::_notification(int p_what) {
    PathTool3D::_notification_path_tool_3d(p_what);
    PathCollisionTool3D::_notification_path_collision_tool_3d(p_what);
}

PackedStringArray PathMesh3D::_get_configuration_warnings() const {
    PackedStringArray warnings;
    for (uint64_t idx = 0; idx < surfaces.size(); ++idx) {
        uint64_t max_count = _get_max_count(idx);
        const SurfaceData &surf = surfaces[idx];
        if (surf.distribution == Distribution::DISTRIBUTE_BY_COUNT && surf.count > max_count) {
            warnings.push_back("Surface " + itos(idx) + ": Count is greater than the maximum " + itos(max_count) + " for the current path length.  The mesh is being squished.");
        }
    }
    return warnings;
}

void PathMesh3D::_get_property_list(List<PropertyInfo> *p_list) const {
    for (uint64_t idx = 0; idx < surfaces.size(); ++idx) {
        const SurfaceData &surf = surfaces[idx];

        String surf_name = "surface_" + itos(idx);
        uint32_t usage = PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL;
            
        p_list->push_back(PropertyInfo(Variant::NIL, surf_name.capitalize(), PROPERTY_HINT_NONE, surf_name + "/", PROPERTY_USAGE_GROUP));
        p_list->push_back(PropertyInfo(Variant::VECTOR2, surf_name + "/offset", PROPERTY_HINT_NONE, "", usage));
        p_list->push_back(PropertyInfo(Variant::VECTOR3, surf_name + "/rotation", PROPERTY_HINT_RANGE, "0.0,360.0,0.01,radians_as_degrees", usage));
        p_list->push_back(PropertyInfo(Variant::INT, surf_name + "/rotation_order", PROPERTY_HINT_ENUM, "XYZ,XZY,YXZ,YZX,ZXY,ZYX", usage));
        p_list->push_back(PropertyInfo(Variant::INT, surf_name + "/distribution", PROPERTY_HINT_ENUM, "By Model Length,By Count,Manual", usage));
        if (surf.distribution != Distribution::DISTRIBUTE_MANUAL) {
            p_list->push_back(PropertyInfo(Variant::INT, surf_name + "/alignment", PROPERTY_HINT_ENUM, "Stretch,From Start,Center,From End", usage));
            if (surf.distribution == Distribution::DISTRIBUTE_BY_COUNT) {
                String hint_str = "1," + itos(_get_max_count(idx)) + ",1,or_greater";
                p_list->push_back(PropertyInfo(Variant::INT, surf_name + "/count", PROPERTY_HINT_RANGE, hint_str, usage));
            }
            p_list->push_back(PropertyInfo(Variant::FLOAT, surf_name + "/mesh_length_override", PROPERTY_HINT_RANGE, "0.0," + rtos(_get_path_length()) + ",0.001", usage));
        } else {
            String hint_str = vformat("%d/%d:%s", Variant::FLOAT, PROPERTY_HINT_RANGE, rtos(_get_surf_length(idx)) + "," + rtos(_get_path_length()) + ",0.01");
            p_list->push_back(PropertyInfo(Variant::ARRAY, surf_name + "/positions", PROPERTY_HINT_ARRAY_TYPE, hint_str, usage));
        }   
        p_list->push_back(PropertyInfo(Variant::BOOL, surf_name + "/warp_along_curve", PROPERTY_HINT_NONE, "", usage));
        p_list->push_back(PropertyInfo(Variant::BOOL, surf_name + "/sample_cubic", PROPERTY_HINT_NONE, "", usage));
        p_list->push_back(PropertyInfo(Variant::BOOL, surf_name + "/tilt", PROPERTY_HINT_NONE, "", usage));
        p_list->push_back(PropertyInfo(Variant::INT, surf_name + "/triangle_count", PROPERTY_HINT_NONE, "", usage | PROPERTY_USAGE_READ_ONLY));
    }
}

bool PathMesh3D::_property_can_revert(const StringName &p_name) const {
    if (p_name.begins_with("surface_")) {
        Pair<uint64_t, String> subprop = _decode_dynamic_propname(p_name);
        String sub_name = subprop.second;
        if (sub_name == "triangle_count") {
            return false;
        } else {
            return true; 
        }
    }

    return false;
}

bool PathMesh3D::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    if (p_name.begins_with("surface_")) {
        Pair<uint64_t, String> subprop = _decode_dynamic_propname(p_name);
        uint64_t surf_idx = subprop.first;
        String sub_name = subprop.second;
        if (sub_name == "distribution") {
            r_property = Distribution::DISTRIBUTE_BY_MODEL_LENGTH;
        } else if (sub_name == "rotation") {
            r_property = Vector3();
        } else if (sub_name == "rotation_order") {
            r_property = EulerOrder::EULER_ORDER_YXZ;
        } else if (sub_name == "alignment") {
            r_property = Alignment::ALIGN_STRETCH;
        } else if (sub_name == "count") {
            r_property = 1;
        } else if (sub_name == "warp_along_curve") {
            r_property = true;
        } else if (sub_name == "mesh_length_override") {
            r_property = 0.0;
        } else if (sub_name == "sample_cubic") {
            r_property = false;
        } else if (sub_name == "tilt") {
            r_property = true;
        } else if (sub_name == "offset") {
            r_property = Vector2();
        } else if (sub_name == "positions") {
            r_property = TypedArray<real_t>();
        } else {
            return false;
        }
        return true;
    }
    return false;
}

bool PathMesh3D::_set(const StringName &p_name, const Variant &p_property) {
    if (p_name.begins_with("surface_")) {
        Pair<uint64_t, String> subprop = _decode_dynamic_propname(p_name);
        uint64_t surf_idx = subprop.first;
        String sub_name = subprop.second;
        if (sub_name == "distribution") {
            set_surface_distribution(surf_idx, Distribution(int(p_property)));
        } else if (sub_name == "rotation") {
            set_surface_rotation(surf_idx, p_property);
        } else if (sub_name == "rotation_order") {
            set_surface_rotation_order(surf_idx, EulerOrder(int(p_property)));
        } else if (sub_name == "alignment") {
            set_surface_alignment(surf_idx, Alignment(int(p_property)));
        } else if (sub_name == "count") {
            set_surface_count(surf_idx, p_property);
        } else if (sub_name == "warp_along_curve") {
            set_surface_warp_along_curve(surf_idx, p_property);
        } else if (sub_name == "mesh_length_override") {
            set_surface_mesh_length_override(surf_idx, p_property);
        } else if (sub_name == "sample_cubic") {
            set_surface_sample_cubic(surf_idx, p_property);
        } else if (sub_name == "tilt") {
            set_surface_tilt(surf_idx, p_property);
        } else if (sub_name == "offset") {
            set_surface_offset(surf_idx, p_property);
        } else if (sub_name == "positions") {
            set_surface_positions(surf_idx, p_property);
        } else {
            return false;
        }
        return true;
    }
    return false;
}

bool PathMesh3D::_get(const StringName &p_name, Variant &r_property) const {
    if (p_name.begins_with("surface_")) {
        Pair<uint64_t, String> subprop = _decode_dynamic_propname(p_name);
        uint64_t surf_idx = subprop.first;
        String sub_name = subprop.second;
        if (sub_name == "distribution") {
            r_property = get_surface_distribution(surf_idx);
        } else if (sub_name == "rotation") {
            r_property = get_surface_rotation(surf_idx);
        } else if (sub_name == "rotation_order") {
            r_property = get_surface_rotation_order(surf_idx);
        } else if (sub_name == "alignment") {
            r_property = get_surface_alignment(surf_idx);
        } else if (sub_name == "count") {
            r_property = get_surface_count(surf_idx);
        } else if (sub_name == "warp_along_curve") {
            r_property = get_surface_warp_along_curve(surf_idx);
        } else if (sub_name == "mesh_length_override") {
            r_property = get_surface_mesh_length_override(surf_idx);
        } else if (sub_name == "sample_cubic") {
            r_property = get_surface_sample_cubic(surf_idx);
        } else if (sub_name == "tilt") {
            r_property = get_surface_tilt(surf_idx);
        } else if (sub_name == "offset") {
            r_property = get_surface_offset(surf_idx);
        } else if (sub_name == "positions") {
            r_property = get_surface_positions(surf_idx);
        } else if (sub_name == "triangle_count") {
            r_property = get_surface_triangle_count(surf_idx);
        } else {
            return false;
        }
        return true;
    }
    return false;
}

void PathMesh3D::_validate_property(PropertyInfo &p_property) const {
    PathCollisionTool3D<PathMesh3D>::_validate_path_collision_tool_3d_property(p_property);
}

void PathMesh3D::_rebuild_mesh() {
    generated_mesh->clear_surfaces();
    for (SurfaceData &surf : surfaces) {
        surf.n_tris = 0;
    }

    _clear_collision_node();

    Path3D *path3d = get_path_3d();
    if (path3d == nullptr || path3d->get_curve().is_null() || !path3d->is_inside_tree() || source_mesh.is_null()) {
        return;
    }

    Transform3D mod_transform = _get_final_transform();

    Ref<Curve3D> curve = path3d->get_curve();
    if (curve->get_point_count() < 2) {
        return;
    }

    double baked_l = curve->get_baked_length();
    if (baked_l == 0.0) {
        return;
    }

    double mesh_l = _get_mesh_length();
    if (mesh_l == 0.0 || baked_l < mesh_l) {
        return;
    }

    ERR_FAIL_COND_EDMSG(source_mesh->get_surface_count() != surfaces.size(),
                          "The number of surfaces in the source mesh does not "
                          "match the number of configured surfaces.");
    
    for (uint64_t idx_surf = 0; idx_surf < source_mesh->get_surface_count(); ++idx_surf) {
        SurfaceData &surf = surfaces[idx_surf];

        Array arrays = source_mesh->surface_get_arrays(idx_surf);

        PackedVector3Array old_verts = arrays[Mesh::ARRAY_VERTEX];

        LocalVector<bool> has_column;
        has_column.resize(Mesh::ARRAY_MAX);
        for (int idx_type = 0; idx_type < Mesh::ARRAY_MAX; ++idx_type) {
            has_column[idx_type] = arrays[idx_type].get_type() != Variant::NIL;
        }

        #define MAKE_OLD_ARRAY(m_type, m_name, m_index) \
            m_type m_name = has_column[m_index] ? m_type(arrays[m_index]) : m_type()
        MAKE_OLD_ARRAY(PackedVector3Array, old_norms, Mesh::ARRAY_NORMAL);
        MAKE_OLD_ARRAY(PackedFloat32Array, old_tang, Mesh::ARRAY_TANGENT);
        MAKE_OLD_ARRAY(PackedVector2Array, old_uv1, Mesh::ARRAY_TEX_UV);
        MAKE_OLD_ARRAY(PackedVector2Array, old_uv2, Mesh::ARRAY_TEX_UV2);
        MAKE_OLD_ARRAY(PackedColorArray, old_colors, Mesh::ARRAY_COLOR);
        MAKE_OLD_ARRAY(PackedByteArray, old_custom0, Mesh::ARRAY_CUSTOM0);
        MAKE_OLD_ARRAY(PackedByteArray, old_custom1, Mesh::ARRAY_CUSTOM1);
        MAKE_OLD_ARRAY(PackedByteArray, old_custom2, Mesh::ARRAY_CUSTOM2);
        MAKE_OLD_ARRAY(PackedByteArray, old_custom3, Mesh::ARRAY_CUSTOM3);
        MAKE_OLD_ARRAY(PackedInt32Array, old_bones /*my nickname in highschool*/, Mesh::ARRAY_BONES);
        MAKE_OLD_ARRAY(PackedFloat32Array, old_weights, Mesh::ARRAY_WEIGHTS);
        MAKE_OLD_ARRAY(PackedInt32Array, old_idx, Mesh::ARRAY_INDEX);
        #undef MAKE_OLD_ARRAY

        arrays.clear();

        // Transform the mesh according to user settings
        double pos_z = -INFINITY;
        for (uint64_t idx = 0; idx < old_verts.size(); ++idx) {
            Vector3 vert = old_verts[idx];

            Basis rot = Basis::from_euler(surf.rotation, surf.rotation_order);
            vert = rot.xform(vert);
            if (has_column[Mesh::ARRAY_NORMAL]) {
                Vector3 norm = old_norms[idx];
                norm = rot.xform(norm);
                old_norms[idx] = norm;
            }
            if (has_column[Mesh::ARRAY_TANGENT]) {
                Vector3 tang = Vector3(old_tang[idx*4], old_tang[idx*4 + 1], old_tang[idx*4 + 2]);
                tang = rot.xform(tang);
                old_tang[idx*4] = tang.x;
                old_tang[idx*4 + 1] = tang.y;
                old_tang[idx*4 + 2] = tang.z;
            }
            vert.x += surf.offset.x;
            vert.y += surf.offset.y;

            old_verts[idx] = vert;

            if (vert.z > pos_z) {
                pos_z = vert.z;
            }
        }
        if (pos_z == -INFINITY) {
            pos_z = 0.0;
        }

        uint64_t max_count = _get_max_count(idx_surf);
        double surf_l = _get_surf_length(idx_surf);
        uint64_t count = 0;
        double real_l = surf_l * double(count);
        double z_stretch = 1.0;
        double z = 0.0;
        switch (surf.distribution) {
            // ---------------------------------------------------------------------------------------------------------
            case Distribution::DISTRIBUTE_BY_COUNT: {
                count = surf.count;
                real_l = surf_l * double(count);

                switch (surf.alignment) {
                    case Alignment::ALIGN_STRETCH: {
                        z_stretch = baked_l / real_l;
                        z = pos_z * z_stretch;
                    } break;

                    case Alignment::ALIGN_FROM_START: {
                        if (real_l > baked_l) {
                            z_stretch = baked_l / real_l;
                        }
                        z = pos_z * z_stretch;
                    } break;

                    case Alignment::ALIGN_CENTERED: {
                        if (real_l > baked_l) {
                            z_stretch = baked_l / real_l;
                        }
                        real_l *= z_stretch;
                        z = (baked_l - real_l) / 2.0 + pos_z * z_stretch;
                    } break;

                    case Alignment::ALIGN_FROM_END: {
                        if (real_l > baked_l) {
                            z_stretch = baked_l / real_l;
                        }
                        real_l *= z_stretch;
                        z = baked_l - real_l + pos_z * z_stretch;
                    } break;

                    default:
                        UtilityFunctions::push_error("Alignment type is incorrect.");
                        continue;
                }
            } break;

            // ---------------------------------------------------------------------------------------------------------
            case Distribution::DISTRIBUTE_BY_MODEL_LENGTH: {
                count = max_count;
                real_l = surf_l * double(count);

                switch (surf.alignment) {
                    case Alignment::ALIGN_STRETCH: {
                        z_stretch = baked_l / real_l;
                        z = pos_z * z_stretch;
                    } break;

                    case Alignment::ALIGN_FROM_START: {
                        z = pos_z;
                    } break;

                    case Alignment::ALIGN_CENTERED: {
                        z = (baked_l - real_l) / 2.0 + pos_z;
                    } break;

                    case Alignment::ALIGN_FROM_END: {
                        z = baked_l - real_l + pos_z;
                    } break;

                    default:
                        UtilityFunctions::push_error("Alignment type is incorrect.");
                        continue;
                }
            } break;

            // ---------------------------------------------------------------------------------------------------------
            case Distribution::DISTRIBUTE_MANUAL: {
                count = surf.positions.size();
                z_stretch = 1.0;
                if (count > 0) {
                    z = surf.positions[0];
                }
            } break;

            default:
                UtilityFunctions::push_error("Distribution type is incorrect.");
                continue;
        }

        if (count == 0) {
            continue;
        }
        
        z = Math::clamp(z, 0.0, baked_l);

        uint64_t new_size = count * old_verts.size();
        PackedVector3Array new_verts; new_verts.resize(new_size);
        PackedVector3Array new_norms; new_norms.resize(new_size);
        PackedFloat32Array new_tang; new_tang.resize(new_size*4);
        PackedVector2Array new_uv1;
        PackedVector2Array new_uv2;
        PackedColorArray new_colors;
        PackedByteArray new_custom0;
        PackedByteArray new_custom1;
        PackedByteArray new_custom2;
        PackedByteArray new_custom3;
        PackedInt32Array new_bones;
        PackedFloat32Array new_weights;
        PackedInt32Array new_idx;

        uint64_t k = 0; // overall index into new arrays

        for (uint64_t idx_count = 0; idx_count < count; ++idx_count) {
            Transform3D transform;
            if (!surf.warp_along_curve) {
                transform = curve->sample_baked_with_rotation(z, surf.cubic, surf.tilt) * 
                        _sample_3d_modifiers_at(z / baked_l);
            }

            for (uint64_t idx_vert = 0; idx_vert < old_verts.size(); ++idx_vert) {
                Vector3 vertex;
                if (surf.warp_along_curve) {
                    double z_offset = z - z_stretch * old_verts[idx_vert].z;
                    transform = curve->sample_baked_with_rotation(z_offset, surf.cubic, surf.tilt) * 
                            _sample_3d_modifiers_at(z_offset / baked_l);
                    
                    // Avoid double transforming the Z position by zeroing out the original Z value
                    vertex = { old_verts[idx_vert].x, old_verts[idx_vert].y, 0.0 };
                } else {
                    // No need to recalculate the transform if not warping
                    vertex = old_verts[idx_vert];
                }
                
                Transform3D final_transform = get_relative_transform() == TRANSFORM_PATH_NODE ? mod_transform * transform : transform;

                new_verts[k] = final_transform.xform(vertex);
                if (has_column[Mesh::ARRAY_NORMAL]) {
                    new_norms[k] = final_transform.basis.xform(old_norms[idx_vert]);
                }
                if (has_column[Mesh::ARRAY_TANGENT]) {
                    Vector3 tang = final_transform.basis.xform(Vector3(old_tang[idx_vert*4], old_tang[idx_vert*4 + 1], old_tang[idx_vert*4 + 2]));
                    new_tang[k*4] = tang.x;
                    new_tang[k*4 + 1] = tang.y;
                    new_tang[k*4 + 2] = tang.z;
                    new_tang[k*4 + 3] = old_tang[idx_vert*4 + 3];
                }

                k++;
            }

            new_uv1.append_array(old_uv1);
            new_uv2.append_array(old_uv2);
            new_colors.append_array(old_colors);
            new_custom0.append_array(old_custom0);
            new_custom1.append_array(old_custom1);
            new_custom2.append_array(old_custom2);
            new_custom3.append_array(old_custom3);
            new_bones.append_array(old_bones);
            new_weights.append_array(old_weights);
            new_idx.append_array(old_idx);
            for (int32_t &idx : old_idx) {
                idx += old_verts.size(); // increase the indices by size each loop
            }
            
            if (surf.distribution == Distribution::DISTRIBUTE_MANUAL && idx_count + 1 < surf.positions.size()) {
                z = surf.positions[idx_count + 1];
            } else {
                z += surf_l * z_stretch;
            }
        }

        arrays.resize(Mesh::ARRAY_MAX);
        arrays.fill(Variant());
        arrays[Mesh::ARRAY_VERTEX] = new_verts;
        #define MAKE_NEW_ARRAY(m_index, m_name) arrays[m_index] = m_name.is_empty() ? Variant() : Variant(m_name)
        MAKE_NEW_ARRAY(Mesh::ARRAY_NORMAL, new_norms);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TANGENT, new_tang);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TEX_UV, new_uv1);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TEX_UV2, new_uv2);
        MAKE_NEW_ARRAY(Mesh::ARRAY_COLOR, new_colors);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM0, new_custom0);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM1, new_custom1);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM2, new_custom2);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM3, new_custom3);
        MAKE_NEW_ARRAY(Mesh::ARRAY_BONES, new_bones);
        MAKE_NEW_ARRAY(Mesh::ARRAY_WEIGHTS, new_weights);
        MAKE_NEW_ARRAY(Mesh::ARRAY_INDEX, new_idx);
        #undef MAKE_NEW_ARRAY

        Mesh::PrimitiveType primitives = Mesh::PRIMITIVE_TRIANGLES;
        Ref<ArrayMesh> arr_mesh = cast_to<ArrayMesh>(source_mesh.ptr());
        if (arr_mesh != nullptr) {
            primitives = arr_mesh->surface_get_primitive_type(idx_surf);
        }

        generated_mesh->add_surface_from_arrays(primitives, arrays, source_mesh->surface_get_blend_shape_arrays(idx_surf));
        generated_mesh->surface_set_material(idx_surf, source_mesh->surface_get_material(idx_surf));

        surf.n_tris = new_verts.size() / 3;
    }

    _rebuild_collision_node();
}

Pair<uint64_t, String> PathMesh3D::_decode_dynamic_propname(const StringName &p_name) const {
    PackedStringArray prop_path = p_name.split("/");
    if (prop_path.size() != 2) {
        return Pair<uint64_t, String>();
    }

    uint64_t surf_idx = prop_path[0].trim_prefix("surface_").to_int();
    String sub_name = prop_path[1];
    return Pair(surf_idx, sub_name);
}

double PathMesh3D::_get_surf_length(uint64_t p_surface_idx) const {
    CHECK_SURFACE_IDX_V(p_surface_idx, 0.0);

    const SurfaceData &surf = surfaces[p_surface_idx];
    return surf.mesh_length_override > 0.0 ? surf.mesh_length_override : _get_mesh_length();
}

double PathMesh3D::_get_mesh_length() const {
    if (source_mesh.is_valid()) {
        double min_z = 0;
        double max_z = 0;

        ERR_FAIL_COND_V_EDMSG(source_mesh->get_surface_count() != surfaces.size(),
                          0.0,
                          "The number of surfaces in the source mesh does not "
                          "match the number of configured surfaces.");

        for (uint64_t idx_surf = 0; idx_surf < source_mesh->get_surface_count(); ++idx_surf) {
            SurfaceData surf = surfaces[idx_surf];

            Array mesh_array = source_mesh->surface_get_arrays(idx_surf);
            PackedVector3Array verts = mesh_array[Mesh::ARRAY_VERTEX];
        
            for (uint64_t idx = 0; idx < verts.size(); ++idx) {
                Vector3 vert = verts[idx];
                Basis rot = Basis::from_euler(surf.rotation, surf.rotation_order);
                vert = rot.xform(vert);
                if (vert.z < min_z) {
                    min_z = vert.z;
                } else if (vert.z > max_z) {
                    max_z = vert.z;
                }
            }
        }
        return Math::absd(max_z - min_z);
    } else {
        return 0.0;
    }
}

uint64_t PathMesh3D::_get_max_count(uint64_t p_surface_idx) const {
    const SurfaceData &surf = surfaces[p_surface_idx];
    double surf_length = _get_surf_length(p_surface_idx);
    if (surf_length > 0.0) {
        return _get_path_length() / surf_length;
    } else {
        return 0;
    }
}

double PathMesh3D::_get_path_length() const {
    Path3D *path3d = get_path_3d();
    if (path3d != nullptr && path3d->get_curve().is_valid() && path3d->get_curve()->get_point_count() > 1) {
        return path3d->get_curve()->get_baked_length();
    } else {
        return 0.0;
    }
}

void PathMesh3D::_on_mesh_changed() {
    if (source_mesh.is_valid()) {
        surfaces.resize(source_mesh->get_surface_count());
    } else {
        surfaces.clear();
    }
    emit_signal("mesh_changed");
    queue_rebuild();
    notify_property_list_changed();
}

PathMesh3D::PathMesh3D() {
    generated_mesh.instantiate();
    set_base(generated_mesh->get_rid());
}

PathMesh3D::~PathMesh3D() {
    if (source_mesh.is_valid()) {
        if (source_mesh->is_connected("changed", callable_mp(this, &PathMesh3D::_on_mesh_changed))) {
            source_mesh->disconnect("changed", callable_mp(this, &PathMesh3D::_on_mesh_changed));
        }
        source_mesh.unref();
    }
    if (generated_mesh.is_valid()) {
        generated_mesh->clear_surfaces();
        generated_mesh.unref();
    }
    surfaces.clear();
}

Ref<ArrayMesh> PathMesh3D::_get_mesh() const {
    return generated_mesh;
}
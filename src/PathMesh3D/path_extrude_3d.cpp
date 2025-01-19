#include "path_extrude_3d.hpp"

#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/geometry2d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>

using namespace godot;

void PathExtrude3D::set_path_3d(Path3D *p_path) {
    if (p_path != path3d) {
        if (path3d != nullptr && path3d->is_connected("curve_changed", callable_mp(this, &PathExtrude3D::_on_curve_changed))) {
            path3d->disconnect("curve_changed", callable_mp(this, &PathExtrude3D::_on_curve_changed));
        }

        path3d = p_path;

        if (path3d != nullptr) {
            path3d->connect("curve_changed", callable_mp(this, &PathExtrude3D::_on_curve_changed));
        }

        _on_curve_changed();
    }
}

Path3D *PathExtrude3D::get_path_3d() const {
    return path3d;
}

void PathExtrude3D::set_profile(const Ref<PathExtrudeProfileBase> &p_profile) {
    if (p_profile != profile) {
        if (profile.is_valid() && profile->is_connected("changed", callable_mp(this, &PathExtrude3D::_on_profile_changed))) {
            profile->disconnect("changed", callable_mp(this, &PathExtrude3D::_on_profile_changed));
        }
        profile = p_profile;
        if (profile.is_valid()) {
            profile->connect("changed", callable_mp(this, &PathExtrude3D::_on_profile_changed));
        }

        _on_profile_changed();
    }
}

Ref<PathExtrudeProfileBase> PathExtrude3D::get_profile() const {
    return profile;
}

void PathExtrude3D::set_smooth(const bool p_smooth) {
    if (smooth != p_smooth) {
        smooth = p_smooth;
        queue_rebuild();
    }
}

bool PathExtrude3D::get_smooth() const {
    return smooth;
}

void PathExtrude3D::set_tessellation_max_stages(const int32_t p_tesselation_max_stages) {
    if (tessellation_max_stages != p_tesselation_max_stages) {
        tessellation_max_stages = p_tesselation_max_stages;
        queue_rebuild();
    }
}

int32_t PathExtrude3D::get_tessellation_max_stages() const {
    return tessellation_max_stages;
}

void PathExtrude3D::set_tessellation_tolerance_degrees(const double p_tessellation_tolerance_degrees) {
    if (tessellation_tolerance_degrees != p_tessellation_tolerance_degrees) {
        tessellation_tolerance_degrees = p_tessellation_tolerance_degrees;
        queue_rebuild();
    }
}

double PathExtrude3D::get_tessellation_tolerance_degrees() const {
    return tessellation_tolerance_degrees;
}

void PathExtrude3D::set_end_cap_mode(const BitField<EndCaps> p_end_cap_mode) {
    if (end_cap_mode != p_end_cap_mode) {
        end_cap_mode = EndCaps(int(p_end_cap_mode));
        queue_rebuild();
    }
}

BitField<PathExtrude3D::EndCaps> PathExtrude3D::get_end_cap_mode() const {
    return end_cap_mode;
}

void PathExtrude3D::set_offset(const Vector2 p_offset) {
    if (offset != p_offset) {
        offset = p_offset;
        queue_rebuild();
    }
}

Vector2 PathExtrude3D::get_offset() const {
    return offset;
}

void PathExtrude3D::set_sample_cubic(const bool p_cubic) {
    if (sample_cubic != p_cubic) {
        sample_cubic = p_cubic;
        queue_rebuild();
    }
}

bool PathExtrude3D::get_sample_cubic() const {
    return sample_cubic;
}

void PathExtrude3D::set_tilt(const bool p_tilt) {
    if (tilt != p_tilt) {
        tilt = p_tilt;
        queue_rebuild();
    }
}

bool PathExtrude3D::get_tilt() const {
    return tilt;
}

Ref<ArrayMesh> PathExtrude3D::get_baked_mesh() const {
    return generated_mesh->duplicate();
}

Node *PathExtrude3D::create_trimesh_collision_node() {
    return _setup_collision_node(generated_mesh->create_trimesh_shape());
}

void PathExtrude3D::create_trimesh_collision() {
    _add_child_collision_node(create_trimesh_collision_node());
}

Node *PathExtrude3D::create_convex_collision_node(bool p_clean, bool p_simplify) {
    return _setup_collision_node(generated_mesh->create_convex_shape(p_clean, p_simplify));
}

void PathExtrude3D::create_convex_collision(bool p_clean, bool p_simplify) {
    _add_child_collision_node(create_convex_collision_node(p_clean, p_simplify));
}

Node *PathExtrude3D::create_multiple_convex_collision_node(const Ref<MeshConvexDecompositionSettings> &p_settings) {
    Ref<MeshConvexDecompositionSettings> settings = p_settings;
    if (settings.is_null()) {
        settings.instantiate();
    }

    // # TODO: GDExtension doesn't have API parity here...
    Vector<Ref<Shape3D>> shapes; // = generated_mesh->convex_decompose(settings); 
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

void PathExtrude3D::create_multiple_convex_collision(const Ref<MeshConvexDecompositionSettings> &p_settings) {
    StaticBody3D *static_body = Object::cast_to<StaticBody3D>(create_multiple_convex_collision_node(p_settings));
    ERR_FAIL_NULL(static_body);
    static_body->set_name(String(get_name()) + "Collision");

    add_child(static_body, true);
    if (get_owner() != nullptr) {
        static_body->set_owner(get_owner());
        int count = static_body->get_child_count();
        for (int i = 0; i < count; ++i) {
            CollisionShape3D *cshape = Object::cast_to<CollisionShape3D>(static_body->get_child(i));
            cshape->set_owner(get_owner());
        }
    }
}

PathExtrude3D::PathExtrude3D() {
    generated_mesh.instantiate();
}

PathExtrude3D::~PathExtrude3D() {
    generated_mesh.unref();
}

void PathExtrude3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("queue_rebuild"), &PathExtrude3D::queue_rebuild);
    ClassDB::bind_method(D_METHOD("get_baked_mesh"), &PathExtrude3D::get_baked_mesh);
    ClassDB::bind_method(D_METHOD("create_trimesh_collision"), &PathExtrude3D::create_trimesh_collision);
    ClassDB::bind_method(D_METHOD("create_convex_collision", "clean", "simplify"), &PathExtrude3D::create_convex_collision);
    ClassDB::bind_method(D_METHOD("create_multiple_convex_collision", "settings"), &PathExtrude3D::create_multiple_convex_collision);

    ClassDB::bind_method(D_METHOD("set_path_3d", "path"), &PathExtrude3D::set_path_3d);
    ClassDB::bind_method(D_METHOD("get_path_3d"), &PathExtrude3D::get_path_3d);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "path_3d", PROPERTY_HINT_NODE_TYPE, "Path3D"), "set_path_3d", "get_path_3d");

    ClassDB::bind_method(D_METHOD("set_profile", "profile"), &PathExtrude3D::set_profile);
    ClassDB::bind_method(D_METHOD("get_profile"), &PathExtrude3D::get_profile);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "profile", PROPERTY_HINT_RESOURCE_TYPE, "PathExtrudeProfileBase"), "set_profile", "get_profile");

    ClassDB::bind_method(D_METHOD("set_smooth", "smooth"), &PathExtrude3D::set_smooth);
    ClassDB::bind_method(D_METHOD("get_smooth"), &PathExtrude3D::get_smooth);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth"), "set_smooth", "get_smooth");

    ClassDB::bind_method(D_METHOD("set_tessellation_max_stages", "tessellation_max_stages"), &PathExtrude3D::set_tessellation_max_stages);
    ClassDB::bind_method(D_METHOD("get_tessellation_max_stages"), &PathExtrude3D::get_tessellation_max_stages);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tessellation_max_stages"), "set_tessellation_max_stages", "get_tessellation_max_stages");

    ClassDB::bind_method(D_METHOD("set_tessellation_tolerance_degrees", "tessellation_tolerance_degrees"), &PathExtrude3D::set_tessellation_tolerance_degrees);
    ClassDB::bind_method(D_METHOD("get_tessellation_tolerance_degrees"), &PathExtrude3D::get_tessellation_tolerance_degrees);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tessellation_tolerance_degrees"), "set_tessellation_tolerance_degrees", "get_tessellation_tolerance_degrees");

    ClassDB::bind_method(D_METHOD("set_end_cap_mode", "end_cap_mode"), &PathExtrude3D::set_end_cap_mode);
    ClassDB::bind_method(D_METHOD("get_end_cap_mode"), &PathExtrude3D::get_end_cap_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "end_cap_mode", PROPERTY_HINT_FLAGS, "Start,End"), "set_end_cap_mode", "get_end_cap_mode");

    ClassDB::bind_method(D_METHOD("set_offset", "offset"), &PathExtrude3D::set_offset);
    ClassDB::bind_method(D_METHOD("get_offset"), &PathExtrude3D::get_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_offset", "get_offset");

    ClassDB::bind_method(D_METHOD("set_sample_cubic", "sample_cubic"), &PathExtrude3D::set_sample_cubic);
    ClassDB::bind_method(D_METHOD("get_sample_cubic"), &PathExtrude3D::get_sample_cubic);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "sample_cubic"), "set_sample_cubic", "get_sample_cubic");

    ClassDB::bind_method(D_METHOD("set_tilt", "tilt"), &PathExtrude3D::set_tilt);
    ClassDB::bind_method(D_METHOD("get_tilt"), &PathExtrude3D::get_tilt);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "tilt"), "set_tilt", "get_tilt");

    ADD_SIGNAL(MethodInfo("cross_section_changed"));
    ADD_SIGNAL(MethodInfo("curve_changed"));

    BIND_BITFIELD_FLAG(END_CAPS_NONE);
    BIND_BITFIELD_FLAG(END_CAPS_START);
    BIND_BITFIELD_FLAG(END_CAPS_END);
    BIND_BITFIELD_FLAG(END_CAPS_BOTH);
}

void PathExtrude3D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_INTERNAL_PROCESS: {
            if (!initial_dirty) {
                break;
            }
            initial_dirty = false;
            set_process_internal(false);
        }
        case NOTIFICATION_READY: {
            set_process_internal(true);
            queue_rebuild();
        } break;
    }
}

Node *PathExtrude3D::_setup_collision_node(const Ref<Shape3D> &shape) {
    StaticBody3D *static_body = memnew(StaticBody3D);
    CollisionShape3D *cshape = memnew(CollisionShape3D);
    cshape->set_shape(shape);
    static_body->add_child(cshape, true);
    return static_body;
}

void PathExtrude3D::_add_child_collision_node(Node *p_node) {
    if (p_node != nullptr) {
        add_child(p_node, true);
        if (get_owner() != nullptr) {
            CollisionShape3D *cshape = Object::cast_to<CollisionShape3D>(p_node->get_child(0));
            p_node->set_owner(get_owner());
            if (cshape != nullptr) {
                cshape->set_owner(get_owner());
            }
        }
    }
}

void PathExtrude3D::queue_rebuild() {
    dirty = true;
    callable_mp(this, &PathExtrude3D::_rebuild_mesh).call_deferred();
}

void PathExtrude3D::_rebuild_mesh() {
    generated_mesh->clear_surfaces();

    if (!dirty) {
        return;
    }
    dirty = false;

    if (profile.is_null() || path3d == nullptr || path3d->get_curve().is_null()) {
        return;
    }

    PackedVector2Array cross_section = profile->get_cross_section();

    Ref<Curve3D> curve = path3d->get_curve();
    if (curve->get_point_count() < 2) {
        return;
    }
    PackedVector3Array tessellated_points = curve->tessellate(tessellation_max_stages, tessellation_tolerance_degrees);
    uint64_t n_slices = tessellated_points.size();
    if (n_slices < 2) {
        return;
    }
    uint64_t n_vertices = cross_section.size();
    if (n_vertices < 2) {
        return;
    }

    PackedVector2Array offset_cs = cross_section.duplicate();
    for (uint64_t i = 0; i < n_vertices; ++i) {
        offset_cs[i] += offset;
    }

    PackedFloat32Array u_vals;
    u_vals.resize(n_vertices);
    u_vals[0] = 0.0;
    double total_width = 0.0;
    for (uint64_t i = 1; i < n_vertices; ++i) {
        Vector2 p0 = offset_cs[i - 1];
        Vector2 p1 = offset_cs[i];
        double width = p0.distance_to(p1);
        u_vals[i] = width;
        total_width += width;
    }
    for (uint64_t i = 1; i < n_vertices; ++i) {
        u_vals[i] /= total_width;
    }

    Vector<Transform3D> tessellated_transforms;
    tessellated_transforms.resize(n_slices);
    for (uint64_t i = 0; i < n_slices; ++i) {
        Vector3 p = tessellated_points[i];
        double offset = curve->get_closest_offset(p);
        tessellated_transforms.write[i] = curve->sample_baked_with_rotation(offset, sample_cubic, tilt);
    }

    Vector<PackedVector3Array> points;
    points.resize(n_vertices);
    Vector<PackedFloat32Array> v_vals;
    v_vals.resize(n_vertices);
    v_vals.fill(PackedFloat32Array());
    PackedFloat32Array total_v_vals;
    total_v_vals.resize(n_vertices);
    total_v_vals.fill(0.0);
    for (uint64_t i = 0; i < n_vertices; ++i) {
        points.write[i].resize(n_slices);
        v_vals.write[i].resize(n_slices);
        for (uint64_t j = 0; j < n_slices; ++j) {
            points.write[i][j] = tessellated_transforms[j].xform(
                Vector3(offset_cs[i].x, offset_cs[i].y, 0.0));
        }
        for (uint64_t j = 1; j < n_slices; ++j) {
            Vector3 p0 = points[i][j - 1];
            Vector3 p1 = points[i][j];
            double v = p0.distance_to(p1);
            v_vals.write[i][j] = v;
            total_v_vals[i] += v;
        }
        for (uint64_t j = 0; j < n_slices; ++j) {
            v_vals.write[i][j] /= total_v_vals[i];
        }
    }

    Vector<PackedVector3Array> norms;
    norms.resize(n_vertices);
    for (uint64_t i = 0; i < n_vertices; ++i) {
        norms.write[i].resize(n_slices);
        uint64_t im1 = i == 0 ? n_vertices - 1 : i - 1;
        uint64_t ip1 = i == n_vertices - 1 ? 0 : i + 1;
        if (smooth) {
            for (uint64_t j = 0; j < n_slices; ++j) {
                uint64_t jm1 = j == 0 ? 0 : j - 1;
                uint64_t jp1 = j == n_slices - 1 ? n_slices - 1 : j + 1;
                norms.write[i][j] = (points[i][jm1] - points[i][jp1]).cross(
                        points[im1][j] - points[ip1][j]).normalized();
            }
        } else {
            for (uint64_t j = 0; j < n_slices; ++j) {
                uint64_t jp1 = j == n_slices - 1 ? n_slices - 1 : j + 1;
                norms.write[i][j] = (points[i][jp1] - points[i][j]).cross(
                        points[ip1][j] - points[i][j]).normalized();
            }
        }
    }

    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;

    uint64_t sz = 6 * (n_slices - 1) * (n_vertices - 1);
    switch (end_cap_mode) {
        case END_CAPS_START:
        case END_CAPS_END:
            sz += 3 * n_vertices;
            break;
        case END_CAPS_BOTH:
            sz += 6 * n_vertices;
            break;
    }

    vertices.resize(sz);
    normals.resize(sz);
    uvs.resize(sz);

    uint64_t k = 0;

    PackedInt32Array cap = end_cap_mode == END_CAPS_NONE ? 
        PackedInt32Array() : Geometry2D::get_singleton()->triangulate_polygon(offset_cs);
    if (profile->get_flip_normals()) {
        cap.reverse();
    }

    if (end_cap_mode & END_CAPS_START) {
        PackedInt32Array cap1 = cap;
        cap1.reverse();
        for (uint64_t i = 0; i < cap1.size(); ++i) {
            vertices[k] = tessellated_transforms[0].xform(Vector3(offset_cs[cap1[i]].x, offset_cs[cap1[i]].y, 0.0));
            normals[k] = tessellated_transforms[0].basis.get_column(2);
            uvs[k] = Vector2(u_vals[cap1[i]], 0.0);
            k++;
        }
    }

    for (uint64_t i = 0; i < n_slices - 1; ++i) {
        for (uint64_t j = 0; j < n_vertices - 1; ++j) {
            uint64_t i0 = i;
            uint64_t i1 = i + 1;
            uint64_t j0 = j;
            uint64_t j1 = j + 1;

            vertices[k] = points[j0][i0];
            normals[k] = norms[j0][i0];
            uvs[k] = Vector2(u_vals[j0], v_vals[j0][i0]);
            k++;

            vertices[k] = points[j1][i0];
            normals[k] = norms[j1][i0];
            uvs[k] = Vector2(u_vals[j1], v_vals[j1][i0]);
            k++;

            vertices[k] = points[j0][i1];
            normals[k] = norms[j0][i1];
            uvs[k] = Vector2(u_vals[j0], v_vals[j0][i1]);
            k++;

            vertices[k] = points[j0][i1];
            normals[k] = norms[j0][i1];
            uvs[k] = Vector2(u_vals[j0], v_vals[j0][i1]);
            k++;

            vertices[k] = points[j1][i0];
            normals[k] = norms[j1][i0];
            uvs[k] = Vector2(u_vals[j1], v_vals[j1][i0]);
            k++;

            vertices[k] = points[j1][i1];
            normals[k] = norms[j1][i1];
            uvs[k] = Vector2(u_vals[j1], v_vals[j1][i1]);
            k++;
        }
    }

    if (end_cap_mode & END_CAPS_END) {
        for (uint64_t i = 0; i < cap.size(); ++i) {
            vertices[k] = tessellated_transforms[n_slices - 1].xform(Vector3(offset_cs[cap[i]].x, offset_cs[cap[i]].y, 0.0));
            normals[k] = tessellated_transforms[n_slices - 1].basis.get_column(2);
            uvs[k] = Vector2(u_vals[cap[i]], 1.0);
            k++;
        }
    }

    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_NORMAL] = normals;
    arrays[Mesh::ARRAY_TEX_UV] = uvs;

    generated_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    set_base(generated_mesh->get_rid());
}

void PathExtrude3D::_on_profile_changed() {
    queue_rebuild();
    emit_signal("cross_section_changed");
}

void PathExtrude3D::_on_curve_changed() {
    queue_rebuild();
    emit_signal("curve_changed");
}
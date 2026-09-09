#include "path_extrude_profile_rect.hpp"

#include <godot_cpp/classes/mesh.hpp>

using namespace godot;

void PathExtrudeProfileRect::set_rect(const Rect2 &p_rect) { 
    if (p_rect != rect) { 
        rect = p_rect;  
        queue_update(); 
    } 
}

Rect2 PathExtrudeProfileRect::get_rect() const { 
    return rect; 
}

void PathExtrudeProfileRect::set_subdivisions(const Vector2i p_subdivisions) {
    if (p_subdivisions != subdivisions) {
        subdivisions = p_subdivisions;
        queue_update();
    }
}

Vector2i PathExtrudeProfileRect::get_subdivisions() const { 
    return subdivisions;
}

void PathExtrudeProfileRect::set_smooth_normals(const bool p_smooth_normals) { 
    if (p_smooth_normals != smooth_normals) { 
        smooth_normals = p_smooth_normals; 
        queue_update(); 
    } 
}

bool PathExtrudeProfileRect::get_smooth_normals() const { 
    return smooth_normals; 
}

void PathExtrudeProfileRect::set_end_cap_mode(const EndCapMode p_mode) {
    if (end_cap_mode != p_mode) {
        end_cap_mode = p_mode;
        notify_property_list_changed();
        queue_update();
    }
}

PathExtrudeProfileRect::EndCapMode PathExtrudeProfileRect::get_end_cap_mode() const {
    return end_cap_mode;
}

void PathExtrudeProfileRect::set_end_cap_arrow_flare_width(const double p_width) {
    const double clamped_width = Math::max(p_width, 0.0);
    if (end_cap_arrow_flare_width != clamped_width) {
        end_cap_arrow_flare_width = clamped_width;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_flare_width() const {
    return end_cap_arrow_flare_width;
}

void PathExtrudeProfileRect::set_end_cap_arrow_flare_height(const double p_height) {
    const double clamped_height = Math::max(p_height, 0.0);
    if (end_cap_arrow_flare_height != clamped_height) {
        end_cap_arrow_flare_height = clamped_height;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_flare_height() const {
    return end_cap_arrow_flare_height;
}

void PathExtrudeProfileRect::set_end_cap_arrow_flare_length(const double p_depth) {
    const double clamped_depth = Math::max(p_depth, 0.0);
    if (end_cap_arrow_flare_length != clamped_depth) {
        end_cap_arrow_flare_length = clamped_depth;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_flare_length() const {
    return end_cap_arrow_flare_length;
}

void PathExtrudeProfileRect::set_end_cap_arrow_tip_length(const double p_depth) {
    const double clamped_depth = Math::max(p_depth, 0.0);
    if (end_cap_arrow_tip_length != clamped_depth) {
        end_cap_arrow_tip_length = clamped_depth;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_tip_length() const {
    return end_cap_arrow_tip_length;
}

void PathExtrudeProfileRect::set_end_cap_arrow_tip_width(const double p_width) {
    const double clamped_width = Math::max(p_width, 0.0);
    if (end_cap_arrow_tip_width != clamped_width) {
        end_cap_arrow_tip_width = clamped_width;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_tip_width() const {
    return end_cap_arrow_tip_width;
}

void PathExtrudeProfileRect::set_end_cap_arrow_tip_height(const double p_height) {
    const double clamped_height = Math::max(p_height, 0.0);
    if (end_cap_arrow_tip_height != clamped_height) {
        end_cap_arrow_tip_height = clamped_height;
        queue_update();
    }
}

double PathExtrudeProfileRect::get_end_cap_arrow_tip_height() const {
    return end_cap_arrow_tip_height;
}


Array PathExtrudeProfileRect::_generate_cross_section() {
    PackedVector2Array cs;
    PackedVector2Array norms;
    
    const double height_subdiv = rect.size.y / (subdivisions.y + 1);
    const double width_subdiv = rect.size.x / (subdivisions.x + 1);
    const double norm_length = 1.0 / Math::sqrt(2.0);

    double x = rect.position.x;
    double y = rect.position.y;

    cs.push_back(Vector2(x, y));
    if (smooth_normals) {
        norms.push_back(Vector2(-norm_length, -norm_length));
    } else {
        norms.push_back(Vector2(0.0, -1.0));
    }

    for (uint64_t idx = 0; idx < subdivisions.x; ++idx) {
        x += width_subdiv;
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(0.0, -1.0));
    }
    x += width_subdiv;

    if (smooth_normals) {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(norm_length, -norm_length));
    } else {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(0.0, -1.0));
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(1.0, 0.0));
    }

    for (uint64_t idx = 0; idx < subdivisions.y; ++idx) {
        y += height_subdiv;
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(1.0, 0.0));
    }
    y += height_subdiv;

    if (smooth_normals) {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(norm_length, norm_length));
    } else {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(1.0, 0.0));
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(0.0, 1.0));
    }

    for (uint64_t idx = 0; idx < subdivisions.x; ++idx) {
        x -= width_subdiv;
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(0.0, 1.0));
    }
    x -= width_subdiv;

    if (smooth_normals) {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(-norm_length, norm_length));
    } else {
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(0.0, 1.0));
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(-1.0, 0.0));
    }

    for (uint64_t idx = 0; idx < subdivisions.y; ++idx) {
        y -= height_subdiv;
        cs.push_back(Vector2(x, y));
        norms.push_back(Vector2(-1.0, 0.0));
    }
    y -= height_subdiv;

    cs.push_back(Vector2(x, y));
    if (smooth_normals) {
        norms.push_back(Vector2(-norm_length, -norm_length));
    } else {
        norms.push_back(Vector2(-1.0, 0.0));
    }

    Array out;
    out.resize(Mesh::ARRAY_MAX);
    out[Mesh::ARRAY_VERTEX] = cs;
    out[Mesh::ARRAY_NORMAL] = norms;
    out[Mesh::ARRAY_TEX_UV] = _generate_v(cs);
    return out;
}

Array PathExtrudeProfileRect::_generate_end_cap() {
    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;

    if (rect.size.x <= 0.0 || rect.size.y <= 0.0) {
        return Array::make(PackedVector3Array());
    }

    const Vector2 rect_end = rect.position + rect.size;
    const Vector2 center = rect.position + rect.size * 0.5;
    const Vector2 flare_size = rect.size + Vector2(end_cap_arrow_flare_width, end_cap_arrow_flare_height);
    const Vector2 flare_min = center - flare_size * 0.5;
    const Vector2 flare_max = center + flare_size * 0.5;
    const Vector2 tip_size(end_cap_arrow_tip_width, end_cap_arrow_tip_height);
    const Vector2 tip_min = center - tip_size * 0.5;
    const Vector2 tip_max = center + tip_size * 0.5;
    const double tip_z = end_cap_arrow_flare_length + end_cap_arrow_tip_length;

        Vector2 uv_min = rect.position;
        Vector2 uv_max = rect_end;
    if (end_cap_mode == END_CAP_ARROW) {
        uv_min = Vector2(Math::min(rect.position.x, Math::min(flare_min.x, tip_min.x)),
            Math::min(rect.position.y, Math::min(flare_min.y, tip_min.y)));
        uv_max = Vector2(Math::max(rect_end.x, Math::max(flare_max.x, tip_max.x)),
            Math::max(rect_end.y, Math::max(flare_max.y, tip_max.y)));
    }
    Vector2 uv_size = uv_max - uv_min;
    if (uv_size.x == 0.0 || uv_size.y == 0.0) {
        return Array::make(PackedVector3Array());
    }

    auto get_uv = [&](const Vector3 &p_vertex) {
        return Vector2(
                Math::clamp(0.5 + (p_vertex.x - center.x) / uv_size.x, 0.0, 1.0),
                Math::clamp(0.5 + (p_vertex.y - center.y) / uv_size.y, 0.0, 1.0));
    };

    auto append_triangle = [&](const Vector3 &p_a, const Vector3 &p_b, const Vector3 &p_c) {
        Vector3 normal = (p_b - p_a).cross(p_c - p_a).normalized();
        vertices.push_back(p_a);
        vertices.push_back(p_b);
        vertices.push_back(p_c);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
        uvs.push_back(get_uv(p_a));
        uvs.push_back(get_uv(p_b));
        uvs.push_back(get_uv(p_c));
    };

    Vector3 base_00(rect.position.x, rect.position.y, 0.0);
    Vector3 base_10(rect_end.x, rect.position.y, 0.0);
    Vector3 base_11(rect_end.x, rect_end.y, 0.0);
    Vector3 base_01(rect.position.x, rect_end.y, 0.0);

    if (end_cap_mode == END_CAP_FLAT) {
        append_triangle(base_00, base_10, base_11);
        append_triangle(base_00, base_11, base_01);
    } else {
        Vector3 flare_00(flare_min.x, flare_min.y, end_cap_arrow_flare_length);
        Vector3 flare_10(flare_max.x, flare_min.y, end_cap_arrow_flare_length);
        Vector3 flare_11(flare_max.x, flare_max.y, end_cap_arrow_flare_length);
        Vector3 flare_01(flare_min.x, flare_max.y, end_cap_arrow_flare_length);
        Vector3 tip_00(tip_min.x, tip_min.y, tip_z);
        Vector3 tip_10(tip_max.x, tip_min.y, tip_z);
        Vector3 tip_11(tip_max.x, tip_max.y, tip_z);
        Vector3 tip_01(tip_min.x, tip_max.y, tip_z);

        append_triangle(base_00, base_10, flare_10);
        append_triangle(base_00, flare_10, flare_00);
        append_triangle(base_10, base_11, flare_11);
        append_triangle(base_10, flare_11, flare_10);
        append_triangle(base_11, base_01, flare_01);
        append_triangle(base_11, flare_01, flare_11);
        append_triangle(base_01, base_00, flare_00);
        append_triangle(base_01, flare_00, flare_01);

        append_triangle(flare_00, flare_10, tip_10);
        append_triangle(flare_00, tip_10, tip_00);
        append_triangle(flare_10, flare_11, tip_11);
        append_triangle(flare_10, tip_11, tip_10);
        append_triangle(flare_11, flare_01, tip_01);
        append_triangle(flare_11, tip_01, tip_11);
        append_triangle(flare_01, flare_00, tip_00);
        append_triangle(flare_01, tip_00, tip_01);

        if (end_cap_arrow_tip_width > 0.0 && end_cap_arrow_tip_height > 0.0) {
            append_triangle(tip_00, tip_10, tip_11);
            append_triangle(tip_00, tip_11, tip_01);
        }
    }

    Array out;
    out.resize(Mesh::ARRAY_MAX);
    out[Mesh::ARRAY_VERTEX] = vertices;
    out[Mesh::ARRAY_NORMAL] = normals;
    out[Mesh::ARRAY_TEX_UV] = uvs;
    return out;
}

void PathExtrudeProfileRect::_validate_property(PropertyInfo &p_property) const {
    if (p_property.name == StringName("end_cap_arrow_flare_width") ||
            p_property.name == StringName("end_cap_arrow_flare_height") ||
            p_property.name == StringName("end_cap_arrow_flare_length") ||
            p_property.name == StringName("end_cap_arrow_tip_length") ||
            p_property.name == StringName("end_cap_arrow_tip_width") ||
            p_property.name == StringName("end_cap_arrow_tip_height")) {
        if (end_cap_mode != END_CAP_ARROW) {
            p_property.usage = PROPERTY_USAGE_NONE;
        }
    }
}

void PathExtrudeProfileRect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_rect", "rect"), &PathExtrudeProfileRect::set_rect);
    ClassDB::bind_method(D_METHOD("get_rect"), &PathExtrudeProfileRect::get_rect);
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "rect"), "set_rect", "get_rect");

    ClassDB::bind_method(D_METHOD("set_subdivisions", "subdivisions"), &PathExtrudeProfileRect::set_subdivisions);
    ClassDB::bind_method(D_METHOD("get_subdivisions"), &PathExtrudeProfileRect::get_subdivisions);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "subdivisions", PROPERTY_HINT_RANGE, "0,256,1,or_greater"), "set_subdivisions", "get_subdivisions");

    ClassDB::bind_method(D_METHOD("set_smooth_normals", "smooth_normals"), &PathExtrudeProfileRect::set_smooth_normals);
    ClassDB::bind_method(D_METHOD("get_smooth_normals"), &PathExtrudeProfileRect::get_smooth_normals);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_normals"), "set_smooth_normals", "get_smooth_normals");

    ADD_GROUP("End Cap", "end_cap_");

    ClassDB::bind_method(D_METHOD("set_end_cap_mode", "mode"), &PathExtrudeProfileRect::set_end_cap_mode);
    ClassDB::bind_method(D_METHOD("get_end_cap_mode"), &PathExtrudeProfileRect::get_end_cap_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "end_cap_mode", PROPERTY_HINT_ENUM, "Flat,Arrow"), "set_end_cap_mode", "get_end_cap_mode");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_flare_width", "width"), &PathExtrudeProfileRect::set_end_cap_arrow_flare_width);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_flare_width"), &PathExtrudeProfileRect::get_end_cap_arrow_flare_width);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_flare_width", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_flare_width", "get_end_cap_arrow_flare_width");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_flare_height", "height"), &PathExtrudeProfileRect::set_end_cap_arrow_flare_height);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_flare_height"), &PathExtrudeProfileRect::get_end_cap_arrow_flare_height);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_flare_height", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_flare_height", "get_end_cap_arrow_flare_height");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_flare_length", "depth"), &PathExtrudeProfileRect::set_end_cap_arrow_flare_length);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_flare_length"), &PathExtrudeProfileRect::get_end_cap_arrow_flare_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_flare_length", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_flare_length", "get_end_cap_arrow_flare_length");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_tip_width", "width"), &PathExtrudeProfileRect::set_end_cap_arrow_tip_width);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_tip_width"), &PathExtrudeProfileRect::get_end_cap_arrow_tip_width);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_tip_width", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_tip_width", "get_end_cap_arrow_tip_width");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_tip_height", "height"), &PathExtrudeProfileRect::set_end_cap_arrow_tip_height);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_tip_height"), &PathExtrudeProfileRect::get_end_cap_arrow_tip_height);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_tip_height", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_tip_height", "get_end_cap_arrow_tip_height");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_tip_length", "depth"), &PathExtrudeProfileRect::set_end_cap_arrow_tip_length);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_tip_length"), &PathExtrudeProfileRect::get_end_cap_arrow_tip_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_tip_length", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_tip_length", "get_end_cap_arrow_tip_length");

    BIND_ENUM_CONSTANT(END_CAP_FLAT);
    BIND_ENUM_CONSTANT(END_CAP_ARROW);
}
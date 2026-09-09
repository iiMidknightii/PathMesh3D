#include "path_extrude_profile_manual.hpp"

#include <godot_cpp/classes/geometry2d.hpp>
#include <godot_cpp/classes/mesh.hpp>

using namespace godot;

void PathExtrudeProfileManual::set_manual_cross_section(const PackedVector2Array &p_cross_section) { 
    if (p_cross_section != manual_cross_section) {
        manual_cross_section = p_cross_section; 
        queue_update();
    }
}

PackedVector2Array PathExtrudeProfileManual::get_manual_cross_section() const {
    return manual_cross_section; 
}

void PathExtrudeProfileManual::set_smooth_normals(bool p_smooth_normals) {
    if (p_smooth_normals != smooth_normals) {
        smooth_normals = p_smooth_normals;
        queue_update();
    }
}

bool PathExtrudeProfileManual::get_smooth_normals() const { 
    return smooth_normals; 
}

void PathExtrudeProfileManual::set_closed(bool p_closed) {
    if (p_closed != closed) {
        closed = p_closed;
        queue_update();
    }
}

bool PathExtrudeProfileManual::get_closed() const { 
    return closed; 
}

Array PathExtrudeProfileManual::_generate_cross_section()  { 
    if (manual_cross_section.is_empty()) {
        return Array::make(PackedVector2Array());
    }

    Vector2 start = manual_cross_section[0];
    Vector2 end = manual_cross_section[manual_cross_section.size() - 1];
    bool already_closed = start.distance_squared_to(end) < 1.0e-6;

    PackedVector2Array cs;
    PackedVector2Array norms;

    if (smooth_normals) {
        cs = manual_cross_section;
        norms.resize(cs.size());
        for (uint64_t i = 0; i < cs.size(); ++i) {
            Vector2 start_last_segment = i == 0 ? 
                (already_closed ? end : start) : manual_cross_section[i - 1];
            Vector2 end_last_segment = manual_cross_section[i];
            
            Vector2 start_next_segment = manual_cross_section[i];
            Vector2 end_next_segment = i == manual_cross_section.size() - 1 ? 
                (already_closed ? start : end) : manual_cross_section[i + 1];

            Vector2 smoothed_normal = (end_last_segment - start_last_segment) + (end_next_segment - start_next_segment);
            norms[i] = smoothed_normal.normalized();
        }
    } else {
        for (uint64_t i = 0; i < manual_cross_section.size() - 1; ++i) {
            Vector2 p1 = manual_cross_section[i];
            Vector2 p2 = manual_cross_section[i + 1];
            Vector2 segment = p2 - p1;
            Vector2 segment_normal = Vector2(segment.y, -segment.x).normalized();
            cs.push_back(p1);
            cs.push_back(p2);
            norms.push_back(segment_normal);
            norms.push_back(segment_normal);
        }
    }

    if (closed && !already_closed) {
        Vector2 end_segment_start = cs[cs.size() - 1];
        Vector2 end_segment_end = cs[0];
        Vector2 end_segment = end_segment_end - end_segment_start;
        Vector2 end_segment_normal = Vector2(end_segment.y, -end_segment.x).normalized();
        cs.push_back(end_segment_start);
        cs.push_back(end_segment_end);
        norms.push_back(end_segment_normal);
        norms.push_back(end_segment_normal);
    }

    Array out;
    out.resize(Mesh::ARRAY_MAX);
    out[Mesh::ARRAY_VERTEX] = cs;
    out[Mesh::ARRAY_NORMAL] = norms;
    out[Mesh::ARRAY_TEX_UV] = _generate_v(cs);
    return out;
}

Array PathExtrudeProfileManual::_generate_end_cap() {
    Array out;
    out.resize(Mesh::ARRAY_MAX);

    if (!closed || manual_cross_section.size() < 3) {
        return out;
    }

    PackedVector2Array polygon = manual_cross_section;
    if (polygon.size() > 1 && polygon[0].distance_squared_to(polygon[polygon.size() - 1]) < 1.0e-6) {
        polygon.resize(polygon.size() - 1);
    }
    if (polygon.size() < 3) {
        return out;
    }

    // Ensure counter-clockwise winding so the generated face points toward +Z.
    real_t area = 0.0;
    for (int i = 0; i < polygon.size(); ++i) {
        const Vector2 &a = polygon[i];
        const Vector2 &b = polygon[(i + 1) % polygon.size()];
        area += a.x * b.y - b.x * a.y;
    }
    if (area < 0.0) {
        PackedVector2Array reversed;
        reversed.resize(polygon.size());
        for (int i = 0; i < polygon.size(); ++i) {
            reversed[i] = polygon[polygon.size() - 1 - i];
        }
        polygon = reversed;
    }

    PackedInt32Array indices = Geometry2D::get_singleton()->triangulate_polygon(polygon);
    if (indices.is_empty()) {
        return out;
    }

    PackedVector3Array vertices;
    PackedVector3Array normals;
    vertices.resize(polygon.size());
    normals.resize(polygon.size());
    for (int i = 0; i < polygon.size(); ++i) {
        vertices[i] = Vector3(polygon[i].x, polygon[i].y, 0.0);
        // End caps are planar regardless of the side normal setting.
        normals[i] = Vector3(0.0, 0.0, 1.0);
    }

    Vector2 bounds_min = polygon[0];
    Vector2 bounds_max = polygon[0];
    for (int i = 1; i < polygon.size(); ++i) {
        bounds_min.x = MIN(bounds_min.x, polygon[i].x);
        bounds_min.y = MIN(bounds_min.y, polygon[i].y);
        bounds_max.x = MAX(bounds_max.x, polygon[i].x);
        bounds_max.y = MAX(bounds_max.y, polygon[i].y);
    }

    const real_t width = bounds_max.x - bounds_min.x;
    const real_t height = bounds_max.y - bounds_min.y;
    PackedVector2Array cap_uvs;
    cap_uvs.resize(polygon.size());
    for (int i = 0; i < polygon.size(); ++i) {
        cap_uvs[i] = Vector2(
                width > 0.0 ? (polygon[i].x - bounds_min.x) / width : 0.0,
                height > 0.0 ? (polygon[i].y - bounds_min.y) / height : 0.0);
    }

    out[Mesh::ARRAY_VERTEX] = vertices;
    out[Mesh::ARRAY_NORMAL] = normals;
    out[Mesh::ARRAY_TEX_UV] = cap_uvs;
    out[Mesh::ARRAY_INDEX] = indices;
    return out;
}

void PathExtrudeProfileManual::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_manual_cross_section", "cross_section"), &PathExtrudeProfileManual::set_manual_cross_section);
    ClassDB::bind_method(D_METHOD("get_manual_cross_section"), &PathExtrudeProfileManual::get_manual_cross_section);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR2_ARRAY, "cross_section"), "set_manual_cross_section", "get_manual_cross_section");

    ClassDB::bind_method(D_METHOD("set_closed", "closed"), &PathExtrudeProfileManual::set_closed);
    ClassDB::bind_method(D_METHOD("get_closed"), &PathExtrudeProfileManual::get_closed);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "closed"), "set_closed", "get_closed");

    ClassDB::bind_method(D_METHOD("set_smooth_normals", "smooth_normals"), &PathExtrudeProfileManual::set_smooth_normals);
    ClassDB::bind_method(D_METHOD("get_smooth_normals"), &PathExtrudeProfileManual::get_smooth_normals);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_normals"), "set_smooth_normals", "get_smooth_normals");
}
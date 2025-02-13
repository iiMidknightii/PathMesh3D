#include "path_extrude_profile_manual.hpp"


using namespace godot;

Array PathExtrudeProfileManual::_generate_cross_section()  { 
    if (manual_cross_section.is_empty()) {
        return Array::make(PackedVector2Array());
    }

    Array out;

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

    out.push_back(cs);
    out.push_back(norms);

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
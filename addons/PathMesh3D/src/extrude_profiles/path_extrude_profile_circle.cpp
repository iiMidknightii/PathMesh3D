#include "path_extrude_profile_circle.hpp"

#include <godot_cpp/classes/mesh.hpp>

using namespace godot;

void PathExtrudeProfileCircle::set_radius(const double p_radius) {
    if (p_radius != radius) {
        radius = p_radius;
        queue_update();
    }
}

double PathExtrudeProfileCircle::get_radius() const { 
    return radius; 
}

void PathExtrudeProfileCircle::set_starting_angle(const double p_starting_angle) {
    if (p_starting_angle != starting_angle) { 
        starting_angle = Math::clamp(p_starting_angle, double(0.0), double(Math_TAU)); 
        if (starting_angle > ending_angle) {
            ending_angle = starting_angle;
        }
        queue_update(); 
    } 
}

double PathExtrudeProfileCircle::get_starting_angle() const { 
    return starting_angle; 
}

void PathExtrudeProfileCircle::set_ending_angle(const double p_ending_angle) { 
    if (p_ending_angle != ending_angle) { 
        ending_angle = Math::clamp(p_ending_angle, double(0.0), double(Math_TAU)); 
        if (ending_angle < starting_angle) {
            starting_angle = ending_angle;
        }
        queue_update(); 
    } 
}

double PathExtrudeProfileCircle::get_ending_angle() const { 
    return ending_angle; 
}

void PathExtrudeProfileCircle::set_smooth_normals(const bool p_smooth_normals) { 
    if (p_smooth_normals != smooth_normals) { 
        smooth_normals = p_smooth_normals; 
        queue_update(); 
    } 
}

bool PathExtrudeProfileCircle::get_smooth_normals() const { 
    return smooth_normals; 
}

void PathExtrudeProfileCircle::set_closed(const bool p_closed) { 
    if (p_closed != closed) { 
        closed = p_closed; 
        queue_update(); 
    } 
}

bool PathExtrudeProfileCircle::is_closed() const { 
    return closed; 
}

void PathExtrudeProfileCircle::set_segments(const uint64_t p_segments) { 
    if (p_segments != segments && p_segments > 1) { 
        segments = p_segments; 
        queue_update(); 
    } 
}

uint64_t PathExtrudeProfileCircle::get_segments() const { 
    return segments; 
}

void PathExtrudeProfileCircle::set_end_cap_mode(const EndCapMode p_mode) {
    if (end_cap_mode != p_mode) {
        end_cap_mode = p_mode;
        queue_update(); 
        notify_property_list_changed();
    } 
}

PathExtrudeProfileCircle::EndCapMode PathExtrudeProfileCircle::get_end_cap_mode() const {
    return end_cap_mode;
}

void PathExtrudeProfileCircle::set_end_cap_length(const double p_length) {
    const double clamped_length = Math::max(p_length, 0.0);
    if (end_cap_length != clamped_length) {
        end_cap_length = clamped_length;
        queue_update(); 
    }
}

double PathExtrudeProfileCircle::get_end_cap_length() const {
    return end_cap_length;
}

void PathExtrudeProfileCircle::set_end_cap_segments(const uint64_t p_segments) {
    if (end_cap_segments != p_segments && p_segments >= 1) {
        end_cap_segments = p_segments;
        queue_update();
    }
}

uint64_t PathExtrudeProfileCircle::get_end_cap_segments() const {
    return end_cap_segments;
}

void PathExtrudeProfileCircle::set_end_cap_arrow_flare_radius(const double p_radius) {
    const double clamped_radius = Math::max(p_radius, 0.0);
    if (end_cap_arrow_flare_radius != clamped_radius) {
        end_cap_arrow_flare_radius = clamped_radius;
        queue_update();
    }
}

double PathExtrudeProfileCircle::get_end_cap_arrow_flare_radius() const {
    return end_cap_arrow_flare_radius;
}

void PathExtrudeProfileCircle::set_end_cap_arrow_flare_length(const double p_length) {
    const double clamped_length = Math::max(p_length, 0.0);
    if (end_cap_arrow_flare_length != clamped_length) {
        end_cap_arrow_flare_length = clamped_length;
        queue_update();
    }
}

double PathExtrudeProfileCircle::get_end_cap_arrow_flare_length() const {
    return end_cap_arrow_flare_length;
}

void PathExtrudeProfileCircle::set_end_cap_arrow_tip_length(const double p_length) {
    const double clamped_length = Math::max(p_length, 0.0);
    if (end_cap_arrow_tip_length != clamped_length) {
        end_cap_arrow_tip_length = clamped_length;
        queue_update();
    }
}

double PathExtrudeProfileCircle::get_end_cap_arrow_tip_length() const {
    return end_cap_arrow_tip_length;
}

void PathExtrudeProfileCircle::set_end_cap_arrow_tip_radius(const double p_radius) {
    const double clamped_radius = Math::max(p_radius, 0.0);
    if (end_cap_arrow_tip_radius != clamped_radius) {
        end_cap_arrow_tip_radius = clamped_radius;
        queue_update();
    }
}

double PathExtrudeProfileCircle::get_end_cap_arrow_tip_radius() const {
    return end_cap_arrow_tip_radius;
}

Array PathExtrudeProfileCircle::_generate_cross_section() {
    PackedVector2Array cs;
    PackedVector2Array norms;

    double swept_angle = ending_angle - starting_angle;
    if (swept_angle <= 0.0) {
        return Array::make(PackedVector2Array());
    }

    double da = swept_angle / double(segments);

    if (smooth_normals) {
        cs.resize(segments + 1);
        norms.resize(segments + 1);
        for (uint64_t i = 0; i <= segments; ++i) {
            double ang = starting_angle + da * i;
            norms[i] = Vector2(Math::cos(ang), Math::sin(ang));
            cs[i] = norms[i] * radius;
        }
    } else {
        cs.resize(segments * 2);
        norms.resize(segments * 2);
        for (uint64_t i = 0; i < segments; ++i) {
            double ang = starting_angle + da * i;
            Vector2 start_segment = Vector2(Math::cos(ang), Math::sin(ang));
            Vector2 end_segment = Vector2(Math::cos(ang + da), Math::sin(ang + da));
            Vector2 segment = end_segment - start_segment;
            Vector2 segment_normal = Vector2(segment.y, -segment.x).normalized();

            cs[i * 2] = start_segment * radius;
            cs[i * 2 + 1] = end_segment * radius;
            norms[i * 2] = segment_normal;
            norms[i * 2 + 1] = segment_normal;
        }
    }

    if (closed && cs[0].distance_squared_to(cs[cs.size() - 1]) > 1.0e-6) {
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
    out[Mesh::ARRAY_TEX_UV] = _generate_v(cs);;
    return out;
}

Array PathExtrudeProfileCircle::_generate_end_cap() {
    PackedVector3Array verts;
    PackedVector3Array norms;
    PackedVector2Array uvs;

    if (radius <= 0.0 || segments < 3 || end_cap_segments < 1) {
        return Array::make(PackedVector3Array());
    }

    const double longitude_step = Math_TAU / double(segments);

    auto ring_vertex = [&](double p_ring_radius, double p_length, uint64_t p_longitude) {
        const double longitude = longitude_step * double(p_longitude);
        return Vector3(
                p_ring_radius * Math::cos(longitude),
                p_ring_radius * Math::sin(longitude),
                p_length);
    };

    auto get_uv = [&](const Vector3 &p_vertex) {
        return Vector2(
                0.5 + p_vertex.x / (2.0 * (radius + end_cap_arrow_flare_radius)),
                0.5 + p_vertex.y / (2.0 * (radius + end_cap_arrow_flare_radius)));
    };

    auto append_triangle = [&](const Vector3 &p_a, const Vector3 &p_b, const Vector3 &p_c) {
        Vector3 normal = (p_b - p_a).cross(p_c - p_a).normalized();
        verts.push_back(p_a);
        verts.push_back(p_b);
        verts.push_back(p_c);
        norms.push_back(normal);
        norms.push_back(normal);
        norms.push_back(normal);
        uvs.push_back(get_uv(p_a));
        uvs.push_back(get_uv(p_b));
        uvs.push_back(get_uv(p_c));
    };

    auto append_smooth_triangle = [&](const Vector3 &p_a, const Vector3 &p_b, const Vector3 &p_c,
            const Vector3 &p_normal_a, const Vector3 &p_normal_b, const Vector3 &p_normal_c) {
        verts.push_back(p_a);
        verts.push_back(p_b);
        verts.push_back(p_c);
        norms.push_back(p_normal_a.normalized());
        norms.push_back(p_normal_b.normalized());
        norms.push_back(p_normal_c.normalized());
        uvs.push_back(get_uv(p_a));
        uvs.push_back(get_uv(p_b));
        uvs.push_back(get_uv(p_c));
    };

    switch (end_cap_mode) {
        case END_CAP_SPHERICAL: {
            const double latitude_step = (Math_PI * 0.5) / double(end_cap_segments);
            const double swept_angle = ending_angle - starting_angle;
            if (swept_angle <= 0.0) {
                break;
            }
            const double longitude_step = swept_angle / double(segments);
            auto spherical_vertex = [&](uint64_t p_latitude, uint64_t p_longitude) {
                const double latitude = latitude_step * double(p_latitude);
                const double longitude = starting_angle + longitude_step * double(p_longitude);
                const double cos_latitude = Math::cos(latitude);
                return Vector3(
                        radius * cos_latitude * Math::cos(longitude),
                        radius * cos_latitude * Math::sin(longitude),
                        end_cap_length * Math::sin(latitude));
            };

            for (uint64_t latitude_idx = 0; latitude_idx < end_cap_segments; ++latitude_idx) {
                for (uint64_t longitude_idx = 0; longitude_idx < segments; ++longitude_idx) {
                    const uint64_t next_longitude_idx = longitude_idx + 1;
                    const Vector3 p00 = spherical_vertex(latitude_idx, longitude_idx);
                    const Vector3 p01 = spherical_vertex(latitude_idx, next_longitude_idx);
                    const Vector3 p10 = spherical_vertex(latitude_idx + 1, longitude_idx);
                    const Vector3 p11 = spherical_vertex(latitude_idx + 1, next_longitude_idx);
                    if (smooth_normals) {
                        auto spherical_normal = [&](const Vector3 &p_vertex) {
                            if (end_cap_length == 0.0) {
                                return Vector3(0.0, 0.0, 1.0);
                            }
                            return Vector3(p_vertex.x / radius, p_vertex.y / radius,
                                    p_vertex.z / end_cap_length).normalized();
                        };
                        append_smooth_triangle(p00, p01, p10, spherical_normal(p00), spherical_normal(p01), spherical_normal(p10));
                        append_smooth_triangle(p01, p11, p10, spherical_normal(p01), spherical_normal(p11), spherical_normal(p10));
                    } else {
                        append_triangle(p00, p01, p10);
                        append_triangle(p01, p11, p10);
                    }
                }
            }

            if (closed && swept_angle < Math_TAU) {
                for (uint64_t latitude_idx = 0; latitude_idx < end_cap_segments; ++latitude_idx) {
                    const Vector3 start_base = spherical_vertex(latitude_idx, 0);
                    const Vector3 start_next = spherical_vertex(latitude_idx + 1, 0);
                    const Vector3 end_base = spherical_vertex(latitude_idx, segments);
                    const Vector3 end_next = spherical_vertex(latitude_idx + 1, segments);

                    if (smooth_normals) {
                        const Vector3 bridge_normal = (start_next - start_base).cross(end_next - start_base).normalized();
                        append_smooth_triangle(start_base, start_next, end_next,
                                bridge_normal, bridge_normal, bridge_normal);
                        append_smooth_triangle(start_base, end_next, end_base,
                                bridge_normal, bridge_normal, bridge_normal);
                    } else {
                        append_triangle(start_base, start_next, end_next);
                        append_triangle(start_base, end_next, end_base);
                    }
                }
            }
        } break;
        case END_CAP_ARROW: {
            const double flare_ring_radius = radius + end_cap_arrow_flare_radius;
            const double tip_ring_radius = end_cap_arrow_tip_radius;
            const double tip_length = end_cap_arrow_flare_length + end_cap_arrow_tip_length;
            for (uint64_t longitude_idx = 0; longitude_idx < segments; ++longitude_idx) {
                const uint64_t next_longitude_idx = longitude_idx + 1;
                const Vector3 base_a = ring_vertex(radius, 0.0, longitude_idx);
                const Vector3 base_b = ring_vertex(radius, 0.0, next_longitude_idx);
                const Vector3 flare_a = ring_vertex(flare_ring_radius, end_cap_arrow_flare_length, longitude_idx);
                const Vector3 flare_b = ring_vertex(flare_ring_radius, end_cap_arrow_flare_length, next_longitude_idx);

                if (smooth_normals) {
                    const Vector3 base_normal_a(base_a.x, base_a.y, 0.0);
                    const Vector3 base_normal_b(base_b.x, base_b.y, 0.0);
                    const Vector3 flare_normal_a(flare_a.x, flare_a.y, 0.0);
                    const Vector3 flare_normal_b(flare_b.x, flare_b.y, 0.0);
                    append_smooth_triangle(base_a, base_b, flare_b, base_normal_a, base_normal_b, flare_normal_b);
                    append_smooth_triangle(base_a, flare_b, flare_a, base_normal_a, flare_normal_b, flare_normal_a);

                    if (end_cap_arrow_tip_radius > 0.0) {
                        const Vector3 tip_a = ring_vertex(tip_ring_radius, tip_length, longitude_idx);
                        const Vector3 tip_b = ring_vertex(tip_ring_radius, tip_length, next_longitude_idx);
                        const Vector3 tip_normal_a(tip_a.x, tip_a.y, 0.0);
                        const Vector3 tip_normal_b(tip_b.x, tip_b.y, 0.0);
                        append_smooth_triangle(flare_a, flare_b, tip_b, flare_normal_a, flare_normal_b, tip_normal_b);
                        append_smooth_triangle(flare_a, tip_b, tip_a, flare_normal_a, tip_normal_b, tip_normal_a);
                    }
                } else {
                    append_triangle(base_a, base_b, flare_b);
                    append_triangle(base_a, flare_b, flare_a);
                }

                if (!smooth_normals && end_cap_arrow_tip_radius > 0.0) {
                    const Vector3 tip_a = ring_vertex(tip_ring_radius, tip_length, longitude_idx);
                    const Vector3 tip_b = ring_vertex(tip_ring_radius, tip_length, next_longitude_idx);
                    append_triangle(flare_a, flare_b, tip_b);
                    append_triangle(flare_a, tip_b, tip_a);
                } else if (end_cap_arrow_tip_radius == 0.0) {
                    const Vector3 tip(0.0, 0.0, tip_length);
                    append_triangle(flare_a, flare_b, tip);
                }
            }

            if (end_cap_arrow_tip_radius > 0.0) {
                const Vector3 tip_center(0.0, 0.0, tip_length);
                for (uint64_t longitude_idx = 0; longitude_idx < segments; ++longitude_idx) {
                    append_triangle(tip_center,
                            ring_vertex(tip_ring_radius, tip_length, longitude_idx),
                            ring_vertex(tip_ring_radius, tip_length, longitude_idx + 1));
                }
            }
        } break;
        default: {
            ERR_PRINT("Invalid end cap mode");
        } break;
    }

    Array out;
    out.resize(Mesh::ARRAY_MAX);
    out[Mesh::ARRAY_VERTEX] = verts;
    out[Mesh::ARRAY_NORMAL] = norms;
    out[Mesh::ARRAY_TEX_UV] = uvs;
    return out;
}

bool PathExtrudeProfileCircle::_property_can_revert(const StringName &p_name) const {
    return p_name == StringName("end_cap_length") || p_name == StringName("end_cap_segments") ||
            p_name == StringName("end_cap_arrow_flare_radius") ||
            p_name == StringName("end_cap_arrow_tip_length") || p_name == StringName("end_cap_arrow_tip_radius");
}

bool PathExtrudeProfileCircle::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    if (p_name == StringName("end_cap_length") || p_name == StringName("end_cap_arrow_flare_radius") || p_name == StringName("end_cap_arrow_tip_radius")) {
        r_property = p_name == StringName("end_cap_length") ? radius : 0.0;
        return true;
    }
    if (p_name == StringName("end_cap_segments")) {
        r_property = segments / 2;
        return true;
    }
    if (p_name == StringName("end_cap_arrow_tip_length")) {
        r_property = radius;
        return true;
    }
    return false;
}

void PathExtrudeProfileCircle::_validate_property(PropertyInfo &p_property) const {
    const bool is_spherical_property = p_property.name == StringName("end_cap_length") ||
        p_property.name == StringName("end_cap_segments");

    const bool is_arrow_property = p_property.name == StringName("end_cap_arrow_flare_radius") ||
        p_property.name == StringName("end_cap_arrow_flare_length") ||
        p_property.name == StringName("end_cap_arrow_tip_length") ||
        p_property.name == StringName("end_cap_arrow_tip_radius");

    if (is_spherical_property && end_cap_mode != END_CAP_SPHERICAL || is_arrow_property && end_cap_mode != END_CAP_ARROW) {
        p_property.usage = PROPERTY_USAGE_NONE;
    }
}

void PathExtrudeProfileCircle::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PathExtrudeProfileCircle::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &PathExtrudeProfileCircle::get_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_radius", "get_radius");

    ClassDB::bind_method(D_METHOD("set_starting_angle", "starting_angle"), &PathExtrudeProfileCircle::set_starting_angle);
    ClassDB::bind_method(D_METHOD("get_starting_angle"), &PathExtrudeProfileCircle::get_starting_angle);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "starting_angle", PROPERTY_HINT_RANGE, "0.0,360.0,0.01,radians_as_degrees"), "set_starting_angle", "get_starting_angle");

    ClassDB::bind_method(D_METHOD("set_ending_angle", "ending_angle"), &PathExtrudeProfileCircle::set_ending_angle);
    ClassDB::bind_method(D_METHOD("get_ending_angle"), &PathExtrudeProfileCircle::get_ending_angle);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ending_angle", PROPERTY_HINT_RANGE, "0.0,360.0,0.01,radians_as_degrees"), "set_ending_angle", "get_ending_angle");

    ClassDB::bind_method(D_METHOD("set_closed", "closed"), &PathExtrudeProfileCircle::set_closed);
    ClassDB::bind_method(D_METHOD("get_closed"), &PathExtrudeProfileCircle::is_closed);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "closed"), "set_closed", "get_closed");

    ClassDB::bind_method(D_METHOD("set_segments", "segments"), &PathExtrudeProfileCircle::set_segments);
    ClassDB::bind_method(D_METHOD("get_segments"), &PathExtrudeProfileCircle::get_segments);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "segments", PROPERTY_HINT_RANGE, "0,256,1,or_greater"), "set_segments", "get_segments");

    ClassDB::bind_method(D_METHOD("set_smooth_normals", "smooth_normals"), &PathExtrudeProfileCircle::set_smooth_normals);
    ClassDB::bind_method(D_METHOD("get_smooth_normals"), &PathExtrudeProfileCircle::get_smooth_normals);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_normals"), "set_smooth_normals", "get_smooth_normals");

    ADD_GROUP("End Cap", "end_cap_");

    ClassDB::bind_method(D_METHOD("set_end_cap_mode", "mode"), &PathExtrudeProfileCircle::set_end_cap_mode);
    ClassDB::bind_method(D_METHOD("get_end_cap_mode"), &PathExtrudeProfileCircle::get_end_cap_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "end_cap_mode", PROPERTY_HINT_ENUM, "Spherical,Arrow"), "set_end_cap_mode", "get_end_cap_mode");

    ClassDB::bind_method(D_METHOD("set_end_cap_length", "height"), &PathExtrudeProfileCircle::set_end_cap_length);
    ClassDB::bind_method(D_METHOD("get_end_cap_length"), &PathExtrudeProfileCircle::get_end_cap_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_length", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_length", "get_end_cap_length");

    ClassDB::bind_method(D_METHOD("set_end_cap_segments", "segments"), &PathExtrudeProfileCircle::set_end_cap_segments);
    ClassDB::bind_method(D_METHOD("get_end_cap_segments"), &PathExtrudeProfileCircle::get_end_cap_segments);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "end_cap_segments", PROPERTY_HINT_RANGE, "1,256,1,or_greater"), "set_end_cap_segments", "get_end_cap_segments");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_flare_radius", "radius"), &PathExtrudeProfileCircle::set_end_cap_arrow_flare_radius);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_flare_radius"), &PathExtrudeProfileCircle::get_end_cap_arrow_flare_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_flare_radius", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_flare_radius", "get_end_cap_arrow_flare_radius");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_flare_length", "depth"), &PathExtrudeProfileCircle::set_end_cap_arrow_flare_length);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_flare_length"), &PathExtrudeProfileCircle::get_end_cap_arrow_flare_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_flare_length", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_flare_length", "get_end_cap_arrow_flare_length");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_tip_radius", "radius"), &PathExtrudeProfileCircle::set_end_cap_arrow_tip_radius);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_tip_radius"), &PathExtrudeProfileCircle::get_end_cap_arrow_tip_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_tip_radius", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_tip_radius", "get_end_cap_arrow_tip_radius");

    ClassDB::bind_method(D_METHOD("set_end_cap_arrow_tip_length", "depth"), &PathExtrudeProfileCircle::set_end_cap_arrow_tip_length);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrow_tip_length"), &PathExtrudeProfileCircle::get_end_cap_arrow_tip_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "end_cap_arrow_tip_length", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,or_greater"), "set_end_cap_arrow_tip_length", "get_end_cap_arrow_tip_length");

    BIND_ENUM_CONSTANT(END_CAP_SPHERICAL);
    BIND_ENUM_CONSTANT(END_CAP_ARROW);
}
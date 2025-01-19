#include "path_extrude_profile_circle.hpp"

using namespace godot;

PackedVector2Array PathExtrudeProfileCircle::_generate_cross_section() {
    PackedVector2Array cs;

    double swept_angle = ending_angle - starting_angle;
    if (swept_angle <= 0.0) {
        return cs;
    }

    double da = swept_angle / double(segments);

    cs.resize(segments + 1);
    for (uint64_t i = 0; i <= segments; ++i) {
        double ang = ending_angle - da * i;
        cs[i] = Vector2(Math::sin(ang) * radius, Math::cos(ang) * radius);
    }

    if (closed && cs[0].distance_squared_to(cs[cs.size() - 1]) > 1.0e-6) {
        cs.push_back(cs[0]);
    }

    return cs;
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
}
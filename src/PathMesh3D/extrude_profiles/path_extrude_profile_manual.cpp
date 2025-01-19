#include "path_extrude_profile_manual.hpp"


using namespace godot;

PackedVector2Array PathExtrudeProfileManual::_generate_cross_section()  { 
    if (manual_cross_section.is_empty()) {
        return PackedVector2Array();
    } 

    if (!closed) {
        return manual_cross_section;
    }

    PackedVector2Array out = manual_cross_section;
    Vector2 start = manual_cross_section[0];
    Vector2 end = manual_cross_section[manual_cross_section.size() - 1];
    if (start.distance_squared_to(end) > 1.0e-6) {
        out.push_back(start);
    }

    return out;
}

void PathExtrudeProfileManual::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_manual_cross_section", "cross_section"), &PathExtrudeProfileManual::set_manual_cross_section);
    ClassDB::bind_method(D_METHOD("get_manual_cross_section"), &PathExtrudeProfileManual::get_manual_cross_section);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR2_ARRAY, "cross_section"), "set_manual_cross_section", "get_manual_cross_section");

    ClassDB::bind_method(D_METHOD("set_closed", "closed"), &PathExtrudeProfileManual::set_closed);
    ClassDB::bind_method(D_METHOD("get_closed"), &PathExtrudeProfileManual::get_closed);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "closed"), "set_closed", "get_closed");
}
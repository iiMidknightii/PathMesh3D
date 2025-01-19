#include "path_extrude_profile_rect.hpp"

using namespace godot;


PackedVector2Array PathExtrudeProfileRect::_generate_cross_section() {
    PackedVector2Array cs;
    
    double height_subdiv = rect.size.y / (subdivisions.y + 1);
    double width_subdiv = rect.size.x / (subdivisions.x + 1);

    for (uint64_t i = 0; i <= subdivisions.y; ++i) {
        cs.push_back(Vector2(rect.position.x, rect.position.y + height_subdiv * i));
    }
    for (uint64_t i = 0; i <= subdivisions.x; ++i) {
        cs.push_back(Vector2(rect.position.x + width_subdiv * i, rect.position.y + rect.size.y));
    }
    for (uint64_t i = 0; i <= subdivisions.y; ++i) {
        cs.push_back(Vector2(rect.position.x + rect.size.x, rect.position.y + rect.size.y - height_subdiv * i));
    }
    for (uint64_t i = 0; i <= subdivisions.x; ++i) {
        cs.push_back(Vector2(rect.position.x + rect.size.x - width_subdiv * i, rect.position.y));
    }
    cs.push_back(Vector2(rect.position.x, rect.position.y));

    cs.reverse();

    return cs;
}

void PathExtrudeProfileRect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_rect", "rect"), &PathExtrudeProfileRect::set_rect);
    ClassDB::bind_method(D_METHOD("get_rect"), &PathExtrudeProfileRect::get_rect);
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "rect"), "set_rect", "get_rect");

    ClassDB::bind_method(D_METHOD("set_subdivisions", "subdivisions"), &PathExtrudeProfileRect::set_subdivisions);
    ClassDB::bind_method(D_METHOD("get_subdivisions"), &PathExtrudeProfileRect::get_subdivisions);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "subdivisions", PROPERTY_HINT_RANGE, "0,256,1,or_greater"), "set_subdivisions", "get_subdivisions");

    
}
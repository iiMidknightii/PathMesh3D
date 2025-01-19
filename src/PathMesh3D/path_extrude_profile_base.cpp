#include "path_extrude_profile_base.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedVector2Array PathExtrudeProfileBase::_generate_cross_section() {
    PackedVector2Array out;
    GDVIRTUAL_CALL(_generate_cross_section, out);
    return out;
}

void PathExtrudeProfileBase::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_POSTINITIALIZE: {
            queue_update();
        } break;
    }
}

void PathExtrudeProfileBase::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_cross_section"), &PathExtrudeProfileBase::get_cross_section);
    ClassDB::bind_method(D_METHOD("queue_update"), &PathExtrudeProfileBase::queue_update);

    ClassDB::bind_method(D_METHOD("set_flip_normals", "flip_normals"), &PathExtrudeProfileBase::set_flip_normals);
    ClassDB::bind_method(D_METHOD("get_flip_normals"), &PathExtrudeProfileBase::get_flip_normals);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_normals"), "set_flip_normals", "get_flip_normals");

    GDVIRTUAL_BIND(_generate_cross_section)
}

void PathExtrudeProfileBase::_regen() {
    if (dirty) {
        dirty = false;
        PackedVector2Array new_cross_section = _generate_cross_section();
        if (flip_normals) {
            new_cross_section.reverse();
        }
        if (new_cross_section != cross_section) {
            cross_section = new_cross_section;
            emit_changed();
        }
    }
}
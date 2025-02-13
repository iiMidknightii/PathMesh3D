#ifndef PATH_EXTRUDE_PROFILE_MANUAL_H
#define PATH_EXTRUDE_PROFILE_MANUAL_H

#include "path_extrude_profile_base.hpp"

namespace godot {

class PathExtrudeProfileManual : public PathExtrudeProfileBase {
    GDCLASS(PathExtrudeProfileManual, PathExtrudeProfileBase)

public:
    _ALWAYS_INLINE_ void set_manual_cross_section(const PackedVector2Array &p_cross_section) { 
        if (p_cross_section != manual_cross_section) {
            manual_cross_section = p_cross_section; 
            queue_update();
        }
    }
    _ALWAYS_INLINE_ PackedVector2Array get_manual_cross_section() const { return manual_cross_section; }

    _ALWAYS_INLINE_ void set_smooth_normals(bool p_smooth_normals) {
        if (p_smooth_normals != smooth_normals) {
            smooth_normals = p_smooth_normals;
            queue_update();
        }
    }
    _ALWAYS_INLINE_ bool get_smooth_normals() const { return smooth_normals; }
    
    _ALWAYS_INLINE_ void set_closed(bool p_closed) {
        if (p_closed != closed) {
            closed = p_closed;
            queue_update();
        }
    }
    _ALWAYS_INLINE_ bool get_closed() const { return closed; }

protected:
    Array _generate_cross_section() override;

    static void _bind_methods();

private:
    PackedVector2Array manual_cross_section;
    bool smooth_normals = false;
    bool closed = true;
};

}

#endif
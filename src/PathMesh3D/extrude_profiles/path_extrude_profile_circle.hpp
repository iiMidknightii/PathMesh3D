#ifndef PATH_EXTRUDE_PROFILE_CIRCLE_H
#define PATH_EXTRUDE_PROFILE_CIRCLE_H

#include "path_extrude_profile_base.hpp"

namespace godot {

class PathExtrudeProfileCircle : public PathExtrudeProfileBase {
    GDCLASS(PathExtrudeProfileCircle, PathExtrudeProfileBase)

public:
    _ALWAYS_INLINE_ void set_radius(const double p_radius) {
        if (p_radius != radius) {
            radius = p_radius;
            queue_update();
        }
    }
    _ALWAYS_INLINE_ double get_radius() const { return radius; }

    _ALWAYS_INLINE_ void set_starting_angle(const double p_starting_angle) {
        if (p_starting_angle != starting_angle) { 
            starting_angle = Math::clamp(p_starting_angle, double(0.0), double(Math_TAU)); 
            if (starting_angle > ending_angle) {
                ending_angle = starting_angle;
            }
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ double get_starting_angle() const { return starting_angle; }

    _ALWAYS_INLINE_ void set_ending_angle(const double p_ending_angle) { 
        if (p_ending_angle != ending_angle) { 
            ending_angle = Math::clamp(p_ending_angle, double(0.0), double(Math_TAU)); 
            if (ending_angle < starting_angle) {
                starting_angle = ending_angle;
            }
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ double get_ending_angle() const { return ending_angle; }

    _ALWAYS_INLINE_ void set_smooth_normals(const bool p_smooth_normals) { 
        if (p_smooth_normals != smooth_normals) { 
            smooth_normals = p_smooth_normals; 
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ bool get_smooth_normals() const { return smooth_normals; }

    _ALWAYS_INLINE_ void set_closed(const bool p_closed) { 
        if (p_closed != closed) { 
            closed = p_closed; 
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ bool is_closed() const { return closed; }

    _ALWAYS_INLINE_ void set_segments(const uint64_t p_segments) { 
        if (p_segments != segments && p_segments > 1) { 
            segments = p_segments; 
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ uint64_t get_segments() const { return segments; }

protected:
    virtual Array _generate_cross_section() override;
    static void _bind_methods();

private:
    double radius = 1.0;
    double starting_angle = 0.0;
    double ending_angle = Math_TAU;
    bool smooth_normals = true;
    bool closed = true;
    uint64_t segments = 32;
};
}

#endif
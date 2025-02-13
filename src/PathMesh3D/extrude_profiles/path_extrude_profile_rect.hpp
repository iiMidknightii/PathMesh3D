#ifndef PATH_EXTRUDE_PROFILE_RECT_H
#define PATH_EXTRUDE_PROFILE_RECT_H


#include "path_extrude_profile_base.hpp"

namespace godot {

class PathExtrudeProfileRect : public PathExtrudeProfileBase {
    GDCLASS(PathExtrudeProfileRect, PathExtrudeProfileBase)

public:
    _ALWAYS_INLINE_ void set_rect(const Rect2 &p_rect) { if (p_rect != rect) { rect = p_rect;  queue_update(); } }
    _ALWAYS_INLINE_ Rect2 get_rect() const { return rect; }

    _ALWAYS_INLINE_ void set_subdivisions(const Vector2i p_subdivisions) {
        if (p_subdivisions != subdivisions) {
            subdivisions = p_subdivisions;
            queue_update();
        }
    }
    _ALWAYS_INLINE_ Vector2i get_subdivisions() const { return subdivisions; }

    _ALWAYS_INLINE_ void set_smooth_normals(const bool p_smooth_normals) { 
        if (p_smooth_normals != smooth_normals) { 
            smooth_normals = p_smooth_normals; 
            queue_update(); 
        } 
    }
    _ALWAYS_INLINE_ bool get_smooth_normals() const { return smooth_normals; }

protected:
    Array _generate_cross_section() override;
    static void _bind_methods();
    
private:
    Rect2 rect;
    Vector2i subdivisions;
    bool smooth_normals = false;
};

}

#endif
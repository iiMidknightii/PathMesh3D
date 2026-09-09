#pragma once

#include "path_extrude_profile_base.hpp"

namespace godot {

class PathExtrudeProfileRect : public PathExtrudeProfileBase {
    GDCLASS(PathExtrudeProfileRect, PathExtrudeProfileBase)

public:
    enum EndCapMode {
        END_CAP_FLAT,
        END_CAP_ARROW,
        END_CAP_MAX,
    };

    void set_rect(const Rect2 &p_rect);
    Rect2 get_rect() const;

    void set_subdivisions(const Vector2i p_subdivisions);
    Vector2i get_subdivisions() const;

    void set_smooth_normals(const bool p_smooth_normals);
    bool get_smooth_normals() const;

    void set_end_cap_mode(const EndCapMode p_mode);
    EndCapMode get_end_cap_mode() const;

    void set_end_cap_arrow_flare_width(const double p_width);
    double get_end_cap_arrow_flare_width() const;

    void set_end_cap_arrow_flare_height(const double p_height);
    double get_end_cap_arrow_flare_height() const;

    void set_end_cap_arrow_flare_length(const double p_depth);
    double get_end_cap_arrow_flare_length() const;

    void set_end_cap_arrow_tip_length(const double p_depth);
    double get_end_cap_arrow_tip_length() const;

    void set_end_cap_arrow_tip_width(const double p_width);
    double get_end_cap_arrow_tip_width() const;

    void set_end_cap_arrow_tip_height(const double p_height);
    double get_end_cap_arrow_tip_height() const;

protected:
    Array _generate_cross_section() override;
    Array _generate_end_cap() override;
    void _validate_property(PropertyInfo &p_property) const;
    static void _bind_methods();
    
private:
    Rect2 rect;
    Vector2i subdivisions;
    bool smooth_normals = false;
    EndCapMode end_cap_mode = END_CAP_FLAT;
    double end_cap_arrow_flare_width = 0.0;
    double end_cap_arrow_flare_height = 0.0;
    double end_cap_arrow_flare_length = 0.0;
    double end_cap_arrow_tip_length = 1.0;
    double end_cap_arrow_tip_width = 0.0;
    double end_cap_arrow_tip_height = 0.0;
};

}

VARIANT_ENUM_CAST(PathExtrudeProfileRect::EndCapMode)
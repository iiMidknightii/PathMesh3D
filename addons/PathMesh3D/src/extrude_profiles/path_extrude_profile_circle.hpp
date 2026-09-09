#pragma once

#include "path_extrude_profile_base.hpp"

namespace godot {

class PathExtrudeProfileCircle : public PathExtrudeProfileBase {
    GDCLASS(PathExtrudeProfileCircle, PathExtrudeProfileBase)

public:
    enum EndCapMode {
        END_CAP_SPHERICAL,
        END_CAP_ARROW,
        END_CAP_MAX,
    };

    void set_radius(const double p_radius);
    double get_radius() const;

    void set_starting_angle(const double p_starting_angle);
    double get_starting_angle() const;

    void set_ending_angle(const double p_ending_angle);
    double get_ending_angle() const;

    void set_smooth_normals(const bool p_smooth_normals);
    bool get_smooth_normals() const;

    void set_closed(const bool p_closed);
    bool is_closed() const;

    void set_segments(const uint64_t p_segments);
    uint64_t get_segments() const;

    void set_end_cap_mode(const EndCapMode p_mode);
    EndCapMode get_end_cap_mode() const;

    void set_end_cap_length(const double p_length);
    double get_end_cap_length() const;

    void set_end_cap_segments(const uint64_t p_segments);
    uint64_t get_end_cap_segments() const;

    void set_end_cap_arrow_flare_radius(const double p_radius);
    double get_end_cap_arrow_flare_radius() const;

    void set_end_cap_arrow_flare_length(const double p_length);
    double get_end_cap_arrow_flare_length() const;

    void set_end_cap_arrow_tip_length(const double p_length);
    double get_end_cap_arrow_tip_length() const;

    void set_end_cap_arrow_tip_radius(const double p_radius);
    double get_end_cap_arrow_tip_radius() const;

protected:
    virtual Array _generate_cross_section() override;
    virtual Array _generate_end_cap() override;
    void _validate_property(PropertyInfo &p_property) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    static void _bind_methods();

private:
    double radius = 1.0;
    double starting_angle = 0.0;
    double ending_angle = Math_TAU;
    bool smooth_normals = true;
    bool closed = true;
    uint64_t segments = 32;
    EndCapMode end_cap_mode = END_CAP_SPHERICAL;
    double end_cap_length = 0.5;
    uint64_t end_cap_segments = 16;
    double end_cap_arrow_flare_radius = 0.0;
    double end_cap_arrow_flare_length = 0.5;
    double end_cap_arrow_tip_length = 1.0;
    double end_cap_arrow_tip_radius = 0.0;
};

}

VARIANT_ENUM_CAST(PathExtrudeProfileCircle::EndCapMode)
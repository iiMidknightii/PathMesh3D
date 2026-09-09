#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/templates/local_vector.hpp>

#include "path_extrude_profile_base.hpp"

using namespace godot;


Array PathExtrudeProfileBase::get_mesh_arrays() const {
    return mesh_array.duplicate();
}

Array PathExtrudeProfileBase::get_end_cap_arrays() const {
    return end_cap_array.duplicate();
}

PackedVector2Array PathExtrudeProfileBase::get_cross_section() const {
    return mesh_array[Mesh::ARRAY_VERTEX].duplicate();
}

PackedVector3Array PathExtrudeProfileBase::get_end_cap() const {
    return end_cap_array[Mesh::ARRAY_VERTEX].duplicate();
}

void PathExtrudeProfileBase::set_flip_normals(bool p_flip_normals) {
    if (p_flip_normals != flip_normals) {
        flip_normals = p_flip_normals;
        queue_update();
    }
}

bool PathExtrudeProfileBase::get_flip_normals() const {
    return flip_normals;
}

void PathExtrudeProfileBase::queue_update() {
    dirty = true;
    emit_changed();
}

bool PathExtrudeProfileBase::regen_if_dirty() {
    bool r_value = dirty;
    
    if (!dirty) {
        return r_value;
    }

    dirty = false;
    Array new_mesh_array = _generate_cross_section();
    Array new_end_cap_array = _generate_end_cap();

    auto do_flip = [&](Array &p_array, bool p_is_3d) {
        auto reverse_array = [&](int p_index) {
            if (p_array.size() <= p_index || p_array[p_index].get_type() == Variant::NIL) {
                return;
            }

            switch (p_array[p_index].get_type()) {
                case Variant::PACKED_VECTOR2_ARRAY: {
                    PackedVector2Array values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_VECTOR3_ARRAY: {
                    PackedVector3Array values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_FLOAT32_ARRAY: {
                    PackedFloat32Array values = p_array[p_index];
                    if (p_index == Mesh::ARRAY_TANGENT && values.size() % 4 == 0) {
                        for (uint64_t idx = 0; idx < values.size(); idx += 4) {
                            values[idx + 3] = -values[idx + 3];
                        }
                        for (uint64_t idx = 0; idx < values.size() / 2; idx += 4) {
                            const uint64_t other = values.size() - idx - 4;
                            for (uint64_t component = 0; component < 4; ++component) {
                                float value = values[idx + component];
                                values[idx + component] = values[other + component];
                                values[other + component] = value;
                            }
                        }
                    } else {
                        values.reverse();
                    }
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_FLOAT64_ARRAY: {
                    PackedFloat64Array values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_COLOR_ARRAY: {
                    PackedColorArray values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_BYTE_ARRAY: {
                    PackedByteArray values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                case Variant::PACKED_INT32_ARRAY: {
                    PackedInt32Array values = p_array[p_index];
                    values.reverse();
                    p_array[p_index] = values;
                } break;
                default:
                    break;
            }
        };

        if (p_array.size() > Mesh::ARRAY_VERTEX) {
            reverse_array(Mesh::ARRAY_VERTEX);
        }
        if (p_array.size() > Mesh::ARRAY_NORMAL) {
            if (p_is_3d && p_array[Mesh::ARRAY_NORMAL].get_type() == Variant::PACKED_VECTOR3_ARRAY) {
                PackedVector3Array normals = p_array[Mesh::ARRAY_NORMAL];
                for (Vector3 &normal : normals) {
                    normal = -normal;
                }
                p_array[Mesh::ARRAY_NORMAL] = normals;
            } else if (!p_is_3d && p_array[Mesh::ARRAY_NORMAL].get_type() == Variant::PACKED_VECTOR2_ARRAY) {
                PackedVector2Array normals = p_array[Mesh::ARRAY_NORMAL];
                for (Vector2 &normal : normals) {
                    normal = -normal;
                }
                p_array[Mesh::ARRAY_NORMAL] = normals;
            }
            reverse_array(Mesh::ARRAY_NORMAL);
        }
        for (int idx_type = Mesh::ARRAY_TANGENT; idx_type < Mesh::ARRAY_MAX; ++idx_type) {
            reverse_array(idx_type);
        }
    };

    if (flip_normals) {
        do_flip(new_mesh_array, false);
        do_flip(new_end_cap_array, true);
    }

    mesh_array = new_mesh_array;
    end_cap_array = new_end_cap_array;

    return r_value;
}

PackedFloat64Array PathExtrudeProfileBase::_generate_v(const PackedVector2Array &p_vertices) {
    PackedFloat64Array v;
    v.resize(p_vertices.size());

    if (v.size() == 0) {
        return v;
    }

    v[0] = 0.0;
    for (uint64_t i = 1; i < p_vertices.size(); ++i) {
        v[i] = v[i - 1] + p_vertices[i].distance_to(p_vertices[i - 1]);
    }
    for (uint64_t i = 1; i < v.size() - 1; ++i) {
        v[i] /= v[v.size() - 1];
    }
    v[v.size() - 1] = 1.0;

    return v;
}

Array PathExtrudeProfileBase::_generate_cross_section() {
    Array out;
    GDVIRTUAL_CALL(_generate_cross_section, out);
    // must have at least an empty array of vertices
    if (out.size() < Mesh::ARRAY_VERTEX) {
        out.resize(Mesh::ARRAY_VERTEX + 1);
        out[Mesh::ARRAY_VERTEX] = PackedVector2Array();
    }
    return out;
}

Array PathExtrudeProfileBase::_generate_end_cap() {
    Array out;
    GDVIRTUAL_CALL(_generate_end_cap, out);
    // must have at least an empty array of vertices
    if (out.size() < Mesh::ARRAY_VERTEX) {
        out.resize(Mesh::ARRAY_VERTEX + 1);
        out[Mesh::ARRAY_VERTEX] = PackedVector3Array();
    }
    return out;
}

void PathExtrudeProfileBase::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_cross_section"), &PathExtrudeProfileBase::get_cross_section);
    ClassDB::bind_method(D_METHOD("get_end_cap_arrays"), &PathExtrudeProfileBase::get_end_cap_arrays);
    ClassDB::bind_method(D_METHOD("queue_update"), &PathExtrudeProfileBase::queue_update);

    ClassDB::bind_method(D_METHOD("set_flip_normals", "flip_normals"), &PathExtrudeProfileBase::set_flip_normals);
    ClassDB::bind_method(D_METHOD("get_flip_normals"), &PathExtrudeProfileBase::get_flip_normals);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_normals"), "set_flip_normals", "get_flip_normals");

    GDVIRTUAL_BIND(_generate_cross_section)
    GDVIRTUAL_BIND(_generate_end_cap)
}
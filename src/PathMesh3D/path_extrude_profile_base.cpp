#include "path_extrude_profile_base.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

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
        Array new_mesh_array = _generate_cross_section();
        if (new_mesh_array == mesh_array) {
            return;
        }

        Vector<bool> has_column;
        has_column.resize(Mesh::ARRAY_MAX);
        for (int idx_type = 0; idx_type < Mesh::ARRAY_MAX; ++idx_type) {
            has_column.write[idx_type] = new_mesh_array.size() > idx_type && new_mesh_array[idx_type].get_type() != Variant::NIL;
        }
        
        PackedVector2Array vertices = new_mesh_array[Mesh::ARRAY_VERTEX];

        #define MAKE_ARRAY(m_type, m_name, m_index) \
            m_type m_name = has_column[m_index] ? m_type(new_mesh_array[m_index]) : m_type()
        MAKE_ARRAY(PackedVector2Array, normals, Mesh::ARRAY_NORMAL);
        MAKE_ARRAY(PackedFloat32Array, tangents, Mesh::ARRAY_TANGENT);
        MAKE_ARRAY(PackedVector2Array, uv1, Mesh::ARRAY_TEX_UV);
        MAKE_ARRAY(PackedVector2Array, uv2, Mesh::ARRAY_TEX_UV2);
        MAKE_ARRAY(PackedColorArray, colors, Mesh::ARRAY_COLOR);
        MAKE_ARRAY(PackedColorArray, custom0, Mesh::ARRAY_CUSTOM0);
        MAKE_ARRAY(PackedByteArray, custom1, Mesh::ARRAY_CUSTOM1);
        MAKE_ARRAY(PackedByteArray, custom2, Mesh::ARRAY_CUSTOM2);
        MAKE_ARRAY(PackedByteArray, custom3, Mesh::ARRAY_CUSTOM3);
        MAKE_ARRAY(PackedInt32Array, bones, Mesh::ARRAY_BONES);
        MAKE_ARRAY(PackedFloat32Array, weights, Mesh::ARRAY_WEIGHTS);
        #undef MAKE_ARRAY
        
        if (flip_normals) {
            vertices.reverse();
            if (has_column[Mesh::ARRAY_NORMAL]) {
                for (uint64_t idx = 0; idx < normals.size(); ++idx) {
                    normals[idx] = -normals[idx];
                }
            }
            normals.reverse();
            if (has_column[Mesh::ARRAY_TANGENT]) {
                for (uint64_t idx = 0; idx < tangents.size(); idx += 4) {
                    tangents[idx + 3] = -tangents[idx + 3];
                }
            }
            tangents.reverse();
            uv1.reverse();
            uv2.reverse();
            colors.reverse();
            custom0.reverse();
            custom1.reverse();
            custom2.reverse();
            custom3.reverse();
            bones.reverse();
            weights.reverse();
        }

        new_mesh_array.resize(Mesh::ARRAY_MAX);
        new_mesh_array.fill(Variant());
        new_mesh_array[Mesh::ARRAY_VERTEX] = vertices;
        #define MAKE_NEW_ARRAY(m_index, m_name) new_mesh_array[m_index] = m_name.is_empty() ? Variant() : Variant(m_name)
        MAKE_NEW_ARRAY(Mesh::ARRAY_NORMAL, normals);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TANGENT, tangents);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TEX_UV, uv1);
        MAKE_NEW_ARRAY(Mesh::ARRAY_TEX_UV2, uv2);
        MAKE_NEW_ARRAY(Mesh::ARRAY_COLOR, colors);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM0, custom0);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM1, custom1);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM2, custom2);
        MAKE_NEW_ARRAY(Mesh::ARRAY_CUSTOM3, custom3);
        MAKE_NEW_ARRAY(Mesh::ARRAY_BONES, bones);
        MAKE_NEW_ARRAY(Mesh::ARRAY_WEIGHTS, weights);
        #undef MAKE_NEW_ARRAY
        
        mesh_array = new_mesh_array;
        emit_changed();
    }
}
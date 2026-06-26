use std::ops::BitOr;

use godot::prelude::*;
use godot::classes::{IGeometryInstance3D, GeometryInstance3D, Material, ArrayMesh};

mod profile_base;
pub use profile_base::PathExtrudeProfileBase;

use crate::path_tool_3d::*;

#[derive(GodotConvert, Var, Export, Default, Debug, Clone, Copy, PartialEq)]
#[repr(i32)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum EndCaps {
    END_CAPS_NONE,
    END_CAPS_START = (1 << 0),
    END_CAPS_END = (1 << 1),
    #[default]
    END_CAPS_BOTH = (1 << 0 | 1 << 1),
}

#[derive(GodotClass)]
#[class(base=GeometryInstance3D)]
pub struct PathExtrude3D {
    #[export]
    profile: Option<Gd<PathExtrudeProfileBase>>,
    #[export]
    tesselation_max_stages: i32,
    #[export]
    tesselation_tolerance_degrees: real,
    #[export]
    end_cap_mode: EndCaps,
    #[export]
    offset: Vector2,
    #[export]
    offset_angle: real,
    #[export]
    sample_cubic: bool,
    #[export]
    tilt: bool,
    #[export]
    material: Option<Gd<Material>>,
    #[export]
    #[var(get = get_triangle_count, no_set)]
    triangle_count: PhantomVar<u32>,

    helper: PathCollisionToolHelper,
    generated_mesh: Gd<ArrayMesh>,
}

#[godot_api]
impl IGeometryInstance3D for PathExtrude3D {
    pub fn init(base: Base<GeometryInstance3D>) -> Self {
        Self {
            profile: None,
            tesselation_max_stages: 10,
            tesselation_tolerance_degrees: 5.0,
            end_cap_mode: EndCaps::END_CAPS_BOTH,
            offset: Vector2::ZERO,
            offset_angle: 0.0,
            sample_cubic: false,
            tilt: true,
            material: None,
            triangle_count: PhantomVar::default(),
            helper: PathCollisionToolHelper::new(base.to_init_gd().instance_id()),
            generated_mesh: ArrayMesh::new_gd(),
        }
    }
}
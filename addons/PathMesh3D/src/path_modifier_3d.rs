use godot::prelude::*;
use godot::global::{randf, randf_range, remap};
use godot::classes::Curve;
use godot::classes::notify::Node3DNotification;

use crate::path_tool_3d::PathTool3D;

#[derive(GodotConvert, Var, Export, Default, Clone)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum RotationMode {
    #[default]
    ROTATION_EDIT_MODE_EULER = 0,
    ROTATION_EDIT_MODE_QUATERION = 1,
    ROTATION_EDIT_MODE_BASIS = 2,
}

#[derive(GodotClass)]
#[class(base=Node3D)]
pub struct PathModifier3D {
    base: Base<Node3D>,

    #[export]
    curve_offset_ratio_start: real,
    #[export]
    curve_offset_ratio_end: real,

    #[export]
    influence: Option<Gd<Curve>>,

    #[export]
    position_modifier: Vector3,
    #[export]
    position_randomness: Vector3,
    #[export]
    rotation_modifier_mode: RotationMode,
    #[export]
    rotation_modifier: Quaternion,
    #[export]
    rotation_randomness: Quaternion,
    #[export]
    scale_modifier: Vector3,
    #[export]
    scale_randomness: Vector3,
    #[export]
    uv_offset_modifier: Vector2,
    #[export]
    uv_offset_randomness: Vector2,
    #[export]
    uv_scale_modifier: Vector2,
    #[export]
    uv_scale_randomness: Vector2,

    dirty: bool,
    tool_interface_id: Option<InstanceId>,
}

#[godot_api]
impl INode3D for PathModifier3D {
    fn init(base: Base<Node3D>) -> Self {
        Self {
            curve_offset_ratio_start: 0.0,
            curve_offset_ratio_end: 1.0,
            influence: None,
            position_modifier: Vector3::ZERO,
            position_randomness: Vector3::ZERO,
            rotation_modifier_mode: RotationMode::default(),
            rotation_modifier: Quaternion::IDENTITY,
            rotation_randomness: Quaternion::IDENTITY,
            scale_modifier: Vector3::ONE,
            scale_randomness: Vector3::ZERO,
            uv_offset_modifier: Vector2::ZERO,
            uv_offset_randomness: Vector2::ZERO,
            uv_scale_modifier: Vector2::ONE,
            uv_scale_randomness: Vector2::ZERO,
            dirty: true,
            tool_interface_id: None,
            base: base,
        }
    }

    fn on_notification(&mut self, p_what: Node3DNotification) {
        match p_what {
            Node3DNotification::ENTER_TREE => {
                // Walk up the parent chain to find a node implementing PathTool3D
                let mut parent = self.base().get_parent();
                while let Some(node) = parent {
                    if let Ok(mut tool) = node.clone().try_dynify::<dyn PathTool3D>() {
                        self.tool_interface_id = Some(tool.instance_id());
                        tool.dyn_bind_mut().helper_mut().register_modifier(self.to_gd());
                        break;
                    } else {
                        parent = node.get_parent();
                    }
                }
            }

            Node3DNotification::EXIT_TREE => {
                if let Some(mut tool) = self._get_tool() {
                    tool.dyn_bind_mut().helper_mut().unregister_modifier(self.to_gd());
                    self.tool_interface_id = None;
                }
            }

            _ => {}
        }
    }
}

#[godot_api]
impl PathModifier3D {
    #[func]
    pub fn sample_3d_modifier_at(&self, p_offset_ratio: real) -> Transform3D {
        let y = self._sample_influence(p_offset_ratio);
        if y == 0.0 {
            Transform3D::IDENTITY
        } else {
            let position = Vector3::ZERO.lerp(self.position_modifier + self.position_randomness * randf_range(-1.0, 1.0) as real, y);
            let rotation = Quaternion::IDENTITY.slerpni(self.rotation_modifier * Quaternion::IDENTITY.slerpni(self.rotation_randomness, randf() as real), y);
            let scale = Vector3::ONE.lerp(self.scale_modifier + self.scale_randomness * randf() as real, y);

            Transform3D { basis: Basis::from_quaternion(rotation).scaled(scale), origin: position }
        }
    }

    #[func]
    pub fn sample_uv_modifier_at(&self, _p_offset_ratio: real) -> Transform2D {
        Transform2D::default()
    }

    #[func]
    pub fn queue_rebuild(&mut self) { 
        self.dirty = true;
    }

    pub fn pop_is_dirty(&mut self) -> bool {
        let is_dirty = self.dirty;
        self.dirty = false;
        is_dirty
    }

    fn _get_tool(&self) -> Option<DynGd<Node3D, dyn PathTool3D>> {
        self.tool_interface_id.and_then(|id| {
            if let Ok(node) = Gd::<Node3D>::try_from_instance_id(id) {
                node.try_dynify::<dyn PathTool3D>().ok()
            } else {
                None
            }
        })
    }

    fn _sample_influence(&self, p_offset_ratio: real) -> real {
        if let Some(influence) = &self.influence {
            if p_offset_ratio < self.curve_offset_ratio_start || p_offset_ratio > self.curve_offset_ratio_end {
                real!(0.0)
            } else {
                let offset = remap(p_offset_ratio as f64, 0.0f64, 1.0f64, self.curve_offset_ratio_start as f64, self.curve_offset_ratio_end as f64);
                let sample = influence.sample_baked(offset as f32) as real;
                sample
            }
        } else if p_offset_ratio >= self.curve_offset_ratio_start && p_offset_ratio <= self.curve_offset_ratio_end {
            real!(1.0)
        } else {
            real!(0.0)
        }
    }

    fn _on_influence_changed(&mut self) {
        self.queue_rebuild();
    }
}
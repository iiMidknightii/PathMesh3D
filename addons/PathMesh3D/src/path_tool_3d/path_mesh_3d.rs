use godot::global::{maxf, clampf, clampi};
use godot::prelude::*;
use godot::builtin::EulerOrder;
use godot::register::info::{PropertyInfo, PropertyHint, PropertyHintInfo, PropertyUsageFlags};
use godot::classes::{ArrayMesh, GeometryInstance3D, IGeometryInstance3D, Mesh, MeshConvexDecompositionSettings, Path3D, StaticBody3D};
use godot::classes::notify::Node3DNotification;
use godot::classes::mesh::{ArrayType, PrimitiveType};
use godot::signal::ConnectHandle;

use crate::path_tool_3d::*;

#[derive(GodotConvert, Var, Export, Default, Debug, Clone, Copy, PartialEq)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum Distribution {
    #[default]
    DISTRIBUTE_BY_MODEL_LENGTH,
    DISTRIBUTE_BY_COUNT,
    DISTRIBUTE_MAX,
}

#[derive(GodotConvert, Var, Export, Default, Debug, Clone, Copy, PartialEq)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum Alignment {
    #[default]
    ALIGN_STRETCH,
    ALIGN_FROM_START,
    ALIGN_CENTERED,
    ALIGN_FROM_END,
    ALIGN_MAX,
}

#[derive(GodotClass)]
#[class(base=GeometryInstance3D)]
pub struct PathMesh3D {
    helper: PathCollisionToolHelper,
    #[export]
    #[var(set, get)]
    mesh: Option<Gd<Mesh>>,
    #[export]
    #[var(get = get_total_triangle_count, no_set)]
    total_triangle_count: PhantomVar<i32>,
    mesh_changed_connection: Option<ConnectHandle>,
    generated_mesh: Gd<ArrayMesh>,
    surfaces: Vec<SurfaceData>,
    base: Base<GeometryInstance3D>,
}

#[godot_api]
impl IGeometryInstance3D for PathMesh3D {
    fn init(base: Base<GeometryInstance3D>) -> Self {
        Self {
            helper: PathCollisionToolHelper::new(base.to_init_gd().instance_id()),
            mesh: None,
            total_triangle_count: PhantomVar::default(),
            mesh_changed_connection: None,
            generated_mesh: ArrayMesh::new_gd(),
            surfaces: Vec::new(),
            base,
        }
    }

    fn on_notification(&mut self, p_what: Node3DNotification) {
        self.helper.handle_notification(p_what);
    }

    fn on_get_property_list(&mut self) -> Vec<PropertyInfo> {
        let mut props = Vec::new();
        for (i, surf) in self.surfaces.iter().enumerate() {
            let surf_name = format!("surface_{}", i);
            let usage = PropertyUsageFlags::DEFAULT | PropertyUsageFlags::INTERNAL;

            props.push(PropertyInfo {
                variant_type: VariantType::NIL,
                property_name: StringName::from(&surf_name.to_uppercase()),
                hint_info: PropertyHintInfo { hint: PropertyHint::GROUP_ENABLE, hint_string: GString::from(&format!("{}/", surf_name)) },
                usage: PropertyUsageFlags::GROUP,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::VECTOR3,
                property_name: StringName::from(&format!("{}/tile_rotation", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::RANGE, hint_string: GString::from("0.0,360.0,0.01,radians_as_degrees") },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::INT,
                property_name: StringName::from(&format!("{}/tile_rotation_order", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::ENUM, hint_string: GString::from("YZX,YXZ,XZY,ZYX,ZXY,XYZ") },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::INT,
                property_name: StringName::from(&format!("{}/distribution", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::ENUM, hint_string: GString::from("By Model Length,By Count") },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::INT,
                property_name: StringName::from(&format!("{}/alignment", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::ENUM, hint_string: GString::from("Stretch,From Start,Center,From End") },
                usage,
                class_name: StringName::from(""),
            });
            if surf.distribution == Distribution::DISTRIBUTE_BY_COUNT {
                props.push(PropertyInfo {
                    variant_type: VariantType::INT,
                    property_name: StringName::from(&format!("{}/count", surf_name)),
                    hint_info: PropertyHintInfo { hint: PropertyHint::RANGE, hint_string: GString::from(&format!("2,{},1,or_greater", self._get_max_count())) },
                    usage,
                    class_name: StringName::from(""),
                });
            }
            props.push(PropertyInfo {
                variant_type: VariantType::BOOL,
                property_name: StringName::from(&format!("{}/warp_along_curve", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage,
                class_name: StringName::from(""),  
            });
            props.push(PropertyInfo {
                variant_type: VariantType::BOOL,
                property_name: StringName::from(&format!("{}/sample_cubic", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::BOOL,
                property_name: StringName::from(&format!("{}/tilt", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::VECTOR2,
                property_name: StringName::from(&format!("{}/offset", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::FLOAT,
                property_name: StringName::from(&format!("{}/mesh_length_offset", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage,
                class_name: StringName::from(""),
            });
            props.push(PropertyInfo {
                variant_type: VariantType::INT,
                property_name: StringName::from(&format!("{}/triangle_count", surf_name)),
                hint_info: PropertyHintInfo { hint: PropertyHint::NONE, hint_string: GString::new() },
                usage: PropertyUsageFlags::DEFAULT | PropertyUsageFlags::READ_ONLY,
                class_name: StringName::from(""),
            });
        }

        props
    }

    fn on_property_get_revert(&self, p_property: StringName) -> Option<Variant> {
        let dynamic_prop = self._decode_dynamic_propname(p_property)?;
        match dynamic_prop {
            DynamicProperty::Distribution(_)      => Some(Distribution::DISTRIBUTE_BY_MODEL_LENGTH.to_variant()),
            DynamicProperty::TileRotation(_)      => Some(Vector3::ZERO.to_variant()),
            DynamicProperty::TileRotationOrder(_) => Some(EulerOrder::YXZ.to_variant()),
            DynamicProperty::Alignment(_)         => Some(Alignment::ALIGN_STRETCH.to_variant()),
            DynamicProperty::Count(_)             => Some(2.to_variant()),
            DynamicProperty::WarpAlongCurve(_)    => Some(true.to_variant()),
            DynamicProperty::SampleCubic(_)       => Some(false.to_variant()),
            DynamicProperty::Tilt(_)              => Some(true.to_variant()),
            DynamicProperty::Offset(_)            => Some(Vector2::ZERO.to_variant()),
            DynamicProperty::MeshLengthOffset(_)  => Some(0.0.to_variant()),
            DynamicProperty::TriangleCount(_)     => Some(0.to_variant()),
        }
    }

    fn on_validate_property(&self, p_property: &mut PropertyInfo) {
        match p_property.property_name.to_string().as_str() {
            "convex_collision_clean" | "convex_collision_simplify" => {
                p_property.usage = if self.helper.get_generate_collision() == CollisionMode::COLLISION_MODE_CONVEX {
                    PropertyUsageFlags::DEFAULT
                } else {
                    PropertyUsageFlags::NONE
                }
            },
            "collision_layer" | "collision_mask" => {
                p_property.usage = if self.helper.get_generate_collision() == CollisionMode::COLLISION_MODE_NONE {
                    PropertyUsageFlags::NONE
                } else {
                    PropertyUsageFlags::DEFAULT
                }
            }
            _ => {}
        }
    }

    fn on_set(&mut self, p_property: StringName, p_value: Variant) -> bool {
            if let Some(dynamic_prop) = self._decode_dynamic_propname(p_property) {
                let idx = dynamic_prop.index() as i32;
                match dynamic_prop {
                    DynamicProperty::TileRotation(_) => {
                        if let Ok(tile_rotation) = p_value.try_to::<Vector3>() {
                            self.set_tile_rotation(idx, tile_rotation)
                        }
                    },
                    DynamicProperty::TileRotationOrder(_) => {
                        if let Ok(rotation_order) = p_value.try_to::<EulerOrder>() {
                            self.set_tile_rotation_order(idx, rotation_order)
                        }
                    },
                    DynamicProperty::Distribution(_) => {
                        if let Ok(distribution) = p_value.try_to::<Distribution>() {
                            self.set_distribution(idx, distribution)
                        }
                    },
                    DynamicProperty::Alignment(_) => {
                        if let Ok(alignment) = p_value.try_to::<Alignment>() {
                            self.set_alignment(idx, alignment)
                        }
                    },
                    DynamicProperty::Count(_) => {
                        if let Ok(count) = p_value.try_to::<i32>() {
                            self.set_count(idx, count)
                        }
                    },
                    DynamicProperty::WarpAlongCurve(_) => {
                        if let Ok(warp_along_curve) = p_value.try_to::<bool>() {
                            self.set_warp_along_curve(idx, warp_along_curve)
                        }
                    },
                    DynamicProperty::SampleCubic(_) => {
                        if let Ok(cubic) = p_value.try_to::<bool>() {
                            self.set_cubic(idx, cubic)
                        }
                    },
                    DynamicProperty::Tilt(_) => {
                        if let Ok(tilt) = p_value.try_to::<bool>() {
                            self.set_tilt(idx, tilt)
                        }
                    },
                    DynamicProperty::Offset(_) => {
                        if let Ok(offset) = p_value.try_to::<Vector2>() {
                            self.set_offset(idx, offset)
                        }
                    },
                    DynamicProperty::MeshLengthOffset(_) => {
                        if let Ok(mesh_length_offset) = p_value.try_to::<real>() {
                            self.set_mesh_length_offset(idx, mesh_length_offset)
                        }
                    },
                    DynamicProperty::TriangleCount(_) => {}, // read-only
                }
                true
            } else {
                false
            }
    }

    fn on_get(&self, p_property: StringName) -> Option<Variant> {
        let dynamic_prop = self._decode_dynamic_propname(p_property)?;
        let idx = dynamic_prop.index() as i32;
        match dynamic_prop {
            DynamicProperty::TileRotation(_) => Some(self.get_tile_rotation(idx).to_variant()),
            DynamicProperty::TileRotationOrder(_) => Some(self.get_tile_rotation_order(idx).to_variant()),
            DynamicProperty::Distribution(_) => Some(self.get_distribution(idx).to_variant()),
            DynamicProperty::Alignment(_) => Some(self.get_alignment(idx).to_variant()),
            DynamicProperty::Count(_) => Some(self.get_count(idx).to_variant()),
            DynamicProperty::WarpAlongCurve(_) => Some(self.get_warp_along_curve(idx).to_variant()),
            DynamicProperty::SampleCubic(_) => Some(self.get_cubic(idx).to_variant()),
            DynamicProperty::Tilt(_) => Some(self.get_tilt(idx).to_variant()),
            DynamicProperty::Offset(_) => Some(self.get_offset(idx).to_variant()),
            DynamicProperty::MeshLengthOffset(_) => Some(self.get_mesh_length_offset(idx).to_variant()),
            DynamicProperty::TriangleCount(_) => Some(self.get_triangle_count(idx).to_variant()),
        }
    }
}

#[godot_api]
impl PathMesh3D {
    #[signal]
    pub fn curve_changed();

    #[signal]
    pub fn mesh_changed();

    #[func]
    pub fn set_path_3d(&mut self, p_path: Option<Gd<Path3D>>) {
        self.helper.base_mut().set_path_3d(p_path);
    }

    #[func]
    pub fn get_path_3d(&self) -> Option<Gd<Path3D>> {
        self.helper.base().get_path_3d()
    }

    #[func]
    pub fn set_relative_transform(&mut self, p_transform: RelativeTransform) {
        self.helper.base_mut().set_relative_transform(p_transform);
    }

    #[func]
    pub fn get_relative_transform(&self) -> RelativeTransform {
        self.helper.base().get_relative_transform()
    }

    #[func]
    pub fn set_tile_rotation(&mut self, p_surface_idx: i32, p_rotation: Vector3) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.tile_rotation != p_rotation {
            surface.tile_rotation = p_rotation;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_tile_rotation(&self, p_surface_idx: i32) -> Vector3 {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.tile_rotation).unwrap_or_default()
    }

    #[func]
    pub fn set_tile_rotation_order(&mut self, p_surface_idx: i32, p_rotation_order: EulerOrder) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.tile_rotation_order != p_rotation_order {
            surface.tile_rotation_order = p_rotation_order;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_tile_rotation_order(&self, p_surface_idx: i32) -> EulerOrder {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.tile_rotation_order).unwrap_or(EulerOrder::YXZ)
    }


    #[func]
    pub fn set_distribution(&mut self, p_surface_idx: i32, p_distribution: Distribution) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.distribution != p_distribution {
            surface.distribution = p_distribution;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_distribution(&self, p_surface_idx: i32) -> Distribution {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.distribution).unwrap_or(Distribution::DISTRIBUTE_MAX)
    }

    #[func]
    pub fn set_alignment(&mut self, p_surface_idx: i32, p_alignment: Alignment) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.alignment != p_alignment {
            surface.alignment = p_alignment;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_alignment(&self, p_surface_idx: i32) -> Alignment {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.alignment).unwrap_or(Alignment::ALIGN_MAX)
    }

    #[func]
    pub fn set_count(&mut self, p_surface_idx: i32, p_count: i32) {
        let count = p_count.clamp(2, self._get_max_count());
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.count != count {
            surface.count = count;
            if surface.distribution == Distribution::DISTRIBUTE_BY_COUNT {
                self.helper.queue_rebuild();
            }
        }
    }

    #[func]
    pub fn get_count(&self, p_surface_idx: i32) -> i32 {
        if let Some(surface) = self.surfaces.get(p_surface_idx as usize) {
            match surface.distribution {
                Distribution::DISTRIBUTE_BY_COUNT => surface.count,
                _ => 0,
            }
        } else {
            0
        }
    }

    #[func]
    pub fn set_warp_along_curve(&mut self, p_surface_idx: i32, p_warp_along_curve: bool) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.warp_along_curve != p_warp_along_curve {
            surface.warp_along_curve = p_warp_along_curve;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_warp_along_curve(&self, p_surface_idx: i32) -> bool {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.warp_along_curve).unwrap_or_default()
    }

    #[func]
    pub fn set_cubic(&mut self, p_surface_idx: i32, p_cubic: bool) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.cubic != p_cubic {
            surface.cubic = p_cubic;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_cubic(&self, p_surface_idx: i32) -> bool {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.cubic).unwrap_or_default()
    }

    #[func]
    pub fn set_tilt(&mut self, p_surface_idx: i32, p_tilt: bool) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.tilt != p_tilt {
            surface.tilt = p_tilt;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_tilt(&self, p_surface_idx: i32) -> bool {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.tilt).unwrap_or_default()
    }

    #[func]
    pub fn set_offset(&mut self, p_surface_idx: i32, p_offset: Vector2) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.offset != p_offset {
            surface.offset = p_offset;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_offset(&self, p_surface_idx: i32) -> Vector2 {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.offset).unwrap_or_default()
    }

    #[func]
    pub fn set_mesh_length_offset(&mut self, p_surface_idx: i32, p_mesh_length_offset: real) {
        if let Some(surface) = self.surfaces.get_mut(p_surface_idx as usize) && surface.mesh_length_offset != p_mesh_length_offset {
            surface.mesh_length_offset = p_mesh_length_offset;
            self.helper.queue_rebuild();
        }
    }

    #[func]
    pub fn get_mesh_length_offset(&self, p_surface_idx: i32) -> real {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.mesh_length_offset).unwrap_or_default()
    }

    #[func]
    pub fn set_mesh(&mut self, p_mesh: Option<Gd<Mesh>>) {
        if self.mesh != p_mesh {
            if let Some(conn) = self.mesh_changed_connection.take() {
                conn.disconnect();
            }

            self.mesh = p_mesh;

            if let Some(ref mesh) = self.mesh {
                self.mesh_changed_connection = Some(mesh.signals().changed().connect_other(&self.to_gd(), Self::_on_mesh_changed));
            }

            self._on_mesh_changed();

            self.base_mut().notify_property_list_changed();
        }
    }

    #[func]
    pub fn get_mesh(&self) -> Option<Gd<Mesh>> {
        self.mesh.clone()
    }

    #[func]
    pub fn get_baked_mesh(&self) -> Gd<ArrayMesh> {
        self.generated_mesh.duplicate_resource()
    }

    #[func]
    pub fn get_triangle_count(&self, p_surface_idx: i32) -> i32 {
        self.surfaces.get(p_surface_idx as usize).map(|s| s.n_tris).unwrap_or_default()
    }

    #[func]
    pub fn get_total_triangle_count(&self) -> i32 {
        self.surfaces.iter().map(|s| s.n_tris).sum()
    }

    #[func]
    pub fn set_generate_collision(&mut self, p_mode: CollisionMode) {
        self.helper.set_generate_collision(p_mode);
    }

    #[func]
    pub fn get_generate_collision(&self) -> CollisionMode {
        self.helper.get_generate_collision()
    }

    #[func]
    pub fn set_convex_collision_clean(&mut self, p_clean: bool) {
        self.helper.set_convex_collision_clean(p_clean);
    }

    #[func]
    pub fn get_convex_collision_clean(&self) -> bool {
        self.helper.get_convex_collision_clean()
    }

    #[func]
    pub fn set_convex_collision_simplify(&mut self, p_simplify: bool) {
        self.helper.set_convex_collision_simplify(p_simplify);
    }

    #[func]
    pub fn get_convex_collision_simplify(&self) -> bool {
        self.helper.get_convex_collision_simplify()
    }

    #[func]
    pub fn get_collision_node(&self) -> Option<Gd<Node>> {
        self.helper.get_collision_node().and_then(|static_body| Some(static_body.upcast::<Node>()))
    }

    #[func]
    pub fn get_collision_debug_node(&self) -> Option<Gd<Node>> {
        self.helper.get_collision_debug_node().and_then(|debug_mesh| Some(debug_mesh.upcast::<Node>()))
    }

    #[func]
    pub fn queue_rebuild_collision(&mut self) {
        self.helper.queue_rebuild();
    }

    #[func]
    pub fn set_collision_layer(&mut self, p_layer: u32) {
        self.helper.set_collision_layer(p_layer);
    }

    #[func]
    pub fn get_collision_layer(&self) -> u32 {
        self.helper.get_collision_layer()
    }

    #[func]
    pub fn set_collision_mask(&mut self, p_mask: u32) {
        self.helper.set_collision_mask(p_mask);
    }

    #[func]
    pub fn get_collision_mask(&self) -> u32 {
        self.helper.get_collision_mask()
    }

    #[func]
    pub fn create_trimesh_collision_node(&self) -> Option<Gd<StaticBody3D>> {
        self.helper.create_trimesh_collision_node()
    }

    #[func]
    pub fn create_convex_collision_node(&self, p_clean: bool, p_simplify: bool) -> Option<Gd<StaticBody3D>> {
        self.helper.create_convex_collision_node(p_clean, p_simplify)
    }

    #[func]
    pub fn create_multiple_convex_collision_nodes(&self, p_settings: Option<Gd<MeshConvexDecompositionSettings>>) -> Option<Gd<StaticBody3D>> {
        self.helper.create_multiple_convex_collision_node(p_settings)
    }

    fn _get_max_count(&self) -> i32 {
        if let Some(curve) = self.helper.base().get_curve_3d() && self.mesh.is_some() {
            (curve.get_baked_length() / self._get_mesh_length()).floor() as i32
        } else {
            100
        }
    }

    fn _get_mesh_length(&self) -> real {
        if let Some(mesh) = &self.mesh {
            let mut max_length = real!(0.0);

            for (idx_surf, surf) in self.surfaces.iter().enumerate() {
                let mesh_array = mesh.surface_get_arrays(idx_surf as i32);
                let verts = mesh_array.at(ArrayType::VERTEX.ord() as usize).to::<PackedVector3Array>();
                let mut min_z = f64::MAX as real;
                let mut max_z = f64::MIN as real;
                for idx_vert in 0..verts.len() {
                    let mut vert = verts[idx_vert as usize];
                    let rot = Basis::from_euler(surf.tile_rotation_order, surf.tile_rotation);
                    vert = rot * vert;
                    if vert.z < min_z {
                        min_z = vert.z;
                    } else if vert.z > max_z {
                        max_z = vert.z;
                    }
                }
                
                max_length = real::max(max_length, real::abs(max_z - min_z) + surf.mesh_length_offset);
            }

            max_length
        } else {
            1.0
        }
    }

    fn _decode_dynamic_propname(&self, p_property: StringName) -> Option<DynamicProperty> {
        let prop_path = p_property.split("/");
        if prop_path.len() == 2 {
            let surf_idx = prop_path[0].trim_prefix("surface_").to_int();
            let sub_name = prop_path[1].clone();
            match sub_name.to_string().as_str() {
                "tile_rotation"       => Some(DynamicProperty::TileRotation(surf_idx as usize)),
                "tile_rotation_order" => Some(DynamicProperty::TileRotationOrder(surf_idx as usize)),
                "distribution"        => Some(DynamicProperty::Distribution(surf_idx as usize)),
                "alignment"           => Some(DynamicProperty::Alignment(surf_idx as usize)),
                "count"               => Some(DynamicProperty::Count(surf_idx as usize)),
                "warp_along_curve"    => Some(DynamicProperty::WarpAlongCurve(surf_idx as usize)),
                "sample_cubic"        => Some(DynamicProperty::SampleCubic(surf_idx as usize)),
                "tilt"                => Some(DynamicProperty::Tilt(surf_idx as usize)),
                "offset"              => Some(DynamicProperty::Offset(surf_idx as usize)),
                "mesh_length_offset"  => Some(DynamicProperty::MeshLengthOffset(surf_idx as usize)),
                "triangle_count"      => Some(DynamicProperty::TriangleCount(surf_idx as usize)),
                _                     => None
            }
        } else {
            None
        }
    }

    fn _on_mesh_changed(&mut self) {
        if let Some(mesh) = &self.mesh {
            let count = mesh.get_surface_count() as usize;
            if count != self.surfaces.len() {
                self.surfaces.resize(count, SurfaceData::new());
            }
        } else {
            self.surfaces.clear();
        }
        self.helper.queue_rebuild();
        self.signals().mesh_changed().emit();
        self.base_mut().notify_property_list_changed();
    }

    fn _on_curve_changed(&mut self) {
        self.helper.queue_rebuild();
        self.signals().curve_changed().emit();
    }
}

#[godot_dyn]
impl PathTool3D for PathMesh3D {
    fn helper_mut(&mut self) -> &mut PathToolHelper {
        self.helper.base_mut()
    }

    fn connect_curve_changed(&mut self, p_path: Gd<Path3D>) {
        p_path.signals().curve_changed().connect_other(&self.to_gd(), Self::_on_curve_changed);
    }

    fn mesh(&self) -> Gd<ArrayMesh> {
        self.generated_mesh.clone()
    }

    fn rebuild_mesh(&mut self) {
        self.generated_mesh.clear_surfaces();
        self.surfaces.iter_mut().for_each(|s| s.n_tris = 0);

        let Some(mesh) = &self.mesh else { return };
        let Some(curve) = self.helper.base().get_curve_3d() else { return };
        if curve.get_point_count() < 2 {
            return;
        }

        let baked_l = curve.get_baked_length();
        if baked_l <= 0.0 {
            return;
        }

        let mesh_l = self._get_mesh_length();
        if mesh_l <= 0.0 || baked_l < mesh_l {
            return;
        }

        let max_count = self._get_max_count();

        for (idx_surf, surf) in self.surfaces.iter_mut().enumerate() {
            let arrays = mesh.surface_get_arrays(idx_surf as i32);
            let mut old_verts = arrays.at(ArrayType::VERTEX.ord() as usize).to::<PackedVector3Array>();
            let mut old_norms = arrays.get(ArrayType::NORMAL.ord() as usize).map(|norms| norms.to::<PackedVector3Array>());
            let mut old_tang = arrays.get(ArrayType::TANGENT.ord() as usize).map(|tang| tang.to::<PackedFloat32Array>());
            let old_uv1 = arrays.get(ArrayType::TEX_UV.ord() as usize).map(|uv| uv.to::<PackedVector2Array>());
            let old_uv2 = arrays.get(ArrayType::TEX_UV2.ord() as usize).map(|uv| uv.to::<PackedVector2Array>());
            let old_colors = arrays.get(ArrayType::COLOR.ord() as usize).map(|cols| cols.to::<PackedColorArray>());
            let old_custom0 = arrays.get(ArrayType::CUSTOM0.ord() as usize).map(|c0| c0.to::<PackedByteArray>());
            let old_custom1 = arrays.get(ArrayType::CUSTOM1.ord() as usize).map(|c1| c1.to::<PackedByteArray>());
            let old_custom2 = arrays.get(ArrayType::CUSTOM2.ord() as usize).map(|c2| c2.to::<PackedByteArray>());
            let old_custom3 = arrays.get(ArrayType::CUSTOM3.ord() as usize).map(|c3| c3.to::<PackedByteArray>());
            let old_bones /* my nickname in highschool */ = arrays.get(ArrayType::BONES.ord() as usize).map(|bones| bones.to::<PackedInt32Array>());
            let old_weights = arrays.get(ArrayType::WEIGHTS.ord() as usize).map(|weights| weights.to::<PackedFloat32Array>());
            let mut old_indices = arrays.get(ArrayType::INDEX.ord() as usize).map(|idx| idx.to::<PackedInt32Array>());

            let mut pos_z = -real::INFINITY;
            for idx in 0..old_verts.len() {
                let mut vert = old_verts[idx];

                let rot = Basis::from_euler(surf.tile_rotation_order, surf.tile_rotation);
                vert = rot * vert;
                vert.x += surf.offset.x;
                vert.y += surf.offset.y;

                old_verts[idx] = vert;

                if vert.z > pos_z {
                    pos_z = vert.z;
                }

                if let Some(norms) = &mut old_norms {
                    norms[idx] = rot * norms[idx];
                }

                if let Some(tang) = &mut old_tang {
                    let tvec = rot * Vector3::new(tang[idx * 4], tang[idx * 4 + 1], tang[idx * 4 + 2]);
                    tang[idx * 4] = tvec.x;
                    tang[idx * 4 + 1] = tvec.y;
                    tang[idx * 4 + 2] = tvec.z;
                }
            }

            let count = match surf.distribution {
                Distribution::DISTRIBUTE_BY_COUNT => clampi(surf.count.into(), 2, max_count.into()) as i32,
                Distribution::DISTRIBUTE_BY_MODEL_LENGTH => max_count,
                Distribution::DISTRIBUTE_MAX => return,
            };

            let (mut z_stretch, mut z) = match surf.alignment {
                Alignment::ALIGN_STRETCH => {
                    let z_stretch = match surf.distribution {
                        Distribution::DISTRIBUTE_BY_MODEL_LENGTH => baked_l / (mesh_l * count as real),
                        _ => baked_l / mesh_l / (count - 1) as real,
                    };
                    (z_stretch, pos_z * z_stretch)
                },
                Alignment::ALIGN_FROM_START => (1.0, pos_z),
                Alignment::ALIGN_CENTERED => (1.0, (baked_l - mesh_l * count as real) / 2.0 + pos_z),
                Alignment::ALIGN_FROM_END => (1.0, baked_l - mesh_l * count as real + pos_z),
                Alignment::ALIGN_MAX => return,
            };
            z_stretch = maxf(z_stretch as f64, 1.0) as real;
            z = clampf(z as f64, 0.0, baked_l as f64) as real;

            let new_size = count * old_verts.len() as i32;
            let mut new_verts = PackedVector3Array::new(); new_verts.resize(new_size as usize);
            let mut new_norms = PackedVector3Array::new(); new_norms.resize(new_size as usize);
            let mut new_tang = PackedFloat32Array::new(); new_tang.resize((new_size * 4) as usize);
            let mut new_uv1 = PackedVector2Array::new();
            let mut new_uv2 = PackedVector2Array::new();
            let mut new_colors = PackedColorArray::new();
            let mut new_custom0 = PackedByteArray::new();
            let mut new_custom1 = PackedByteArray::new();
            let mut new_custom2 = PackedByteArray::new();
            let mut new_custom3 = PackedByteArray::new();
            let mut new_bones = PackedInt32Array::new();
            let mut new_weights = PackedFloat32Array::new();
            let mut new_indices = PackedInt32Array::new();

            let mut k = 0 as usize;
            for _ in 0..count {
                let mut transform = if !surf.warp_along_curve {
                    curve.sample_baked_with_rotation_ex()
                        .offset(z)
                        .cubic(surf.cubic)
                        .apply_tilt(surf.tilt).done() * 
                            self.helper.base().sample_3d_modifier_at(z / baked_l)
                } else {
                    Transform3D::IDENTITY
                };

                for idx_vert in 0..old_verts.len() {
                    let vertex = if surf.warp_along_curve {
                        let z_offset = z - z_stretch * old_verts[idx_vert].z;
                        transform = curve.sample_baked_with_rotation_ex()
                            .offset(z_offset)
                            .cubic(surf.cubic)
                            .apply_tilt(surf.tilt)
                            .done() * 
                                self.helper.base().sample_3d_modifier_at(z_offset / baked_l);

                        // Avoid double transforming the Z position by zeroing out the original Z value
                        Vector3 { x: old_verts[idx_vert].x, y: old_verts[idx_vert].y, z: 0.0 }
                    } else {
                        // No need to recalculate the transform if not warping
                        old_verts[idx_vert]
                    };

                    self.helper.base().transform_relative_to_nodes(&mut transform);

                    new_verts[k] = transform * vertex;
                    if let Some(norms) = &old_norms {
                        new_norms[k] = transform.basis * norms[idx_vert];
                    }
                    if let Some(tang) = &old_tang {
                        let tvec = transform.basis * Vector3::new(tang[idx_vert * 4], tang[idx_vert * 4 + 1], tang[idx_vert * 4 + 2]);
                        new_tang[idx_vert * 4] = tvec.x;
                        new_tang[idx_vert * 4 + 1] = tvec.y;
                        new_tang[idx_vert * 4 + 2] = tvec.z;
                        new_tang[idx_vert * 4 + 3] = tang[idx_vert * 4 + 3];
                    }

                    k += 1;
                }

                if let Some(ref uv1) = old_uv1 {
                    new_uv1.extend_array(uv1);
                }
                if let Some(ref uv2) = old_uv2 {
                    new_uv2.extend_array(uv2);
                }
                if let Some(ref cols) = old_colors {
                    new_colors.extend_array(cols); 
                }
                if let Some(ref c0) = old_custom0 {
                    new_custom0.extend_array(c0);
                }
                if let Some(ref c1) = old_custom1 {
                    new_custom1.extend_array(c1);
                }
                if let Some(ref c2) = old_custom2 {
                    new_custom2.extend_array(c2);
                }
                if let Some(ref c3) = old_custom3 {
                    new_custom3.extend_array(c3);
                }
                if let Some(ref bones) = old_bones {
                    new_bones.extend_array(bones);
                }
                if let Some(ref weights) = old_weights {
                    new_weights.extend_array(weights);
                }
                if let Some(ref mut indices) = old_indices {
                    new_indices.extend_array(indices);
                    for idx in 0..indices.len() {
                        indices[idx] += old_verts.len() as i32;
                    }
                }

                z += mesh_l * z_stretch;
            }

            let mut new_arrays = varray!();
            new_arrays.resize((ArrayType::MAX.ord() - 1) as usize, &Variant::nil());
            new_arrays.set(ArrayType::VERTEX.ord() as usize, &new_verts.to_variant());
            new_arrays.set(ArrayType::NORMAL.ord() as usize, &new_norms.to_variant());
            new_arrays.set(ArrayType::TANGENT.ord() as usize, &new_tang.to_variant());
            new_arrays.set(ArrayType::TEX_UV.ord() as usize, &new_uv1.to_variant());
            new_arrays.set(ArrayType::TEX_UV2.ord() as usize, &new_uv2.to_variant());
            new_arrays.set(ArrayType::COLOR.ord() as usize, &new_colors.to_variant());
            new_arrays.set(ArrayType::CUSTOM0.ord() as usize, &new_custom0.to_variant());
            new_arrays.set(ArrayType::CUSTOM1.ord() as usize, &new_custom1.to_variant());
            new_arrays.set(ArrayType::CUSTOM2.ord() as usize, &new_custom2.to_variant());
            new_arrays.set(ArrayType::CUSTOM3.ord() as usize, &new_custom3.to_variant());
            new_arrays.set(ArrayType::BONES.ord() as usize, &new_bones.to_variant());
            new_arrays.set(ArrayType::WEIGHTS.ord() as usize, &new_weights.to_variant());
            new_arrays.set(ArrayType::INDEX.ord() as usize, &new_indices.to_variant());

            let primitives = if let Ok(arr_mesh) = mesh.clone().try_cast::<ArrayMesh>() {
                arr_mesh.surface_get_primitive_type(idx_surf as i32)
            } else { 
                PrimitiveType::TRIANGLES
            };

            let blend_shapes = mesh.surface_get_blend_shape_arrays(idx_surf as i32).iter_shared()
                .map(|inner| inner.upcast_any_array())
                .collect();

            self.generated_mesh.add_surface_from_arrays_ex(primitives, &new_arrays).blend_shapes(&blend_shapes).done();
            self.generated_mesh.surface_set_material(idx_surf as i32, mesh.surface_get_material(idx_surf as i32).to_godot());

            surf.n_tris = new_verts.len() as i32 / 3;
        }
    }
}

impl Drop for PathMesh3D {
    fn drop(&mut self) {
        if let Some(conn) = self.mesh_changed_connection.take() {
            conn.disconnect();
        }
    }
}


#[derive(Clone)]
struct SurfaceData {
    pub tile_rotation: Vector3,
    pub tile_rotation_order: EulerOrder,
    pub distribution: Distribution,
    pub alignment: Alignment,
    pub count: i32,
    pub warp_along_curve: bool,
    pub cubic: bool,
    pub tilt: bool,
    pub offset: Vector2,
    pub n_tris: i32,
    pub mesh_length_offset: real,
}

impl SurfaceData {
    pub fn new() -> Self {
        Self {
            tile_rotation: Vector3::ZERO,
            tile_rotation_order: EulerOrder::YXZ,
            distribution: Distribution::DISTRIBUTE_BY_MODEL_LENGTH,
            alignment: Alignment::ALIGN_STRETCH,
            count: 2,
            warp_along_curve: true,
            cubic: false,
            tilt: true,
            offset: Vector2::ZERO,
            n_tris: 0,
            mesh_length_offset: 0.0,
        }
    }
}

enum DynamicProperty {
        TileRotation(usize),
        TileRotationOrder(usize),
        Distribution(usize),
        Alignment(usize),
        Count(usize),
        WarpAlongCurve(usize),
        SampleCubic(usize),
        Tilt(usize),
        Offset(usize),
        MeshLengthOffset(usize),
        TriangleCount(usize),
}

impl DynamicProperty {
    fn index(&self) -> usize {
        match self {
            Self::TileRotation(idx) | 
            Self::TileRotationOrder(idx) |
            Self::Distribution(idx) |
            Self::Alignment(idx) |
            Self::Count(idx) |
            Self::WarpAlongCurve(idx) |
            Self::SampleCubic(idx) |
            Self::Tilt(idx) |
            Self::Offset(idx) |
            Self::MeshLengthOffset(idx) |
            Self::TriangleCount(idx) => *idx,
        }
    }
}
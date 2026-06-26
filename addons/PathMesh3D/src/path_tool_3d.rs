use godot::prelude::*;
use godot::builtin::math::ApproxEq;
use godot::classes::notify::Node3DNotification;
use godot::signal::ConnectHandle;
use godot::classes::{
    Node3D, Path3D, MeshInstance3D, StaticBody3D, CollisionShape3D, 
    MeshConvexDecompositionSettings, Shape3D, Curve3D, ArrayMesh};
use godot::classes::node::InternalMode;

use crate::path_modifier_3d::PathModifier3D;

mod path_mesh_3d;
#[allow(unused_imports)]
pub use path_mesh_3d::*;

mod path_extrude_3d;
#[allow(unused_imports)]
pub use path_extrude_3d::*;

#[derive(GodotConvert, Var, Export, Default, Debug, Clone, PartialEq)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum RelativeTransform {
    #[default]
    TRANSFORM_LOCAL,
    TRANSFORM_PATH_NODE,
    TRANSFORM_MAX,
}

#[derive(GodotConvert, Var, Export, Default, Debug, Clone, PartialEq)]
#[godot(via = i32)]
#[allow(non_camel_case_types)]
pub enum CollisionMode {
    #[default]
    COLLISION_MODE_NONE,
    COLLISION_MODE_TRIMESH,
    COLLISION_MODE_CONVEX,
    COLLISION_MODE_MULTIPLE_CONVEX,
}

pub trait PathTool3D {
    fn helper_mut(&mut self) -> &mut PathToolHelper;
    fn connect_curve_changed(&mut self, p_path: Gd<Path3D>);
    fn mesh(&self) -> Gd<ArrayMesh>;
    fn rebuild_mesh(&mut self);
}

pub struct PathToolHelper {
    tool: InstanceId,
    path_id : Option<InstanceId>,
    curve_changed_connection: Option<ConnectHandle>,
    relative_transform: RelativeTransform,
    self_transform: Transform3D,
    path_transform: Transform3D,
    modifier_ids: Vec<InstanceId>,
    dirty: bool,
}

impl PathToolHelper {
    pub fn new(p_tool: InstanceId) -> Self {
        Self {
            tool: p_tool,
            path_id: None,
            curve_changed_connection: None,
            relative_transform: RelativeTransform::default(),
            self_transform: Transform3D::IDENTITY,
            path_transform: Transform3D::IDENTITY,
            modifier_ids: Vec::new(),
            dirty: true,
        }
    }

    pub fn tool(&self) -> DynGd<Node3D, dyn PathTool3D> {
        Gd::<Node3D>::from_instance_id(self.tool).try_dynify::<dyn PathTool3D>().unwrap()
    }

    pub fn set_path_3d(&mut self, p_path: Option<Gd<Path3D>>) {
        let old_path_node = self.path_id.and_then(|p| Gd::<Path3D>::try_from_instance_id(p).ok());
        if old_path_node != p_path {
            if let Some(conn) = self.curve_changed_connection.take() {
                (conn as ConnectHandle).disconnect();
            }

            if let Some(path) = p_path {
                self.path_id = Some(path.instance_id());
                self.tool().dyn_bind_mut().connect_curve_changed(path);
            } else {
                self.path_id = None;
            }
        }
    }

    pub fn get_path_3d(&self) -> Option<Gd<Path3D>> {
        self.path_id.and_then(|p| Gd::try_from_instance_id(p).ok())
    }

    pub fn get_curve_3d(&self) -> Option<Gd<Curve3D>> {
        self.get_path_3d().and_then(|p| p.get_curve())
    }

    pub fn set_relative_transform(&mut self, p_transform: RelativeTransform) {
        self.relative_transform = p_transform;
        self.queue_rebuild();
    }

    pub fn get_relative_transform(&self) -> RelativeTransform {
        self.relative_transform.clone()
    }

    pub fn register_modifier(&mut self, p_modifier: Gd<PathModifier3D>) {
        if !self.modifier_ids.contains(&p_modifier.instance_id()) {
            self.modifier_ids.push(p_modifier.instance_id());
            self.queue_rebuild();
        }
    }

    pub fn unregister_modifier(&mut self, p_modifier: Gd<PathModifier3D>) {
        self.modifier_ids.retain(|&id| id != p_modifier.instance_id());
        self.queue_rebuild();
    }

    pub fn queue_rebuild(&mut self) {
        self.dirty = true;
    }

    pub fn handle_notification(&mut self, p_what: Node3DNotification) {
        match p_what {
            Node3DNotification::READY => {
                self.tool().set_process_internal(true);
                self.tool().dyn_bind_mut().rebuild_mesh();
            }
            Node3DNotification::INTERNAL_PROCESS => {
                if self._pop_is_dirty() {
                    self.tool().dyn_bind_mut().rebuild_mesh();
                }
            }
            _ => {}
        }
    }

    pub fn transform_relative_to_nodes(&self, p_trans: &mut Transform3D) {
        if self.get_relative_transform() == RelativeTransform::TRANSFORM_PATH_NODE {
            *p_trans = self.self_transform.affine_inverse() * self.path_transform * *p_trans;
        }
    }

    pub fn sample_3d_modifier_at(&self, p_offset_ratio: real) -> Transform3D {
        let mut out = Transform3D::IDENTITY;
        for id in &self.modifier_ids {
            if let Ok(modifier) = Gd::<PathModifier3D>::try_from_instance_id(*id) {
                out = out * modifier.bind().sample_3d_modifier_at(p_offset_ratio);
            }
        }
        out
    }

    pub fn sample_uv_modifier_at(&self, p_offset_ratio: real) -> Transform2D {
        let mut out = Transform2D::default();
        for id in &self.modifier_ids {
            if let Ok(modifier) = Gd::<PathModifier3D>::try_from_instance_id(*id) {
                out = out * modifier.bind().sample_uv_modifier_at(p_offset_ratio);
            }
        }
        out
    }

    fn _pop_is_dirty(&mut self) -> bool {
        let mut is_dirty = self.dirty;
        if !is_dirty && self.relative_transform == RelativeTransform::TRANSFORM_PATH_NODE {
            let tmp = self.tool().get_global_transform();
            is_dirty = is_dirty || !tmp.approx_eq(&self.self_transform);
            self.self_transform = tmp;
            if let Some(path_node) = self.get_path_3d() {
                let tmp = path_node.get_global_transform();
                is_dirty = is_dirty || !tmp.approx_eq(&self.path_transform);
                self.path_transform = tmp;
            }
        }

        for id in &self.modifier_ids {
            if let Ok(mut modifier) = Gd::<PathModifier3D>::try_from_instance_id(*id) {
                is_dirty = is_dirty || modifier.bind_mut().pop_is_dirty();
            }
        }
        
        self.dirty = false;
        is_dirty
    }
}

impl Drop for PathToolHelper {
    fn drop(&mut self) {
        if let Some(conn) = self.curve_changed_connection.take() {
            (conn as ConnectHandle).disconnect();
        }
    }
}

pub struct PathCollisionToolHelper {
    base: PathToolHelper,
    mode: CollisionMode,
    convex_clean: bool,
    convex_simplify: bool,
    node_id: Option<InstanceId>,
    debug_id: Option<InstanceId>,
}

impl PathCollisionToolHelper {
    pub fn new(p_tool: InstanceId) -> Self {
        Self {
            base: PathToolHelper::new(p_tool),
            mode: CollisionMode::COLLISION_MODE_NONE,
            convex_clean: true,
            convex_simplify: false,
            node_id: None,
            debug_id: None,
        }
    }

    pub fn base(&self) -> &PathToolHelper {
        &self.base
    }

    pub fn base_mut(&mut self) -> &mut PathToolHelper {
        &mut self.base
    }

    pub fn mesh(&self) -> Gd<ArrayMesh> {
        self.base().tool().dyn_bind().mesh()
    }

    pub fn set_generate_collision(&mut self, p_mode: CollisionMode) {
        if self.mode != p_mode {
            self.mode = p_mode;
            self.queue_rebuild();
            self.base().tool().notify_property_list_changed();
        }
    }

    pub fn get_generate_collision(&self) -> CollisionMode {
        self.mode.clone()
    }

    pub fn set_convex_collision_clean(&mut self, p_clean: bool) {
        if self.convex_clean != p_clean {
            self.convex_clean = p_clean;
            self.queue_rebuild();
        }
    }

    pub fn get_convex_collision_clean(&self) -> bool {
        self.convex_clean
    }

    pub fn set_convex_collision_simplify(&mut self, p_simplify: bool) {
        if self.convex_simplify != p_simplify {
            self.convex_simplify = p_simplify;
            self.queue_rebuild();
        }
    }

    pub fn get_convex_collision_simplify(&self) -> bool {
        self.convex_simplify
    }

    pub fn get_collision_node(&self) -> Option<Gd<StaticBody3D>> {
        Gd::<StaticBody3D>::try_from_instance_id(self.node_id?).ok()
    }

    pub fn get_collision_debug_node(&self) -> Option<Gd<MeshInstance3D>> {
        Gd::<MeshInstance3D>::try_from_instance_id(self.debug_id?).ok()
    }

    pub fn set_collision_layer(&mut self, p_layer: u32) {
        if let Some(mut node) = self.get_collision_node() && node.get_collision_layer() != p_layer {
            node.set_collision_layer(p_layer);
        }
    }

    pub fn get_collision_layer(&self) -> u32 {
        self.get_collision_node().map(|node| node.get_collision_layer()).unwrap_or(0)
    }

    pub fn set_collision_mask(&mut self, p_mask: u32) {
        if let Some(mut node) = self.get_collision_node() && node.get_collision_mask() != p_mask {
            node.set_collision_mask(p_mask);
        }
    }

    pub fn get_collision_mask(&self) -> u32 {
        self.get_collision_node().map(|node| node.get_collision_mask()).unwrap_or(0)
    }

    pub fn queue_rebuild(&mut self) {
        self.base_mut().queue_rebuild();
    }

    pub fn create_trimesh_collision_node(&self) -> Option<Gd<StaticBody3D>> {
        self._setup_collision_node(
            self.mesh().create_trimesh_shape()
                .and_then(|shape| Some(shape.upcast())))
    }

    pub fn create_convex_collision_node(&self, p_clean: bool, p_simplify: bool) -> Option<Gd<StaticBody3D>> {
        self._setup_collision_node(
            self.mesh().create_convex_shape_ex().clean(p_clean).simplify(p_simplify).done()
                .and_then(|shape| Some(shape.upcast())))
    }

    pub fn create_multiple_convex_collision_node(&self, p_settings: Option<Gd<MeshConvexDecompositionSettings>>) -> Option<Gd<StaticBody3D>> {
        let _settings = p_settings.unwrap_or(MeshConvexDecompositionSettings::new_gd());

        // TODO: GDExtension doesn't have API parity here...
        if let Some(shapes) = None::<Vec<Gd<Shape3D>>> { // self.tool.dyn_bind().get_mesh().and_then(|mesh| mesh.convex_decompose(settings)) {
            let mut static_body = StaticBody3D::new_alloc();
            for shape in shapes {
                let mut collision_shape = CollisionShape3D::new_alloc();
                collision_shape.set_shape(&shape);
                static_body.add_child_ex(&collision_shape).force_readable_name(false).internal(InternalMode::BACK).done();
            }
            Some(static_body)
        } else {
            None
        }
    }

    pub fn handle_notification(&mut self, p_what: Node3DNotification) {
        self.base_mut().handle_notification(p_what);

        match p_what {
            Node3DNotification::READY => {
                self.base().tool().set_process_internal(true);
                self._rebuild();
            }
            Node3DNotification::INTERNAL_PROCESS => {
                if self._pop_is_dirty() {
                    self._rebuild();
                }
            }
            _ => {}
        }
    }

    fn _pop_is_dirty(&mut self) -> bool {
        self.base_mut()._pop_is_dirty()
    }

    fn _clear_collision_node(&mut self) {
        if let Some(mut node) = self.get_collision_node() {
            node.queue_free();
            self.node_id = None;
        }

        if let Some(mut node) = self.get_collision_debug_node() {
            node.queue_free();
            self.debug_id = None;
        }
    }

    fn _setup_collision_node(&self, p_shape: Option<Gd<Shape3D>>) -> Option<Gd<StaticBody3D>> {
        let mut static_body = StaticBody3D::new_alloc();
        let mut cshape = CollisionShape3D::new_alloc();
        cshape.set_shape(&p_shape?);
        static_body.add_child_ex(&cshape).force_readable_name(true).done();
        Some(static_body)
    }

    fn _rebuild(&mut self) {
        self.base().tool().dyn_bind_mut().rebuild_mesh();
        self._clear_collision_node();

        let collision_node: Option<Gd<StaticBody3D>> = match self.mode {
            CollisionMode::COLLISION_MODE_TRIMESH => self.create_trimesh_collision_node(),
            CollisionMode::COLLISION_MODE_CONVEX => self.create_convex_collision_node(self.convex_clean, self.convex_simplify),
            CollisionMode::COLLISION_MODE_MULTIPLE_CONVEX => self.create_multiple_convex_collision_node(None),
            CollisionMode::COLLISION_MODE_NONE => None,
        };

        if let Some(collision_node) = collision_node {

            self.node_id = Some(collision_node.instance_id());

            self.base().tool().add_child_ex(&collision_node).force_readable_name(false).internal(InternalMode::BACK).done();

            if let Some(tree) = self.base().tool().get_tree_or_null() && tree.is_debugging_collisions_hint() {
                if let Some(mesh) = collision_node.get_child(0)
                            .and_then(|child: Gd<Node>| child.try_cast::<CollisionShape3D>().ok())
                            .and_then(|cshape: Gd<CollisionShape3D>| cshape.get_shape())
                            .and_then(|shape: Gd<Shape3D>| shape.get_debug_mesh()) {
                    let mut collision_debug_node = MeshInstance3D::new_alloc();
                    collision_debug_node.set_mesh(&mesh);
                    self.base().tool().add_child_ex(&collision_debug_node).force_readable_name(false).internal(InternalMode::BACK).done();
                    self.debug_id = Some(collision_debug_node.instance_id());
                }
            }
        }
    }

}
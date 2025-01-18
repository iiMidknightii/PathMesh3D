@tool
extends EditorPlugin

const PathMesh3DOptions := preload("res://addons/PathMesh3D/scripts/path_mesh_3d_options.gd")
const PathMesh3DOptionsScene := preload("res://addons/PathMesh3D/scenes/path_mesh_3d_options.tscn")

var _mesh_editor_button: PathMesh3DOptions = PathMesh3DOptionsScene.instantiate()


func _enter_tree() -> void:
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, _mesh_editor_button)
	_mesh_editor_button.hide()

func _exit_tree() -> void:
	remove_control_from_container(CONTAINER_SPATIAL_EDITOR_MENU, _mesh_editor_button)


func _handles(object: Object) -> bool:
	return object is PathMesh3D or object is PathExtrude3D


func _edit(object: Object) -> void:
	if object is PathMesh3D or object is PathExtrude3D and _mesh_editor_button:
		_mesh_editor_button.path_mesh = object
		_mesh_editor_button.ur = get_undo_redo()


func _make_visible(visible: bool) -> void:
	_mesh_editor_button.visible = visible

@tool extends PathModifier3D


func _sample_offset_modifier_at(offset_ratio: float, mesh_length: float) -> float:
	return 1.0


func _sample_3d_modifier_at(offset_ratio: float) -> Transform3D:
	return Transform3D(Basis(), Vector3.UP * 1.0)

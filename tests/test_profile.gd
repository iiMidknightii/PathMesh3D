@tool class_name PathExtrudeProfileTest extends PathExtrudeProfileBase



func _generate_cross_section() -> Array:
	return [PackedVector2Array([Vector2.LEFT, Vector2.UP, Vector2.RIGHT, Vector2.DOWN])]

"""

mesh generation fully manually (ArrayMesh)


"""
@tool
extends MeshInstance3D


func _ready() -> void:
    pass # Replace with function body.


func macro_quad_example() -> void:
    
    
    var surface_array = []
    surface_array.resize(Mesh.ARRAY_MAX) # length of 13
    
    var verts = PackedVector3Array([Vector3(-1,0,-1), Vector3(1,0,-1), Vector3(1,0,1), Vector3(-1,0,1)])
    var uvs := PackedVector2Array([Vector2(0,0), Vector2(1,0), Vector2(1,1), Vector2(0,1)])
    var normals := PackedVector3Array([Vector3(0,1,0), Vector3(0,1,0), Vector3(0,1,0), Vector3(0,1,0)])
    var indices := PackedInt32Array([0,1,2,0,2,3])
    
    surface_array[Mesh.ARRAY_VERTEX] = verts
    surface_array[Mesh.ARRAY_TEX_UV] = uvs
    surface_array[Mesh.ARRAY_NORMAL] = normals
    surface_array[Mesh.ARRAY_INDEX] = indices
    
    mesh = ArrayMesh.new()
    mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_array)
    
    
    


func _process(delta: float) -> void:
    pass

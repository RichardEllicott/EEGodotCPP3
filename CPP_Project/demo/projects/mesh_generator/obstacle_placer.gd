"""

placing obstacals in a 3d world, like for an RTS


"""
@tool
extends Node3D



func clear_children():
    
    for child in get_children():
        remove_child(child)
        
func macro_clear_children():
    clear_children()

func macro_test_box():
    
    #var _position := Vector3(0,2.5,0)
    
    
    var _position := Vector3(0,4,0) # height is 8 at white, 0 at black
    
    _position += Vector3(0,0.5,0)
    
    
    var _size := Vector3(1,1,1)
    
    print(check_box(_position, _size))
    
    place_box(_position, _size)
    pass
    
    
func place_box(_position: Vector3, _size: Vector3 = Vector3(1,1,1)):
    
    var box := MeshInstance3D.new()
    var mesh := BoxMesh.new()
    box.mesh = mesh
    mesh.size = _size
    
    var static_body3d := StaticBody3D.new()
    var collision_shape3d := CollisionShape3D.new()
    var shape := BoxShape3D.new()
    shape.size = _size
    
    add_child(box)
    box.owner = get_tree().edited_scene_root
    box.position = _position
    
    box.add_child(static_body3d)
    static_body3d.add_child(collision_shape3d)
    collision_shape3d.shape = shape
    
    
    static_body3d.owner = get_tree().edited_scene_root
    collision_shape3d.owner = get_tree().edited_scene_root
    


func query_box(_position: Vector3, _size: Vector3 = Vector3(1,1,1)) -> Array[Dictionary]:
    
    var shape := BoxShape3D.new()
    
    shape.size = _size
    
    var _transform := Transform3D()
    _transform.origin = _position
    
    var query: PhysicsShapeQueryParameters3D = PhysicsShapeQueryParameters3D.new()
    query.transform = _transform
    query.collide_with_areas = false
    query.collide_with_bodies = true
    query.shape = shape
    
    var direct_space_state := get_world_3d().direct_space_state
    var results: Array[Dictionary] = direct_space_state.intersect_shape(query, 100)
    return results

func check_box(position: Vector3, size: Vector3 = Vector3(1,1,1)) -> bool:
    #StaticLib6.intersect_shape()
    
    var query := query_box(position, size)
    
    if query == null:
        return true
    if query.is_empty():
        return true
    else:
         return false
    
    


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass

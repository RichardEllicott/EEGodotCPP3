"""

edit a midi sequence


"""
@tool
extends Panel
class_name PatternEditor

@export var position_min: Vector2 = Vector2.ZERO
@export var position_max: Vector2 = Vector2(1000000000000,1000000000000)

@export_group("grid")
@export var snap_to_grid: bool = true
@export var grid_step: Vector2 = Vector2(32,32)
@export_group("")

@export var pattern_object_scene: PackedScene



func macro_setup_objects():
    
    for child in get_children():
        remove_child(child)
    
    
class Note:
    var length: float = 0.0
    
    #func _init(_length: float = 0.0):
        #length = _length


# key is a vector 2 of position
func _get_notes() -> Dictionary:
    
    var notes = {}
    
    for child in get_children():
        if child is PatternEditorObject:
            var child2: PatternEditorObject = child
            
            if snap_to_grid:
                child2.position = child2.position.snapped(grid_step)
            
            var note_position := child2.position / grid_step
            var note_length := child2.size.x / grid_step.x
            
            var note := Note.new()
            note.length = note_length
            
            notes[note_position] = Note
            
            
    return notes

func _update():
    
    for child in get_children():
        
        if child is PatternEditorObject:
            
            if snap_to_grid:
                child.position = child.position.snapped(grid_step)
            
            var note_length: float = child.size.x / grid_step.x
            
            print("%s: %s %s" % [child.name, child.grid_position, note_length])


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    _update()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass

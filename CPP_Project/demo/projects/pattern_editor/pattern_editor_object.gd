"""
"""
@tool
extends Panel
class_name PatternEditorObject


## parent must be present on ready
var parent: PatternEditor

func _ready() -> void:
    parent = get_parent()
    
    if parent.snap_to_grid:
        #grid_position = grid_position
        position = position.snapped(parent.grid_step) # snap to grid
    
    
    mouse_entered.connect(_on_mouse_entered)
    mouse_entered.connect(_on_mouse_exited)




func _on_mouse_entered() -> void:
    pass # Replace with function body.


func _on_mouse_exited() -> void:
    pass # Replace with function body.
    
    
enum State{
    none,
    drag,
    resize,
}

var state: State = State.none


enum Edge{
    top,
    right,
    bottom,
    left
}

#var is_dragging: bool = false
#var is_resizing: bool = false


var drag_is_valid: bool = true



var drag_offset: Vector2 = Vector2.ZERO





var hover_state: State = State.none

var hover_edge: Edge = 0
        
    
var orginal_position: Vector2
var orginal_size: Vector2

var drag_start_mouse_position: Vector2


@export_group("resize")
@export var resize_edge_threshold: float = 4.0  # Threshold for resizing edges

@export var resizeable_top := true
@export var resizeable_right := true
@export var resizeable_bottom := true
@export var resizeable_left := true
@export_group("")




func snap_position_to_grid(position: Vector2):
    var grid_position := position_to_grid(position)
    return grid_to_position(grid_position)
    

### grid functions
func position_to_grid(position: Vector2) -> Vector2i:
    return Vector2(position) / parent.grid_step

func grid_to_position(position: Vector2i) -> Vector2:
    return parent.grid_step * Vector2(position)    

var grid_position: Vector2i:
    get:
        return position_to_grid(position)
    set(value):
        position = grid_to_position(value)

        

func _snap():
    position = snap_position_to_grid(position)
    size = snap_position_to_grid(size)
        
#func get_grid_step():
    #return parent.grid_step
        
        
    
func _input(event: InputEvent) -> void:
    
    
    Vector2.RIGHT
    
    #if not is_instance_valid(parent): return ## if no parent, skip
        
    if event is InputEventMouseButton:
        if event.button_index == MOUSE_BUTTON_LEFT:
            if event.pressed:
                
                match hover_state:
                    State.drag:
                        state = State.drag
                        drag_offset = position - get_global_mouse_position()
                        drag_start_mouse_position = get_global_mouse_position()
                        
                        
                        orginal_position = position
                        orginal_size = size
                        
                    State.resize:
                        state = State.resize
                        drag_offset = position - get_global_mouse_position()
                        drag_start_mouse_position = get_global_mouse_position()
                        
                        
                        orginal_position = position
                        orginal_size = size
                
                
            else:
                if not drag_is_valid:
                    position = orginal_position
                    size = orginal_size
                    
                
                
                state = State.none
                
    elif event is InputEventMouseMotion:
        
        match state:
            State.drag:
                            
                position = get_global_mouse_position() + drag_offset            
                #if is_instance_valid(parent):
                position.x = clamp(position.x, parent.position_min.x, parent.position_max.x)
                position.y = clamp(position.y, parent.position_min.y, parent.position_max.y)
            
                if parent.snap_to_grid:
                    #position = position.snapped(parent.grid_step) # snap to grid
                    _snap()
                    #grid_position = grid_position
                    
                    
              
            State.resize:
                
                #var new_position = get_global_mouse_position() + drag_offset
                #var new_position = get_global_mouse_position() - global_position
                
                var new_position = get_local_mouse_position()
                
                var drag := get_global_mouse_position() - drag_start_mouse_position
                
                if parent.snap_to_grid:
                    drag = drag.snapped(parent.grid_step)
                    new_position = new_position.snapped(parent.grid_step)
                

                match hover_edge:
                    Edge.top: # top
                        # we need the orginal position to save this drag
                        
                        var new_pos := orginal_position + drag
                        position.y = new_pos.y
                        size.y = orginal_size.y - drag.y
                        
                    Edge.right: # right
                        
                        
                        
                        size.x = new_position.x
                        pass
                    Edge.bottom: # bottom
                        size.y = new_position.y
                    Edge.left: # left
                        
                        var new_pos := orginal_position + drag
                        position.x = new_pos.x
                        size.x = orginal_size.x - drag.x
                        
                
            State.none: # no state yet, set the mouse cursor and hover states
                
                var local_mouse_position = get_local_mouse_position()
                var is_hovering :=  Rect2(Vector2.ZERO, size).has_point(local_mouse_position)
                
                if not is_hovering: # not inside control rect
                    hover_state = State.none
                
                elif local_mouse_position.y < resize_edge_threshold: # top
                    set_default_cursor_shape(CURSOR_VSIZE)
                    hover_state = State.resize
                    hover_edge = Edge.top # top
                    
                elif local_mouse_position.y > size.y - resize_edge_threshold: #  bottom
                    set_default_cursor_shape(CURSOR_VSIZE)
                    hover_state = State.resize
                    hover_edge = Edge.bottom
                    
                elif local_mouse_position.x < resize_edge_threshold: # left
                    set_default_cursor_shape(CURSOR_HSIZE)
                    hover_state = State.resize
                    hover_edge = Edge.left
                    
                elif local_mouse_position.x > size.x - resize_edge_threshold: # right
                    set_default_cursor_shape(CURSOR_HSIZE)
                    hover_state = State.resize
                    hover_edge = Edge.right
                    
                else: # if no edge resize, then drag
                    set_default_cursor_shape(CURSOR_POINTING_HAND)
                    hover_state = State.drag
                
                
                

    

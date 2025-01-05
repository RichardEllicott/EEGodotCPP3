"""

abstract drag drop container

will make the childs drag dropable, this is lighter than including scripts in all childs
(and only slightly more complex)

"""
extends Control
class_name DragDropContainer


@export var draggable: bool = true
@export var resize_top: bool = true
@export var resize_right: bool = true
@export var resize_bottom: bool = true
@export var resize_left: bool = true


@export var snap_to_grid: bool = true
@export var grid_step: Vector2 = Vector2(16,16)

@export var required_meta: String = "" # if not empty, need a meta tag to enable child drag


enum Action{
    NONE,
    DRAG,
    RESIZE,
}

var action: Action = Action.NONE # the current action
var hovered_action: Action = Action.NONE # the action we would start if we clicked the mouse


enum Edge{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
}

var hovered_edge: Edge # the hovered edge, if we are hovering the edge


@export var resize_edge_threshold: float = 3.0



var hovered # hovered control (or null)
var action_start_mouse_position: Vector2 # where we start our action, like drag or resize

var action_target # link to the control we are dragging (or null)

var action_target_start_position: Vector2 # orginal position of action target
var action_target_start_size: Vector2
var drag_is_valid: bool = true



func _ready() -> void:
    _update()
        
func _notification(what: int) -> void:
    if what == NOTIFICATION_CHILD_ORDER_CHANGED:
        print("Child order changed or a child was added.")        
        _update()
    

func _update():
    
    for child in get_children():
        if child is Panel:
            var panel: Panel = child
            
            if not panel.mouse_entered.is_connected(_on_child_mouse_entered):
                panel.mouse_entered.connect(_on_child_mouse_entered.bind(panel))
            
            if not panel.mouse_exited.is_connected(_on_child_mouse_exited):
                panel.mouse_exited.connect(_on_child_mouse_exited.bind(panel))
            
            
            
                

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass
    


func _on_child_mouse_entered(node) -> void:
    print("_on_child_mouse_entered ", node)
    hovered = node


func _on_child_mouse_exited(node) -> void:
    print("_on_child_mouse_exited ", node)
    if hovered == node:
        hovered = null




func _input(event: InputEvent) -> void:



    if event is InputEventMouseButton:
        if event.button_index == MOUSE_BUTTON_LEFT:
            
           
            if event.pressed: # button pressed over a child
                
                match hovered_action:
                    
                    
                    Action.DRAG: # start drag
                        
                        action = Action.DRAG
                        #drag_offset = position - get_global_mouse_position() # corrects to center
                        action_start_mouse_position = get_global_mouse_position()
                        action_target_start_position = hovered.position
                        action_target_start_size = hovered.size
                        action_target = hovered
                        
                        
                    Action.RESIZE: # start resize
                        
                        action = Action.RESIZE
                        action_start_mouse_position = get_global_mouse_position()
                        action_target_start_position = hovered.position
                        action_target_start_size = hovered.size
                        action_target = hovered
                    
                    
                    
            else: # cancel actions (button not pressed)
                if not drag_is_valid:
                    position = action_target_start_position
                    size = action_target_start_size
                
                action = Action.NONE
                
                
    elif event is InputEventMouseMotion:
        
        match action:
            Action.DRAG:
                #dragging_node.global_position = get_global_mouse_position() + drag_offset     
                
                var mouse_travel = get_global_mouse_position() - action_start_mouse_position
                action_target.position = action_target_start_position + mouse_travel
                       
                #if is_instance_valid(parent):
                #position.x = clamp(position.x, parent.position_min.x, parent.position_max.x)
                #position.y = clamp(position.y, parent.position_min.y, parent.position_max.y)
                
                
                if snap_to_grid:
                    action_target.position = action_target.position.snapped(grid_step) # snap to grid
                #if parent.snap_to_grid:
                    #position = position.snapped(parent.grid_step) # snap to grid
                    #_snap()
                    #grid_position = grid_position
            Action.RESIZE:
                match hovered_edge:
                    
                    
                    pass
            
            Action.NONE:
                
                if not hovered:
                    hovered_action = Action.NONE
                    #set_default_cursor_shape(CURSOR_ARROW)
                    
                else:
                    if hovered is Panel:
                        var panel: Panel = hovered
                        
                        var local_position: Vector2 = event.global_position - hovered.global_position
                        
                        var checks = [
                            local_position.y < resize_edge_threshold, # top
                            local_position.x > panel.size.x - resize_edge_threshold, # right
                            local_position.y > panel.size.y - resize_edge_threshold, # bottom
                            local_position.x < resize_edge_threshold
                        ]
      
                                
                        if checks[Edge.TOP]:
                            set_default_cursor_shape(CURSOR_VSIZE)
                            hovered_action = Action.RESIZE
                            hovered_edge = Edge.TOP

                        elif checks[Edge.RIGHT]:
                            set_default_cursor_shape(CURSOR_HSIZE)
                            hovered_action = Action.RESIZE
                            hovered_edge = Edge.RIGHT
                            
                        elif checks[Edge.BOTTOM]:
                            set_default_cursor_shape(CURSOR_VSIZE)
                            hovered_action = Action.RESIZE
                            hovered_edge = Edge.BOTTOM
                            
                        elif checks[Edge.LEFT]:
                            set_default_cursor_shape(CURSOR_HSIZE)
                            hovered_action = Action.RESIZE
                            hovered_edge = Edge.LEFT
                            
       
                            
                        else: # if no edge resize, then drag
                            set_default_cursor_shape(CURSOR_POINTING_HAND)
                            hovered_action = Action.DRAG
                            

        






             
                
                

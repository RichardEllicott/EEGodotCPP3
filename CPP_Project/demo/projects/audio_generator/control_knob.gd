"""
Knob control script
"""
@tool
extends Control

# Properties for the knob
var rotation_angle: float = 0.0


var max_rotation: float = 180.0 - + 45.0/2.0 # Maximum rotation in degrees
var min_rotation: float = -180.0 + 45.0/2.0    # Minimum rotation in degrees

@export var range_min: float = 0.0
@export var range_max: float = 1.0


@export var step_mode: bool = false
@export var step_distance = 0.125


var is_dragging: bool = false # Track mouse state


@export var focus_highlight_color: Color = Color.MAGENTA # Highlight color

@export var focus_hover_color: Color = Color.CYAN


# Sensitivity for mouse drag rotation
@export var sensitivity: float = 1.0


var drag_start_position: Vector2 # save mouse pos when we drag (as confining the mouse looses it)

@export var rotate_control: Control
@export var label: Label

@export var format_string = "%.2f"



@export_group("drawing")
@export var draw_knob: bool = true
@export var knob_radius: float = 32.0
@export var knob_back_color: Color = Color.WHITE
@export var knob_dial_color: Color = Color.BLACK

@export var arc_radius: float = 28.0 - 4.0
@export var arc_width: float = 8.0

@export var line_start_offset = 4.0
@export var line_end_offset = 28.0

@export var line_width = 4.0


@export var draw_offset: Vector2 = Vector2(32, 32)
@export_group("")


@export_group("target_vars")

@export var target_node: Node

@export var use_name_as_variable_name: bool = true

@export var target_float: String = ""



@export_group("")



signal dial_changed(new_value_float)



func _on_dial_changed():
    
    pass



func map_range(in_min: float, in_max: float, out_min: float, out_max: float, value: float) -> float:
    # Avoid division by zero if input range is 0
    if in_max == in_min:
        return out_min
    # Map the value from the input range to the output range
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min)


func get_actual_position():
    
    return map_range(min_rotation, max_rotation, range_min, range_max, rotation_angle)






func _ready():
    # Enable focus for this control to work with Tab navigation
    set_focus_mode(FOCUS_ALL)
    
    queue_redraw()
    







func _update_controls():
    if is_instance_valid(rotate_control):
        rotate_control.rotation_degrees = rotation_angle
        
                
    if is_instance_valid(label):
        label.text = format_string % get_actual_position()
        
    



func _gui_input(event):
    # Detect mouse button press
    if event is InputEventMouseButton:
        if event.button_index == MOUSE_BUTTON_LEFT:
            if event.pressed:
                is_dragging = true
                drag_start_position = get_viewport().get_mouse_position()
                
                #Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)  # HACK, seems to hide mouse flicker
                Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)  # Lock mouse
                
            else:
                is_dragging = false
                ## note these extra steps with the mouse mode prevent flickers
            
                Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)  # FLICKERS THIS ORDER
                
                #Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)  # unlock mouse
                Input.warp_mouse(drag_start_position) # move position
                #Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)  # make visible



    #if event is InputEventMouseMotion:
        #modulate = focus_hover_color
       

    
    # Rotate the knob when dragging
    if event is InputEventMouseMotion and is_dragging:
        # Adjust rotation based on vertical mouse drag
        rotation_angle -= event.relative.y * sensitivity
        rotation_angle = clamp(rotation_angle, min_rotation, max_rotation)
        
        _update_controls()
        
        
        var actual_position: float = get_actual_position()
        print("set actual_position ", actual_position)
        if is_instance_valid(target_node):
            
            if use_name_as_variable_name:
                target_float = name
            
            target_node.set(target_float, actual_position)
        dial_changed.emit(actual_position) # signal to show new position
        
        
        ## Update rotation for child controls
        #var rotate_child = get_node_or_null("TextureRect")
        #if rotate_child is Control:
            #
            
        queue_redraw()  # Redraw the knob for visual updates

func _input(event: InputEvent) -> void:
    pass
    
    

func _unhandled_input(event: InputEvent) -> void:    
    
    if event is InputEventMouseMotion:
        
        if get_rect().has_point(event.position - draw_offset):
            modulate = focus_hover_color
        else:
            modulate = Color.WHITE
    
    if event.is_action_pressed("ui_accept") and has_focus():
        # Respond to Enter or Space when focused
        print("Knob activated!")
    elif event.is_action_pressed("ui_focus_next"):
        # Pass focus to the next control on Tab
        get_parent().focus_next()

#func _get_minimum_size() -> Vector2:
    ## Define the size of the control
    #return Vector2(100, 100)
    


func _draw():
    #draw_string(font, Vector(0, 0), 'this is some text')
    
    
    if draw_knob:
    
        draw_circle(draw_offset, knob_radius, knob_back_color, true, -1.0, true)
    
        draw_arc(draw_offset, arc_radius, deg_to_rad(min_rotation - 90.0), deg_to_rad(rotation_angle - 90.0), 32, knob_dial_color, arc_width, true)

        var line_angle = deg_to_rad(rotation_angle - 90.0)
    
        var line_vector = Vector2(cos(line_angle),   sin(line_angle))
        draw_line(line_vector * line_start_offset + draw_offset, line_vector * line_end_offset + draw_offset , knob_dial_color, line_width, true)
    
    
    
    # Highlight the control when it has focus
    if has_focus() or is_dragging:
        #draw_rect(Rect2(Vector2.ZERO, size), focus_highlight_color)
        modulate = focus_highlight_color
    else:
        modulate = Color.WHITE

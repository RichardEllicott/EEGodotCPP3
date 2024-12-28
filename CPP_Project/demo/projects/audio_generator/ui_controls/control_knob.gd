"""

control knob script

draws the knob graphics for a look similar to ableton and various vsts

translates the knob position to a value range and updates a label of the value

sets a target node with the variable (any node, dynamic)


"""
@tool
extends Control



## the actual max/min values the dial represents
@export var range_min: float = 0.0
@export var range_max: float = 1.0

@export var default_value: float = 0.0

var rotation_angle: float = 0.0 # actual rotation of knob, used to draw the knob



@export var format_string = "%.2f"



## unused
@export var step_mode: bool = false
@export var step_distance = 0.125


var is_dragging: bool = false # Track mouse state


@export var focus_highlight_color: Color = Color.MAGENTA # Highlight color

@export var focus_hover_color: Color = Color.CYAN


var drag_start_position: Vector2 # save mouse pos when we drag (as confining the mouse looses it)


@export var rotate_control: Control # rotate a knob image, use a TextureRect with the pivot set
@export var label: Label




@export_group("knob settings")

## limits of the dial rotation, set similar to many DAWs
@export var max_rotation: float = 180.0 - 22.5 # Maximum rotation in degrees
@export var min_rotation: float = -180.0 + 22.5    # Minimum rotation in degrees
# Sensitivity for mouse drag rotation
@export var sensitivity: float = 1.0


@export_group("drawing")

@export var draw_knob: bool = true
@export var knob_radius: float = 32.0
@export var knob_back_color: Color = Color.WHITE
@export var knob_dial_color: Color = Color.BLACK

@export var arc_radius: float = 28.0 - 4.0
@export var arc_width: float = 8.0

@export var line_start_offset: float = 4.0
@export var line_end_offset: float = 28.0

@export var line_width: float = 4.0


@export var draw_offset: Vector2 = Vector2(32, 32)
@export_group("")


@export_group("target_vars")

@export var target_node: Node

@export var use_name_as_property_name: bool = true

@export var property_name: String = ""



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


func get_actual_value():
    return map_range(min_rotation, max_rotation, range_min, range_max, rotation_angle)

func set_actual_value(actual_value: float):
    rotation_angle = map_range(range_min, range_max, min_rotation, max_rotation, actual_value)




func format_to_title_case(input: String) -> String:
    var parts = input.split("_")
    var capitalized_parts = []
    for part in parts:
        capitalized_parts.append(part.capitalize())
    return " ".join(capitalized_parts)

func _ready():
    # Enable focus for this control to work with Tab navigation
    set_focus_mode(FOCUS_ALL)
    
    set_actual_value(default_value)
        
    if use_name_as_property_name:
        property_name = name
        
    tooltip_text = format_to_title_case(property_name)
    
    
    
    _UPDATE()





# the update function sets various values and triggers a new draw
func _UPDATE():
    
    var actual_value: float = get_actual_value()
    
    
    if is_instance_valid(rotate_control):
        rotate_control.rotation_degrees = rotation_angle
                
    if is_instance_valid(label):
        label.text = format_string % actual_value
        
    if is_instance_valid(target_node):        
        target_node.set(property_name, actual_value) # warning no error if not valid
        
        print("%s: %s" % [self.name, target_node.get(property_name)])
        
    dial_changed.emit(actual_value) # signal to show new position
    
    queue_redraw()



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
        
        _UPDATE()
        ## Update rotation for child controls
        #var rotate_child = get_node_or_null("TextureRect")
        #if rotate_child is Control:
            #
            
        

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

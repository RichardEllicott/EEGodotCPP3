"""

creates a keyboard but works different from buttons as we can hold the mouse and slide across the keys

works as plain Control, but also now works with a HBoxContainer which might make UI layout easier



"""
@tool
extends Control

@export var poly_synth: PolySynth

@export_group("keyboard setup")


@export var key_count: int = 61
#@export var key_wrap: int = 36

@export var start_key: int = 0


@export var key_size = Vector2(16, 64)

@export var white_stylebox: StyleBox
@export var black_stylebox: StyleBox

@export_group("")


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    _setup()



var seq_timer: float = 0.0
@export var seq_delay: float = 1.0
@export var counter: int = 0

#var notes: Array[int] = [0, 4, 5, 7]




@export var play_test_sequence := false

@export var hover_color = Color.PALE_VIOLET_RED

@export var pressed_color = Color.ROYAL_BLUE


@export var octave: int = 0 # shift with the 

var pressed_keys = {}


@export var root_note: float = -36;



@export_group("keyboard and joystick")


# like ableton live
var keyboard_keys: Array[int] = [
    KEY_A, # C
    KEY_W, # C#
    KEY_S, # D
    KEY_E, # D#
    KEY_D, # E
    KEY_F, # F
    KEY_T, # F#
    KEY_G, # G
    KEY_Y, # G#
    KEY_H, # A
    KEY_U, # A#
    KEY_J, # B
    KEY_K, # C
    KEY_O, # C#
    KEY_L, # D
]

var octave_up_key = KEY_X
var octave_down_key = KEY_Z
            
var mouse0_down = false    

# standard key pattern of whites and blacks starting from C (1 is white, 0 black)
const key_pattern: Array[int] = [
    0, # C
    1, # C#
    0, # D
    1, # D#
    0, # E
    0, # F
    1, # F#
    0, # G
    1, # G#
    0, # A
    1, # A#
    0 # B
    ]

@export var joypad_scale = [0, 3, 5, 7]
@export var joypad_keys = [JOY_BUTTON_A, JOY_BUTTON_B, JOY_BUTTON_X, JOY_BUTTON_Y]
            
            
@export_group("")


func _input(event):
    
    if event is InputEventKey:
        
        for i in keyboard_keys.size(): # actual computer keyboard as keys
            var key = keyboard_keys[i]
            if event.keycode == key:                
                if event.pressed:
                    _on_key_down(i)
                else:
                    _on_key_up(i)

        if event.pressed:
            if event.keycode == octave_up_key:
                octave += 1
            elif event.keycode == octave_down_key:
                octave -= 1
    
    elif event is InputEventJoypadButton: # joypad to play notes! funny
        for i in joypad_keys.size(): # check all joypad keys
            if event.button_index == joypad_keys[i]: # key match
                if event.pressed:
                     _synth_add_note(joypad_scale[i]) # add note
                else:
                    _synth_clear_note(joypad_scale[i]) # clear note
        
            
    # Handle stick movement
    elif event is InputEventJoypadMotion:
        #if event.device == 0:  # Check the device ID
        if event.axis == JOY_AXIS_LEFT_X:
            print("Left stick X-axis moved: ", event.axis_value)
        elif event.axis == JOY_AXIS_LEFT_Y:
            print("Left stick Y-axis moved: ", event.axis_value)
            

                
    elif event is InputEventMouseMotion:
        pass
        

    
    elif event is InputEventMouseButton:
        match event.button_index:
            0:
                if event.pressed():
                    pass
                else:
                    pass
                    
                    
                    


func _process(delta: float) -> void:
    
    for control in hovering:
        var id: int = hovering[control]
                
        if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
                        
            _synth_add_note(id)
            control.modulate = pressed_color
        else:
            _synth_clear_note(id)
            control.modulate = Color.WHITE
            
            
            
            

func _physics_process(delta: float) -> void:
    

    pass
    
    
func clear_children():
    
    for child in get_children():
        remove_child(child)




## finally call the synth
func _synth_add_note(key: float, volume: float = 1.0):
    
    key += root_note
        
    if is_instance_valid(poly_synth):
        poly_synth.add_note(key, volume)
        
func _synth_clear_note(key: float):

    key += root_note

    if is_instance_valid(poly_synth):
        poly_synth.clear_note(key)

    
func _on_key_up(key: float):
    
    key += octave * 12.0
    
    if int(key) in key_to_control:
        var control =  key_to_control[int(key)]
        control.modulate = Color.WHITE
        
    _synth_clear_note(key)
        

func _on_key_down(key: float):
    
    key += octave * 12.0
    
    if int(key) in key_to_control:
        var control =  key_to_control[int(key)]
        control.modulate = pressed_color

    _synth_add_note(key)
        
            


func macro_test_key0():
    _on_key_down(0)
    
func macro_test_key3():
    _on_key_down(3)
    
func macro_test_key5():
    _on_key_down(5)
    
func macro_test_key7():
    _on_key_down(7)




#var mouse_hovered: int = -1




var hovering = {}

func _on_mouse_entered(id: int, control: Control) -> void:
    #print("_on_mouse_entered() ", id)
    hovering[control] = id
    control.modulate = hover_color


func _on_mouse_exited(id: int, control: Control) -> void:
    #print("_on_mouse_exited() ", id)
    
    if control in hovering:
        control.modulate = Color.WHITE
        hovering.erase(control)
        
        
    _synth_clear_note(id)
        


#var controls = []
var control_to_note = {}


var key_to_control = {}

func _setup():
    
    control_to_note = {}
    key_to_control = {}
    
    clear_children()
    
    for i in key_count:
        
        var offset = Vector2(key_size.x, 0.0) # position of key goes right to left
        
        # i am using a Panel, it uses a stylebox which is nice and detects hovering
        # the button click behaviour doesn't work for glisendo
        # the TextureRect/ColorRect doesn't have a stylebox but could also work
        var node: Panel = StaticLib6.get_or_create_child(self, "%s" % i, Panel)
        node.size = key_size        
        node.position = offset * i
        
        node.custom_minimum_size = key_size # stop a HBoxContainer squishing me (if we use a HBoxContainer)
        
        #if i > key_wrap:
            #node.position.x -= key_size.x * key_wrap
            #node.position.y += key_size.y
        
        var ivory_ref = (i + start_key) + 24
        
        if key_pattern[ivory_ref % 12] == 0:
            node.add_theme_stylebox_override("panel", white_stylebox)

            #tex_rect.color = Color.WHITE
        else:
            #node.add_theme_stylebox_override("normal", black_stylebox)
            node.add_theme_stylebox_override("panel", black_stylebox)

            #tex_rect.color = Color.BLACK
            
        #key.button_down.connect(_on_key_down.bind(i))    
        #key.button_up.connect(_on_key_up.bind(i))
        
        
        node.mouse_entered.connect(_on_mouse_entered.bind(i, node))
        node.mouse_exited.connect(_on_mouse_exited.bind(i, node))
        
        #controls.append(node)
        
        control_to_note[node] = i
        key_to_control[i] = node

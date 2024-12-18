"""
"""
@tool
extends Control


@export var s1_audio_gen: S1AudioGenerator


@export var key_count := 13
@export var key_wrap := 36



# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    _setup()



var seq_timer: float = 0.0
@export var seq_delay: float = 1.0
@export var counter: int = 0

var notes: Array[int] = [0, 4, 5, 7]



@export var key_size = Vector2(32, 128)

@export var white_stylebox: StyleBox
@export var black_stylebox: StyleBox

@export var play_test_sequence := false

@export var hover_color = Color.PALE_VIOLET_RED

@export var pressed_color = Color.ROYAL_BLUE

func _input(event):
    
    
    if event is InputEventMouseMotion:
        
        
        pass
    
    if event is InputEventKey:
        #if event.keycode == KEY_A:
            #print("T was pressed")
        
        for i in keyboard_keys.size():
            var key = keyboard_keys[i]
            if event.keycode == key:
                print("key %s was pressed!" % i)
                
                if event.pressed:
                    _on_key_down(i)
                else:
                    _on_key_up(i)
    
    if event is InputEventMouseButton:
        match event.button_index:
            0:
                if event.pressed():
                    pass
                else:
                    pass
                    
                    
                    


func _process(delta: float) -> void:
    
    if not is_instance_valid(s1_audio_gen): return

    
    for control in hovering:
        var id: int = hovering[control]
        
        if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
                _synth_add_note(id)
                control.modulate = pressed_color
        else:
            _synth_clear_note(id)
            control.modulate = Color.WHITE
            
            
            
            

func _physics_process(delta: float) -> void:
    
    
    if not is_instance_valid(s1_audio_gen): return
        
        
    if mouse0_down:
        
        pass
        
    seq_timer += delta
    if seq_timer >= seq_delay:
        seq_timer -= seq_delay
        
        s1_audio_gen.clear_note(notes[counter % notes.size()])
        
        counter += 1
        
        if play_test_sequence:     
            s1_audio_gen.add_note(notes[counter % notes.size()], 1.0, 1.0)
        
        
        
        
    
        
        
        pass
    pass
    
    
func clear_children():
    
    for child in get_children():
        remove_child(child)



var pressed_keys = {}


func _synth_add_note(key: float, volume: float = 1.0, duration: float = 1.0):
    if is_instance_valid(s1_audio_gen):
        s1_audio_gen.add_note(key, volume, duration)
        
        
func _synth_clear_note(key: float):
    
    if is_instance_valid(s1_audio_gen):
        s1_audio_gen.clear_note(key)




    
func _on_key_up(key: float):
    
    if int(key) in key_to_control:
        var control =  key_to_control[int(key)]
        control.modulate = Color.WHITE
        
    _synth_clear_note(key)
        

func _on_key_down(key: float):
    
    if int(key) in key_to_control:
        var control =  key_to_control[int(key)]
        control.modulate = pressed_color

    _synth_add_note(key)
        

# like ableton live
var keyboard_keys = [
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

            
var mouse0_down = false    
        
const key_pattern := [
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
    #print("_on_mouse_entered() ", ref)
    hovering[control] = id
    control.modulate = hover_color


func _on_mouse_exited(id: int, control: Control) -> void:
    #print("_on_mouse_exited() ", ref)
    
    if control in hovering:
        control.modulate = Color.WHITE
        hovering.erase(control)
        
        _synth_clear_note(id)


#var controls = []
var control_to_note = {}


var key_to_control = {}

func _setup():
    
    #controls = []
    control_to_note = {}
    key_to_control = {}
    
    
    
    clear_children()
    
    for i in key_count:
        
        var offset = Vector2(key_size.x, 0.0)
        
        #StaticL
        #var key: Button = StaticLib6.get_or_create_child(self, "%s" % i, Button)
        #var node: ColorRect = StaticLib6.get_or_create_child(self, "%s" % i, ColorRect)
        var node: Panel = StaticLib6.get_or_create_child(self, "%s" % i, Panel)

        #key.size = key_size
        node.size = key_size
        
        #key.position = offset * i
        node.position = offset * i
        
        
        if i > key_wrap:
            node.position.x -= key_size.x * key_wrap
            node.position.y += key_size.y

        
        if key_pattern[i % 12] == 0:
            #node.add_theme_stylebox_override("normal", white_stylebox)
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

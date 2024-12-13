"""
"""
@tool
extends Control


@export var s1_audio_gen: S1AudioGenerator


@export var keys := 13

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    _setup()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass
    
    
func clear_children():
    
    for child in get_children():
        remove_child(child)

@export var key_size = Vector2(32, 128)

@export var white_stylebox: StyleBox
@export var black_stylebox: StyleBox




var pressed_keys = {}


func macro_control_test():
    
    if is_instance_valid(s1_audio_gen):
        print("check1")
        s1_audio_gen.add_note(0.0, 1.0, 1.0)

    
func _on_key_up(key: float):
    print("_on_key_up ", key)
    #var pitch = 440.0*2**(key/12.0)
    if is_instance_valid(s1_audio_gen):
        s1_audio_gen.clear_note(key)
        pass
        
    
    

func _on_key_down(key: float):
    if is_instance_valid(s1_audio_gen):
        s1_audio_gen.add_note(key, 1.0, 1.0)
        

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


func _input(event):
    if event is InputEventKey:
        #if event.keycode == KEY_A:
            #print("T was pressed")
        
        for i in keyboard_keys.size():
            var key = keyboard_keys[i]
            if event.keycode == key:
                print("key %s was pressed!" % i)
                
                if event.pressed:
                    _on_key_down(i - 12)
                else:
                    _on_key_up(i -12)
            
            
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
    

func _setup():
    
    clear_children()
    
    
    
    for i in keys:
        
        var offset = Vector2(key_size.x, 0.0)
        
        #StaticL
        var key: Button = StaticLib6.get_or_create_child(self, "%s" % i, Button)
        key.size = key_size
        
        key.position = offset * i
        
        if key_pattern[i % 12] == 0:
            key.add_theme_stylebox_override("normal", white_stylebox)
        else:
            key.add_theme_stylebox_override("normal", black_stylebox)
            
            
        key.button_down.connect(_on_key_down.bind(i))    
        key.button_up.connect(_on_key_up.bind(i))
        

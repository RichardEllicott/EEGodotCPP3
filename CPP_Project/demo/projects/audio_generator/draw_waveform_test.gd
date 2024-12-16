"""
"""
@tool
extends Control



@export var s1_audio_generator: S1AudioGenerator

func _draw() -> void:
    
    if not is_instance_valid(s1_audio_generator): return
    
    for i in 256:
        
        
        #var pos = s1_audio_generator.history_bu
        
        pass
    
    pass

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    queue_redraw()

"""

we can override C++ with GDScript!


"""
@tool
extends S1AudioGenerator


func _ready() -> void:
    
    print("hello from control script!")
    
    print(macro_test2("gggg", 356))
    


@export var save_path := "user://"
@export var save_filename := "data.wav"

func macro_save_wave():
    
    audio_stream_wav.save_to_wav("%s%s" % [save_path, save_filename])
    

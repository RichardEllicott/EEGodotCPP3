"""
"""
@tool
extends Node


func _ready() -> void:
    
    # Access the audio bus
    var bus_index = AudioServer.get_bus_index("Master")
    var effect = AudioServer.get_bus_effect(bus_index, 0)  # Access the first effect on the "Master" bus

    # Ensure the effect is of the correct type
    if effect is AudioEffectReverb:
        var reverb_effect = effect as AudioEffectReverb
        reverb_effect.dry = 0.5  # Set the dry level
        reverb_effect.wet = 0.7  # Set the wet level
        reverb_effect.room_size = 0.8  # Set the room size
    pass # Replace with function body.


func _process(delta: float) -> void:
    pass

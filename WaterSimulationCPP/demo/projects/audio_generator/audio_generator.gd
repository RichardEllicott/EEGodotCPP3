"""
corrects flaws from:
    https://docs.godotengine.org/en/stable/classes/class_audiostreamgenerator.html

"""
@tool
extends Node

var playback: AudioStreamGeneratorPlayback # Will hold the AudioStreamGeneratorPlayback.


@onready var sample_hz: float = $AudioStreamPlayer.stream.mix_rate



@export var pulse_hz: float = 440.0 # The frequency of the sound wave.


@export var enabled := false

var timer: float = 0.0
@export var delay: float = 0.125
var phase: float = 0.0 


func _process(delta: float) -> void:
    
    if enabled: fill_buffer()
    
    
    

func _ready() -> void:
    $AudioStreamPlayer.play()
    playback = $AudioStreamPlayer.get_stream_playback()
    #fill_buffer()



func fill_buffer() -> void:
    
    var increment: float = pulse_hz / sample_hz
    var frames_available: int = playback.get_frames_available() # check how many frames we can fill
        
    for i in range(frames_available):
        playback.push_frame(Vector2.ONE * sin(phase * TAU))
        phase = fmod(phase + increment, 1.0)

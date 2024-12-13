"""
"""
@tool
extends Control


@export var grid_size := Vector2i(16, 24)

@export var cell_size := Vector2(16, 16)

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



func _ready() -> void:
    pass


func _process(delta: float) -> void:
    #queue_redraw()
        
    pass
    
    



func _draw():
    
    for y in grid_size.y:
        
        for x in grid_size.x:
            
            var _position := Vector2(x, y) * cell_size
            
            var rect := Rect2(_position, cell_size);
            
            var color := Color.BLACK
            
            if key_pattern[y % 12] == 0:
                color = Color.WHITE
            
            draw_rect(rect, color, true, 1.0, false)
            
            
            
            
            

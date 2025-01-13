"""
"""
@tool
extends Node


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    
    # Access the static function
    #var result = MyLibrary.my_static_function(5, 3) 
    #print(result)  # Output: 8
    
    #var result = MyLibrary.new().my_static_function(5, 3) 
    #MyLibrary.
    
    print(CPPLibrary.get_lib().my_static_function(3,4))
    
    
    
    
    
    
    pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass

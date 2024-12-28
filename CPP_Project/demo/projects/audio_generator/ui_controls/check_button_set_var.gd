"""

check button to set a bool on a target

"""
@tool
extends CheckButton

@export_group("")

@export var target_node: Node
@export var use_name_as_property_name: bool = true
@export var property_name: String = ""

@export_group("")

func _ready() -> void:
    
    _on_toggled(button_pressed) # trigger this here to make the synth start with the same variable value
    
    toggled.connect(_on_toggled)
    
    if use_name_as_property_name:
        property_name = name
        
        
func _on_toggled(toggled_on: bool) -> void:   
    if is_instance_valid(target_node):
        target_node.set(property_name, toggled_on)

"""

use a spinbox to set a property on target

"""

@tool
extends SpinBox

@export_group("")

@export var target_node: Node
@export var use_name_as_property_name: bool = true
@export var property_name: String = ""

@export_group("")

func _ready() -> void:
    
    value_changed.connect(_on_value_changed)
    
    if use_name_as_property_name:
        property_name = name
        
        
func _on_value_changed(value: float) -> void:   
    if is_instance_valid(target_node):
        target_node.set(property_name, value)

        

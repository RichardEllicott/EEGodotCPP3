"""

use a button trigger a method or set a property

"""
@tool
extends Button


@export_group("")


@export var target_node: Node
@export var use_name_as_property_name: bool = true
@export var property_or_method_name: String = ""


@export_group("")

func _ready() -> void:
    
    toggled.connect(_on_toggled) # toggled will try to set a bool
    pressed.connect(_on_pressed) # pressed will try to launch a method
    
    #for prop in get_property_list():
        #print(prop)
    
    if use_name_as_property_name:
        property_or_method_name = name
        

    
    
func _on_pressed() -> void:
    if is_instance_valid(target_node):
        if target_node.has_method(property_or_method_name):
            target_node.call(property_or_method_name)
        
        
        
func _on_toggled(value: bool) -> void:   
    if is_instance_valid(target_node):
        target_node.set(property_or_method_name, value)

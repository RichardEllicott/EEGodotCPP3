@tool
extends SpinBox

@export_group("target_vars")

@export var target_node: Node
@export var use_name_as_variable_name: bool = true
@export var target_int: String = ""

@export_group("")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    
    
    if is_instance_valid(target_node):
        # Connect the 'value_changed' signal of the SpinBox to the custom function
        #self.connect("value_changed", self, "_on_value_changed")
        
        value_changed.connect(_on_value_changed)

# Called when the value of the SpinBox changes
func _on_value_changed(value: float) -> void:
    # Check if we are using the name as the variable name
    if use_name_as_variable_name and target_int == "":
        target_int = self.name  # Set target_int to the name of the SpinBox if it's empty

    # Ensure the target node has the variable (with target_int as the variable name)
    if target_node and target_int != "":
        # Dynamically set the target variable using the name of the variable
        target_node.set(target_int, int(value))

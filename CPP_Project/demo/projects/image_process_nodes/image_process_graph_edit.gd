"""




"""
@tool
extends GraphEdit
class_name ImageProcessGraphEdit


enum ConnectionType{
    grayscale,
    color,
}


func _on_connection_request(from_node: StringName, from_port: int, to_node: StringName, to_port: int):
    connect_node(from_node, from_port, to_node, to_port)



func _add_node():
    
    
    var graph_node := GraphNode.new()
    
    add_child(graph_node)
    
    if Engine.is_editor_hint():
        # we always set the owner after adding the node to a tree if using tool mode
        graph_node.owner = get_tree().edited_scene_root 
        
        
        
    
    pass






# Handle disconnection requests
func _on_disconnection_request(from_node: String, from_port: int, to_node: String, to_port: int):
    
    print("_on_disconnection_request %s %s %s %s" % [from_node, from_port, to_node, to_port] )
    # Disconnect the specified nodes and ports
    if is_node_connected(from_node, from_port, to_node, to_port):
        disconnect_node(from_node, from_port, to_node, to_port)
        print("Disconnected:", from_node, "port", from_port, "from", to_node, "port", to_port)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    
    #add_valid_left_disconnect_type(0)
    #add_valid_left_disconnect_type(1)
    add_valid_right_disconnect_type(0)
    add_valid_right_disconnect_type(1)
    
    connection_request.connect(_on_connection_request)
    disconnection_request.connect(_on_disconnection_request)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass

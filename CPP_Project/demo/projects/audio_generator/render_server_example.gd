"""
"""
@tool
extends Control


# RenderingServer expects references to be kept around.
@export var texture: Texture

var ci_rid: RID

func _enter_tree() -> void:    
    
    
    
    
    # Create a canvas item, child of this node.
    ci_rid = RenderingServer.canvas_item_create()
    # Make this node the parent.
    RenderingServer.canvas_item_set_parent(ci_rid, get_canvas_item())
    # Draw a texture on it.
    # Remember, keep this reference.
    #texture = load("res://my_texture.png")
    # Add it, centered.
    #RenderingServer.canvas_item_add_texture_rect(ci_rid, Rect2(-texture.get_size() / 2, texture.get_size()), texture)
    
    RenderingServer.canvas_item_add_circle(ci_rid, Vector2.ZERO, 16.0, Color.AQUAMARINE, true) # works but not rect

    RenderingServer.canvas_item_add_rect(ci_rid, Rect2(Vector2(), Vector2(32, 16) ) , Color.DARK_RED, true   )
    
    
    
    
    
    ## Add the item, rotated 45 degrees and translated.
    #var xform = Transform2D().rotated(deg_to_rad(45)).translated(Vector2(20, 30))
    #RenderingServer.canvas_item_set_transform(ci_rid, xform)
    
    
func _exit_tree() -> void:
    RenderingServer.canvas_item_clear(ci_rid)
    
    
    

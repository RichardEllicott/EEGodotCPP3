"""

my editor plugin, this script requires a dock scene but all code here

if you edit me, disable/enable me in "project settings"->"plugins"


"Script Trigger 1.0"

when we select a node, looks for methods on the object that begin with "macro"
creates buttons in the dock to allow triggering these scripts

default regex:
    
    (?i)(?<!\\w)_ready(?!\\w)|^macro_|^_on_
    
    note have to add double \\ here!
    
    finds functions matching
    _ready
    _on_*
    macro*

"""
@tool
extends EditorPlugin

var dock: Control
var editor_selection: EditorSelection = get_editor_interface().get_selection()


#var dock_position = DOCK_SLOT_LEFT_UL # left side (good for debug)
var dock_position = DOCK_SLOT_RIGHT_UL # right side with the inspector (best place)


func _enter_tree():
    dock = preload("res://addons/script_trigger/script_trigger_dock.tscn").instantiate()
    add_control_to_dock(dock_position, dock) # easier for testing
    
    # Note that LEFT_UL means the left of the editor, upper-left dock.
    editor_selection.connect("selection_changed", _on_selection_changed)
    


func _exit_tree():
    editor_selection.disconnect("selection_changed", _on_selection_changed) # not sure this is required but just incase
    remove_control_from_docks(dock) # Remove the dock.
    dock.free() # Erase the control from the memory.




## triggered by signal when buttons pressed
func _on_vbox_button_pressed(button: Button):

    for selected_node in selected_nodes:
        if not selected_node.has_method(button.name):
            
            var err = "script trigger cannot call \"%s\" on node \"%s\" as it doesn't exist!" % [button.name, selected_node.name]
            print('💥 method "%s()" does not exist on "%s"' % [button.name, selected_node.name])
        else:
            
            
            if not selected_node.script.is_tool():
                var err = 'script trigger cannot call "%s" on node "%s" as it is not a tool script' % [button.name, selected_node.name]
                #print()
                print('💥 "%s" probabally does not have a tool script!' % [selected_node.name])

            

            else:
                #print("🚀 call %s.%s()" % [selected_node2.name, button.name])
                print('🚀 call method "%s()" on node "%s"' % [button.name, selected_node.name])
                selected_node.call(button.name)
                    
                

        
    


var selected_nodes
var selected_node



func _find_child():
    
    
    pass



var TARGETS: Label: # list all selected nodes
    get:
        if not TARGETS:
            TARGETS = dock.find_child("TARGETS")
        return TARGETS
        
        
var TARGET: Label:
    get:
        if not TARGET:
            TARGET = dock.find_child("TARGET")
        return TARGET
        
        
var REGEX: LineEdit: # allows editing the regex pattern
    get:
        if not REGEX:
            REGEX = dock.find_child("REGEX")
        return REGEX


var MACROS: VBoxContainer: # has a liost of command buttons
    get:
        if not MACROS:
            MACROS = dock.find_child("MACROS")
        return MACROS

var GLOBALS: VBoxContainer: # has a liost of command buttons
    get:
        if not GLOBALS:
            GLOBALS = dock.find_child("GLOBALS")
        return GLOBALS

func _ready():
    
        
    if not REGEX.text_changed.is_connected(_on_regex_text_changed):
        REGEX.text_changed.connect(_on_regex_text_changed.bind())
    get_regex()
    


func get_regex_stylebox() -> StyleBoxFlat:
    
    var stylebox: StyleBoxFlat = REGEX.get_theme_stylebox("normal")
    if stylebox == null:
        stylebox = StyleBoxFlat.new()
        REGEX.set_theme_stylebox_override("normal", stylebox)
    return stylebox


var regex: RegEx = RegEx.new() 
var regex_text: String = "" # last compiled string, default: "(?i)(?<!\w)_ready(?!\w)|^macro"


func get_regex():
    
    if regex_text != REGEX.text:
        regex_text = REGEX.text
        regex.compile(REGEX.text)
        if regex.is_valid():
            get_regex_stylebox().bg_color = Color(0,0.5,0)
        else:
            get_regex_stylebox().bg_color = Color.RED
    
    return regex


func _on_regex_text_changed(_new_text):    
    get_regex()
    
    

func MACRO_Drop_To_Floor():
    print("try to drop all selected nodes to floor...")
    
    


# https://docs.godotengine.org/en/stable/tutorials/editor/default_key_mapping.html#d-spatial-editor

    
func MACRO_Snap_To_Grid():
    print("try to drop all selected nodes to floor...")
    
    #get_editor_interface().
    
    var settings = EditorInterface.get_editor_settings()
    print(settings.get_setting("some/property"))
    
    
    pass
    
    
func _on_globals_button_pressed(button: Button):    
    call(button.name)


# special macros to run on all the nodes
# you only need to call this once unless working on the script
func _update_global_macros():
    
    
    for child in GLOBALS.get_children():
        GLOBALS.remove_child(child)
    
    for method in get_method_list():
        var method_name: String = method['name']
        
        var macro_tag = "MACRO_"
        
        if method_name.begins_with(macro_tag):
            
            var button_text = method_name.substr(macro_tag.length(), -1).replace("_", " ")
            
            var button: Button = Button.new() # create a whole new button
            button.modulate = Color(1.0, 0.5, 0.5)
            GLOBALS.add_child(button)
            button.owner = owner # editor scripts need this to show the node
            button.name = method_name # name must match method
            button.text = button_text
            button.pressed.connect(_on_globals_button_pressed.bind(button)) # bind a signal sending the button ref
            
            button.tooltip_text = "trigger method \"%s\" on script_trigger.gd" % [method_name]

    
    #var button: Button = Button.new() # create a whole new button
    #button.modulate = Color(1.0, 0.5, 0.5)
    #GLOBALS.add_child(button)
    #button.owner = owner # editor scripts need this to show the node
    #button.name = "GOLBAL COMMAND"
    #button.text = button.name
    
    
    #button.pressed.connect(_on_vbox_button_pressed.bind(button)) # bind a signal sending the button ref

    #button.tooltip_text = "trigger method \"%s\" on \"%s\"   " % [method_name, selected_node.name]




func _select_target_node(_selected_node):
    selected_node = _selected_node
    
    #print("🚀 selected node: ", selected_node)

    get_regex()
    
    var added_method_log = {} # stops the same method being added twice
    
    TARGET.text = '%s' % selected_node.name
    
    var selected_nodes_string = ""
    for i in selected_nodes.size():
        var i1 = selected_nodes.size() - i - 1
        selected_nodes_string += "%s, " % selected_nodes[i1].name
    
    TARGETS.text = selected_nodes_string

    if selected_node is Button:
        # we might add signal sending features
#                print(self, " SELCTED NODE IS A BUTTON, TODO: MAKE OUR PLUGIN SEND SIGNALS FROM BUTTONS")
        pass
    

    var methods = selected_node.get_method_list()
    for method in methods:
        var method_name: String = method['name']
        
        var is_valid: bool = regex.search(method_name) != null # if not null, method matches pattern
            
        if method_name in added_method_log: # stops the same method being added twice
            is_valid = false
        
        if is_valid:
            
            added_method_log[method_name] = null # mark the log (prevents bug adding same function twice)
                        
            var button: Button = Button.new() # create a whole new button
            MACROS.add_child(button)
            button.owner = owner # editor scripts need this to show the node
            button.name = method_name
            button.text = button.name
            button.pressed.connect(_on_vbox_button_pressed.bind(button)) # bind a signal sending the button ref
        
            button.tooltip_text = "trigger method \"%s\" on \"%s\"   " % [method_name, selected_node.name]
        
        
var last_selected_node

func _clear_buttons():
    for child in MACROS.get_children(): # remove all the previous macro buttons
        MACROS.remove_child(child)     


## editor selection changed
func _on_selection_changed():
    
    _update_global_macros()
 
    _clear_buttons() # we clear all old buttons
                   
    selected_nodes = editor_selection.get_selected_nodes()
    
    
    
    
    
    
    print("🚀 selected nodes: ", selected_nodes)
    if selected_nodes.size() > 0:
        
        
        #_select_target_node(selected_nodes[0]) # if we have actually select a node (note only one is assumed selected)
        _select_target_node(selected_nodes.back()) # now use last selected as prime like Blender
            
            
            
                

        



    
    

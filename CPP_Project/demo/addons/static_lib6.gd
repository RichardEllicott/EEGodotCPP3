"""

static library of helper functions, too much code duplication!

made

03/05/2024


so far

tag_extract
get_or_create_child (super duck typed verison)


"""
@tool
class_name StaticLib6
    
    
   
## tag extract discreet version:
## tags like <tag <subtag>> will return back as "tag <subtag>"
## but normally throws error as max_nest = 1!
static func tag_extract(input_string: String, open_symbol: String = '<', close_symbol: String = '>') -> Array[String]:

    var nesting: int = 0
    var text: String = "" # store all text not in tags
    var tags: Array[String] = []
    var bracketed_text: String = ""
    var max_nest: int = 5

    for _char: String in input_string:
        
        if _char == open_symbol: ## when we open the brackets
            nesting += 1
            if nesting > max_nest:
                #push_error("tag extract nest exceeded %s (malformed xml)" % [max_nest])
                pass
            if nesting > 1: # as we are already nested so we store the < char
                bracketed_text += _char
            
        elif _char == close_symbol: ## when we close the brackets
            nesting -= 1
            if nesting > 0: # we still have a tag open so store the > char
                bracketed_text += _char
            
            if nesting == 0: # we must have closed the tag, so save this text
                tags.append(bracketed_text)
                bracketed_text = ""
            
        elif nesting == 0: # at nest zero no tags are open
            text += _char
        elif nesting > 0: # if tags open
            bracketed_text += _char
            
    if nesting > 0:
        push_error("tag extract, tag not closed (malformed xml)")
        pass
        
    #tags = [text] + tags
    tags.push_front(text)
    
    return tags
    

## the following function is used to simplify instantiating controls

## super duck typed version of my standard get_or_create_child function
##
## either returns the existing node, or spawns a new one (including in tool mode)
##
## _parent: target parent to add childs to
## _name: if this is set as a string, will try to grab and return existing child (so as not to duplicate)
##        if null, will just create a new child
## _type: accepts a built in node like Button, Node3D etc
##        or a string of a built in, like "Button", "Node3D"
##        or a PackedScene
##        or a node we will duplicate
static func get_or_create_child(_parent: Node, _name: String, _type = Node, add_to_edit_tree = true) -> Node:
    var child = _parent.get_node_or_null(_name) # attempt to get the node by name
    if child == null: # if no node, we need to create one
        
        if _type is PackedScene: # if a PackedScene, instantiate it
            child = _type.instantiate()
            
        elif _type is String: # if a string, try to match it in the ClassDB
            if ClassDB.class_exists(_type):
                child = ClassDB.instantiate(_type)
            else: # unrecognised type string
                child = Label.new() 
                push_error("unrecognised type string: \"%s\"" % _type)
        
        elif _type is Node: # if a Node, duplicate it
            child = _type.duplicate() # warning using duplicate may be less effecient
        
        else: # last assumption is a plain type and try to call "new"
            child = _type.new()
        child.name = _name
        
        _parent.add_child(child)
        
        if add_to_edit_tree and Engine.is_editor_hint():
            # we always set the owner after adding the node to a tree if using tool mode
            child.owner = _parent.get_tree().edited_scene_root 

    return child



## group singleton, throws an error to crash if no result!
static func get_group_singleton(_self: Node, tag: String) -> Node:
    var nodes = _self.get_tree().get_nodes_in_group(tag)
    
    # errors
    if nodes.is_empty(): 
        push_error('no node found with tag "%s"' % tag)
    elif nodes.size() > 1:
        push_error('found more than 1 node with tag "%s"' % tag)
    
    return nodes[0]
    


## random bytes with optional input rng (for procedural)
## examples:
## print(get_random_bytes(8).hex_encode()) # print 8 random bytes as hex
## print(Marshalls.raw_to_base64(get_random_bytes(8))) # print 8 random bytes in base64
##

static var rng: RandomNumberGenerator = RandomNumberGenerator.new()

static func get_random_bytes(count: int, rng: RandomNumberGenerator = rng) -> PackedByteArray:
    var bytes = PackedByteArray()
    bytes.resize(count) 
    for i in count:
        bytes[i] = rng.randi() % 255
    return bytes
    
## example
## StaticLib6.get_random_bytes(8).hex_encode()
    







## used chatgp
static var random_words = """Shadowblade
Ironclad
Reaper
Mystic
Wraith
Phoenix
Knight
Rune
Sorcerer
Darkstorm
Warrior
Thunderforge
Nightshade
Assassin
Crimson
Valor
Frostbite
Champion
Arcane
Crusader
Celestial
Specter
Drakon
Blood
Ember
Fury
Shadowthorn
Stormbreaker
Nightfire
Rogue
Silverthorn
Voidwalker
Warlock
Blade
"""

static var _random_words
static func get_random_username2():
    if not _random_words:
        _random_words = random_words.split("\n")
    var ret = _random_words[randi() % _random_words.size()] 
    ret += str(randi() % 100)
    return ret
    



static func unix_time_to_timestamp(seconds: float, format: String = "{year}-{month}-{day} {hour}:{minute}:{second}") -> String:
    return format.format(Time.get_datetime_dict_from_unix_time(seconds))
    
## my preferred get time function (not system time)
## gives seconds since program start
static func time():
    return Time.get_ticks_usec() / 1000000.0



## SIMPLE SAVING USER DATA

## simple store var (as godot binary, kept as example)
static func save_data(save_path: String, data) -> void:
    var file: FileAccess = FileAccess.open(save_path, FileAccess.WRITE)
    file.store_var(data)

## simple load var (as godot binary), return null if no file
static func load_data(save_path: String):
    var file: FileAccess = FileAccess.open(save_path, FileAccess.READ)
    if file:
        return file.get_var()
        
        
## save string as file
static func save_string(save_path: String, string: String) -> void:
    FileAccess.open(save_path, FileAccess.WRITE).store_string(string)

## load string as file, returns empty string if no file
static func load_string(save_path: String) -> String:
    var file: FileAccess = FileAccess.open(save_path, FileAccess.READ)
    if file:
        return file.get_as_text()
    return ""

## save as json, will save numbers, strings, arrays, dictionarys (with string keys)
static func save_json(save_path: String, data) -> void:
    save_string(save_path, JSON.stringify(data, "  "))

## load json, return null if no data, missing file etc
static func load_json(save_path: String):
    var json = JSON.new()
    var error = json.parse(load_string(save_path))
    if error == OK:
        return json.data
        
        
## saving a JSON keyfile
## this file we can easily add keys to like a registry, adding json compatible data

static var key_file_path: String = "user://keyfile.json"
static var _keys

## get all keys (as a dictionary)
static func load_keys() -> Dictionary:
    if _keys: # skip loading keys if already loaded
        return _keys
    _keys = load_json(key_file_path) # try json load
    if not _keys:
        _keys = {} # if null, make new key dict
    return _keys
    
## save all keys at once
static func save_keys(keys: Dictionary) -> void:
    _keys = keys
    save_json(key_file_path, keys)

## save key (note saves entire file)
static func save_key(key: String, value) -> void:
    var keys = load_keys()
    keys[key] = value
    save_keys(keys)

## get var (if present) or null
static func load_key(key: String):
    var keys = load_keys()
    if key in keys:
        return keys[key]





# get all children with a max depth (can help to prevent finding too many nodes)
static func get_all_children(_self: Node, max_depth: int = 4, children: Array[Node] = [], depth: int = 0) -> Array[Node]:
    if depth <= max_depth:
        children.push_back(_self)
        for child in _self.get_children():
            children = get_all_children(child, max_depth, children, depth + 1)
    return children
    
    
    



## find children using a predicate
## no recursion, checks level by level (faster than recursion)
## has a max depth
##
## EXAMPLE: find using the owner as root to find all GridMap objects:
## var gridmaps = StaticLib6.find_children(owner, func(x): return x is GridMap)
##
## WARNING: like the built in functions these are also slow, needs a cache
##
## THIS IS THE MAIN FUNCTION WE USE, WE WILL REUSE IT
##
static func find_children(
    _self: Node,
    match_predicate: Callable,
    max_depth: int = 0, # WARNING default no depth
    max_results: int = 10000000,
    ) -> Array[Node]:
    
    var results: Array[Node] = [] # matching nodes
    
    var checks: int = 0 # check count (debug)
    
    var waiting: Array[Node] = _self.get_children() # to process
    var waiting_depth: Array[int] = []
    for node in waiting:
        waiting_depth.append(0)
        
    while waiting.size() > 0:
        if results.size() == max_results: # max results found break
            break
            
        var node := waiting.pop_front() as Node        
        var depth := waiting_depth.pop_front() as int
        
        #if depth <= max_depth:            
        if match_predicate.call(node):
            results.append(node)
        checks += 1
        
        var new_depth: int = depth + 1
        
        if new_depth <= max_depth:
            for child in node.get_children():
                waiting.append(child)
                waiting_depth.append(new_depth)
                
    #print("checks: ", checks) # if we allow too much depth and search too large trees we will be very slow
    #print("result count: ", results.size())
    
    return results

## dress up function to find just one match
## example
## var animation_player = StaticLib6.find_child(self, func(x): return x is AnimationPlayer, 1)
## searches all first childs at depth 0, then the child childs at depth 1
##
static func find_child(
    _self: Node,
    match_predicate: Callable,
    max_depth: int = 0,
    ) -> Node:
    
    var results: Array = find_children(_self, match_predicate, max_depth, 1)
    if not results.is_empty():
        return results[0]
    else:
        return null

## find a children with a regex match on their names
##
## example:
## result = StaticLib6.find_children_with_regex(_self.owner, "^START")
##
## WARNING: slow function, need a cache

static func find_children_with_regex(
    _self: Node,
    regex_string: String = "^START", # starts with example
    max_depth: int = 0,
    max_results: int = 10000000,
    ) -> Array[Node]:
        
    var regex: RegEx = RegEx.new()
    regex.compile(regex_string)
        
    return find_children(
        _self,
        func(child): return regex.search(child.name),
        max_depth,
        max_results
    )
    
## dress up function to find just one match
static func find_child_with_regex(
    _self: Node,
    regex_string: String = "^START", # starts with example
    max_depth: int = 0,
    ) -> Node:
    
    var results: Array = find_children_with_regex(_self, regex_string, max_depth, 1)
    if not results.is_empty():
        return results[0]
    else:
        return null




## simple version for copy paste on a node (no depth)
## returns Node or null

func get_children(): # just required for this example to not break
    return []
    
func _find_child(match_predicate: Callable) -> Node:
    for child in get_children():
        if match_predicate.call(child):
            return child
    return null

    
## find in group, this dressup is short but as it has a match predicate it could be used like:
##
## var player = find_in_group(self, "PLAYER", func(node): return node.get_multiplayer_authority() == 1)
##
## by default the predicate just returns true, so we just get the first member of the group back (singleton)
##

static func find_in_group(
    _self: Node,
    group: String,
    match_predicate: Callable = func(x): return true # defauult find first in group
    ) -> Node:
        
    for node in _self.get_tree().get_nodes_in_group(group):
        if match_predicate.call(node):
            return node
    return null
    

static func find_nodes_in_group(
    _self: Node,
    group: String,
    match_predicate: Callable = func(x): return true # defauult find first in group
    ) -> Array[Node]:
    
    var ret: Array[Node] = []
    for node in _self.get_tree().get_nodes_in_group(group):
        if match_predicate.call(node):
            ret.append(node)
    return ret


static func find_node3ds_in_group(
    _self: Node,
    group: String,
    match_predicate: Callable = func(x): return true # defauult find first in group
    ) -> Array[Node3D]:
    
    var ret: Array[Node3D] = []
    for node in _self.get_tree().get_nodes_in_group(group):
        if node is Node3D and match_predicate.call(node):
            ret.append(node)
    return ret



## for example, find with multiplayer_authority
static func find_nodes_in_group_with_multiplayer_authority(_self: Node, group: String, multiplayer_authority: int = 1):
    return find_nodes_in_group(_self, group, func(node: Node): return node.get_multiplayer_authority() == multiplayer_authority)
    
    
            
## convert a glob pattern to regex:
##
## we replace * with .*
## we add ^ and $ to the front and back
##
## ^ starts with
## $ ends with
## . any char
## any amount of that char (even 0)
##
static func glob_to_regx(glob_pattern = "*.png"):
    return "^%s$" % glob_pattern.replace("*", ".*")

    
# some examples of these find child functions
static func _find_child_examples(_self: Node):

    var result
    
    ## global search of all GridMap objects
    ## note owner only works from the top of scene trees, eg do not call inside a unit itself
    ##
    ## but owner is the easiest way to get the root node of your scene, somewhat decoupled
    ##
    
    ## this example we show how we can search for a type (not possible in one line without the predicate)
    result = StaticLib6.find_children(_self.owner, func(x): return x is GridMap, 10)
    print(result)
    
    ## regex example, find children starting with string:
    result = StaticLib6.find_children_with_regex(_self.owner, "^START")
    print(result)
    
    ## BAD EXAMPLE:
    # this would find our gridmap but would check thousands of nodes to get there
    result = StaticLib6.find_children(_self.get_tree().root, func(x): return x is GridMap, 100)
    print(result)
    
    




## just for testing
static func __sort_ascending(a, b):
    if a[0] < b[0]:
        return true
    return false
    


## sort but gives us back just the references
## i have not bothered to implement a better sort in gdscript as this C is probabally faster
##
## in C# i use an insert sort and no arrays need to be copied
##
static func sort_array_to_refs(array: Array, _lambda: Callable = func(a, b): return a[0] < b[0]) -> Array[int]:
    
    var sort_me = []
    sort_me.resize(array.size())
    for i in array.size():
        sort_me[i] = [array[i], i]
    
    sort_me.sort_custom(_lambda)
    
    var ret: Array[int] = []
    for item in sort_me:
        ret.append(item[1]) # return just the ref    
    return ret

## example of how to sort ascending
## note annoying subref syntax "a[0]" ... based on the docs example
static func sort_array_ascending(array: Array):
    return sort_array_to_refs(array, func(a, b): return a[0] < b[0])
    

static func __sort_test():
    var my_items = [[5, "Potato"], [9, "Rice"], [4, "Tomato"]]
    my_items.sort_custom(__sort_ascending)
    print(my_items) # Prints [[4, Tomato], [5, Potato], [9, Rice]].

    # Descending, lambda version.
    my_items.sort_custom(func(a, b): return a[0] > b[0])
    print(my_items) # Prints [[9, Rice], [5, Potato], [4, Tomato]].
    


## sort a list of nodes by distance to point
## use the sort_array_to_refs function
static func sort_nodes_by_distance(targets: Array[Node3D], origin: Vector3) -> Array[Node3D]:
        
    # build array of distances
    var distances: Array[float] = []
    distances.resize(targets.size())
    for i in targets.size():
        distances[i] = targets[i].position.distance_to(origin)
    
    # sort by distances
    var sorter = sort_array_to_refs(distances, func(a, b): return a[0] < b[0])
    
    # build array of Node3D to return
    var ret: Array[Node3D] = []
    ret.resize(sorter.size())
    for i in sorter.size():  
        ret[i] = targets[sorter[i]]
    return ret


### sort nodes by distance, from the "from" Vector3 position
### distances are from nearest to furthest
#static func sort_nodes_by_distance(from: Vector3, targets: Array[Node3D]) -> Array[Node3D]:
    #
    #if targets.is_empty(): # in case we pass empty array
        #return [] # pass empty back
        #
    #var distances: Array[float] = [] # get all the distances in an array
    #for node in targets:
        #distances.append(node.position.distance_to(from))
        #
    #var sorted = StaticLib6.sort_array_ascending(distances) # the sort references
    #
    #var ret: Array[Node3D] = [] # build return array of Nodes sorted by distance (closest to furtherst)
    #for i in sorted.size():
        #ret.append(targets[sorted[i]])
    #return ret
    



## convert the mouse position (which is on the screen) to a world position on a plane
## default plane normal faces up (+y)
static func get_mouse_plane_position(camera: Camera3D, mouse_position: Vector2, plane = Plane(Vector3(0,1,0))):
    # https://godotengine.org/qa/25922/how-to-get-3d-position-of-the-mouse-cursor
    var from = camera.project_ray_origin(mouse_position)
    var to = camera.project_ray_normal(mouse_position)
    return plane.intersects_ray(from, to)     
    
    
    
## WORLD QUERYS
    
## query a shape in the world, get all areas and bodies
##
## to get the space state, call:
##
## get_world_3d().direct_space_state
##
## on Node3D
##
static func intersect_shape(
    direct_space_state: PhysicsDirectSpaceState3D,
    shape: Shape3D,
    transform: Transform3D,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    max_results: int = 32,
    ) -> Array[Dictionary]:
    
    var query: PhysicsShapeQueryParameters3D = PhysicsShapeQueryParameters3D.new()
    query.transform = transform
    query.collide_with_areas = collide_with_areas
    query.collide_with_bodies = collide_with_bodies
    query.shape = shape
    var results: Array[Dictionary] = direct_space_state.intersect_shape(query, max_results)
    
    return results
    
    #var results: Array[Dictionary] = direct_space_state.intersect_shape(query)
    #
    #var ret: Array[Node3D] = [] # resort to just a simple list of nodes
    #if results:
        #ret.resize(results.size())
        #for i in results.size():
            #ret[i] = results[i].collider
    #return ret

## query a sphere in the world, get all areas and bodies
##
## intersect_sphere(get_world_3d().direct_space_state, _position, radius)
static func intersect_sphere(
    direct_space_state: PhysicsDirectSpaceState3D,
    _position: Vector3,
    radius: float,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    max_results: int = 32
    ) -> Array[Dictionary]:
    
    var sphere: SphereShape3D = SphereShape3D.new()
    sphere.radius = radius
    return intersect_shape(
        direct_space_state,
        sphere,
        Transform3D(Vector3.RIGHT, Vector3.UP, Vector3.BACK, _position),
        collide_with_areas,
        collide_with_bodies,
        max_results
        )



## raycast Godot 4, note this raycast doesn't hit from the inside of shapes, it can be set that way
##
## intersect_ray(get_world_3d().direct_space_state, from, to)
static func intersect_ray(
    direct_space_state: PhysicsDirectSpaceState3D,
    from: Vector3,
    to: Vector3,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    #max_results: int = 32
    exclude: Array[RID] = []
    ) -> Dictionary:
        
    var query = PhysicsRayQueryParameters3D.create(from, to)
    
    query.collide_with_areas = collide_with_areas
    query.collide_with_bodies = collide_with_bodies
    
    query.exclude = exclude
    
    return direct_space_state.intersect_ray(query)


## standard los check, checks for the targets parent incase we hit a child
## 
## los_check(get_world_3d().direct_space_state, self, target_node)
static func los_check(
    spacestate: PhysicsDirectSpaceState3D,
    from_node: Node3D,
    to_node: Node3D,
    ray_offsey: Vector3 = Vector3(0,1,0) # this allows us to fire the ray from say slightly above the origin (which might be the floor)
    ):
    
    var exclude = []
    for child in from_node.get_children():
        if child is CollisionObject3D:
            exclude.append(child.get_rid())
        
    var ray = StaticLib6.intersect_ray( # raycast
        spacestate,
        from_node.global_position + ray_offsey,
        to_node.global_position + ray_offsey,
        )
        
    if ray:
        var collider = ray.collider
        if collider == to_node:
            return true
        elif collider.get_parent() == to_node:
            return true
        
    return false



## goes through colliders to find them all!
##
## get_world_3d().direct_space_state
static func intersect_xray(
    direct_space_state: PhysicsDirectSpaceState3D,
    from: Vector3,
    to: Vector3,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    max_results: int = 32
    ) -> Array[Dictionary]:
        
    var ret: Array[Dictionary] = []
    
    var current_position: Vector3 = from
    
    var query = PhysicsRayQueryParameters3D.new()
    query.collide_with_areas = collide_with_areas
    query.collide_with_bodies = collide_with_bodies
    query.from = from
    query.to = to
    
    for i in max_results:
        
        var result = direct_space_state.intersect_ray(query)
        
        if result:
            ret.append(result)
            query.from = result.position
        else:
            break
        

    return ret
        
    #var query = PhysicsRayQueryParameters3D.create(from, to)
    #
    #query.collide_with_areas = collide_with_areas
    #query.collide_with_bodies = collide_with_bodies
    #return direct_space_state.intersect_ray(query)


## 2D World Query

## get mouse position:
## StaticLib6.point_cast2d(get_world_2d().direct_space_state, get_global_mouse_position())
##
static func intersect_point_2d(
    direct_space_state: PhysicsDirectSpaceState2D,
    position: Vector2,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    max_results: int = 32,
    ):
    
    var query: PhysicsPointQueryParameters2D = PhysicsPointQueryParameters2D.new()
    query.position = position
    query.collide_with_areas = collide_with_areas
    query.collide_with_bodies = collide_with_bodies
    return direct_space_state.intersect_point(query, max_results)



static func intersect_shape_2d(
    direct_space_state: PhysicsDirectSpaceState2D,
    transform: Transform2D,
    shape: Shape2D,
    collide_with_areas: bool = false,
    collide_with_bodies: bool = true,
    max_results: int = 32,
    ):
    
    var query: PhysicsShapeQueryParameters2D = PhysicsShapeQueryParameters2D.new()
    query.shape = shape

    query.transform
    
    query.collide_with_areas = collide_with_areas
    query.collide_with_bodies = collide_with_bodies
    return direct_space_state.intersect_shape(query, max_results)



## random rotation on three axis
static func random_basis(rng: RandomNumberGenerator) -> Basis:
    var ret: Basis = Basis.IDENTITY
    var _360d = deg_to_rad(360.0)
    ret = ret.rotated(Vector3.UP, rng.randf() * _360d)
    ret = ret.rotated(Vector3.RIGHT, rng.randf() * _360d)
    ret = ret.rotated(Vector3.FORWARD, rng.randf() * _360d)
    return ret

## random Vector3D any direction (normalized)
static func random_vector(rng: RandomNumberGenerator = null) -> Vector3:
    var ret: Vector3 = Vector3.FORWARD # start with -z
    var _360d = deg_to_rad(360.0)
    
    if rng:
        ret = ret.rotated(Vector3.RIGHT, rng.randf() * _360d) # tilt
        ret = ret.rotated(Vector3.UP, rng.randf() * _360d) # rotate by pole
    else:
        ret = ret.rotated(Vector3.RIGHT, randf() * _360d) # tilt
        ret = ret.rotated(Vector3.UP, randf() * _360d) # rotate by pole
    return ret
    

## random position in a range
static func random_position(range: Vector3 = Vector3(32,1,32)) -> Vector3:
    return Vector3(range.x * randf(), range.y * randf(), range.z * randf())

## random normal position
static func random_normal_distribution_vector3(rng: RandomNumberGenerator = null) -> Vector3:
    if not rng:
        return Vector3(randfn(0.0, 1.0), randfn(0.0, 1.0), randfn(0.0, 1.0))
    else:
        return Vector3(rng.randfn(0.0, 1.0), rng.randfn(0.0, 1.0), rng.randfn(0.0, 1.0))
        
        

## folded normal with an average output of 1
## the folded normal only returns positive numbers
## the constant used is (2.0/pi)**0.5
##
## good for delays like lightning
##
static func folded_normal(normal: float = randfn(0.0, 1.0)) -> float:    
    return abs(normal) / 0.7978845608028654



## ported from chatgp generated python
## https://chatgpt.com/c/19b23630-655c-4827-88ac-f378b8da464b
## find the nearest point on a line to a point (usually not on the line)
##
## used to figure out which direction to move in dodging a bullet
## as for example it can work out how close a projectile following a straight path will come to our unit
## (line direction does not need to be normalized)
##
static func nearest_point_on_line(point_on_line: Vector3, line_direction: Vector3, point: Vector3) -> Vector3:
        
    var x0: float = point_on_line.x
    var y0: float = point_on_line.y
    var z0: float = point_on_line.z
    
    var a: float = line_direction.x
    var b: float = line_direction.y
    var c: float = line_direction.z
    
    var x1: float = point.x
    var y1: float = point.y
    var z1: float = point.z
    
    # Calculate the dot products
    var dot_dQ: float = a * (x1 - x0) + b * (y1 - y0) + c * (z1 - z0)
    var dot_dd: float = a * a + b * b + c * c

    # Calculate the parameter t for the nearest point
    var t: float = dot_dQ / dot_dd

    # Calculate the nearest point using the parametric equation of the line
    var Px: float = x0 + t * a
    var Py: float = y0 + t * b
    var Pz: float = z0 + t * c

    return Vector3(Px, Py, Pz)


## connect signal checking without throwing an already connected error
##
## example (with persist)
## connect_signal(Button.pressed, _on_button_pressed, CONNECT_PERSIST)
##
## example (with bind):
## connect_signal(Button.pressed, _on_button_pressed.bind(1234))
##
## flags:
## https://docs.godotengine.org/en/stable/classes/class_object.html#enum-object-connectflags
##
static func connect_signal(from: Signal, to: Callable, flags: int = 0) -> void:
    if not from.is_connected(to):
        from.connect(to, flags)
    
        
        



## kept also as notes
static func get_standard_material() -> StandardMaterial3D:
    
    var material: StandardMaterial3D = StandardMaterial3D.new()
    material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA # use alpha
    material.vertex_color_use_as_albedo = true # make colors work
    
    return material







## hard limit a vector, means it's length cannot exceed a certain length
## the ignore will ignore that axis, ie ignore y to ignore the gravity velocity
static func hard_limit_vector(
    vector: Vector3,
    limit: float,
    ignore_x: bool = false,
    ignore_y: bool = false,
    ignore_z: bool = false
    ) -> Vector3:
        
    # create return vector
    var new_vector: Vector3 = vector
    
    # 0 the ignored axes
    if ignore_x: new_vector.x = 0.0 
    if ignore_y: new_vector.y = 0.0
    if ignore_z: new_vector.z = 0.0
    
    # if longer than limit set to limit
    if new_vector.length() > limit: new_vector = new_vector.normalized() * limit 
    
    # put back ignored axes
    if ignore_x: new_vector.x = vector.x 
    if ignore_y: new_vector.y = vector.y
    if ignore_z: new_vector.z = vector.z
        
    return new_vector
    



#static func vector2_deg_to_rad(input: Vector2):
    #return Vector2(deg_to_rad(input.x), deg_to_rad(input.y))
#
#static func vector2_rad_to_deg(input: Vector2):
    #return Vector2(rad_to_deg(input.x), rad_to_deg(input.y))
#
#static func vector3_deg_to_rad(input: Vector3):
    #return Vector3(deg_to_rad(input.x), deg_to_rad(input.y), deg_to_rad(input.z))
#
#static func vector3_rad_to_deg(input: Vector3):
    #return Vector3(rad_to_deg(input.x), rad_to_deg(input.y), rad_to_deg(input.z))



static func _deg_to_rad(input):
    if input is Vector3:
        return Vector3(deg_to_rad(input.x), deg_to_rad(input.y), deg_to_rad(input.z))
    elif input is Vector2:
        return Vector2(deg_to_rad(input.x), deg_to_rad(input.y))
    elif input is float or input is int:
        return deg_to_rad(input)
    else:
        push_error("unrecognized type!")
        
static func _rad_to_deg(input):
    if input is Vector3:
        return Vector3(rad_to_deg(input.x), rad_to_deg(input.y), rad_to_deg(input.z))
    elif input is Vector2:
        return Vector2(rad_to_deg(input.x), rad_to_deg(input.y))
    elif input is float or input is int:
        return rad_to_deg(input)
    else:
        push_error("unrecognized type!")



# like blender nodes
func map_range(input, from_min, from_max, to_min, to_max):
    
    pass
    
    
    



## get all the filenames in a path
## boilerplate to avoid writing all this walking junk, creatin an Array is not costly for this scenario
##
## keywords:
## directory_contents
## path_contents
## folder_contents
static func dir_contents(path: String, files = true, folders = false) -> PackedStringArray:
    var filenames := PackedStringArray()
    var dir = DirAccess.open(path)
    if dir:
        dir.list_dir_begin()
        var filename = dir.get_next()
        while filename != "":
            if dir.current_is_dir():
                if folders:
                    filenames.append(filename)
            else:
                if files:
                    filenames.append(filename)
            filename = dir.get_next()
    else:
        #print("An error occurred when trying to access the path.")
        pass # well ret will just be empty!
    return filenames

"""

hooks to c++ functions

relies on the HArrayMesh and CStaticLibrary


"""
@tool
extends MeshInstance3D


@export var heightmap: Texture2D

@export var heightmap_processed: Texture2D
@export var normal_map_out: Texture2D





func _build_mesh():

    
    var clib := CStaticLibrary.new()
    
    if is_instance_valid(heightmap):
        
        var image := heightmap.get_image()
        var image_size := image.get_size()
        var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
        
        
        floats = clib.blur_image(floats, image_size, normal_map_blur, false)
        
        
        var image2 := clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
        heightmap_processed = ImageTexture.create_from_image(image2) 
        
    
        if is_instance_valid(mesh) and mesh is HArrayMesh:
            
            var harray: HArrayMesh = mesh
            
            harray.heightmap = heightmap_processed
            harray.generate_normals = false
            harray.trigger_update = true



@export var normal_map_blur = 1.0
@export var normal_map_strength = 1.0

@export var erode_iterations = 100



func _generate_normal_map():
    
    var clib := CStaticLibrary.new()
    if is_instance_valid(heightmap):
        var image := heightmap.get_image()
        var image_size := image.get_size()
        var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
        
        floats = clib.blur_image(floats, image_size, normal_map_blur, false) # blur
        
        
        
        
        
        
        
        
        #var normals := clib.generate_normal_map(floats, image_size, 2.0)
        var normals := clib.generate_normal_map(floats, image_size, normal_map_strength, false)
        #
        var normal_map := clib.colors_to_image(normals, image_size)
        #
        normal_map_out = ImageTexture.create_from_image(normal_map)
        
        if is_instance_valid(mesh) and mesh is HArrayMesh:
            var harray: HArrayMesh = mesh
            
            var material = get_surface_override_material(0)
            if is_instance_valid(material) and material is StandardMaterial3D:
                
                var smaterial: StandardMaterial3D = material
                
                smaterial.normal_enabled = true
                smaterial.normal_texture = normal_map_out
                
                


func macro_build_mesh():
    #_generate_normal_map()
    _build_mesh()
    
    

func macro_generate_normal_map():
    _generate_normal_map()
    
        
        
    
    

func macro_test_erosion():
    
                
    var clib := CStaticLibrary.new()
    var erosion:= HydraulicErosion.new()
    
    if is_instance_valid(heightmap):
        var image := heightmap.get_image()
        var image_size := image.get_size()
        var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
        
        #floats = clib.blur_image(floats, image_size, normal_map_blur, false) # blur
        
        floats = erosion.erode(floats, image_size.x, erode_iterations)
        
        
        var image2 = clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
        heightmap_processed = ImageTexture.create_from_image(image2)
        
        
        
        
        
        
    #if is_instance_valid(mesh) and mesh is HArrayMesh:    
        #var harray: HArrayMesh = mesh
        #
        #if is_instance_valid(heightmap_processed):
            #harray.heightmap = heightmap_processed
        #
        #harray._update()
            #
            ##heightmap_processed = Texture2D.
            #
            ##harray.heightmap
            #
            #
        #
        ##CPPStaticLibrary.new().im
        
        
    
    
    
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
    pass

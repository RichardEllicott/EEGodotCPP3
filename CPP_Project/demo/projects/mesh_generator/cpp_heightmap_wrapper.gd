"""

hooks to c++ functions

relies on the HArrayMesh and CStaticLibrary


"""
@tool
extends MeshInstance3D

@export var heightmap: Texture2D

@export var heightmap_processed: Texture2D
@export var normal_map_out: Texture2D

@export var noise_texture: NoiseTexture2D

func macro_generate_noise_texture():
    
    var texture := NoiseTexture2D.new()
    texture.seamless = true
    texture.noise = FastNoiseLite.new()
    await texture.changed
    var image := texture.get_image()
    #var data := image.get_data()
    heightmap = ImageTexture.create_from_image(image)
    


func _build_mesh(heightmap: Texture2D):
    
    if not is_instance_valid(mesh):
        mesh = HArrayMesh.new()
            
    #if is_instance_valid(mesh) and mesh is HArrayMesh:
    var harray: HArrayMesh = mesh
    
    harray.heightmap = heightmap
    #harray.generate_normals = false
    harray.trigger_update = true


func blur_image(image_texture: ImageTexture, blur_strength: float) -> ImageTexture:
    
    var clib := CStaticLibrary.new()
    
    var image := heightmap.get_image()
    var image_size := image.get_size()
    var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
    floats = clib.blur_image(floats, image_size, blur_strength, false)
    var image2 := clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
    
    var ret :=  ImageTexture.create_from_image(image2)
    return ret





func macro_build_mesh_blured():
    
    var clib := CStaticLibrary.new()
    
    #heightmap_processed = heightmap
    
    #var image := heightmap.get_image()
    #var image_size := image.get_size()
    #var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
    #floats = clib.blur_image(floats, image_size, normal_map_blur, false)
    #var image2 := clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
    #heightmap_processed = ImageTexture.create_from_image(image2) 
    heightmap_processed = blur_image(heightmap, normal_map_blur)
    _build_mesh(heightmap_processed)


func macro_build_mesh():
    _build_mesh(heightmap)
    
    
    
    
    #var clib := CStaticLibrary.new()
    #
    #if is_instance_valid(heightmap):
        #
        #
        #
        #var image := heightmap.get_image()
        #var image_size := image.get_size()
        #var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
        #floats = clib.blur_image(floats, image_size, normal_map_blur, false)
        #
        #var image2 := clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
        #heightmap_processed = ImageTexture.create_from_image(image2) 
        #
    #
        #if is_instance_valid(mesh) and mesh is HArrayMesh:
            #
            #var harray: HArrayMesh = mesh
            #
            #harray.heightmap = heightmap_processed
            #harray.generate_normals = false
            #harray.trigger_update = true



@export var normal_map_blur = 1.0
@export var normal_map_strength = 1.0

@export var erode_iterations = 100





func _generate_normal_map(heightmap: Texture2D) -> ImageTexture:
    
    var clib := CStaticLibrary.new()
    
    var image := heightmap.get_image()
    var image_size := image.get_size()
    var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
    
    if normal_map_blur > 0.0:
        floats = clib.blur_image(floats, image_size, normal_map_blur, false) # blur
    
    
    #var normals := clib.generate_normal_map(floats, image_size, 2.0)
    var normals := clib.generate_normal_map(floats, image_size, normal_map_strength, false)
    #
    var normal_map := clib.colors_to_image(normals, image_size)
    #
    return ImageTexture.create_from_image(normal_map)
        
        

func macro_generate_normal_map():
    normal_map_out = _generate_normal_map(heightmap_processed)
    
        


func _erode_heightmap(heightmap: ImageTexture) -> ImageTexture:
    
    var clib := CStaticLibrary.new()
    var erosion:= HydraulicErosion.new()
    
    var image := heightmap.get_image()
    var image_size := image.get_size()
    var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
    
    #floats = clib.blur_image(floats, image_size, normal_map_blur, false) # blur
    
    floats = erosion.erode(floats, image_size.x, erode_iterations)
    
    var image2 = clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
    return ImageTexture.create_from_image(image2)
    
     
    

func macro_test_erosion():
    
    heightmap_processed = _erode_heightmap(heightmap)
    
                
    #var clib := CStaticLibrary.new()
    #var erosion:= HydraulicErosion.new()
    #
    #if is_instance_valid(heightmap):
        #var image := heightmap.get_image()
        #var image_size := image.get_size()
        #var floats := clib.image_channel_to_floats(image, 0) # convert red to float array
        #
        ##floats = clib.blur_image(floats, image_size, normal_map_blur, false) # blur
        #
        #floats = erosion.erode(floats, image_size.x, erode_iterations)
        #
        #
        #var image2 = clib.floats_to_image(floats, image_size, Image.FORMAT_L8)
        #heightmap_processed = ImageTexture.create_from_image(image2)
        
        
        
        
        
        
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
        
        

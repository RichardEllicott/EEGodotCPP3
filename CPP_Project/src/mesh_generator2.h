#ifndef MESH_GENERATOR2_H
#define MESH_GENERATOR2_H

#include <helper.h>  // includes a print function
#include <macros.h>  // my macros to help declare properties

// #include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/noise_texture2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>


// #include <std>

#include <godot_cpp/classes/mesh_instance3d.hpp>

using namespace godot;

// struct QuadRef {
//     int a;
//     int b;
//     int c;
//     int d;

//     // Constructor
//     QuadRef(int _a, int _b, int _c, int _d)
//         : a(_a), b(_b), c(_c), d(_d) {}
// };

class MeshGenerator2 : public Node3D {
    GDCLASS(MeshGenerator2, Node3D)

    // we need to also add two more lines to the cpp file per a property we want to @export

    // DECLARE_PROPERTY(bool, enabled) // variants don't need the Ref<> syntax
    // DECLARE_PROPERTY(float, speed)

    // DECLARE_PROPERTY(Ref<Texture2D>, texture2d) // note the Ref type is correct for this pattern, the ref will delete it's pointer automaticly

    DECLARE_PROPERTY(PackedVector3Array, verts)
    DECLARE_PROPERTY(PackedVector2Array, uvs)
    DECLARE_PROPERTY(PackedVector3Array, normals)
    DECLARE_PROPERTY(PackedInt32Array, indices)
    DECLARE_PROPERTY(Array, surface_array)

    DECLARE_PROPERTY(Ref<Texture2D>, heightmap1)
    DECLARE_PROPERTY(Ref<Texture2D>, heightmap2)
    DECLARE_PROPERTY(Ref<Texture2D>, heightmap3)
    DECLARE_PROPERTY(Ref<Texture2D>, image_out)
    DECLARE_PROPERTY(Ref<Texture2D>, process_image)




    DECLARE_PROPERTY(float, heightmap1_scale)
    DECLARE_PROPERTY(float, heightmap2_scale)
    DECLARE_PROPERTY(float, heightmap3_scale)



    DECLARE_PROPERTY(Vector2i, grid_size)  // rem to initialize
    DECLARE_PROPERTY(Vector3, scale)       // rem to initialize
    DECLARE_PROPERTY(Vector2, uv_scale)    // rem to initialize
    DECLARE_PROPERTY(float, normal_scale)  // rem to initialize
    DECLARE_PROPERTY(float, normal_step)   // rem to initialize

    DECLARE_PROPERTY(float, blur_value)   // rem to initialize


    // std::vector<QuadRef> quads;
    std::vector<std::vector<int>> ngons;

   private:
   protected:
    static void _bind_methods();

   public:
    MeshGenerator2();
    ~MeshGenerator2();

    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;

    Ref<RandomNumberGenerator> rng;

    MeshInstance3D *mesh_instance3d = nullptr;  // set nullptr even though a header file! this avoids crashing (the alternative is setting in constructor)
    MeshInstance3D *get_mesh_instance3d();

    Ref<ArrayMesh> array_mesh;  // ref to a child (beware of circular refs)
    Ref<ArrayMesh> get_array_mesh();

    void clear();  // clear cache

    void generate_mesh();  // generate the final mesh, and clear the cache

    float get_terrain_height(Vector2 position); // get height of terrain (from texture2d)

    Vector3 get_terrain_normal(Vector2 position); // get normal using the height function, step should be a small fraction

    void add_terrain();  // add terrain data (to internal arrays)

    void _generate_surface_array();  // generate the mesh (in the child MeshInstance3D with an ArrayMesh)

    void macro_test();  // register me in the bind

    void macro_test_blur();


    void macro_generate_terrain();  // register me in the bind

};

#endif
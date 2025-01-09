#ifndef MESH_GENERATOR2_H
#define MESH_GENERATOR2_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

// #include <godot_cpp/classes/sprite2d.hpp>
// #include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

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
    // DECLARE_PROPERTY(Vector2i, grid_size)

    // DECLARE_PROPERTY(Ref<Texture2D>, texture2d) // note the Ref type is correct for this pattern, the ref will delete it's pointer automaticly

    DECLARE_PROPERTY(PackedVector3Array, verts)
    DECLARE_PROPERTY(PackedVector2Array, uvs)
    DECLARE_PROPERTY(PackedVector3Array, normals)
    DECLARE_PROPERTY(PackedInt32Array, indices)
    DECLARE_PROPERTY(Array, surface_array)

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

    float get_terrain_height(Vector2 position);

    void add_terrain();


    void _generate_surface_array();
};

#endif
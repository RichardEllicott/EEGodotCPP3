#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

// #include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>

#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/height_map_shape3d.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/immediate_mesh.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

class MeshGenerator : public Node3D
{
    GDCLASS(MeshGenerator, Node3D)

#pragma region GDCLASS_EXPAND // not yet used binding

#pragma endregion

    // MACROS from macros.h
    DECLARE_PROPERTY(bool, enabled) // we need to also add two more lines to the cpp file per a property we want to @export
    DECLARE_PROPERTY(float, height)
    DECLARE_PROPERTY(Vector2i, grid_size)


    DECLARE_PROPERTY(NodePath, node_path)


#pragma region Texture2DExport // not yet used binding

    DECLARE_PROPERTY(Ref<Texture2D>, texture2d)

// private:
//     Ref<Texture2D> texture2d;

// public:
//     void set_texture2d(const Ref<Texture2D> &p_texture2d);
//     Ref<Texture2D> get_texture2d() const;
#pragma endregion

private:
protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;

    void add_quad();
    void add_terrain();

public:
    PackedVector3Array verts;
    PackedVector2Array uvs;
    PackedVector3Array normals;
    PackedInt32Array indices;
    Array surface_array;

    // PackedVector3Array verts = PackedVector3Array();
    // PackedVector2Array uvs= PackedVector2Array();
    // PackedVector3Array normals= PackedVector3Array();
    // PackedInt32Array indices= PackedInt32Array();
    // Array surface_array= Array();

    void clear()
    {
        verts.clear();
        uvs.clear();
        normals.clear();
        indices.clear();

        surface_array.resize(Mesh::ARRAY_MAX);
        surface_array[Mesh::ARRAY_VERTEX] = verts;
        surface_array[Mesh::ARRAY_TEX_UV] = uvs;
        surface_array[Mesh::ARRAY_NORMAL] = normals;
        surface_array[Mesh::ARRAY_INDEX] = indices;
    }

    // quick shortcut to get MeshInstance3D pointer
    // note we cannot use Ref<MeshInstance3D> as these are Nodes and don't use the same system
    // we are forced to get a pointer back
    //
    // WARNING: this code has low memory saftey if node goes missing! (it will probabally crash)
    //
    MeshInstance3D *mesh_instance3d = nullptr; // setting nullptr here seems to avoid crash, i think either here of the deconstructor is required

    MeshInstance3D *get_mesh_instance3d() // returns NULL if not valid (does cause errors, it's hard to call get_node_or_null, takes longer)
    {

        // TEST WITHOUT THIS.... i think it causes a bug maybe when i reload the same scene
        if (mesh_instance3d == nullptr)                                   // note i keep both NULL and nullptr due to some unexpected behaviours
            mesh_instance3d = get_node<MeshInstance3D>("MeshInstance3D"); // note "get_node_or_null" won't work

        return mesh_instance3d;
    }

    //  Ref<ArrayMesh> mesh_ref = mesh_instance3d->get_mesh(); // i think this may crash

    Ref<ArrayMesh> array_mesh = nullptr;
    Ref<ArrayMesh> get_array_mesh()
    {
        if (array_mesh == nullptr)
        {
            mesh_instance3d = get_mesh_instance3d();
            if (mesh_instance3d != nullptr)
                array_mesh = mesh_instance3d->get_mesh();
        }
        return array_mesh;
    }

    Ref<ImmediateMesh> intermediate_mesh = nullptr;
    Ref<ImmediateMesh> get_intermediate_mesh()
    {
        if (intermediate_mesh == nullptr)
        {
            mesh_instance3d = get_mesh_instance3d();
            if (mesh_instance3d != nullptr)
                intermediate_mesh = mesh_instance3d->get_mesh();
        }
        return intermediate_mesh;
    }

    void _exit_tree() override
    {
        // mesh_instance3d = nullptr; // Reset the pointer ... PREVENTS A CRASH VERY IMPORTANT!!
        // mesh_instance3d = NULL;    // Reset the pointer ... PREVENTS A CRASH VERY IMPORTANT!!
    }

    StaticBody3D *static_body3d = nullptr;

    StaticBody3D *get_static_body3d()
    {
        if (static_body3d == nullptr)                               // raw version
            static_body3d = get_node<StaticBody3D>("StaticBody3D"); // note "get_node_or_null" won't work

        return static_body3d;
    }

    CollisionShape3D *collision_shape3d = nullptr;

    CollisionShape3D *get_collision_shape3d()
    {
        if (collision_shape3d == nullptr)
        {
            static_body3d = get_static_body3d();
            if (static_body3d != nullptr)
                collision_shape3d = static_body3d->get_node<CollisionShape3D>("CollisionShape3D");
        }
        return collision_shape3d;
    }

    Ref<HeightMapShape3D> height_map_shape3d = nullptr;

    Ref<HeightMapShape3D> get_height_map_shape3d()
    {
        if (height_map_shape3d == nullptr)
        {
            collision_shape3d = get_collision_shape3d();

            if (collision_shape3d != nullptr)
            {
                height_map_shape3d = collision_shape3d->get_shape();
            }
        }

        return height_map_shape3d;
    }

    // int position_to_ref(Vector2i size, Vector2i position)
    // {
    //     return (position.x + position.y * size.x) % (size.x * size.y);
    // }

    // safer according to chatgp, will test
    int position_to_ref(Vector2i size, Vector2i position)
    {
        int ref = position.x + position.y * size.x;
        return (ref % (size.x * size.y) + (size.x * size.y)) % (size.x * size.y);
    }

    Vector2i ref_to_position(Vector2i size, int ref)
    {
        int x = ref % size.x; // Compute the x-coordinate (column)
        int y = ref / size.x; // Compute the y-coordinate (row)
        return Vector2i(x, y);
    }

    Ref<RandomNumberGenerator> rng;

    MeshGenerator()
    {
        clear();

        rng.instantiate();
        // rng->randf();
        rng->set_seed(0);

        // mesh_instance3d = NULL;
        grid_size = Vector2i(8, 8);
        height = 1.0;
    };
    ~MeshGenerator()
    {
        // it seems safest to clear the pointers here, avoiding a crash with the editor
        mesh_instance3d = nullptr; // safest pattern, clearing on exit
        static_body3d = nullptr;   // safest pattern, clearing on exit
        collision_shape3d = nullptr;

        array_mesh.unref(); // i don't think i need this, because it should be automatic
        height_map_shape3d.unref();
    };
};

#endif
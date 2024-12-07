#ifndef MESH_GENERATOR_CPP
#define MESH_GENERATOR_CPP

#include <mesh_generator.h>

#include <macros.h>
#include <helper.h>

// chat gp
#include <godot_cpp/classes/array_mesh.hpp>
// #include <godot_cpp/classes/packed_vector2_array.hpp>
// #include <godot_cpp/classes/packed_vector3_array.hpp>
// #include <godot_cpp/classes/packed_int32_array.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

// MeshGenerator::MeshGenerator()
// {
//     enabled = false;
//     speed = 35.7;
// }

// MeshGenerator::~MeshGenerator()
// {
// }

void MeshGenerator::_process(double delta)
{
}

// void MeshGenerator::_draw()
// {
// }

void MeshGenerator::_physics_process(double delta)
{
}

void MeshGenerator::add_quad()
{
    verts.push_back(Vector3(-1, 0, -1));
    verts.push_back(Vector3(1, 0, -1));
    verts.push_back(Vector3(1, 0, 1));
    verts.push_back(Vector3(-1, 0, 1));

    uvs.push_back(Vector2(0, 0));
    uvs.push_back(Vector2(1, 0));
    uvs.push_back(Vector2(1, 1));
    uvs.push_back(Vector2(0, 1));

    normals.push_back(Vector3(0, 1, 0));
    normals.push_back(Vector3(0, 1, 0));
    normals.push_back(Vector3(0, 1, 0));
    normals.push_back(Vector3(0, 1, 0));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);
}

void MeshGenerator::add_terrain()
{

    // heights
    for (int y = 0; y < grid_size.y; y++)
    {
        for (int x = 0; x < grid_size.x; x++)
        {
            float _height = rng->randf() * height;

            // int ref_0 = (x + 0) + (y + 0) * grid_size.x;
            // int ref_1 = (x + 1) %  + (y + 0) * grid_size.x;

            verts.push_back(Vector3(x, _height, y));
            uvs.push_back(Vector2(x, y));
        }
    }

    // normals
    for (int y = 0; y < grid_size.y; y++)
    {
        for (int x = 0; x < grid_size.x; x++)
        {
            normals.push_back(Vector3(0, 1, 0));

            if (x < grid_size.x - 1 && y < grid_size.y - 1)
            {
                int ref0 = position_to_ref(grid_size, Vector2i(x + 0, y + 0));
                int ref1 = position_to_ref(grid_size, Vector2i(x + 1, y + 0));
                int ref2 = position_to_ref(grid_size, Vector2i(x + 1, y + 1));
                int ref3 = position_to_ref(grid_size, Vector2i(x + 0, y + 1));

                float height0 = verts[ref0].y;
                float height1 = verts[ref1].y;
                float height2 = verts[ref2].y;
                float height3 = verts[ref3].y;
            }
        }
    }

    // quads
    for (int y = 0; y < grid_size.y - 1; y++)
    {
        for (int x = 0; x < grid_size.x - 1; x++)
        {
            int ref_0 = (x + 0) + (y + 0) * grid_size.x;
            int ref_1 = (x + 1) + (y + 0) * grid_size.x;
            int ref_2 = (x + 1) + (y + 1) * grid_size.x;
            int ref_3 = (x + 0) + (y + 1) * grid_size.x;

            indices.push_back(ref_0);
            indices.push_back(ref_1);
            indices.push_back(ref_2);

            indices.push_back(ref_0);
            indices.push_back(ref_2);
            indices.push_back(ref_3);
        }
    }
}

void MeshGenerator::_ready()
{

    clear();

    array_mesh = get_array_mesh(); // update the cache

    if (array_mesh != nullptr && array_mesh.is_valid())
    {

        // add_quad();

        add_terrain();

        // PackedVector3Array verts;
        // verts.push_back(Vector3(-1, 0, -1));
        // verts.push_back(Vector3(1, 0, -1));
        // verts.push_back(Vector3(1, 0, 1));
        // verts.push_back(Vector3(-1, 0, 1));

        // PackedVector2Array uvs;
        // uvs.push_back(Vector2(0, 0));
        // uvs.push_back(Vector2(1, 0));
        // uvs.push_back(Vector2(1, 1));
        // uvs.push_back(Vector2(0, 1));

        // PackedVector3Array normals;
        // normals.push_back(Vector3(0, 1, 0));
        // normals.push_back(Vector3(0, 1, 0));
        // normals.push_back(Vector3(0, 1, 0));
        // normals.push_back(Vector3(0, 1, 0));

        // PackedInt32Array indices;
        // indices.push_back(0);
        // indices.push_back(1);
        // indices.push_back(2);
        // indices.push_back(0);
        // indices.push_back(2);
        // indices.push_back(3);

        // TRYING TO REMOVE .................UNCOMMENT TO WORK
        // we need this because the PackedVector3Array is a value type i think
        surface_array.resize(Mesh::ARRAY_MAX);
        surface_array[Mesh::ARRAY_VERTEX] = verts;
        surface_array[Mesh::ARRAY_TEX_UV] = uvs;
        surface_array[Mesh::ARRAY_NORMAL] = normals;
        surface_array[Mesh::ARRAY_INDEX] = indices;

        // Create and populate the mesh
        // Ref<ArrayMesh> mesh = memnew(ArrayMesh);
        // mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

        print("found mesh_ref!"); // note we crash though running twice
        array_mesh->clear_surfaces();
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
    }

    if (get_static_body3d() != nullptr)
    {
        print("get_static_body3d() != nullptr");
    }

    if (get_collision_shape3d() != nullptr)
    {
        print("get_collision_shape3d() != nullptr");
    }

    if (get_height_map_shape3d() != nullptr && get_height_map_shape3d().is_valid())
    {
        print("get_height_map_shape3d() != nullptr && get_height_map_shape3d().is_valid()");
    }

    if (get_intermediate_mesh() != nullptr && get_intermediate_mesh().is_valid())
    {
        print("get_intermediate_mesh() != nullptr && get_intermediate_mesh().is_valid()");
    }
}


#pragma region CREATE_GET_SET_MACROS

CREATE_GETTER_SETTER(MeshGenerator, bool, enabled)
CREATE_GETTER_SETTER(MeshGenerator, float, height)
CREATE_GETTER_SETTER(MeshGenerator, Vector2i, grid_size)

CREATE_GETTER_SETTER(MeshGenerator, Ref<Texture2D>, texture2d)

CREATE_GETTER_SETTER(MeshGenerator, NodePath, node_path)

#pragma endregion



void MeshGenerator::_bind_methods()
{
    // to find variants with autotype, use the Variant::
    // Variant::PACKED_VECTOR2_ARRAY


#pragma region VARIANT_BINDINGS
    CREATE_CLASSDB_BINDINGS(MeshGenerator, BOOL, enabled)
    CREATE_CLASSDB_BINDINGS(MeshGenerator, FLOAT, height)
    CREATE_CLASSDB_BINDINGS(MeshGenerator, VECTOR2I, grid_size)

    CREATE_CLASSDB_BINDINGS(MeshGenerator, NODE_PATH, node_path)

#pragma endregion

#pragma region REFERENCE_BINDINGS // generated this with chat gp!

    CREATE_CLASSDB_BINDINGS2(MeshGenerator, "Texture2D", texture2d); // note different macro required for ref types (Texture2D, AudioStream etc)


    // ClassDB::bind_method(D_METHOD("set_texture2d", "texture2d"), &MeshGenerator::set_texture2d);
    // ClassDB::bind_method(D_METHOD("get_texture2d"), &MeshGenerator::get_texture2d);
    // ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture2d", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture2d", "get_texture2d");

#pragma endregion
}

#endif
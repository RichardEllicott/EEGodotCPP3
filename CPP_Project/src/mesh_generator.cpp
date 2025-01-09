#ifndef MESH_GENERATOR_CPP
#define MESH_GENERATOR_CPP

#include <mesh_generator.h>


using namespace godot;



void MeshGenerator::_process(double delta) {
}

void MeshGenerator::_physics_process(double delta) {
}

void MeshGenerator::add_quad() {
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

float MeshGenerator::get_terrain_height(Vector2 position) {
    // auto image = texture2d->get_image();

    // if (image != nullptr) {
    //     auto color = sample_image(image, position);
    //     return color.r;
    // }

    return 0.0f;
}






void MeshGenerator::add_terrain() {
    Vector2 _lerp;

    // heights
    for (int y = 0; y < grid_size.y; y++) {
        _lerp.y = float(y) / float(grid_size.y - 1);

        for (int x = 0; x < grid_size.x; x++) {
            _lerp.x = float(x) / float(grid_size.x - 1);

            float _height = get_terrain_height(_lerp);
            // float _height = rng->randf() * height;

            // int ref_0 = (x + 0) + (y + 0) * grid_size.x;
            // int ref_1 = (x + 1) %  + (y + 0) * grid_size.x;

            verts.push_back(Vector3(x, _height, y));
            uvs.push_back(Vector2(x, y));
            
        }
    }

    // normals
    for (int y = 0; y < grid_size.y; y++) {
        for (int x = 0; x < grid_size.x; x++) {
            normals.push_back(Vector3(0, 1, 0));  // unfinished

            if (x < grid_size.x - 1 && y < grid_size.y - 1) {
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
    for (int y = 0; y < grid_size.y - 1; y++) {
        for (int x = 0; x < grid_size.x - 1; x++) {
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

void MeshGenerator::_ready() {
    
    clear();
    add_terrain();
    generate_mesh();

}

#pragma region CREATE_GET_SET_MACROS

CREATE_GETTER_SETTER(MeshGenerator, bool, enabled)
CREATE_GETTER_SETTER(MeshGenerator, float, height)
CREATE_GETTER_SETTER(MeshGenerator, Vector2i, grid_size)

CREATE_GETTER_SETTER(MeshGenerator, Ref<Texture2D>, texture2d)

CREATE_GETTER_SETTER(MeshGenerator, NodePath, node_path)

#pragma endregion

void MeshGenerator::_bind_methods() {
    // to find variants with autotype, use the Variant::
    // Variant::PACKED_VECTOR2_ARRAY

#pragma region VARIANT_BINDINGS
    CREATE_VAR_BINDINGS(MeshGenerator, BOOL, enabled)
    CREATE_VAR_BINDINGS(MeshGenerator, FLOAT, height)
    CREATE_VAR_BINDINGS(MeshGenerator, VECTOR2I, grid_size)

    CREATE_VAR_BINDINGS(MeshGenerator, NODE_PATH, node_path)

#pragma endregion

#pragma region REFERENCE_BINDINGS // generated this with chat gp!

    CREATE_CLASS_BINDINGS(MeshGenerator, "Texture2D", texture2d);  // note different macro required for ref types (Texture2D, AudioStream etc)

    // ClassDB::bind_method(D_METHOD("set_texture2d", "texture2d"), &MeshGenerator::set_texture2d);
    // ClassDB::bind_method(D_METHOD("get_texture2d"), &MeshGenerator::get_texture2d);
    // ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture2d", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture2d", "get_texture2d");

#pragma endregion
}

#endif
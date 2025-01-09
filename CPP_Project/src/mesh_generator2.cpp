#ifndef MESH_GENERATOR2_CPP
#define MESH_GENERATOR2_CPP

// #include <mesh_generator/mesh_generator2.h>
#include <mesh_generator2.h>

using namespace godot;

MeshGenerator2::MeshGenerator2() {
    // enabled = false;
    // speed = 35.7;

    print("hello from MeshGenerator2!");

    rng.instantiate();
    // rng->randf();
    rng->set_seed(0);
}

MeshGenerator2::~MeshGenerator2() {
}

void MeshGenerator2::_ready() {
    add_terrain();
    generate_mesh();  // we need to call in ready, as then the children will be in the tree
    // emit_signal("test_signal", this, Vector2(3, 7));
}

void MeshGenerator2::_process(double delta) {
}

void MeshGenerator2::_physics_process(double delta) {
}

MeshInstance3D* MeshGenerator2::get_mesh_instance3d()  // returns NULL if not valid (does cause errors, it's hard to call get_node_or_null, takes longer)
{
    // TEST WITHOUT THIS.... i think it causes a bug maybe when i reload the same scene
    if (mesh_instance3d == nullptr) {  // note we might still have a dangling pointer if we interfere with the mesh object
        // mesh_instance3d = get_node<MeshInstance3D>("MeshInstance3D");           // note "get_node_or_null" won't work
        mesh_instance3d = get_node_as<MeshInstance3D>(this, "MeshInstance3D");  // alt using my template??
    }
    return mesh_instance3d;
}

Ref<ArrayMesh> MeshGenerator2::get_array_mesh() {
    if (array_mesh == nullptr) {
        mesh_instance3d = get_mesh_instance3d();
        if (mesh_instance3d != nullptr)
            array_mesh = mesh_instance3d->get_mesh();
    }
    return array_mesh;
}

void MeshGenerator2::clear() {
    verts.clear();
    uvs.clear();
    normals.clear();
    indices.clear();
    surface_array.clear();
    // quads.clear();
    ngons.clear();
}

void MeshGenerator2::_generate_surface_array() {
    // // turn the quads into triangles
    // for (const auto& quad : quads) {
    //     // abc, acd

    //     // triangle 1
    //     indices.push_back(quad.a);
    //     indices.push_back(quad.b);
    //     indices.push_back(quad.c);
    //     // triangle 2
    //     indices.push_back(quad.a);
    //     indices.push_back(quad.c);
    //     indices.push_back(quad.d);
    // }
    // quads.clear();

    // turn ngons into triangles
    for (const auto& ngon : ngons) {
        if (ngon.size() == 4) {  // quad
            // triangle 1
            indices.push_back(ngon[0]);
            indices.push_back(ngon[1]);
            indices.push_back(ngon[2]);

            // indices.append_array();


            // triangle 2 acd
            indices.push_back(ngon[0]);
            indices.push_back(ngon[2]);
            indices.push_back(ngon[3]);
        } else if (ngon.size() == 3) {  // triangle
            // triangle 1
            indices.push_back(ngon[0]);
            indices.push_back(ngon[1]);
            indices.push_back(ngon[2]);
        } else if (ngon.size() >= 4) { // larger ngon
        }
    }
    ngons.clear();

    // we need to copy the arrays over as they are value types
    surface_array.resize(Mesh::ARRAY_MAX);
    surface_array[Mesh::ARRAY_VERTEX] = verts;
    surface_array[Mesh::ARRAY_TEX_UV] = uvs;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_INDEX] = indices;
}

void MeshGenerator2::generate_mesh() {
    array_mesh = get_array_mesh();  // update the cache

    _generate_surface_array();

    if (array_mesh != nullptr && array_mesh.is_valid()) {
        array_mesh->clear_surfaces();
        array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
    }

    clear();
}

float MeshGenerator2::get_terrain_height(Vector2 position) {
    return 0.0;
}

void MeshGenerator2::add_terrain() {
    Vector2i grid_size = Vector2i(32, 32);

    Vector2 _lerp;

    std::vector<Vector2> lerps;
    lerps.resize((grid_size.y - 1) * (grid_size.x - 1));

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

            normals.push_back(Vector3(0, 1, 0));  // unfinished (just left up)
        }
    }

    // quads cache (may enable subdiv)
    for (int y = 0; y < grid_size.y - 1; y++) {
        for (int x = 0; x < grid_size.x - 1; x++) {
            // quad positions
            int a = (x + 0) + (y + 0) * grid_size.x;
            int b = (x + 1) + (y + 0) * grid_size.x;
            int c = (x + 1) + (y + 1) * grid_size.x;
            int d = (x + 0) + (y + 1) * grid_size.x;

            // quads.push_back(QuadRef(a, b, c, d));
            ngons.push_back(std::vector<int>{a, b, c, d});  // new unused
        }
    }
}

// BINDINGS:
// macros from macros.h
// CREATE_GETTER_SETTER(MeshGenerator2, bool, enabled)
// CREATE_GETTER_SETTER(MeshGenerator2, float, speed)
// CREATE_GETTER_SETTER(MeshGenerator2, Vector2i, grid_size)
// CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, texture2d)

CREATE_GETTER_SETTER(MeshGenerator2, PackedVector3Array, verts)
CREATE_GETTER_SETTER(MeshGenerator2, PackedVector2Array, uvs)
CREATE_GETTER_SETTER(MeshGenerator2, PackedVector3Array, normals)
CREATE_GETTER_SETTER(MeshGenerator2, PackedInt32Array, indices)
CREATE_GETTER_SETTER(MeshGenerator2, Array, surface_array)

void MeshGenerator2::_bind_methods() {
    // macros from macros.h
    // CREATE_VAR_BINDINGS(MeshGenerator2, BOOL, enabled)
    // // CREATE_CLASSDB_BINDINGS2(Template, "bool", enabled) // tested alt macro? CRASHED!!!
    // CREATE_VAR_BINDINGS(MeshGenerator2, FLOAT, speed)
    // CREATE_VAR_BINDINGS(MeshGenerator2, VECTOR2I, grid_size)

    // CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", texture2d)

    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR3_ARRAY, verts)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR2_ARRAY, uvs)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR3_ARRAY, normals)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_INT32_ARRAY, indices)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::ARRAY, surface_array)
}

#endif
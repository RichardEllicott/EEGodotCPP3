#ifndef MESH_GENERATOR2_CPP
#define MESH_GENERATOR2_CPP

// #include <mesh_generator/mesh_generator2.h>
#include <mesh_generator2.h>

using namespace godot;

MeshGenerator2::MeshGenerator2() {
    // init vars
    grid_size = Vector2i(16, 16);
    scale = Vector3(1.0f, 1.0f, 1.0f);
    uv_scale = Vector2(1, 1);
    normal_scale = 1.0f;
    normal_step = 1.0f / 32.0f;
    blur_value = 2.0f;

    heightmap1_scale = 1.0f;
    heightmap2_scale = 1.0f;
    heightmap3_scale = 1.0f;

    // init rng
    rng.instantiate();
    rng->set_seed(0);
}

MeshGenerator2::~MeshGenerator2() {
}

void MeshGenerator2::_ready() {
    clear();
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
    // clear all arrays
    verts.clear();
    uvs.clear();
    normals.clear();
    indices.clear();
    surface_array.clear();
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
        } else if (ngon.size() >= 4) {  // larger ngon
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
    float sample = 0.0f;

    // 3 heightmaps read the same
    if (heightmap1.is_valid()) {
        auto image = heightmap1->get_image();

        if (image.is_valid() && !image->is_compressed()) {
            auto _sample = ImageHelper::sample_image(image, position);
            sample += _sample.r * heightmap1_scale;
        }
    }

    if (heightmap2.is_valid()) {
        auto image = heightmap2->get_image();

        if (image.is_valid() && !image->is_compressed()) {
            auto _sample = ImageHelper::sample_image(image, position);
            sample += _sample.r * heightmap2_scale;
        }
    }

    if (heightmap3.is_valid()) {
        auto image = heightmap3->get_image();

        if (image.is_valid() && !image->is_compressed()) {
            auto _sample = ImageHelper::sample_image(image, position);
            sample += _sample.r * heightmap3_scale;
        }
    }
    return sample;
    // Ref<NoiseTexture2D> noise_texture2d = texture2d;
    // if (noise_texture2d.is_valid()) {
    // }
}

// uncheked chatgp (can be errors sometimes)
Vector3 MeshGenerator2::get_terrain_normal(Vector2 position) {
    // Normal calculation function
    // Sample heights around the point
    float hL = get_terrain_height(Vector2(position.x - normal_step, position.y));  // Height to the left
    float hR = get_terrain_height(Vector2(position.x + normal_step, position.y));  // Height to the right
    float hD = get_terrain_height(Vector2(position.x, position.y - normal_step));  // Height below
    float hU = get_terrain_height(Vector2(position.x, position.y + normal_step));  // Height above

    // Compute gradient (partial derivatives)
    float dx = hR - hL;  // Gradient in X direction
    float dz = hU - hD;  // Gradient in Z direction

    // Adjust the Y scale for normalized height data
    dx *= normal_scale;
    dz *= normal_scale;

    // Normal vector
    Vector3 normal(-dx, 2.0f * normal_step, -dz);

    // Normalize the vector
    return normal.normalized();  // not sure i need a normalize here?
}

void MeshGenerator2::add_terrain() {
    //
    Vector2 _lerp;  // track position x,y from 0 to 1

    std::vector<Vector2> lerps;
    lerps.resize((grid_size.y - 1) * (grid_size.x - 1));

    // heights
    for (int y = 0; y < grid_size.y; y++) {
        _lerp.y = float(y) / float(grid_size.y - 1);  // calculate y position from 0 to 1

        for (int x = 0; x < grid_size.x; x++) {
            _lerp.x = float(x) / float(grid_size.x - 1);  // calculate x position from 0 to 1

            float _height = get_terrain_height(_lerp);

            // auto final_pos = Vector3(_lerp.x, _height, _lerp.y) * scale;  // scaled allows height and dimension control
            auto final_pos = Vector3(_lerp.x - 0.5f, _height, _lerp.y - 0.5f) * scale;  // with center correction

            // verts.push_back(Vector3(x, _height, y));
            verts.push_back(final_pos);

            // uvs.push_back(Vector2(x, y));
            uvs.push_back(_lerp * uv_scale);

            auto normal = get_terrain_normal(_lerp);  // get normal
            normals.push_back(normal);                // unfinished (just left up)
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

void MeshGenerator2::macro_test() {
    print("MeshGenerator2::macro_test()...");
    _ready();
}

void MeshGenerator2::macro_generate_terrain() {
    print("MeshGenerator2::macro_generate_terrain()...");

    if (process_image.is_valid()) {
        print("found process image...");
        auto image = process_image->get_image();
        auto image_size = process_image->get_size();
        auto image_floats = ImageHelper::image_channel_to_floats(image, 0);  // red channel to floats

        if (blur_value > 0.0f) {

            
            print("apply blur ", blur_value, "");
            image_floats = ImageHelper::blur_image(image_floats, image_size, blur_value);  // blur  (SEEMS TO CRASH AT THE MO!!!)
        }
        auto new_image = ImageHelper::floats_to_image(image_floats, image_size, Image::FORMAT_L8);  // grey
        // auto new_image = ImageHelper::floats_to_image(image_floats, image_size, Image::FORMAT_RGB8);  // rgb
        // auto new_image = ImageHelper::floats_to_image(image_floats, image_size, Image::FORMAT_RGBA8);  // rgba

        heightmap1 = ImageTexture::create_from_image(new_image);
    }

    clear();
    add_terrain();
    generate_mesh();  // we need to call in ready, as then the children will be in the tree
}

void MeshGenerator2::macro_test_blur() {
    if (heightmap1.is_valid()) {
        auto image_size = heightmap1->get_size();

        auto image = heightmap1->get_image();

        auto floats = ImageHelper::image_channel_to_floats(image, 0);  // red channel to floats

        if (blur_value > 0.0f) {
            floats = ImageHelper::blur_image(floats, image_size, blur_value);  // blur  (SEEMS TO CRASH AT THE MO!!!)
        }
        auto new_image = ImageHelper::floats_to_image(floats, image_size, Image::FORMAT_L8);  // black and white
        // auto new_image = ImageHelper::floats_to_image(floats, image_size, Image::FORMAT_RGB8);  // RGB
        // auto new_image =  ImageHelper::floats_to_image(floats, image_size, Image::FORMAT_RGBA8); // with alpha

        image_out = ImageTexture::create_from_image(new_image);
    }
}

// BINDINGS:
// macros from macros.h
// CREATE_GETTER_SETTER(MeshGenerator2, bool, enabled)
// CREATE_GETTER_SETTER(MeshGenerator2, float, speed)
// CREATE_GETTER_SETTER(MeshGenerator2, Vector2i, grid_size)
// CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, texture2d)

// macros from macros.h
CREATE_GETTER_SETTER(MeshGenerator2, PackedVector3Array, verts)
CREATE_GETTER_SETTER(MeshGenerator2, PackedVector2Array, uvs)
CREATE_GETTER_SETTER(MeshGenerator2, PackedVector3Array, normals)
CREATE_GETTER_SETTER(MeshGenerator2, PackedInt32Array, indices)
CREATE_GETTER_SETTER(MeshGenerator2, Array, surface_array)

CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, heightmap1)
CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, heightmap2)
CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, heightmap3)
CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, image_out)
CREATE_GETTER_SETTER(MeshGenerator2, Ref<Texture2D>, process_image)

CREATE_GETTER_SETTER(MeshGenerator2, float, heightmap1_scale)
CREATE_GETTER_SETTER(MeshGenerator2, float, heightmap2_scale)
CREATE_GETTER_SETTER(MeshGenerator2, float, heightmap3_scale)

CREATE_GETTER_SETTER(MeshGenerator2, Vector3, scale)
CREATE_GETTER_SETTER(MeshGenerator2, Vector2i, grid_size)
CREATE_GETTER_SETTER(MeshGenerator2, Vector2, uv_scale)
CREATE_GETTER_SETTER(MeshGenerator2, float, normal_scale)
CREATE_GETTER_SETTER(MeshGenerator2, float, normal_step)

CREATE_GETTER_SETTER(MeshGenerator2, float, blur_value)

void MeshGenerator2::_bind_methods() {
    // macros from macros.h
    // CREATE_VAR_BINDINGS(MeshGenerator2, BOOL, enabled)
    // // CREATE_CLASSDB_BINDINGS2(Template, "bool", enabled) // tested alt macro? CRASHED!!!
    // CREATE_VAR_BINDINGS(MeshGenerator2, FLOAT, speed)
    // CREATE_VAR_BINDINGS(MeshGenerator2, VECTOR2I, grid_size)

    // CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", texture2d)

    // macros from macros.h
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR3_ARRAY, verts)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR2_ARRAY, uvs)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_VECTOR3_ARRAY, normals)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::PACKED_INT32_ARRAY, indices)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::ARRAY, surface_array)

    CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", heightmap1);
    CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", heightmap2);
    CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", heightmap3);
    CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", image_out);
    CREATE_CLASS_BINDINGS(MeshGenerator2, "Texture2D", process_image);

    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, heightmap1_scale)  // different macro for classes
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, heightmap2_scale)  // different macro for classes
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, heightmap3_scale)  // different macro for classes

    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::VECTOR3, scale)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::VECTOR2I, grid_size)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::VECTOR2, uv_scale)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, normal_scale)
    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, normal_step)

    CREATE_VAR_BINDINGS(MeshGenerator2, Variant::FLOAT, blur_value)

    // manual bind of macro (see examples of this in poly_synth.h)
    ClassDB::bind_method(D_METHOD("macro_test"), &MeshGenerator2::macro_test);
    ClassDB::bind_method(D_METHOD("macro_test_blur"), &MeshGenerator2::macro_test_blur);
    ClassDB::bind_method(D_METHOD("macro_generate_terrain"), &MeshGenerator2::macro_generate_terrain);

}

#endif
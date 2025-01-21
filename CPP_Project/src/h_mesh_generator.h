/*

new more simple ArrayMesh inherited object

*/
#ifndef H_MESH_GENERATOR_H
#define H_MESH_GENERATOR_H

#include <helper.h>
#include <macros.h>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

// works!
class HArrayMesh : public ArrayMesh {
    GDCLASS(HArrayMesh, ArrayMesh)

   public:
    // DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, trigger_update, false)

   public:
    bool get_trigger_update() {
        return false;
    };
    void set_trigger_update(const bool p_trigger_update) {
        if (p_trigger_update) {
            _update();
        }
    };

    void _update() {
        print("magic update trigger!! from c++");
        _generate_heightmap();
        _generate_mesh();
    }

    DECLARE_PROPERTY_SINGLE_FILE(Ref<Texture2D>, heightmap)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector3, heightmap_scale, Vector3(1, 1, 1))
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2i, grid_size, Vector2i(64, 64))
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(Vector2, uv_scale, Vector2(1, 1))

    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(bool, generate_normals, true) // if false just use up (0,1,0)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, normal_scale, 1.0f)
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(float, normal_step, 0.001f)

   protected:
    static void _bind_methods() {
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::BOOL, trigger_update);

        CREATE_CLASS_BINDINGS(HArrayMesh, "Texture2D", heightmap)
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::VECTOR3, heightmap_scale);
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::VECTOR2I, grid_size);
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::VECTOR2, uv_scale);
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::BOOL, generate_normals);
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::FLOAT, normal_scale);
        CREATE_VAR_BINDINGS(HArrayMesh, Variant::FLOAT, normal_step);

        // // manual method binding (so the methods can be called from GDScript or other languages)
        ClassDB::bind_method(D_METHOD("_update"), &HArrayMesh::_update);

        // ClassDB::bind_method(D_METHOD("macro_test2", "input", "number"), &TemplateSFile::macro_test2); // method bind with pars
    }

   public:
    PackedVector3Array verts;
    PackedVector2Array uvs;
    PackedVector3Array normals;
    PackedInt32Array indices;

    PackedColorArray colors;

    std::vector<std::vector<int>> ngons;

    // void clear_data()

    float get_heightmap_height(Vector2 position) {
        if (heightmap.is_valid()) {
            auto image = heightmap->get_image();

            if (image.is_valid() && !image->is_compressed()) {
                auto _sample = ImageHelper::sample_image(image, position);
                return _sample.r;
            }
        }

        return 0.0f;
    }

    // uncheked chatgp (can be errors sometimes)
    Vector3 get_heightmap_normal(Vector2 position) {

        if (!generate_normals){
            return Vector3(0,1,0);
        }

        // Normal calculation function
        // Sample heights around the point
        float hL = get_heightmap_height(Vector2(position.x - normal_step, position.y));  // Height to the left
        float hR = get_heightmap_height(Vector2(position.x + normal_step, position.y));  // Height to the right
        float hD = get_heightmap_height(Vector2(position.x, position.y - normal_step));  // Height below
        float hU = get_heightmap_height(Vector2(position.x, position.y + normal_step));  // Height above

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

    void _generate_heightmap() {
        //
        Vector2 _lerp;  // track position x,y from 0 to 1

        std::vector<Vector2> lerps;
        lerps.resize((grid_size.y - 1) * (grid_size.x - 1));

        // heights
        for (int y = 0; y < grid_size.y; y++) {
            _lerp.y = float(y) / float(grid_size.y - 1);  // calculate y position from 0 to 1

            for (int x = 0; x < grid_size.x; x++) {
                _lerp.x = float(x) / float(grid_size.x - 1);  // calculate x position from 0 to 1

                float _height = get_heightmap_height(_lerp);

                // auto final_pos = Vector3(_lerp.x, _height, _lerp.y) * scale;  // scaled allows height and dimension control
                auto final_pos = Vector3(_lerp.x - 0.5f, _height, _lerp.y - 0.5f) * heightmap_scale;  // with center correction

                // verts.push_back(Vector3(x, _height, y));
                verts.push_back(final_pos);

                // uvs.push_back(Vector2(x, y));
                uvs.push_back(_lerp * uv_scale);

                auto normal = get_heightmap_normal(_lerp);  // get normal
                normals.push_back(normal);                  // unfinished (just left up)
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

                ngons.push_back(std::vector<int>{a, b, c, d});  // new unused
            }
        }
    }

    void _generate_mesh() {
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
        Array surface_array;
        surface_array.resize(Mesh::ARRAY_MAX);
        surface_array[Mesh::ARRAY_VERTEX] = verts;  // working on this
        surface_array[Mesh::ARRAY_TEX_UV] = uvs;
        surface_array[Mesh::ARRAY_NORMAL] = normals;
        surface_array[Mesh::ARRAY_INDEX] = indices;

        // surface_array[Mesh::ARRAY_COLOR] = colors;

        verts.clear();
        uvs.clear();
        normals.clear();
        indices.clear();

        clear_surfaces();

        add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
    }
};

#endif
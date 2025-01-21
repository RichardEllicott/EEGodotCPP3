/*

Helper Functions



*/
#ifndef HELPER_H  // header guard
#define HELPER_H

#include <cstddef>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node.hpp>                     // Node
#include <godot_cpp/classes/random_number_generator.hpp>  // Node
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>  // godot::UtilityFunctions::print(input);
#include <godot_cpp/variant/vector2.hpp>
#include <stdexcept>
#include <vector>

// #include "core/object.h"
// #include "core/class_db.h"
// #include "core/math/vector2.h"
#include <random>

// #include <cmath>
// #include <algorithm>

// #include <godot_cpp/variant/packed_vector2_array.hpp>

using namespace godot;

// VIP functions
#pragma region GLOBALS

static const double DEG_TO_RAD = Math_PI / 180.0;  // multiply by this to convert
static const double RAD_TO_DEG = 180.0 / Math_PI;  // multiply by this to convert

// print shortcut
// #ifndef PRINT
// #define PRINT
template <typename... Args>
void print(Args... args) {
    UtilityFunctions::print(args...);
}
// #endif

// lerp template function, works with c++ types
// see also:
// UtilityFunctions::lerp and UtilityFunctions::lerpf
#ifndef LERP
#define LERP
template <typename T>
T lerp(T a, T b, T alpha) {
    return a + (b - a) * alpha;
}
#endif

// positive mod (as c++ mod is actually division remainder)
// works the same as the python one, so never returns negative, allows seamless wrapping
// #ifndef POS_MOD
// #define POS_MOD
inline int pos_mod(int a, int b) {
    const int result = a % b;
    return result >= 0 ? result : result + b;
}
// #endif
// #pragma endregion

// get any node (staticly)
// usage:
// auto node2d = get_node_as<Node2D>(this, "Node2D");
//
// #ifndef GET_NODE_AS
// #define GET_NODE_AS
template <typename T>
T* get_node_as(Node* current_node, const NodePath& path) {
    if (!current_node) {
        UtilityFunctions::printerr("Current node is null!");
        return nullptr;
    }

    Node* node_ptr = current_node->get_node_or_null(path);
    return Object::cast_to<T>(node_ptr);
}
// #endif

// less important functions in a library
class ImageHelper {
   public:
    // Static method to sample an image at UV coordinates using bilinear interpolation
    static Color sample_image(const Image& image, const Vector2& uv);      // for & ref
    static Color sample_image(const Ref<Image> image, const Vector2& uv);  // for Godot ref. i think the normal way

    // convert an image channel to float data
    static PackedFloat32Array image_channel_to_floats(const Ref<Image> image, int channel);

    static Ref<Image> floats_to_image(const PackedFloat32Array& floats, Vector2i image_size, Image::Format image_format);

    static Ref<Image> colors_to_image(const PackedColorArray& floats, Vector2i image_size, Image::Format image_format);

    // worked from:
    // https://gemini.google.com/app/2eb57cf690ac3d2e
    static PackedFloat32Array blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius, bool wrap);

    // Helper function to create a 1D Gaussian kernel
    static void create_gaussian_kernel(std::vector<float>& kernel, float radius);

    // overlay images of matching size
    static void overlay_image(PackedFloat32Array& target_image, const PackedFloat32Array& overlay_image, int mode);

    // offset image by absolute pixel values
    static PackedFloat32Array offset_image(const PackedFloat32Array& image, Vector2i image_size, Vector2i offset, bool wrap);

    static PackedFloat32Array resample_image_bicubic(const PackedFloat32Array& image, Vector2i image_size, Vector2i new_image_size);

    static PackedColorArray generate_normal_map(const PackedFloat32Array& image, Vector2i image_size, float normal_strength, bool wrap);
    static PackedColorArray generate_normal_map_old(const PackedFloat32Array& image, Vector2i image_size, float normal_strength);
    
    // might help normals
    static PackedFloat32Array sobel_filter(const PackedFloat32Array& image, Vector2i image_size) {
        PackedFloat32Array result;
        result.resize(image.size());  // Initialize the result array with the same size as the input image.

        // Sobel filter kernels for detecting edges in the X and Y directions
        float sobel_x[3][3] = {
            {-1.0f, 0.0f, 1.0f},
            {-2.0f, 0.0f, 2.0f},
            {-1.0f, 0.0f, 1.0f}};

        float sobel_y[3][3] = {
            {-1.0f, -2.0f, -1.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 2.0f, 1.0f}};

        // Apply the Sobel filter to each pixel in the image
        for (int y = 1; y < image_size.y - 1; ++y) {
            for (int x = 1; x < image_size.x - 1; ++x) {
                float gx = 0.0f;
                float gy = 0.0f;

                // Apply the Sobel X and Y kernels to the 3x3 neighborhood of the pixel
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        int idx = (y + j) * image_size.x + (x + i);
                        float pixel_value = image[idx];  // Access pixel value in the height map

                        gx += pixel_value * sobel_x[j + 1][i + 1];
                        gy += pixel_value * sobel_y[j + 1][i + 1];
                    }
                }

                // Compute the gradient magnitude
                float magnitude = sqrt(gx * gx + gy * gy);

                // Store the gradient magnitude in the result array
                int idx = y * image_size.x + x;
                result[idx] = magnitude;
            }
        }

        return result;
    }



    static int image_position_to_index(Vector2i position, Vector2i size, bool wrap);

        static Vector2i index_to_image_position(int index, Vector2i image_size);





};

//
//
//
//
// // Simple Frequency Modulation function
//
// double fm_wave(double x, double carrier_freq, double modulating_freq, double modulation_index)
// {
// 	// FM Wave equation: y = sin(2 * pi * carrier_freq * x + modulation_index * sin(2 * pi * modulating_freq * x))
// 	return sin(2 * Math_PI * carrier_freq * x + modulation_index * sin(2 * Math_PI * modulating_freq * x));
// }

#pragma region HISTORY_BUFFER
// trouble using templates for this pattern as it involves two types, PackedVector2Array and Vector2
class PackedVector2ArrayBuffer {
   public:
    PackedVector2Array buffer = PackedVector2Array();

    int buffer_position = 0;

    PackedVector2ArrayBuffer(int size) {
        buffer.resize(size);
    }

    void resize(int size) {
        buffer.resize(size);
    }
    void add(Vector2 value) {
        buffer[buffer_position] = value;
        buffer_position = (buffer_position + 1) % buffer.size();
    }

    int size() {
        return buffer.size();
    }

    Vector2 get(int position) {
        position = pos_mod(buffer_position + position - 1, buffer.size());
        return buffer[position];
    }

    // PackedVector2Array get_frame(int position, int size) {
    //     position = pos_mod(buffer_position + position, buffer.size());

    //     PackedVector2Array return_buffer = PackedVector2Array();
    //     return_buffer.resize(size);
    //     for (int i = 0; i < size; i++) {
    //         int j = (position + i) % buffer.size();
    //         return_buffer[i] = buffer[j];
    //     }
    //     return return_buffer;
    // }

    // PackedVector2Array get_last_frame(int size) {
    //     return get_frame(size - 1, size);
    // }
};

#pragma endregion

// ported C# to Godot C++: "Coding Adventure: Hydraulic Erosion" by Sebastian Lague
class SebastianErosion {
   public:
    struct HeightAndGradient {
       public:
        float height;
        float gradientX;
        float gradientY;
    };

    int seed = 0;
    // [Range (2, 8)]
    int erosion_radius = 3;
    // [Range (0, 1)]
    float inertia = .05f;                // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction.
    float sediment_capacity_factor = 4;  // Multiplier for how much sediment a droplet can carry
    float min_sediment_capacity = .01f;  // Used to prevent carry capacity getting too close to zero on flatter terrain
                                         // [Range (0, 1)]
    float erode_speed = .3f;
    // [Range (0, 1)]
    float deposit_speed = .3f;
    // [Range (0, 1)]

    float evaporate_speed = .01f;
    float gravity = 4;
    int max_droplet_lifetime = 30;

    float initial_water_volume = 1;
    float initial_speed = 1;

    // Indices and weights of erosion brush precomputed for every node
    std::vector<std::vector<int>> erosion_brush_indices;

    std::vector<std::vector<float>> erosion_brush_weights;

    Ref<RandomNumberGenerator> rng;

    int current_erosion_radius = 0;

    int current_map_size = 0;

    SebastianErosion(int seed = 0) {
        rng.instantiate();
        rng->set_seed(seed);
    }

    // Initialization creates a System.Random object and precomputes indices and weights of erosion brush
    void _init(int map_size) {
        if (erosion_brush_indices.size() == 0 || current_erosion_radius != erosion_radius || current_map_size != map_size) {
            InitializeBrushIndices(map_size, erosion_radius);

            current_erosion_radius = erosion_radius;
            current_map_size = map_size;
        }
    }

    void erode(PackedFloat32Array& map, int map_size, int iterations = 1) {
        // throw error for bad pars
        if (map.size() != map_size * map_size) {
            UtilityFunctions::push_error("map.size() != map_size * map_size");
            return;
        }

        _init(map_size);

        current_erosion_radius = erosion_radius;
        current_map_size = map_size;

        for (int iteration = 0; iteration < iterations; iteration++) {
            // Create water droplet at random point on map
            Vector2 pos = Vector2(
                rng->randf_range(0, map_size - 1),
                rng->randf_range(0, map_size - 1));

            Vector2 dir;  // direction

            float speed = initial_speed;
            float water = initial_water_volume;
            float sediment = 0;

            for (int lifetime = 0; lifetime < max_droplet_lifetime; lifetime++) {
                // int nodeX = pos.x;
                // int nodeY = pos.y;

                Vector2 node = pos;  // seems to be the start position, as we used this to find the offset

                int dropletIndex = node.y * map_size + node.x;

                // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
                Vector2 cell_offset = pos - node;

                // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
                HeightAndGradient heightAndGradient = CalculateHeightAndGradient(map, map_size, pos.x, pos.y);

                // Update the droplet's direction and position (move position 1 unit regardless of speed)
                dir.x = (dir.x * inertia - heightAndGradient.gradientX * (1 - inertia));
                dir.y = (dir.y * inertia - heightAndGradient.gradientY * (1 - inertia));
                // Normalize direction

                float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len != 0) {
                    dir.x /= len;
                    dir.y /= len;
                }
                pos.x += dir.x;
                pos.y += dir.y;

                // Stop simulating droplet if it's not moving or has flowed over edge of map
                if ((dir.x == 0 && dir.y == 0) || pos.x < 0 || pos.x >= map_size - 1 || pos.y < 0 || pos.y >= map_size - 1) {
                    break;
                }

                // Find the droplet's new height and calculate the deltaHeight
                float newHeight = CalculateHeightAndGradient(map, map_size, pos.x, pos.y).height;
                float deltaHeight = newHeight - heightAndGradient.height;

                // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
                float sedimentCapacity = MAX(-deltaHeight * speed * water * sediment_capacity_factor, min_sediment_capacity);

                // If carrying more sediment than capacity, or if flowing uphill:
                if (sediment > sedimentCapacity || deltaHeight > 0) {
                    // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                    float amountToDeposit = (deltaHeight > 0) ? MIN(deltaHeight, sediment) : (sediment - sedimentCapacity) * deposit_speed;
                    sediment -= amountToDeposit;

                    // Add the sediment to the four nodes of the current cell using bilinear interpolation
                    // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                    map[dropletIndex] += amountToDeposit * (1 - cell_offset.x) * (1 - cell_offset.y);
                    map[dropletIndex + 1] += amountToDeposit * cell_offset.x * (1 - cell_offset.y);
                    map[dropletIndex + map_size] += amountToDeposit * (1 - cell_offset.x) * cell_offset.y;
                    map[dropletIndex + map_size + 1] += amountToDeposit * cell_offset.x * cell_offset.y;

                } else {
                    // Erode a fraction of the droplet's current carry capacity.
                    // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
                    float amountToErode = MIN((sedimentCapacity - sediment) * erode_speed, -deltaHeight);

                    // Use erosion brush to erode from all nodes inside the droplet's erosion radius
                    for (int brushPointIndex = 0; brushPointIndex < erosion_brush_indices[dropletIndex].size(); brushPointIndex++) {
                        int nodeIndex = erosion_brush_indices[dropletIndex][brushPointIndex];
                        float weighedErodeAmount = amountToErode * erosion_brush_weights[dropletIndex][brushPointIndex];
                        float deltaSediment = (map[nodeIndex] < weighedErodeAmount) ? map[nodeIndex] : weighedErodeAmount;
                        map[nodeIndex] -= deltaSediment;
                        sediment += deltaSediment;
                    }
                }

                // Update droplet's speed and water content
                speed = sqrt(speed * speed + deltaHeight * gravity);
                water *= (1 - evaporate_speed);
            }
        }
    }

    HeightAndGradient CalculateHeightAndGradient(PackedFloat32Array& nodes, int mapSize, float posX, float posY) {
        int coordX = posX;
        int coordY = posY;

        // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
        float x = posX - coordX;
        float y = posY - coordY;

        // Calculate heights of the four nodes of the droplet's cell
        int nodeIndexNW = coordY * mapSize + coordX;
        float heightNW = nodes[nodeIndexNW];
        float heightNE = nodes[nodeIndexNW + 1];
        float heightSW = nodes[nodeIndexNW + mapSize];
        float heightSE = nodes[nodeIndexNW + mapSize + 1];

        // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
        float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
        float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

        // Calculate height with bilinear interpolation of the heights of the nodes of the cell
        float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

        // return new HeightAndGradient(){height = height, gradientX = gradientX, gradientY = gradientY};
        return HeightAndGradient{height, gradientX, gradientY};
    }

    void InitializeBrushIndices(int mapSize, int radius) {
        // erosionBrushIndices = new int[mapSize * mapSize][];
        // erosionBrushWeights = new float[mapSize * mapSize][];

        erosion_brush_indices.clear();
        erosion_brush_indices.resize(mapSize * mapSize);
        erosion_brush_weights.clear();
        erosion_brush_weights.resize(mapSize * mapSize);

        std::vector<int> xOffsets(radius * radius * 4);
        std::vector<int> yOffsets(radius * radius * 4);
        std::vector<float> weights(radius * radius * 4);

        float weightSum = 0;
        int addIndex = 0;

        for (int i = 0; i < erosion_brush_indices.size(); i++) {
            int centreX = i % mapSize;
            int centreY = i / mapSize;

            if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius + 1 || centreX >= mapSize - radius) {
                weightSum = 0;
                addIndex = 0;
                for (int y = -radius; y <= radius; y++) {
                    for (int x = -radius; x <= radius; x++) {
                        float sqrDst = x * x + y * y;
                        if (sqrDst < radius * radius) {
                            int coordX = centreX + x;
                            int coordY = centreY + y;

                            if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize) {
                                float weight = 1 - sqrt(sqrDst) / radius;
                                weightSum += weight;
                                weights[addIndex] = weight;
                                xOffsets[addIndex] = x;
                                yOffsets[addIndex] = y;
                                addIndex++;
                            }
                        }
                    }
                }
            }

            int numEntries = addIndex;
            // erosionBrushIndices[i] = new int[numEntries];
            erosion_brush_indices[i].resize(numEntries);  // warning this might not clear the vars (not sure if a problem)

            // erosionBrushWeights[i] = new float[numEntries];
            erosion_brush_weights[i].resize(numEntries);  // warning this might not clear the vars (not sure if a problem)

            for (int j = 0; j < numEntries; j++) {
                erosion_brush_indices[i][j] = (yOffsets[j] + centreY) * mapSize + xOffsets[j] + centreX;
                erosion_brush_weights[i][j] = weights[j] / weightSum;
            }
        }
    }
};

#endif
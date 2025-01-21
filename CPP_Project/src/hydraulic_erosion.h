/*





*/
#ifndef HYDRAULIC_EROSION_H
#define HYDRAULIC_EROSION_H

#include <helper.h>
#include <macros.h>

using namespace godot;

// NOTE CHANGE TO OBJECT??? does't need to be node

class HydraulicErosion : public Object {
    GDCLASS(HydraulicErosion, Object)

   protected:
    static void _bind_methods() {
        CREATE_VAR_BINDINGS(HydraulicErosion, Variant::INT, erosion_radius)

        ClassDB::bind_method(D_METHOD("erode", "map", "map_size", "iterations"), &HydraulicErosion::erode);
    }

   public:
    struct HeightAndGradient {
       public:
        float height;
        float gradientX;
        float gradientY;
    };

    int seed = 0;

    // int erosion_radius = 3;
    DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(int, erosion_radius, 3)  // [Range (2, 8)]

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

    HydraulicErosion(int seed = 0) {
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

    // GDScript compatible (returns a new object)
    PackedFloat32Array erode(PackedFloat32Array map, int map_size, int iterations) {
        _erode(map, map_size, iterations);
        return map;
    }

    // references can only be used internally in the c++ it seems
    void _erode(PackedFloat32Array& map, int map_size, int iterations) {
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


                // continue; // NO CRASH TO THIS POINT!


                // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
                HeightAndGradient heightAndGradient = CalculateHeightAndGradient(map, map_size, pos);

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


                // continue; // STILL WORKS FOR 1M... slight freeze



                // Find the droplet's new height and calculate the deltaHeight
                float newHeight = CalculateHeightAndGradient(map, map_size, pos).height;
                float deltaHeight = newHeight - heightAndGradient.height;

                // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
                float sedimentCapacity = MAX(-deltaHeight * speed * water * sediment_capacity_factor, min_sediment_capacity);


                // continue; // NO CRASH

                // If carrying more sediment than capacity, or if flowing uphill:
                if (sediment > sedimentCapacity || deltaHeight > 0) {
                    // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                    float amountToDeposit = (deltaHeight > 0) ? MIN(deltaHeight, sediment) : (sediment - sedimentCapacity) * deposit_speed;
                    sediment -= amountToDeposit;

                    // Add the sediment to the four nodes of the current cell using bilinear interpolation
                    // Deposition is not distributed over a radius (like erosion) so that it can fill small pits

                    // maybe overflow here????

                    // map[dropletIndex] += amountToDeposit * (1 - cell_offset.x) * (1 - cell_offset.y);
                    // map[dropletIndex + 1] += amountToDeposit * cell_offset.x * (1 - cell_offset.y);
                    // map[dropletIndex + map_size] += amountToDeposit * (1 - cell_offset.x) * cell_offset.y;
                    // map[dropletIndex + map_size + 1] += amountToDeposit * cell_offset.x * cell_offset.y;

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

                    // continue; CRASH BY THIS POINT!!! (crash is above)


                // Update droplet's speed and water content
                speed = sqrt(speed * speed + deltaHeight * gravity);
                water *= (1 - evaporate_speed);
            }
        }
    }

    HeightAndGradient CalculateHeightAndGradient(PackedFloat32Array& nodes, int mapSize, Vector2 pos) {
        Vector2i coord = pos;

        // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
        float x = pos.x - coord.x;
        float y = pos.y - coord.y;

        // Calculate heights of the four nodes of the droplet's cell
        int nodeIndexNW = coord.y * mapSize + coord.x;
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
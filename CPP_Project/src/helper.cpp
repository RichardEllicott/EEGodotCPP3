#ifndef HELPER_CPP
#define HELPER_CPP

#include <helper.h>

using namespace godot;

// for c++ reference
Color ImageHelper::sample_image(const Image& image, const Vector2& uv) {
    // Clamp UV coordinates to [0, 1]
    Vector2 clamped_uv = uv.clamp(Vector2(0.0, 0.0), Vector2(1.0, 1.0));

    // Map UV to pixel coordinates
    float px = clamped_uv.x * (image.get_width() - 1);
    float py = clamped_uv.y * (image.get_height() - 1);

    // Get the integer and fractional parts
    int x0 = static_cast<int>(std::floor(px));
    int x1 = std::min(x0 + 1, image.get_width() - 1);
    int y0 = static_cast<int>(std::floor(py));
    int y1 = std::min(y0 + 1, image.get_height() - 1);

    float fx = px - x0;
    float fy = py - y0;

    // Sample the four surrounding pixels
    Color c00 = image.get_pixel(x0, y0);
    Color c10 = image.get_pixel(x1, y0);
    Color c01 = image.get_pixel(x0, y1);
    Color c11 = image.get_pixel(x1, y1);

    // Bilinear interpolation
    Color c0 = c00.lerp(c10, fx);
    Color c1 = c01.lerp(c11, fx);
    return c0.lerp(c1, fy);
}

// for Godot ref. i think the normal way for images in godot
Color ImageHelper::sample_image(const Ref<Image> image, const Vector2& uv) {
    // Clamp UV coordinates to [0, 1]
    Vector2 clamped_uv = uv.clamp(Vector2(0.0, 0.0), Vector2(1.0, 1.0));

    // Map UV to pixel coordinates
    float px = clamped_uv.x * (image->get_width() - 1);
    float py = clamped_uv.y * (image->get_height() - 1);

    // Get the integer and fractional parts
    int x0 = static_cast<int>(std::floor(px));
    int x1 = std::min(x0 + 1, image->get_width() - 1);
    int y0 = static_cast<int>(std::floor(py));
    int y1 = std::min(y0 + 1, image->get_height() - 1);

    float fx = px - x0;
    float fy = py - y0;

    // Sample the four surrounding pixels
    Color c00 = image->get_pixel(x0, y0);
    Color c10 = image->get_pixel(x1, y0);
    Color c01 = image->get_pixel(x0, y1);
    Color c11 = image->get_pixel(x1, y1);

    // Bilinear interpolation
    Color c0 = c00.lerp(c10, fx);
    Color c1 = c01.lerp(c11, fx);
    return c0.lerp(c1, fy);
}



Ref<Image> ImageHelper::floats_to_image(PackedFloat32Array float_image, Vector2i image_size, Image::Format format) {
    int bytes_per_pixel = 4;  // RGBA8 format

    // set byte count based on format
    switch (format) {
        case Image::FORMAT_RGBA8:
            bytes_per_pixel = 4;
            break;
        case Image::FORMAT_RGB8:  // crashes!
            bytes_per_pixel = 3;
            break;

        case Image::FORMAT_L8:  // single channel
            bytes_per_pixel = 1;
            break;

        default:
            UtilityFunctions::push_error("unknown format!");
            break;
    }

    int pixel_count = image_size.x * image_size.y;

    // build the image as bytes
    PackedByteArray image_bytes;
    image_bytes.resize(pixel_count * bytes_per_pixel);

    for (int i = 0; i < pixel_count; i++) {
        auto i2 = i * bytes_per_pixel;

        auto pixel_float = float_image[i];
        uint8_t pixel_denormalized = static_cast<uint8_t>(pixel_float * 255.0f);  // convert the float to an 8 bit int (0-255)

        image_bytes[i2] = pixel_denormalized;  // red

        if (bytes_per_pixel >= 3) {
            image_bytes[i2 + 1] = pixel_denormalized;  // green
            image_bytes[i2 + 2] = pixel_denormalized;  // blue
        }
        if (bytes_per_pixel >= 4) {
            image_bytes[i2 + 3] = 255;  // alpha
        }
    }

    // create the image from the bytes (no mips)
    auto image = Image::create_from_data(image_size.x, image_size.y, false, format, image_bytes);

    return image;
}

PackedFloat32Array ImageHelper::image_channel_to_floats(const Ref<Image> image, int channel = 0) {
    PackedFloat32Array float_data;  // convert image to floats

    if (!image.is_valid()) {
        UtilityFunctions::push_error("!image.is_valid()");
        return float_data;
    }
    if (channel < 0 || channel > 3) {
        UtilityFunctions::push_error("channel < 0 || channel > 3");
        return float_data;
    }

    auto image_size = image->get_size();
    auto image_format = image->get_format();
    auto image_data = image->get_data();

    int pixel_count = image_size.x * image_size.y;

    // Ref<Image> img_out = Image::create(image_size.x, image_size.y, false, Image::FORMAT_RGBA8);

    float_data.resize(pixel_count);

    int bytes_per_pixel = 4;  // RGBA8 format

    switch (image_format) {
        case Image::FORMAT_RGBA8:
            bytes_per_pixel = 4;
            break;
        case Image::FORMAT_RGB8:
            bytes_per_pixel = 3;
            break;
        default:
            UtilityFunctions::push_error("image is unknown format!");
            return float_data;
            break;
    }

    for (int i = 0; i < pixel_count; i++) {
        auto i2 = i * bytes_per_pixel;
        uint8_t pixel = image_data[i2 + channel];
        float pixel_normalized = pixel / 255.0f;  // Normalize to [0, 1]
        float_data[i] = pixel_normalized;
    }

    // for (int y = 0; y < image_size.y; y++) {
    //     for (int x = 0; x < image_size.x; x++) {
    //         int index = (y * image_size.x + x) * bytes_per_pixel;

    //         uint8_t pixel = image_data[index + channel];

    //         float pixel_normalized = pixel / 255.0f;  // Normalize to [0, 1]

    //         float_data[index] = pixel_normalized;
    //     }
    // }

    return float_data;
}



// Helper function to create a 1D Gaussian kernel
void ImageHelper::create_gaussian_kernel(std::vector<float>& kernel, float radius) {
    int kernel_size = 2 * radius + 1;
    kernel.resize(kernel_size);

    float sigma = radius / 2.0f;
    float sum = 0.0f;

    for (int i = 0; i < kernel_size; ++i) {
        float x = i - radius;
        kernel[i] = std::exp(-(x * x) / (2.0f * sigma * sigma));
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < kernel_size; ++i) {
        kernel[i] /= sum;
    }
}


// this one i asked gemeni to modify it to pass vertical and horizontal seperatly
// seems to be a bit faster but blur is a tad slow, moreso for larger radius
// https://gemini.google.com/app/2eb57cf690ac3d2e


PackedFloat32Array ImageHelper::blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius) {
    PackedFloat32Array output_image;
    output_image.resize(input_image.size());

    // Create a 1D Gaussian kernel
    std::vector<float> kernel;
    create_gaussian_kernel(kernel, radius);

    // Horizontal Blur
    for (int y = 0; y < image_size.y; ++y) {
        for (int x = 0; x < image_size.x; ++x) {
            float blurred_value = 0.0f;
            float total_weight = 0.0f;

            for (int kx = -radius; kx <= radius; ++kx) {
                int x_neighbor = x + kx;

                x_neighbor = CLAMP(x_neighbor, 0, image_size.x - 1);

                float weight = kernel[kx + radius]; 
                blurred_value += input_image[y * image_size.x + x_neighbor] * weight;
                total_weight += weight;
            }

            output_image[y * image_size.x + x] = blurred_value / total_weight;
        }
    }

    // Vertical Blur
    for (int y = 0; y < image_size.y; ++y) {
        for (int x = 0; x < image_size.x; ++x) {
            float blurred_value = 0.0f;
            float total_weight = 0.0f;

            for (int ky = -radius; ky <= radius; ++ky) {
                int y_neighbor = y + ky;

                y_neighbor = CLAMP(y_neighbor, 0, image_size.y - 1);

                float weight = kernel[ky + radius]; 
                blurred_value += output_image[y_neighbor * image_size.x + x] * weight; 
                total_weight += weight;
            }

            output_image[y * image_size.x + x] = blurred_value / total_weight;
        }
    }

    return output_image;
}




// PackedFloat32Array ImageHelper::erode(const PackedFloat32Array& heightmap, Vector2i image_size, int iterations, float deposition, float erosion) {
//         PackedFloat32Array eroded_heightmap = heightmap;

//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_int_distribution<> dis(0, image_size.x - 1);

//         for (int i = 0; i < iterations; ++i) {
//             // Random starting point
//             int x = dis(gen);
//             int y = dis(gen);

//             float water = 1.0f;
//             float sediment = 0.0f;

//             while (water > 0.0f) {
//                 Vector2 flow_direction(0, 0);

//                 for (int dx = -1; dx <= 1; ++dx) {
//                     for (int dy = -1; dy <= 1; ++dy) {
//                         if (dx == 0 && dy == 0) {
//                             continue;
//                         }

//                         int neighbor_x = CLAMP(x + dx, 0, image_size.x - 1);
//                         int neighbor_y = CLAMP(y + dy, 0, image_size.y - 1);

//                         int current_index = y * image_size.x + x;
//                         int neighbor_index = neighbor_y * image_size.x + neighbor_x;

//                         float current_height = heightmap[current_index];
//                         float neighbor_height = heightmap[neighbor_index];

//                         if (neighbor_height < current_height) {
//                             flow_direction += Vector2(dx, dy);
//                         }
//                     }
//                 }

//                 flow_direction = flow_direction.normalized();

//                 x += (int)round(flow_direction.x);
//                 y += (int)round(flow_direction.y);

//                 x = CLAMP(x, 0, image_size.x - 1);
//                 y = CLAMP(y, 0, image_size.y - 1);

//                 float distance = flow_direction.length(); 

//                 if (distance == 0.0f) {
//                     // Deposition
//                     sediment *= deposition; // Corrected line
//                     eroded_heightmap[y * image_size.x + x] += sediment;
//                     sediment -= sediment * deposition;
//                 } else {
//                     // Erosion
//                     float erosion_amount = water * erosion * distance;
//                     eroded_heightmap[y * image_size.x + x] -= erosion_amount; 
//                     sediment += erosion_amount;
//                 }

//                 water -= 0.01f;
//             }
//         }

//         return eroded_heightmap;
//     }



     PackedFloat32Array ImageHelper::erode(const PackedFloat32Array& heightmap,  Vector2i image_size, int iterations, float deposition, float erosion) {
        PackedFloat32Array eroded_heightmap = heightmap;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, image_size.x - 1);

        for (int i = 0; i < iterations; ++i) {
            // Random starting point
            int x = dis(gen);
            int y = dis(gen);

            float water = 1.0f;
            float sediment = 0.0f;

            while (water > 0.0f) {
                Vector2 flow_direction(0, 0);

                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) {
                            continue;
                        }

                        int neighbor_x = CLAMP(x + dx, 0, image_size.x - 1);
                        int neighbor_y = CLAMP(y + dy, 0, image_size.y - 1);

                        int current_index = y * image_size.x + x;
                        int neighbor_index = neighbor_y * image_size.x + neighbor_x;

                        float current_height = heightmap[current_index];
                        float neighbor_height = heightmap[neighbor_index];

                        if (neighbor_height < current_height) {
                            flow_direction += Vector2(dx, dy);
                        }
                    }
                }

                flow_direction = flow_direction.normalized();

                x += (int)round(flow_direction.x);
                y += (int)round(flow_direction.y);

                x = CLAMP(x, 0, image_size.x - 1);
                y = CLAMP(y, 0, image_size.y - 1);

                float distance = flow_direction.length(); 

                if (eroded_heightmap[y * image_size.x + x] <= 0.0f) { 
                    // If water reaches the lowest point (height 0), stop depositing sediment
                    sediment = 0.0f; 
                }

                if (distance == 0.0f) {
                    // Deposition
                    sediment *= deposition; 
                    eroded_heightmap[y * image_size.x + x] += sediment;
                    sediment -= sediment * deposition;
                } else {
                    // Erosion
                    float erosion_amount = water * erosion * distance;
                    eroded_heightmap[y * image_size.x + x] -= erosion_amount; 
                    sediment += erosion_amount;
                }

                water -= 0.01f;
            }
        }

        return eroded_heightmap;
    }




#endif
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

Ref<Image> ImageHelper::floats_to_image(const PackedFloat32Array& floats, Vector2i image_size, Image::Format format = Image::FORMAT_L8) {
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

        auto pixel_float = floats[i];
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

Ref<Image> ImageHelper::colors_to_image(const PackedColorArray& floats, Vector2i image_size, Image::Format format = Image::FORMAT_RGB8) {
    int bytes_per_pixel = 3;  // RGB8 format only!

    format = Image::FORMAT_RGB8;

    int pixel_count = image_size.x * image_size.y;

    // build the image as bytes
    PackedByteArray image_bytes;
    image_bytes.resize(pixel_count * bytes_per_pixel);

    for (int i = 0; i < pixel_count; i++) {
        auto i2 = i * bytes_per_pixel;

        auto pixel_color = floats[i];
        uint8_t r_denormalized = static_cast<uint8_t>(pixel_color.r * 255.0f);  // red
        uint8_t g_denormalized = static_cast<uint8_t>(pixel_color.g * 255.0f);  // green
        uint8_t b_denormalized = static_cast<uint8_t>(pixel_color.b * 255.0f);  // blue

        image_bytes[i2] = r_denormalized;      // red
        image_bytes[i2 + 1] = g_denormalized;  // green
        image_bytes[i2 + 2] = b_denormalized;  // blue
    }

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

PackedFloat32Array ImageHelper::blur_image(const PackedFloat32Array& input_image, Vector2i image_size, float radius, bool wrap = false) {
    PackedFloat32Array output_image;

    if (input_image.size() != image_size.x * image_size.y) {  // godot error
        UtilityFunctions::push_error("input_image.size() != image_size.x * image_size.y");
        return output_image;
    }

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

                if (wrap) {
                    x_neighbor = pos_mod(x_neighbor, image_size.x);
                } else {
                    x_neighbor = CLAMP(x_neighbor, 0, image_size.x - 1);
                }

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

                if (wrap) {
                    y_neighbor = pos_mod(y_neighbor, image_size.y);
                } else {
                    y_neighbor = CLAMP(y_neighbor, 0, image_size.y - 1);
                }

                float weight = kernel[ky + radius];
                blurred_value += output_image[y_neighbor * image_size.x + x] * weight;
                total_weight += weight;
            }

            output_image[y * image_size.x + x] = blurred_value / total_weight;
        }
    }

    return output_image;
}

void ImageHelper::overlay_image(PackedFloat32Array& target_image, const PackedFloat32Array& overlay_image, int mode = 0) {
    auto image_size = target_image.size();
    if (image_size != overlay_image.size()) {
        UtilityFunctions::push_error("image_size != overlay_image.size()");
        return;  // error
    }

    if (mode < 0 || mode > 1) {
        UtilityFunctions::push_error("mode < 0 || mode > 1");
        return;  // error
    }

    for (int i = 0; i < target_image.size(); i++) {
        switch (mode) {
            case 0:
                target_image[i] += overlay_image[i];
                break;
            case 1:
                target_image[i] -= overlay_image[i];
                break;
            default:
                break;
        }
    }
}

PackedFloat32Array ImageHelper::offset_image(const PackedFloat32Array& image, Vector2i image_size, Vector2i offset, bool wrap = true) {
    PackedFloat32Array result_image;

    auto array_size = image_size.x * image_size.y;

    if (array_size != image.size()) {
        UtilityFunctions::push_error("image_size.x * image_size.y != image.size()");
        return result_image;
    }

    result_image.resize(array_size);

    for (int y = 0; y < image_size.y; ++y) {
        for (int x = 0; x < image_size.x; ++x) {
            Vector2i new_pos = Vector2i(x + offset.x, y + offset.y);

            if (wrap) {
                new_pos.x = pos_mod(new_pos.x, image_size.x);
                new_pos.y = pos_mod(new_pos.y, image_size.y);
            } else {
                new_pos.x = CLAMP(new_pos.x, 0, image_size.x - 1);
                new_pos.y = CLAMP(new_pos.y, 0, image_size.y - 1);
            }

            int old_index = y * image_size.x + x;
            int new_index = new_pos.y * image_size.x + new_pos.x;

            result_image[new_index] = image[old_index];
        }
    }

    return result_image;
}

PackedFloat32Array ImageHelper::resample_image_bicubic(const PackedFloat32Array& image, Vector2i image_size, Vector2i new_image_size) {
    PackedFloat32Array result_image;
    result_image.resize(new_image_size.x * new_image_size.y);

    float scale_x = static_cast<float>(image_size.x) / new_image_size.x;
    float scale_y = static_cast<float>(image_size.y) / new_image_size.y;

    auto cubic_interpolate = [](float x) -> float {
        x = CLAMP(x, -2.0f, 2.0f);
        if (x <= 1.0f) {
            return (1.5f * x - 2.5f) * x * x + 1.0f;
        } else {
            return (-0.5f * x + 2.5f) * x * x - 4.0f * x + 2.0f;
        }
    };

    for (int y = 0; y < new_image_size.y; ++y) {
        for (int x = 0; x < new_image_size.x; ++x) {
            float src_x = x * scale_x;
            float src_y = y * scale_y;

            // Calculate source pixel coordinates
            int src_x_int = static_cast<int>(src_x);
            int src_y_int = static_cast<int>(src_y);
            float src_x_frac = src_x - src_x_int;
            float src_y_frac = src_y - src_y_int;

            float result = 0.0f;

            for (int j = -1; j <= 2; ++j) {
                for (int i = -1; i <= 2; ++i) {
                    int x_idx = CLAMP(src_x_int + i, 0, image_size.x - 1);
                    int y_idx = CLAMP(src_y_int + j, 0, image_size.y - 1);
                    int index = y_idx * image_size.x + x_idx;

                    float weight_x = cubic_interpolate(i - src_x_frac);
                    float weight_y = cubic_interpolate(j - src_y_frac);

                    result += image[index] * weight_x * weight_y;
                }
            }

            result_image[y * new_image_size.x + x] = result;
        }
    }

    return result_image;
}

/*



function getPartialDerivatives( heightMap ) {
    let height = heightMap.length;
    let width = heightMap[0].length;
    let out = [];

    for(let i = 0; i < height; i++) {
        out.push([]); // Add a new row.

        for(let j = 0; j < width; j++) {
            // Fetch surrounding pixel heights (handle borders with `clamp`).
            let top = heightMap[clamp(i-1, 0, height-1)][j];
            let topRight = heightMap[clamp(i-1, 0, height-1)][clamp(j+1, 0, width-1)];
            let right = heightMap[i][clamp(j+1, 0, width-1)];
            let bottomRight = heightMap[clamp(i+1, 0, height-1)][clamp(j+1, 0, width-1)];
            let bottom = heightMap[clamp(i+1, 0, height-1)][j];
            let bottomLeft = heightMap[clamp(i+1, 0, height-1)][clamp(j-1, 0, width-1)];
            let left = heightMap[i][clamp(j-1, 0, width-1)];
            let topLeft = heightMap[clamp(i-1, 0, height-1)][clamp(j-1, 0, width-1)];

            // Apply the Sobel operator for X and Y gradients.
            let partialDerivativeX = (topRight + 2*right + bottomRight) - (topLeft + 2*left + bottomLeft);
            let partialDerivativeY = (bottomLeft + 2*bottom + bottomRight) - (topLeft + 2*top + topRight);

            // Normalize values to a 0–1 range.
            partialDerivativeX /= 255;
            partialDerivativeY /= 255;

            out[i].push(new THREE.Vector3(partialDerivativeX, partialDerivativeY, 0)); // Store the gradient.
        }
    }
    return out;
}



*/

int ImageHelper::image_position_to_index(Vector2i position, Vector2i image_size, bool wrap = false) {
    if (wrap) {
        position.x = pos_mod(position.x, image_size.x);
        position.y = pos_mod(position.y, image_size.y);
    } else {
        position.x = CLAMP(position.x, 0, image_size.x - 1);
        position.y = CLAMP(position.y, 0, image_size.y - 1);
    }

    return position.x + position.y * image_size.x;
}

Vector2i ImageHelper::index_to_image_position(int index, Vector2i image_size) {
    return Vector2i(index % image_size.x, index / image_size.x);
}

std::vector<Vector2i> _get_surrounding_positions(const Vector2i& position) {
    return {
        {position.x, position.y - 1},      // Top
        {position.x + 1, position.y - 1},  // Top-right
        {position.x + 1, position.y},      // Right
        {position.x + 1, position.y + 1},  // Bottom-right
        {position.x, position.y + 1},      // Bottom
        {position.x - 1, position.y + 1},  // Bottom-left
        {position.x - 1, position.y},      // Left
        {position.x - 1, position.y - 1}   // Top-left
    };
}

// using basic sobel filter, i think the same as
// https://github.com/cpetry/NormalMap-Online/
//
PackedColorArray ImageHelper::generate_normal_map(const PackedFloat32Array& image, Vector2i image_size, float normal_strength = 1.0f, bool wrap = false) {
    PackedColorArray normal_map;
    if (image_size.x * image_size.y != image.size()) {
        auto error = "Image size does not match the number of pixels in the height map.";
        UtilityFunctions::push_error(error);
        // throw std::runtime_error(error);
        return normal_map;
    }

    int array_size = image_size.x * image_size.y;
    normal_map.resize(array_size);

    for (int y = 0; y < image_size.y; y++) {
        for (int x = 0; x < image_size.x; x++) {
            // clockwise from top
            float top = image[image_position_to_index(Vector2i(x, y - 1), image_size, wrap)];
            float top_right = image[image_position_to_index(Vector2i(x + 1, y - 1), image_size, wrap)];
            float right = image[image_position_to_index(Vector2i(x + 1, y), image_size, wrap)];
            float bottom_right = image[image_position_to_index(Vector2i(x + 1, y + 1), image_size, wrap)];
            float bottom = image[image_position_to_index(Vector2i(x, y + 1), image_size, wrap)];
            float bottom_left = image[image_position_to_index(Vector2i(x - 1, y + 1), image_size, wrap)];
            float left = image[image_position_to_index(Vector2i(x - 1, y), image_size, wrap)];
            float top_left = image[image_position_to_index(Vector2i(x - 1, y - 1), image_size, wrap)];

            // // Apply the Sobel operator for X and Y gradients.
            float partialDerivativeX = (top_right + 2 * right + bottom_right) - (top_left + 2 * left + bottom_left);
            float partialDerivativeY = (bottom_left + 2 * bottom + bottom_right) - (top_left + 2 * top + top_right);


            // get the normal
            // Vector3 normal(partialDerivativeX, partialDerivativeY, 1.0f * normal_strength); // like normal map online

            // seems to work here, i am not sure what godot wants when we do it this way
            Vector3 normal(partialDerivativeX, -partialDerivativeY, 1.0f * normal_strength); // correct in godot (i believe opengl)

            normal = normal.normalized();

            // Convert the normal to color space ([-1, 1] -> [0, 1])
            Color color(
                0.5f + 0.5f * normal.x,  // Red
                0.5f + 0.5f * normal.y,  // Green
                0.5f + 0.5f * normal.z   // Blue
            );

            int index = image_position_to_index(Vector2i(x, y), image_size, wrap);

            normal_map[index] = color;
        }
    }

    return normal_map;
}

// this is now an older one
// just checks 4 pixels, not very good
PackedColorArray ImageHelper::generate_normal_map_old(const PackedFloat32Array& image, Vector2i image_size, float normal_strength = 1.0f) {
    PackedColorArray normal_map;
    if (image_size.x * image_size.y != image.size()) {
        auto error = "Image size does not match the number of pixels in the height map.";
        UtilityFunctions::push_error(error);
        // throw std::runtime_error(error);
        return normal_map;
    }

    normal_map.resize(image.size());

    // Loop through each pixel
    for (int y = 0; y < image_size.y; ++y) {
        for (int x = 0; x < image_size.x; ++x) {
            // Get indices of neighbors, clamping edges
            int left = (x > 0) ? (y * image_size.x + (x - 1)) : (y * image_size.x + x);
            int right = (x < image_size.x - 1) ? (y * image_size.x + (x + 1)) : (y * image_size.x + x);
            int top = (y > 0) ? ((y - 1) * image_size.x + x) : (y * image_size.x + x);
            int bottom = (y < image_size.y - 1) ? ((y + 1) * image_size.x + x) : (y * image_size.x + x);

            // Calculate gradients
            float dx = image[right] - image[left];
            float dy = image[bottom] - image[top];

            // Compute the normal vector (assuming z = 1 for height scale)
            Vector3 normal(-dx, -dy, 1.0f);
            normal = normal.normalized();

            // Apply the normal strength factor
            normal *= normal_strength;

            // Convert the normal to color space ([-1, 1] -> [0, 1])
            Color color(
                0.5f + 0.5f * normal.x,  // Red
                0.5f + 0.5f * normal.y,  // Green
                0.5f + 0.5f * normal.z   // Blue
            );

            // Set the color
            int index = y * image_size.x + x;
            normal_map[index] = color;
        }
    }

    return normal_map;
}

// void ImageHelper::image_clamp_range(PackedFloat32Array& image, float min = 0.0f, float max = 0.0f) {
//     auto image_size = image.size();
//     for (int i = 0; i < image_size; i++) {
//         image[i] = CLAMP(image[i], min, max);
//     }
// }

#endif
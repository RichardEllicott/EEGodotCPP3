#ifndef HELPER_CPP
#define HELPER_CPP

#include <helper.h>

using namespace godot;


Color ImageHelper::sample_image(const Image &image, const Vector2 &uv) {
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





#endif
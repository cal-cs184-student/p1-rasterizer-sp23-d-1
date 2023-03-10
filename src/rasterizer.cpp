#include "rasterizer.h"
#include <array>

using namespace std;

namespace CGL {

RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
                             size_t width, size_t height,
                             unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height * sample_rate, Color::White);
}

// Used by rasterize_point and rasterize_line
void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    int step_size = sqrt(sample_rate);

    for (int x_loc = 0; x_loc < step_size; ++x_loc) {
        for (int y_loc = 0; y_loc < step_size; ++y_loc) {
            fill_sample_pixel(x * step_size + x_loc, y * step_size + y_loc, c);
        }
    }
}

void RasterizerImp::fill_sample_pixel(size_t x, size_t y, Color c) {

    int step_size = sqrt(sample_rate);
    sample_buffer[y * width * step_size + x] = c;
}

// Rasterize a point: simple example to help you start familiarizing
// yourself with the starter code.
//
void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

    fill_pixel(sx, sy, color);
    return;
}

// Rasterize a line.
void RasterizerImp::rasterize_line(float x0, float y0,
                                   float x1, float y1,
                                   Color color) {
    if (x0 > x1) {
        swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
        dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
        dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
        rasterize_point(pt[0], pt[1], color);
        pt[0] += dpt[0]; pt[1] += dpt[1];
    }
}

array<Vector2D, 2> triangle_bounding_box(size_t width, size_t height, Vector2D* vertices) {

    Vector2D bounding_min = Vector2D(width, height);
    Vector2D bounding_max = Vector2D(0, 0);

    // Get the bounding box of the triangle.
    for (int i = 0; i < 3; ++i) {
        bounding_min.x = min(bounding_min.x, vertices[i].x);
        bounding_min.y = min(bounding_min.y, vertices[i].y);

        bounding_max.x = max(bounding_max.x, vertices[i].x);
        bounding_max.y = max(bounding_max.y, vertices[i].y);
    }

    bounding_min.x = max(0.0, bounding_min.x);
    bounding_min.y = max(0.0, bounding_min.y);

    bounding_max.x = min(double(width), bounding_max.x);
    bounding_max.y = min(double(height), bounding_max.y);

    return array<Vector2D, 2>{bounding_min, bounding_max};
}

bool triangle_test_point_inbound(Vector2D p, Vector2D* vertices) {
    uint8_t count = 0;
    for (int k = 0; k < 3; ++k) {
        Vector2D line = vertices[(k + 1) % 3] - vertices[k];
        Vector2D p_test_vector = p - vertices[k];
        if (cross(line, p_test_vector) < 0)
            count++;
    }

    return !count || count == 3;
}

// Rasterize a triangle.
void RasterizerImp::rasterize_triangle(float x0, float y0,
                                       float x1, float y1,
                                       float x2, float y2,
                                       Color color) {
    size_t step_size = sqrt(sample_rate);
    Vector2D vertices[] = {
        Vector2D(x0, y0) * step_size,
        Vector2D(x1, y1) * step_size,
        Vector2D(x2, y2) * step_size,
    };

    array<Vector2D, 2> bounding_box = triangle_bounding_box(width * step_size, height * step_size, vertices);
    Vector2D bounding_min = bounding_box[0];
    Vector2D bounding_max = bounding_box[1];

    for (int y_test = bounding_min.y; y_test <= bounding_max.y; ++y_test) {
        bool seen_inbound = false;
        for (int x_test = bounding_min.x; x_test <= bounding_max.x; ++x_test) {
            Vector2D p = Vector2D(x_test + 0.5, y_test + 0.5);
            if (triangle_test_point_inbound(p, vertices)) {
                seen_inbound = true;
                fill_sample_pixel(x_test, y_test, color);
            } else {
                if (seen_inbound)
                    break;
            }
        }
    }
}


Vector3D get_barycentric_coordinates(Vector2D v, Vector2D* vertices) {
    Matrix3x3 m = Matrix3x3(vertices[0].x, vertices[1].x, vertices[2].x,
                            vertices[0].y, vertices[1].y, vertices[2].y,
                            1,             1,             1  );
    return m.inv() * Vector3D(v.x, v.y, 1);
}


void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
                                                          float x1, float y1, Color c1,
                                                          float x2, float y2, Color c2) {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle
    
    // Rasterize triangle
    size_t step_size = sqrt(sample_rate);
    Vector2D vertices[] = {
        Vector2D(x0, y0) * step_size,
        Vector2D(x1, y1) * step_size,
        Vector2D(x2, y2) * step_size,
    };

    array<Vector2D, 2> bounding_box = triangle_bounding_box(width * step_size, height * step_size, vertices);
    Vector2D bounding_min = bounding_box[0];
    Vector2D bounding_max = bounding_box[1];

    for (int y_test = bounding_min.y; y_test <= bounding_max.y; ++y_test) {
        bool seen_inbound = false;
        for (int x_test = bounding_min.x; x_test <= bounding_max.x; ++x_test) {
            Vector2D p = Vector2D(x_test + 0.5, y_test + 0.5);
            if (triangle_test_point_inbound(p, vertices)) {
                seen_inbound = true;
                Vector3D coord = get_barycentric_coordinates(p, vertices);
                Color c = (c0 * coord.x) + (c1 * coord.y) + (c2 * coord.z);
                fill_sample_pixel(x_test, y_test, c);
            } else {
                if (seen_inbound)
                    break;
            }
        }
    }
}


void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
                                                float x1, float y1, float u1, float v1,
                                                float x2, float y2, float u2, float v2,
                                                Texture& tex) {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    
    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
    size_t step_size = sqrt(sample_rate);
    Vector2D vertices[] = {
        Vector2D(x0, y0) * step_size,
        Vector2D(x1, y1) * step_size,
        Vector2D(x2, y2) * step_size,
    };

    array<Vector2D, 2> bounding_box = triangle_bounding_box(width * step_size, height * step_size, vertices);
    Vector2D bounding_min = bounding_box[0];
    Vector2D bounding_max = bounding_box[1];

    for (int y_test = bounding_min.y; y_test <= bounding_max.y; ++y_test) {
        bool seen_inbound = false;
        for (int x_test = bounding_min.x; x_test <= bounding_max.x; ++x_test) {
            Vector2D p = Vector2D(x_test + 0.5, y_test + 0.5);
            if (triangle_test_point_inbound(p, vertices)) {
                seen_inbound = true;
                SampleParams params;
                Vector3D coord = get_barycentric_coordinates(p, vertices);
                params.p_uv = Vector2D(u0, v0) * coord.x + Vector2D(u1, v1) * coord.y + Vector2D(u2, v2) * coord.z;

                coord = get_barycentric_coordinates(Vector2D(p.x + 1, p.y), vertices);
                params.p_dx_uv = Vector2D(u0, v0) * coord.x + Vector2D(u1, v1) * coord.y + Vector2D(u2, v2) * coord.z;

                coord = get_barycentric_coordinates(Vector2D(p.x, p.y + 1), vertices);
                params.p_dy_uv = Vector2D(u0, v0) * coord.x + Vector2D(u1, v1) * coord.y + Vector2D(u2, v2) * coord.z;

                params.psm = psm;
                params.lsm = lsm;
                Color c = tex.sample(params);
                fill_sample_pixel(x_test, y_test, c);
            } else {
                if (seen_inbound)
                    break;
            }
        }
    }
}

void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;


    this->sample_buffer.resize(width * height * rate, Color::White);
}


void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
                                           size_t width, size_t height)
{
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;


    this->sample_buffer.resize(width * height, Color::White);
}


void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
}


// This function is called at the end of rasterizing all elements of the
// SVG file.  If you use a supersample buffer to rasterize SVG elements
// for antialising, you could use this call to fill the target framebuffer
// pixels from the supersample buffer data.x
//
void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support
    int step_size = sqrt(sample_rate);

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            int x_bound = x * step_size;
            int y_bound = y * step_size;

            float r = 0; float g = 0; float b = 0;
            for (int x_actual = x_bound;
                 x_actual < x_bound + step_size;
                 ++x_actual) {

                for (int y_actual = y_bound;
                     y_actual < y_bound + step_size;
                     ++y_actual) {

                    Color sample_color = sample_buffer[
                        y_actual * width * step_size + x_actual
                    ];
                    r += sample_color.r;
                    g += sample_color.g;
                    b += sample_color.b;
                }
            }
            Color col = Color(r / sample_rate, g / sample_rate, b / sample_rate);


            for (int k = 0; k < 3; ++k) {
                this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
            }
        }
    }

}

Rasterizer::~Rasterizer() { }


}// CGL

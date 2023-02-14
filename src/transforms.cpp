#include "transforms.h"

#include "CGL/matrix3x3.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v) {
	Vector3D mv = m * Vector3D(v.x, v.y, 1);
	return Vector2D(mv.x / mv.z, mv.y / mv.z);
}

Matrix3x3 translate(float dx, float dy) {
	// Part 3: Fill this in.
    Matrix3x3 transform_m = Matrix3x3(0, 0, dx, 0, 0, dy, 0, 0, 1);
	return transform_m;
}

Matrix3x3 scale(float sx, float sy) {
	// Part 3: Fill this in.
    Matrix3x3 transform_m = Matrix3x3(sx, sy, 0, sx, sy, 0, 0, 0, 1);
	return transform_m;
}

// The input argument is in degrees counterclockwise
Matrix3x3 rotate(float deg) {
	// Part 3: Fill this in.


	return Matrix3x3();
}

}

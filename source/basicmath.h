#ifndef BASICMATH_H
#define BASICMATH_H
#include <stdio.h>
#include <math.h>

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))

struct Vector3f
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
};

struct Vertex {
    Vector3f pos;
    Vector3f color;

    Vertex() {}

    Vertex(float x, float y, float z)
    {
        pos = Vector3f(x, y, z);

        float red = (float)rand() / (float)RAND_MAX;
        float green = (float)rand() / (float)RAND_MAX;
        float blue = (float)rand() / (float)RAND_MAX;
        color = Vector3f(red, green, blue);
    }

    Vertex(float x, float y, float z, float _red, float _green, float _blue)
    {
        pos = Vector3f(x, y, z);

        float red = _red;
        float green = _green;
        float blue = _blue;
        color = Vector3f(red, green, blue);
    }
};

struct Cube {
    Vertex Vertices[8];

    Cube()
    {
        Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);
        Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f);
        Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f);
        Vertices[3] = Vertex(0.5f, -0.5f, -0.5f);
        Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
        Vertices[5] = Vertex(0.5f, 0.5f, -0.5f);
        Vertices[6] = Vertex(0.5f, -0.5f, 0.5f);
        Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f);
    }

    Cube(float _red, float _green, float _blue)
    {
        Vertices[0] = Vertex(0.5f, 0.5f, 0.5f, _red, _green, _blue);
        Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f, _red, _green, _blue);
        Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f, _red, _green, _blue);
        Vertices[3] = Vertex(0.5f, -0.5f, -0.5f, _red, _green, _blue);
        Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f, _red, _green, _blue);
        Vertices[5] = Vertex(0.5f, 0.5f, -0.5f, _red, _green, _blue);
        Vertices[6] = Vertex(0.5f, -0.5f, 0.5f, _red, _green, _blue);
        Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f, _red, _green, _blue);
    }

    Vertex* vertexBuffer()
    {
        return Vertices;
    }
};

class Matrix4f
{
public:
    float m[4][4];

    Matrix4f() {}

    Matrix4f(float a00, float a01, float a02, float a03,
             float a10, float a11, float a12, float a13,
             float a20, float a21, float a22, float a23,
             float a30, float a31, float a32, float a33)
    {
        m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
        m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
        m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
        m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
    }

    inline Matrix4f operator*(const Matrix4f& Right) const
    {
        Matrix4f Ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                              m[i][1] * Right.m[1][j] +
                              m[i][2] * Right.m[2][j] +
                              m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }
};

#endif // BASICMATH_H
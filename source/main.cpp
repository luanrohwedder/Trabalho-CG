#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include "basicmath.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

GLuint VBO;
GLuint IBO;
GLuint Cube1;
GLuint Cube2;
GLuint Cube3;

GLuint VBO2;
GLuint VBO3;
GLuint IBO2;
GLuint IBO3;
GLint gWorldLocation;

static void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

#ifdef _WIN64
    Scale += 0.001f;
#else
    Scale += 0.002f;
#endif
    float FOV = 90.0f;
    float tanHalfFOV = tanf(ToRadian(FOV / 2.0f));
    float d = 1 / tanHalfFOV;

    float ar = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    float NearZ = 0.5f;
    float FarZ = 300.0f;

    float zRange = NearZ - FarZ;

    float A = (-FarZ - NearZ) / zRange;
    float B = 2.0f * FarZ * NearZ / zRange;

    {
        Matrix4f Rotation(cosf(Scale), 0.0f, -sinf(Scale), 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          sinf(Scale), 0.0f, cosf(Scale), 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Translation(1.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 3.5f,
                             0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Projection(d / ar, 0.0f, 0.0f, 0.0f,
                            0.0f, d, 0.0f, 0.0f,
                            0.0f, 0.0f, A, B,
                            0.0f, 0.0f, 1.0f, 0.0f);

        Matrix4f FinalMatrix = Projection * Translation * Rotation;

        glBindVertexArray(Cube1);
        glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    {
        Matrix4f Rotation(cosf(Scale), 0.0f, -sinf(Scale), 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          sinf(Scale), 0.0f, cosf(Scale), 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Translation(1.0f, 0.0f, 0.0f, 1.0f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 1.5f,
                             0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Projection(d / ar, 0.0f, 0.0f, 0.0f,
                            0.0f, d, 0.0f, 0.0f,
                            0.0f, 0.0f, A, B,
                            0.0f, 0.0f, 1.0f, 0.0f);

        Matrix4f FinalMatrix = Projection * Translation * Rotation;

        glBindVertexArray(Cube2);
        glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    {
        Matrix4f Rotation(cosf(-Scale), 0.0f, -sinf(-Scale), 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          sinf(-Scale), 0.0f, cosf(-Scale), 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Translation(1.0f, 0.0f, 0.0f, -1.5f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 2.5f,
                             0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4f Projection(d / ar, 0.0f, 0.0f, 0.0f,
                            0.0f, d, 0.0f, 0.0f,
                            0.0f, 0.0f, A, B,
                            0.0f, 0.0f, 1.0f, 0.0f);

        Matrix4f FinalMatrix = Projection * Translation * Rotation;

        glBindVertexArray(Cube3);
        glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glutPostRedisplay();

    glutSwapBuffers();
}

GLint LoadShader(const string &filename, GLenum shaderType)
{
    char const *p;
    GLint res;
    string shaderCode;
    ifstream shaderStream(filename, std::ios::in);

    if (shaderStream.good() == false)
    {
        fprintf(stderr, "Error Opening!\n");
        exit(0);
        
    }
    else 
    {
        shaderCode.assign((std::istreambuf_iterator<char>(shaderStream)),
						  (std::istreambuf_iterator<char>()));
    }

    GLuint shaderID = glCreateShader(shaderType);

    if (shaderID == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", shaderType);
        exit(0);
    }

    p = shaderCode.c_str();

    glShaderSource(shaderID, 1, &p, nullptr);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &res);

    if(!res)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, InfoLog);
        exit(1);
    }

    return shaderID;
}

static void LinkShader(GLint vertexShaderID, GLint fragmentShaderID)
{
    GLint shaderProgram = glCreateProgram();

    if (shaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    glAttachShader(shaderProgram, vertexShaderID);
    glAttachShader(shaderProgram, fragmentShaderID);

    glLinkProgram(shaderProgram);

    GLint res = 0;
    GLchar errorLog[1024] = { 0 };

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &res);
    if (res == 0)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", errorLog);
        exit(1);
    }

    gWorldLocation = glGetUniformLocation(shaderProgram, "gWorld");
    if (gWorldLocation == -1)
    {
        printf("Error getting uniform location of 'gScale'\n");
        exit(1);
    }

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &res);
    if (!res) {
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", errorLog);
        exit(1);
    }

    glUseProgram(shaderProgram);
}

static void CreateVertexBuffers()
{
    Cube cube_red = Cube(1, 0, 0);
    Cube cube_blue = Cube(0, 1, 0);
    Cube cube_color = Cube();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_red.Vertices), cube_red.Vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_blue.Vertices), cube_blue.Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color.Vertices), cube_color.Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffers()
{
    unsigned int Indices[] = {
                              0, 1, 2,
                              1, 3, 4,
                              5, 6, 3,
                              7, 3, 6,
                              2, 4, 7,
                              0, 7, 6,
                              0, 5, 1,
                              1, 5, 3,
                              5, 0, 6,
                              7, 4, 3,
                              2, 1, 4,
                              0, 2, 7
    };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &IBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void SetupVertexArrays()
{
    glGenVertexArrays(1, &Cube1);
    glGenVertexArrays(1, &Cube2);
    glGenVertexArrays(1, &Cube3);

    glBindVertexArray(Cube1);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBindVertexArray(Cube2);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO3);

    glBindVertexArray(Cube3);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO3);
}

int main(int argc, char** argv)
{
    srandom(3);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Hello World!");

    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);

    CreateVertexBuffers();
    CreateIndexBuffers();
    SetupVertexArrays();
    
    GLint vertexShaderID = LoadShader("./shaders/hello.vp", GL_VERTEX_SHADER);
    GLint fragmentsShaderID = LoadShader("./shaders/hello.fp", GL_FRAGMENT_SHADER);
    LinkShader(vertexShaderID, fragmentsShaderID);

    glutDisplayFunc(RenderScene);
    glutMainLoop();

    return 0;
}
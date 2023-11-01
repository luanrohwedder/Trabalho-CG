#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

GLuint VBO;
GLint gTranslationLocation;

struct Vertices {
    float x, y, z;
};

struct Matrix4f {
    float data[4][4];
};

static Matrix4f CreateMatrix4f(float a00, float a01, float a02, float a03,
                               float a10, float a11, float a12, float a13,
                               float a20, float a21, float a22, float a23,
                               float a30, float a31, float a32, float a33)
{
    Matrix4f m;

    m.data[0][0] = a00; m.data[0][1] = a01; m.data[0][2] = a02; m.data[0][3] = a03;
    m.data[1][0] = a10; m.data[1][1] = a11; m.data[1][2] = a12; m.data[1][3] = a13;
    m.data[2][0] = a20; m.data[2][1] = a21; m.data[2][2] = a22; m.data[2][3] = a23;
    m.data[3][0] = a30; m.data[3][1] = a31; m.data[3][2] = a32; m.data[3][3] = a33;

    return m;
}

static void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Angle = 0.0f;
    static float Delta = 0.01f;

    Angle += Delta;
    if ((Angle >= 1.5708f) || (Angle <= -1.5708f))
    {
        Delta *= -1.0f;
    }

    Matrix4f Translation = CreateMatrix4f(cos(Angle), -sin(Angle), 0.0f, 0.0f,
                                          sin(Angle), cos(Angle), 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0,
                                          0.0f, 0.0f, 0.0f, 1.0f);

    glUniformMatrix4fv(gTranslationLocation, 1, GL_TRUE, &Translation.data[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

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

    gTranslationLocation = glGetUniformLocation(shaderProgram, "gTranslation");
    if (gTranslationLocation == -1)
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
    

    Vertices vertices[3];

    vertices[0].x = -1.0f;
    vertices[0].y = -1.0f;
    vertices[0].z = 0.0f;

    vertices[1].x = 1.0f;
    vertices[1].y = -1.0f;
    vertices[1].z = 0.0f;

    vertices[2].x = 0.0f;
    vertices[2].y = 1.0f;
    vertices[2].z = 0.0f;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Hello World!");

    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    CreateVertexBuffers();
    
    GLint vertexShaderID = LoadShader("./shaders/hello.vp", GL_VERTEX_SHADER);
    GLint fragmentsShaderID = LoadShader("./shaders/hello.fp", GL_FRAGMENT_SHADER);
    LinkShader(vertexShaderID, fragmentsShaderID);

    glutDisplayFunc(RenderScene);
    glutMainLoop();

    return 0;
}
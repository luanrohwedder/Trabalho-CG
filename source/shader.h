#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class Shader
{
private:
    std::string getFileContent(std::string filepath)
    {
        std::string fileContent;
        std::ifstream file;

        file.exceptions (std::ifstream::badbit);

        try
        {
            file.open(filepath);
            std::stringstream fileStream;
            fileStream << file.rdbuf();
            file.close();
            fileContent = fileStream.str();
        }
        catch(std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        return fileContent;
    }

    void checkErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

public:
    unsigned int ID;
    
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        GLuint vertex, fragment;

        std::string vFile, fFile;

        vFile = getFileContent(vertexPath);
        fFile = getFileContent(fragmentPath);

        const char* verteShaderCode = vFile.c_str();
        const char* fragmentShaderCode = fFile.c_str();

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &verteShaderCode, nullptr);
        glCompileShader(vertex);
        checkErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
        glCompileShader(fragment);
        checkErrors(fragment, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void use()
    {
        glUseProgram(ID);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

    GLint getWorldPos() const
    {
        return glGetUniformLocation(ID, "gWorld");
    }
};
#endif
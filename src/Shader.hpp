#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr GLuint INVALID_HANDLE = std::numeric_limits<decltype(INVALID_HANDLE)>::max();

struct Shader{
    Shader(const nlohmann::json&);
    Shader() {}
    void Use() const;
    void Destroy();
    
    /**
     Inform the shader program about the canvas size
     @param width canvas width in pixels
     @param height canvas height in pixels
     */
    void SetWindowUniform(int width, int height) const;
    void SetTimeUniform(float time) const;

    GLuint windowSizeUniform = INVALID_HANDLE;
    GLuint timeUniform = INVALID_HANDLE;
    GLuint programHandle = INVALID_HANDLE;
    struct Sampler {
        GLuint sampler = INVALID_HANDLE;
        GLuint texhandle = INVALID_HANDLE;
        GLuint type = INVALID_HANDLE;
    };
    Sampler iChannel[4];
    uint8_t nsamplers = 0;
};

struct Renderer {
    Renderer();

    void SetupShader(const nlohmann::json& shadertoyData);

    void Draw();
    void UpdateResolution(int, int) const;

    void SetTime(float time) const;
private:
    GLuint VBO, VAO;

    Shader mainImageShader;
};
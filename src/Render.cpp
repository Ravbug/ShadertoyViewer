#include "Shader.hpp"
#include "Utility.hpp"
using namespace nlohmann;

Renderer::Renderer() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -10.f, -10.f, 0.0f, // left
         10.f, -10.f, 0.0f, // right
         0.0f,  10.f, 0.0f  // top
    };

   
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::UpdateResolution(int w, int h) const{
    mainImageShader.Use();
    mainImageShader.SetWindowUniform(w, h);
}

void Renderer::SetTime(float time) const {
    mainImageShader.Use();
    mainImageShader.SetTimeUniform(time);
}

void Renderer::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    mainImageShader.Use();
    for (uint8_t i = 0; i < mainImageShader.nsamplers; i++) {
        const auto& h = mainImageShader.iChannel[i];
        glBindTexture(h.type, h.handle);
    }
    // init triangle and draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


void Renderer::SetupShader(const nlohmann::json& shadertoyData)
{
    mainImageShader.Destroy();
    mainImageShader = Shader(shadertoyData["Shader"]["renderpass"][0]);
}

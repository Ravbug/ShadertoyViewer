#include "Shader.hpp"
#include <vector>
#include "Utility.hpp"
#include <fmt/format.h>
#include "ImgCache.hpp"


static GLuint vertexShader = INVALID_HANDLE;

using namespace std;
using namespace nlohmann;

/**
 Helper to get the compile error for a shader
 */
static vector<char> getShaderCompileError(GLuint shader){
    int maxLength = 0, infoLog = 0;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
    vector<char> log(maxLength);
    glGetShaderInfoLog(shader,maxLength,&infoLog,log.data());
    return log;
}

Shader::Shader(const json& src){
    programHandle = glCreateProgram();
    GLint shaderCompiled = GL_FALSE;
    // reuse the vertex shader program
    if (vertexShader == INVALID_HANDLE){
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* const vertShaderSrc[] ={
           R"(
                #version 140
                in vec2 a_position;
                void main(){
                    gl_Position = vec4( a_position.x, a_position.y, 0, 1 );
                }
           )"
        };
        glShaderSource(vertexShader,1,vertShaderSrc,NULL);
        
        // attempt to compile the shader
        glCompileShader(vertexShader);
        
        glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &shaderCompiled );
        if (!shaderCompiled){
            auto log = getShaderCompileError(vertexShader);
            Fatal("Failed to compile vertex shader: {}", log.data());
        }
    }
    // add vert shader to program
    glAttachShader(programHandle,vertexShader);
    
    std::string samplersString = "";
    std::string samplerNames[4];
    auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        nsamplers = 0;
        for (const auto& item : src["inputs"]) {
            const char* samplerType = nullptr;
            const std::string& type = item["ctype"];
            bool mipmap = item["sampler"]["filter"] == "mipmap";
            if (type == "texture") {
                samplerType = "2D";
                iChannel[nsamplers].texhandle = ImageCache::GetTexture(item["src"],mipmap);
                iChannel[nsamplers].type = GL_TEXTURE_2D;
            }
            else if (type == "cubemap") {
                samplerType = "3D";
                iChannel[nsamplers].texhandle = ImageCache::GetCubemap(item["src"],mipmap);
                iChannel[nsamplers].type = GL_TEXTURE_CUBE_MAP;
            }
            samplerNames[nsamplers] = fmt::format("iChannel{}", item["channel"].get<uint8_t>());
            samplersString.append(fmt::format("uniform sampler{} {};\n", samplerType, samplerNames[nsamplers]));
          
            nsamplers++;
        }
    }
   
    
    const char* fs_header = R"(
        #version 140
        out vec4 color;
        uniform vec3 iResolution;
        uniform float iTime;
        uniform float iTimeDelta;
        uniform float iFrame;
        uniform float iChannelTime[4];
        uniform vec4 iMouse;
        uniform vec4 iDate;
        uniform float iSampleRate;
        uniform vec3 iChannelResolution[4];
    )";
    const char* fs_footer = R"(
        void main(){
            mainImage(color, gl_FragCoord.xy);
        }
    )";
	
	const std::string& sv = src["code"];
	
    // get the fragment shader
    const GLchar* const fragShaderSrc[] = {
        fs_header,
        samplersString.c_str(),
		sv.data(),
        fs_footer
    };
    glShaderSource(fragShader, sizeof(fragShaderSrc)/sizeof(fragShaderSrc[0]), fragShaderSrc, NULL);
    glCompileShader(fragShader);
    glGetShaderiv( fragShader, GL_COMPILE_STATUS, &shaderCompiled );
    if (!shaderCompiled){
        auto log = getShaderCompileError(fragShader);
        Fatal("Failed to compile fragment shader: {}", log.data());
    }
    // add frag shader to program
    glAttachShader(programHandle, fragShader);
    
    // link program
    glLinkProgram(programHandle);
    glGetProgramiv( programHandle, GL_LINK_STATUS, &shaderCompiled );
    if (!shaderCompiled){
        //auto log = getShaderCompileError()
        Fatal("Failed to link shader");
    }
    glDeleteShader(fragShader); // don't need this anymore
    
    // get the window size uniform (need to do this for every program)
    struct uniformSearchItem {
        const char* const name;
        GLuint& uniform;
    } items[] = { 
        {"iResolution",windowSizeUniform}, 
        {"iTime",timeUniform},
    };

    for (const auto& item : items) {
        auto loc = glGetUniformLocation(programHandle, item.name);
        if (loc >= 0) {
            item.uniform = loc;
        }
    }

    // sampler uniforms
    for (int i = 0; i < nsamplers; i++) {
        auto loc = glGetUniformLocation(programHandle, samplerNames[i].c_str());
        Assert(loc >= 0,"Failed to locate sampler {}",samplerNames[i]); // these must exist
        iChannel[i].sampler = loc;
    }
}

void Shader::Destroy(){
    glDeleteProgram(programHandle);
    programHandle = INVALID_HANDLE;
}

void Shader::Use() const{
    glUseProgram(programHandle);
}

void Shader::SetWindowUniform(int width, int height) const{
    if (windowSizeUniform != INVALID_HANDLE){
        glUniform3f(windowSizeUniform, width, height,0);
    }
}

void Shader::SetTimeUniform(float time) const{
    if (timeUniform != INVALID_HANDLE){
        glUniform1f(timeUniform,time);
    }
}

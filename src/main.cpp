#include "Shader.hpp"
#include "Utility.hpp"
#include <SDL.h>
#include <SDL_events.h>
#include <string>
#include <chrono>
#include "SaveImg.hpp"
#include "NativeDialog.hpp"
#include <thread>
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include "key.hpp"      // create this file with your Shadertoy API key!

using namespace std;

void Screenshot(const std::string& name){
#if 0
    // get output location
    NFD::UniquePathU8 path;
    nfdfilteritem_t filterItem[1] = { { "PNG Screenshot", "png" }};
    auto res = NFD::SaveDialog(path,filterItem,1,nullptr,name.c_str());
    if (res == NFD_OKAY){
        int winWidth, winHeight;
        SDL_GL_GetDrawableSize(win, &winWidth, &winHeight);
        vector<GLubyte> imgData(winWidth * winHeight * 4);  // RGBA
        glReadPixels(0, 0, winWidth, winHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, imgData.data());
        SavePNG(imgData, winWidth, winHeight, path.get());
        Log("Saved Screenshot {}/{}\n",path.get(), name);
    }
#endif
}

static char ShaderInputBuffer[7] = "";


int main(int argc, char** argv){
    
    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO)){
        Fatal("Could not init SDL2: {}", SDL_GetError());
    }
    
    // set context config: OpenGL 3.2 Core Profile
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // create window
    win = SDL_CreateWindow("FractalViewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_INIT_EVENTS);
    if (!win){
        Fatal("Could not create Window");
    }
    
    // create context
    auto glContext = SDL_GL_CreateContext(win);
    if (!glContext){
        Fatal("Could not create OpenGL context: {}", SDL_GetError());
    }
    // make context current
    SDL_GL_MakeCurrent(win, glContext);
    
    // initialize glew
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK){
        Fatal("Error initializing GLEW: ", glewGetErrorString(glewError));
    }
    
    // log OpenGL info
    const auto GPUNAME = glGetString(GL_RENDERER);
    {
        Log("Render info:\n - Version: {} (GLSL {})\n - GPU: {}\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION), GPUNAME);
    }
    
    // use vsync
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ){
        Fatal("Coult not set vsync: ", SDL_GetError());
    }
    
    // init OpenGL and imgui
    glClearColor( 0.f, 0.f, 0.f, 1.f ); // clear with black

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    ImGui_ImplSDL2_InitForOpenGL(win, glContext);
    ImGui_ImplOpenGL3_Init("#version 140");
    ImGui::StyleColorsDark();

    // initialize renderer
    Renderer renderer;
    
    bool exit = false;
    
    auto startTime = std::chrono::high_resolution_clock::now();

    // setting the size uniform
    auto updateSize = [&]{
        int winWidth, winHeight;
        SDL_GL_GetDrawableSize(win, &winWidth, &winHeight);
        renderer.UpdateResolution(winWidth, winHeight);
    };
    updateSize();
    SDL_Event event;
    while (!exit){
        //Bind program
        // set time uniform
        auto fbgin = std::chrono::high_resolution_clock::now();
        auto timeSince = std::chrono::duration_cast<std::chrono::duration<float>>(fbgin - startTime).count();
        while (SDL_PollEvent(&event)){
            switch (event.type) {
                case SDL_QUIT:
                    exit = true;
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
#ifndef __APPLE__
                            // required on Windows but breaks things on mac
                            glViewport(0, 0, event.window.data1, event.window.data2);
#endif
                            updateSize();
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_R:
                            // reset the clock
                            startTime = std::chrono::high_resolution_clock::now();
                            break;
                        case SDL_SCANCODE_P:
                            // take a screenshot
                            Screenshot(fmt::format("ShadertoyViewer_{:.2f}s.png",timeSince));
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            // forward to imgui
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        
        // execute shadertoy shader
        renderer.SetTime(timeSince);
        
        renderer.Draw();

        // draw GUI
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            
            ImGui::Begin("Load Shadertoy");
            
            ImGui::InputText("Code",ShaderInputBuffer,sizeof(ShaderInputBuffer));
            if (ImGui::Button("Go!")) {
                auto str = fetch(fmt::format("https://www.shadertoy.com/api/v1/shaders/{}?key={}", ShaderInputBuffer, API_KEY));   // TODO: input the url from GUI
                if (str.code != 200) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading shader", fmt::format("Error {}: {}", str.code, str.result).c_str(), win);
                }
                else {
                    auto json = nlohmann::json::parse(str.result);
                    if (json.contains("Error")) {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading shader", fmt::format("{}", json["Error"]).c_str(), win);
                    }
                    else {
                        renderer.SetupShader(json);
                        updateSize();
                    } 
                }
            }
            ImGui::End();
            

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
                
        // present & wait for vblank
        SDL_GL_SwapWindow(win);
        auto frameTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - fbgin).count();

        SDL_SetWindowTitle(win, fmt::format("ShadertoyViewer t = {:.2f}s {:.0f} fps - {}", timeSince, 1/frameTime, GPUNAME).c_str());

    }

    // deinit
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}

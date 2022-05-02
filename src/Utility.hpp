#pragma once
#include <SDL_messagebox.h>
#include <fmt/format.h>
#include <stdio.h>

static SDL_Window* win = nullptr;

template<typename T, typename ... arg_t>
void Fatal(T reason, arg_t ... args){
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", fmt::format(reason, args...).c_str(), win);
    throw std::runtime_error(fmt::format(reason, args...));
}

template<typename T, typename ... arg_t>
void Assert(bool value, T formatstr, arg_t ... args){
    if (!value){
        Fatal(formatstr,args...);
    }
}

template<typename T, typename ... arg_t>
void Log(T str, arg_t ... args){
    printf("%s",fmt::format(str, args...).c_str());
}

struct fetchResult {
    std::string result;
    long code;
};
fetchResult fetch(const std::string& url);
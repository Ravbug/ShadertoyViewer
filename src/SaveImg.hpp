#pragma once
#include <vector>
#include <GL/glew.h>
#include <filesystem>

void SavePNG(const std::vector<GLubyte>& data, int width, int height, const std::filesystem::path& path);

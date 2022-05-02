#include "ImgCache.hpp"
#include <stb_image.h>
#include "Utility.hpp"
#include <GL/GL.h>

decltype(ImageCache::cache) ImageCache::cache;

unsigned int ImageCache::Get(const std::string& path) {
	auto it = cache.find(path);
	if (it != cache.end()) {
		return (* it).second;
	}
	// not in the cache, need to make it

    auto res = fetch(fmt::format("https://www.shadertoy.com{}", path));
    Assert(res.code == 200, "Shadertoy API failed: {}", res.code);

    int width, height, channels;
    auto img = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res.result.c_str()), res.result.size(), &width, &height, &channels, 4);

    // make opengl texture
    GLuint texhandle;
    glGenTextures(1, &texhandle);
    glBindTexture(GL_TEXTURE_2D, texhandle);
    // set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);

    cache[path] = texhandle;

    return texhandle;
}

void ImageCache::Clear() {
    for (const auto& item : cache) {
       glDeleteTextures(1,&item.second);
    }
    cache.clear();
}
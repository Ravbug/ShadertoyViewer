#include "ImgCache.hpp"
#include <stb_image.h>
#include "Utility.hpp"

decltype(ImageCache::textureCache) ImageCache::textureCache;
decltype(ImageCache::cubemapCache) ImageCache::cubemapCache;

struct imdata {
    stbi_uc* data;
    int width, height, channels;
};

static imdata DownloadImage(const std::string& path) {
    // not in the cache, need to make it

    auto res = fetch(fmt::format("https://www.shadertoy.com{}", path));
    Assert(res.code == 200, "Shadertoy API failed: {}", res.code);

    int width, height, channels;
    auto img = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res.result.c_str()), res.result.size(), &width, &height, &channels, 4);

    return {img,width,height,channels};
}

unsigned int ImageCache::GetTexture(const std::string& path, bool mipmap) {
	auto it = textureCache.find(path);
	if (it != textureCache.end()) {
		return (* it).second;
	}
	
    auto img = DownloadImage(path);

    // make opengl texture
    GLuint texhandle;
    glGenTextures(1, &texhandle);
    glBindTexture(GL_TEXTURE_2D, texhandle);
    // set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    if (mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(img.data);

    textureCache[path] = texhandle;

    return texhandle;
}

unsigned int ImageCache::GetCubemap(const std::string& path, bool mipmap) {
    auto it = cubemapCache.find(path);
    if (it != cubemapCache.end()) {
        return (*it).second;
    }

    GLuint texhandle;
    glGenTextures(1, &texhandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texhandle);
    
    auto img = DownloadImage(path);

    GLuint faces[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X ,GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    // set all the faces
    for (const auto f : faces) {
        glTexImage2D(f, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(img.data);
}

void ImageCache::Clear() {
    auto& clear = [](auto& cache) {
        for (const auto& item : cache) {
            glDeleteTextures(1, &item.second);
        }
        cache.clear();
    };

    clear(textureCache);
    clear(cubemapCache);
}

#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>

// a simple read-through cache
struct ImageCache {
	ImageCache() = delete;

	// load the texture from shadertoy, or return the cached handle if present
	static GLuint GetTexture(const std::string& path, bool mipmap);
	static GLuint GetCubemap(const std::string& path, bool mipmap);

	// remove everything from the cache
	static void Clear();
private:
	static std::unordered_map<std::string, unsigned int> textureCache, cubemapCache;
};
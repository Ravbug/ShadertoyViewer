#include "SaveImg.hpp"
#include <png.h>
#include "Utility.hpp"

void SavePNG(const std::vector<GLubyte>& data, int width, int height, const std::filesystem::path& path){
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    Assert(png, "PNG write struct allocate failed");
    
    png_infop info = png_create_info_struct(png);
    Assert(info, "PNG info struct allocate failed");
    
    if(setjmp(png_jmpbuf(png))){
        Assert(false, "PNG setjmp failed");
    }
    
    FILE *fp = fopen(path.string().c_str(), "wb");
    
    png_init_io(png, fp);
    
    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_set_compression_level(png, 9);  // best compression
    
    png_write_info(png, info);
    
    // for RGB type, need filler for alpha channel
    //png_set_filler(png, 0, PNG_FILLER_AFTER);
    
    std::vector<const GLubyte*> rowPointers((data.size() / 4) / width);
    for(int i = 0; i < height; i++){
        rowPointers[height - i - 1] = &data[i * width * 4];
    }
    
    png_set_rows(png, info, const_cast<GLubyte**>(rowPointers.data()));
    png_write_png(png, info, PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_BGR, NULL);
    png_write_end(png, NULL);
    
    fclose(fp);
    
    png_destroy_write_struct(&png, &info);
}


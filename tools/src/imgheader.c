/* imgheader.h -- https://github.com/takeiteasy/ceelo
 
 The MIT License (MIT)

 Copyright (c) 2022 George Watson

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_GIF
#include "stb_image.h"
#include "common.h"

static const char *ValidExtensions[10] = {
    "jpg", "jpeg", "png", "bmp", "psd", "tga", "hdr", "pic", "ppm", "pgm"
};

static void usage(void) {
    printf("usage: imgheader <in>\n");
    printf("\tValid extensions:\n");
    for (int i = 0; i < 10; i++)
        printf("\t - %s\n", ValidExtensions[i]);
}

int main(int argc, const char *argv[]) {
    BAIL(argc == 2, "Incorrect arguments: %d, expected 1", argc - 1);
    int ValidExt = 0;
    const char *ext = FileExt(argv[1]);
    int extLength = strlen(ext);
    for (int i = 0; i < 10; i++)
        if (!strncmp(ext, ValidExtensions[i], extLength)) {
            ValidExt = 1;
            break;
        }
    BAIL(ValidExt, "Invalid extension: %s", ext);
    BAIL(!access(argv[1], F_OK), "File doesn't exist at: \"%s\"", argv[1]);
    
#define PSET(X, Y, R, G, B, A) \
    newData[y * w + x] = ((unsigned int)(A) << 24) | ((unsigned int)(R) << 16) | ((unsigned int)(G) << 8) | (B)
    int w, h, n;
    unsigned char *data = stbi_load(argv[1], &w, &h, &n, 4);
    BAIL(data && w > 0 && h > 0, "Failed to load image at: \"%s\"", argv[1]);
    int *newData = malloc(sizeof(int) * w * h);
    BAIL(newData, "%s", "Out of memory");
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            unsigned char *p = data + (x + w * y) * n;
            PSET(x, y, p[0], p[1], p[2], n == 4 ? p[3] : 255);
        }
    
    char outPath[512];
    int pathLength = strlen(argv[1]);
    memcpy(outPath, argv[1], sizeof(char) * pathLength);
    const char *outExt = ".h";
    memcpy(outPath + pathLength, outExt, sizeof(char) * 3);
    outPath[pathLength + 3] = '\0';
    const char *outName = FileName(argv[1]);
    
    printf("// Generated by imgheader.c -- https://github.com/takeiteasy/\n"
           "\n"
           "#ifndef __IMG__%s__H__\n"
           "#define __IMG__%s__H__\n",
           outName, outName);
    const int size = w * h;
    printf("static const float img_%s_data[%d] = {", outName, size);
    for (int i = 0, j = 0; i < size; i++, j++) {
        if (!j)
            printf("\n\t");
        printf("0x%08x%s", newData[i], i == size - 1 ? ",\n};\n" : ", ");
        if (j == 2)
            j = -1;
    }
    printf("static const int img_%s_data_sz = %d;\n", outName, size);
    printf("#endif // __IMG__%s__H__\n", outName);
    return 0;
}

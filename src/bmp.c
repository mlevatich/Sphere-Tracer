#include "../headers/raytrace.h"

// Load a bmp file into memory as an array
uint8_t* load_texture(const char* path, BITMAPINFOHEADER *bitmapInfoHeader)
{
    //open file
    FILE* filePtr = fopen(path, "rb");
    if(!filePtr)
    {
        fprintf(stderr, "Error: texture %s not found\n", path);
        return NULL;
    }

    //read the bitmap file header
    BITMAPFILEHEADER bitmapFileHeader;
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fprintf(stderr, "Error: texture %s not a bmp file\n", path);
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

    //move file point to the beginning of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    uint8_t* bitmapImage = malloc(bitmapInfoHeader->biSizeImage);

    //read in the bitmap image data
    fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

    //swap the r and b values to get RGB (bitmap is BGR)
    for (int imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage-1; imageIdx += 3)
    {
        uint8_t tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}

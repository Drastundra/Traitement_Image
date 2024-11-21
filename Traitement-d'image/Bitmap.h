#pragma once
#include "Image.h"
#include <fstream>
#include <iostream>
#include <vector>

class Bitmap
{
public:

    Bitmap(const int& width = 0, const int& height = 0);

    int getHeaderSize() const;

    int getFileSize() const;

    int getImageSize() const;

    /* void setData(Rgb data, const int& size); */

    int getWidth() const;

    int getHeight() const;

    void setWidth(const int& width);

    void setHeight(const int& height);

    bool save(const std::string& filename);

    bool load(const std::string& filename);

    void setImage(const Image& image);

    Image& getImage();

private:

    int getPpm();

    void updateFileHeader();

    void updateInfoHeader();

    uint8 m_fileHeader[14] = {
        'B', 'M', // Id
        0, 0, 0, 0, // Size
        0, 0, // reserved
        0, 0, // reserved
        54, 0, 0, 0 // offset
    };

    uint8 m_infoHeader[40] = {
        40, 0, 0, 0, // header size
        0, 0, 0, 0, // width
        0, 0, 0, 0, // height
        1, 0,  // number of color plane
        24, 0, // bits per pixels
        0, 0, 0, 0, // compression default
        0, 0, 0, 0, // Image size
        0, 0, 0, 0, // horizontal resolution
        0, 0, 0, 0, // vertical resolution
        0, 0, 0, 0, // number of colors
        0, 0, 0, 0 // important colors without preferences
    };

    Image m_ImageData;
};
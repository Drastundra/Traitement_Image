#pragma once

#include "image.h"

class Image;

namespace processing
{
    void converTogrey(Image& image);

    void convertToBinary(Image& image, const uint8& threshold = 122);

    void reduce(Image& image, float reductionFactor, const Point& origin, const Rgb& fillColor);

    void zoom(Image& image, float zoomFactor, const Rgb& fillColor);
    void zoom(Image& image, float zoomFactor, const Point& origin, const Rgb& fillColor);

    void rotation(Image& image, float angleInDegree, const Point& center, const Rgb& fillColor);
    void rotation(Image& image, float angleInDegree, const Rgb& fillColor);

    void strechHistogram(Image& image);

    void meanFilter(Image& image, const int& width, const int& height);
    void medianFilter(Image& image, const int& width, const int& height);
}
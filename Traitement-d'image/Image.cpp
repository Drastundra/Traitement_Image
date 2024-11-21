#include "Image.h"
#include <iostream>

Rgb::Rgb(const uint8& r, const uint8& g, const uint8& b)
{
    channels[0] = b;
    channels[1] = g;
    channels[2] = r;
}

Image::Image(const int& width, const int& height)
    : m_width(width)
    , m_height(height)
{}

void Image::clear()
{
    m_data.clear();
}

void Image::setSize(const int& width, const int& height)
{
    m_width = width;
    m_height = height;
    m_data.resize(width * height);
}

bool Image::isInside(const int& row, const int& col) const
{
    if (row >= m_height || col >= m_width)
        return false;
    if (row < 0 || col < 0)
        return false;

    return true;
}

Rgb& Image::getPixel(const int& row, const int& col)
{
    if (row >= m_height || col >= m_width)
        std::cerr << "Out of range" << std::endl;

    return m_data[row * m_width + col];
}

const Rgb& Image::getPixel(const int& row, const int& col) const
{
    if (row >= m_height || col >= m_width)
        std::cerr << "Out of range" << std::endl;

    return m_data[row * m_width + col];
}

int Image::getWidth() const
{
    return m_width;
}

int Image::getHeight() const
{
    return m_height;
}

size_t Image::getSize() const
{
    return m_data.size();
}

Rgb& Image::operator[](const size_t& index)
{
    return m_data[index];
}

Point operator-(const Point& op1, const Point& op2)
{
    return Point(op1.x - op2.x, op1.y - op2.y);
}

Point operator+(const Point& op1, const Point& op2)
{
    return Point(op1.x + op2.x, op1.y + op2.y);
}

Point operator/(const Point& op1, float op2)
{
    return Point(op1.x / op2, op1.y / op2);
}

Point operator*(const Point& op1, float op2)
{
    return Point(op1.x * op2, op1.y * op2);
}
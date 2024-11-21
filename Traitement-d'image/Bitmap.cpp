#include "Bitmap.h"
#include "Image.h"
#include "Process.h"
#include <fstream>
#include <iostream>

Bitmap::Bitmap(const int& width, const int& height)
{
	setWidth(width);
	setHeight(height);
}

int Bitmap::getHeaderSize() const
{
	return sizeof(m_fileHeader) + sizeof(m_infoHeader);
}

int Bitmap::getFileSize() const
{
	return getHeaderSize() + getImageSize();
}

int Bitmap::getImageSize() const
{
	const auto imageSize = getWidth() * getHeight();
	return 3 * imageSize;
}

/*
void Bitmap::setData(Rgb* data, const int& size)
{
	m_data.clear();
	m_data.setSize(size);
	for (auto i = 0; i < size; ++i)
		m_data[i] = data[i];

	updateFileHeader();
	updateInfoHeader();

}
*/

int Bitmap::getWidth() const
{
	static constexpr auto startWidth = 4;
	static constexpr auto endWidth = startWidth + 4;
	int width = 0;
	for (auto i = endWidth - 1; i >= startWidth; --i)
		width += (m_infoHeader[i] << ((i - startWidth) * 8));

	return width;
}

int Bitmap::getHeight() const
{
	static constexpr auto startHeight = 8;
	static constexpr auto endHeight = startHeight + 4;
	int height = 0;
	for (auto i = endHeight - 1; i >= startHeight; --i)
		height += (m_infoHeader[i] << ((i - startHeight) * 8));

	return height;
}

void Bitmap::setWidth(const int& width)
{
	static constexpr auto startWidth = 4;
	for (auto i = startWidth; i < startWidth + 4; ++i)
		m_infoHeader[i] = static_cast<unsigned char>(width >> (i - startWidth) * 8);
}

void Bitmap::setHeight(const int& height)
{
	static constexpr auto startHeight = 8;
	for (auto i = startHeight; i < startHeight + 4; ++i)
		m_infoHeader[i] = static_cast<unsigned char>(height >> (i - startHeight) * 8);
}

bool Bitmap::save(const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);

	if (!file.is_open())
		return false;

	file.write(reinterpret_cast<char*>(&m_fileHeader[0]), 14);
	file.write(reinterpret_cast<char*>(&m_infoHeader[0]), 40);

	const auto width = getWidth();
	const auto height = getHeight();

	const int paddingAmount = (4 - (width * 3) % 4) % 4;
	for (int row = 0; row < getHeight(); ++row)
	{
		for (int col = 0; col < getWidth(); ++col)
			file.write(reinterpret_cast<const char*>(&m_ImageData.getPixel(row, col).channels[0]), 3);

		file.write("\0\0\0", paddingAmount); // Write padding bytes
	}

	file.close();
	return true;
}



bool Bitmap::load(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open())
		return false;

	file.read(reinterpret_cast<char*>(&m_fileHeader[0]), 14);
	file.read(reinterpret_cast<char*>(&m_infoHeader[0]), 40);

	const auto width = getWidth();
	const auto height = getHeight();

	m_ImageData.clear();
	m_ImageData.setSize(getWidth(), getHeight());

	const int paddingAmount = (4 - (width * 3) % 4) % 4;

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
			file.read(reinterpret_cast<char*>(&m_ImageData.getPixel(row, col).channels[0]), 3);

		char* padding = new char[paddingAmount];
		file.read(&padding[0], paddingAmount); // read padding bytes
		delete[] padding;
		padding = nullptr;
	}

	file.close();
	return true;
}

void Bitmap::setImage(const Image& image)
{
	m_ImageData = image;
	setWidth(m_ImageData.getWidth());
	setHeight(m_ImageData.getHeight());
}

Image& Bitmap::getImage()
{
	return m_ImageData;
}

int Bitmap::getPpm()
{
	return 72 * 39.3701;
}

void Bitmap::updateFileHeader()
{
	static const int start = 2;
	const auto fileSize = getFileSize();
	for (auto i = start; i < start + 4; ++i)
		m_fileHeader[i] = static_cast<uint8>(fileSize >> (i - start) * 8);
}

void Bitmap::updateInfoHeader()
{
	static const auto startImageSize = 20;
	const auto dataSize = getImageSize();
	for (auto i = startImageSize; i < startImageSize + 4; ++i)
		m_infoHeader[i] = static_cast<uint8>(dataSize >> (i - startImageSize) * 8);

	static const auto startPpm = startImageSize + 4;
	const auto ppm = getPpm();
	for (auto j = 0; j < 2; ++j)
	{
		const auto start = startPpm + 4 * j;
		for (auto i = start; i < startPpm + 4 * (j + 1); ++i)
			m_infoHeader[i] = static_cast<uint8>(ppm >> (i - start) * 8);
	}
}
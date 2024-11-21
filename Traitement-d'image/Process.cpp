#include "Process.h"
#include "Image.h"
#include <algorithm>
#include <math.h>

namespace processing
{
	void converTogrey(Image& image)
	{
		for (auto i = 0; i < image.getSize(); ++i)
		{
			int average = 0;
			for (auto& channel : image[i].channels)
				average += channel;

			average /= 3;

			for (auto channelIdx = 0; channelIdx < 3; ++channelIdx)
				image[i].channels[channelIdx] = static_cast<uint8>(average);
		}
	}

	void convertToBinary(Image& image, const uint8& threshold)
	{
		for (auto i = 0; i < image.getSize(); ++i)
		{
			int average = 0;
			for (auto& channel : image[i].channels)
				average += channel;

			average /= 3; // Moyenne pour évaluer le seuil global

			uint8 value = (average > threshold) ? std::numeric_limits<uint8>::max() : 0;

			for (auto& channel : image[i].channels)
				channel = value;
		}
	}

	void fill(Image& image, const Rgb& fillColor)
	{
		for (auto i = 0; i < image.getSize(); ++i)
		{
			image[i] = fillColor;
		}

	}

	void reduce(Image& image, float reductionFactor, const Point& origin, const Rgb& fillColor)
	{
		Image src = image;
		Image& dest = image;

		fill(dest, fillColor);

		for (int row = 0; row < image.getHeight(); ++row)
		{
			for (int col = 0; col < image.getWidth(); ++col)
			{
				Point destinationCoord = origin + (Point(col, row) - origin) / reductionFactor;
				if (dest.isInside(destinationCoord.y, destinationCoord.x))
					dest.getPixel(destinationCoord.y, destinationCoord.x) = src.getPixel(row, col);

			}
		}
	}

	void zoom(Image& image, float zoomFactor, const Rgb& fillColor)
	{
		Point midPoint(image.getWidth() / 2, image.getHeight() / 2);
		zoom(image, zoomFactor, midPoint, fillColor);
	}

	void zoom(Image& image, float zoomFactor, const Point& origin, const Rgb& fillColor)
	{
		Image src = image;
		Image& dest = image;

		for (int row = 0; row < image.getHeight(); ++row)
		{
			for (int col = 0; col < image.getWidth(); ++col)
			{
				Point destinationCoord(col, row);
				Point sourceCoord = origin + (destinationCoord - origin) / zoomFactor;
				if (src.isInside(sourceCoord.y, sourceCoord.x))
					dest.getPixel(destinationCoord.y, destinationCoord.x) = src.getPixel(sourceCoord.y, sourceCoord.x);
				else
					dest.getPixel(destinationCoord.y, destinationCoord.x) = fillColor;
			}
		}
	}

	float degToRad(float angle)
	{
		return (angle * 3.1415) / 180;
	}

	void rotation(Image& image, float angleInDegree, const Point& center, const Rgb& fillColor)
	{
		Image src = image;
		Image& dest = image;

		for (int row = 0; row < image.getHeight(); ++row)
		{
			for (int col = 0; col < image.getWidth(); ++col)
			{
				float cosa = cos(-degToRad(angleInDegree));
				float sina = sin(-degToRad(angleInDegree));

				Point Msrc(col, row);
				Msrc = Msrc - center;

				Point Mdest(Msrc.x * cosa - Msrc.y * sina,
					Msrc.x * sina + Msrc.y * cosa);

				Mdest = Mdest + center;


				Point destinationCoord(col, row);

				if (src.isInside(Mdest.y, Mdest.x))
					dest.getPixel(destinationCoord.y, destinationCoord.x) = src.getPixel(Mdest.y, Mdest.x);
				else
					dest.getPixel(destinationCoord.y, destinationCoord.x) = fillColor;
			}
		}
	}

	void rotation(Image& image, float angleInDegree, const Rgb& fillColor)
	{
		Point midPoint(image.getWidth() / 2, image.getHeight() / 2);
		rotation(image, angleInDegree, midPoint, fillColor);
	}

	void strechHistogram(Image& image)
	{
		Rgb max, min;
		const auto absoluteMax = std::numeric_limits<uint8>::max();
		const auto absoluteMin = std::numeric_limits<uint8>::min();

		for (auto& channel : max.channels)
			channel = absoluteMin;

		for (auto& channel : min.channels)
			channel = absoluteMax;

		// Trouver les max et min par canal
		for (auto i = 0; i < image.getSize(); ++i)
		{
			for (auto channelIdx = 0; channelIdx < Rgb::channel_count; ++channelIdx)
			{
				max.channels[channelIdx] = std::max(max.channels[channelIdx], image[i].channels[channelIdx]);
				min.channels[channelIdx] = std::min(min.channels[channelIdx], image[i].channels[channelIdx]);
			}
		}

		// Étirement
		for (auto i = 0; i < image.getSize(); ++i)
		{
			for (auto channelIdx = 0; channelIdx < Rgb::channel_count; ++channelIdx)
			{
				if (max.channels[channelIdx] != min.channels[channelIdx])
				{
					const int numerator = absoluteMax * (image[i].channels[channelIdx] - min.channels[channelIdx]);
					image[i].channels[channelIdx] = static_cast<uint8>(numerator / (max.channels[channelIdx] - min.channels[channelIdx]));
				}
				else
				{
					image[i].channels[channelIdx] = min.channels[channelIdx];
				}
			}
		}
	}

	struct Roi
	{
		Roi(const int& width_ = 0, const int& height_ = 0)
			: width(width_)
			, height(height_)
		{
			pixels.resize(width * height);
		}

		void attachImage(const Image& image, const int& rowCenter, const int& colCenter)
		{
			int r = 0;
			for (auto row = rowCenter - height / 2; row <= rowCenter + height / 2; ++row)
			{
				int c = 0;
				for (auto col = colCenter - width / 2; col <= colCenter + width / 2; ++col)
				{
					pixels[r * width + c] = image.getPixel(row, col);
					++c;
				}

				++r;
			}
		}

		Rgb meanOf(const Image& image, const int& rowCenter, const int& colCenter)
		{
			attachImage(image, rowCenter, colCenter);
			return mean();
		}

		Rgb mean()
		{
			int numPixel = static_cast<int>(pixels.size());
			int totalR = 0, totalG = 0, totalB = 0;

			for (auto i = 0; i < height * width; ++i)
			{
				totalR += pixels[i].channels[CHANNEL_RED];
				totalG += pixels[i].channels[CHANNEL_GREEN];
				totalB += pixels[i].channels[CHANNEL_BLUE];
			}

			return 
			{
				static_cast<uint8>(totalR / numPixel),
				static_cast<uint8>(totalG / numPixel),
				static_cast<uint8>(totalB / numPixel)
			};
		}

		Rgb medianOf(const Image& image, const int& rowCenter, const int& colCenter)
		{
			attachImage(image, rowCenter, colCenter);
			return median();
		}

		Rgb median()
		{
			std::vector<uint8> red, green, blue;

			for (auto i = 0; i < height * width; ++i)
			{
				red.push_back(pixels[i].channels[CHANNEL_RED]);
				green.push_back(pixels[i].channels[CHANNEL_GREEN]);
				blue.push_back(pixels[i].channels[CHANNEL_BLUE]);
			}

			std::sort(red.begin(), red.end());
			std::sort(green.begin(), green.end());
			std::sort(blue.begin(), blue.end());

			auto medianValue = [](std::vector<uint8>& channel) -> uint8 {
				size_t size = channel.size();
				if (size % 2 == 0)
					return static_cast<uint8>((channel[size / 2 - 1] + channel[size / 2]) / 2);
				else
					return channel[size / 2];
				};

			return {
				medianValue(red),
				medianValue(green),
				medianValue(blue)
			};
		}

		int width;
		int height;
		std::vector<Rgb> pixels;
	};

	void meanFilter(Image& image, const int& width, const int& height)
	{
		Image src = image;
		Image& dest = image;

		Roi roi(width, height);

		const auto halfBorderHeight = height / 2;
		const auto halfBorderWidth = width / 2;

		const auto rowBorder = image.getHeight() - halfBorderHeight;
		const auto colBorder = image.getWidth() - halfBorderWidth;

		for (auto row = 0; row < image.getHeight(); ++row)
		{
			for (auto col = 0; col < image.getWidth(); ++col)
			{
				if (row < halfBorderHeight - rowBorder || row > rowBorder ||
					col < image.getWidth() - colBorder || col > colBorder)
					dest.getPixel(row, col) = src.getPixel(row, col);
				else
					dest.getPixel(row, col) = roi.meanOf(src, row, col);
			}
		}
	}

	void medianFilter(Image& image, const int& width, const int& height)
	{
		Image src = image;
		Image dest = image;

		Roi roi(width, height);

		const auto halfBorderHeight = height / 2;
		const auto halfBorderWidth = width / 2;
		const auto rowBorder = image.getHeight() - height / 2;
		const auto colBorder = image.getWidth() - width / 2;

		for (auto row = 0; row < image.getHeight(); ++row)
		{
			for (auto col = 0; col < image.getWidth(); ++col)
			{
				if (!(row < halfBorderHeight || row > rowBorder ||
					col < halfBorderWidth - colBorder || col > colBorder))
					dest.getPixel(row, col) = roi.medianOf(src, row, col);
				//on garde le pixel
				else
					dest.getPixel(row, col) = src.getPixel(row, col);
			}
		}
	}
}
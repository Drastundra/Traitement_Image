#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "Bitmap.h"
#include "Process.h"

struct ImageEntry 
{
    std::string filename;
    Bitmap bitmap;
};

void displayMenu() 
{
    std::cout << "\nImage Processing Menu:\n";
    std::cout << "1. Convert to Grayscale\n";
    std::cout << "2. Convert to Binary\n";
    std::cout << "3. Reduce Image\n";
    std::cout << "4. Zoom Image\n";
    std::cout << "5. Rotate Image\n";
    std::cout << "6. Stretch Histogram\n";
    std::cout << "7. Apply Mean Filter\n";
    std::cout << "8. Apply Median Filter\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter your choice: ";
}

void processImage(std::vector<ImageEntry>& images, size_t imageIndex, int choice)
{
    ImageEntry original = images[imageIndex];
    auto image = original.bitmap.getImage();
    std::string newFilename = original.filename;
    bool transformationApplied = false; // Indique si une modification a été faite

    switch (choice)
    {
    case 1:
        processing::converTogrey(image);
        newFilename = "Grey_" + original.filename;
        transformationApplied = true;
        break;

    case 2:
    {
        int threshold;
        std::cout << "Enter binary threshold (0-255): ";
        std::cin >> threshold;
        processing::convertToBinary(image, threshold);
        newFilename = "Binary_" + original.filename;
        transformationApplied = true;
        break;
    }

    case 3:
    {
        float factor;
        std::cout << "Enter reduction factor (exemple : 2.0 for 50%): ";
        std::cin >> factor;
        processing::reduce(image, factor, Point(image.getWidth() / 2, image.getHeight() / 2), Rgb(0, 0, 0));
        newFilename = "Reduced_" + original.filename;
        transformationApplied = true;
        break;
    }

    case 4:
    {
        float factor;
        std::cout << "Enter zoom factor (exemple : 4.0 for 400%): ";
        std::cin >> factor;
        processing::zoom(image, factor, Rgb(0, 0, 0));
        newFilename = "Zoomed_" + original.filename;
        transformationApplied = true;
        break;
    }

    case 5:
    {
        float angle;
        std::cout << "Enter rotation angle in degrees: ";
        std::cin >> angle;
        processing::rotation(image, angle, Rgb(0, 0, 0));
        newFilename = "Rotated_" + original.filename;
        transformationApplied = true;
        break;
    }

    case 6:
        processing::strechHistogram(image);
        newFilename = "Stretched_" + original.filename;
        transformationApplied = true;
        break;

    case 7:
    {
        int kernelX, kernelY;
        std::cout << "Enter kernel size (exemple : 3 for 3x3): ";
        std::cin >> kernelX >> kernelY;
        processing::meanFilter(image, kernelX, kernelY);
        newFilename = "Mean_" + original.filename;
        transformationApplied = true;
        break;
    }

    case 8:
    {
        int kernelX, kernelY;
        std::cout << "Enter kernel size (exemple : 3 for 3x3): ";
        std::cin >> kernelX >> kernelY;
        processing::medianFilter(image, kernelX, kernelY);
        newFilename = "Median_" + original.filename;
        transformationApplied = true;
        break;
    }

    default:
        std::cout << "Invalid choice!\n";
        return;
    }

    // Appliquer les changements et sauvegarder seulement si nécessaire
    if (transformationApplied)
    {
        Bitmap newBitmap;
        newBitmap.setImage(image);
        newBitmap.save(newFilename);
        images.push_back({ newFilename, newBitmap }); // Ajouter une nouvelle entrée dans la liste
        std::cout << "Image saved as: " << newFilename << "\n";
    }
}

int main() 
{
    std::vector<ImageEntry> images;
    std::string inputFilename;

    std::cout << "Enter the initial image filename (exemple : Image.bmp): ";
    std::cin >> inputFilename;

    Bitmap initialImage;

    if (!initialImage.load(inputFilename)) 
    {
        std::cerr << "Failed to load image : " << inputFilename << "\n";
        return 1;
    }
    images.push_back({ inputFilename, initialImage });

    while (true) 
    {
        std::cout << "\nAvailable images:\n";
        for (size_t i = 0; i < images.size(); ++i) 
        {
            std::cout << i + 1 << ". " << images[i].filename << "\n";
        }

        size_t imageChoice;
        std::cout << "Select an image to modify (enter number): ";
        std::cin >> imageChoice;

        if (imageChoice < 1 || imageChoice > images.size()) 
        {
            std::cout << "Invalid choice!\n";
            continue;
        }

        displayMenu();
        int actionChoice;
        std::cin >> actionChoice;

        if (actionChoice == 9) 
        {
            std::cout << "Exiting program.\n";
            break;
        }

        processImage(images, imageChoice - 1, actionChoice);
        images.push_back(images[imageChoice - 1]); // Add modified image back to list
    }

    return 0;
}

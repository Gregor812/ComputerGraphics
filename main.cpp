#include "TgaImage.h"

void DrawLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float divider = (float)(x1 - x0);
    for (int x = x0; x <= x1; ++x)
    {
        float t = (x - x0) / divider;
        int y = y0 * (1.0 - t) + y1 * t;

        if (steep)
        {
            image.SetTgaColor(y, x, color);
        }
        else
        {
            image.SetTgaColor(x, y, color);
        }
    }
}

const TgaColor white = TgaColor(255, 255, 255, 255);
const TgaColor red = TgaColor(255, 0, 0, 255);
const TgaColor green = TgaColor(0, 255, 0, 255);

int main(int argc, char **argv)
{
    TgaImage image(100, 100, TgaImage::RGB);
    image.SetTgaColor(10, 10, white);
    image.SetTgaColor(11, 10, white);
    image.SetTgaColor(11, 11, white);
    image.SetTgaColor(10, 11, white);
    image.SetTgaColor(52, 41, red);
    DrawLine(30, 5, 60, 95, image, green);
    image.WriteTgaFile("output.tga");
    image.FlipVertically();
    image.WriteTgaFile("outputFlipped.tga");

    return 0;
}

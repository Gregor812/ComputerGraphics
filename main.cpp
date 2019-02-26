#include "Line.h"

const TgaColor white = TgaColor(255, 255, 255, 255);
const TgaColor red = TgaColor(255, 0, 0, 255);
const TgaColor green = TgaColor(0, 255, 0, 255);

int main(int argc, char **argv)
{
    TgaImage image(100, 100, TgaImage::RGB);
    for (int i = 10; i < 20; ++i)
    {
        for (int j = 15; j < 25; ++j)
        {
            image.SetTgaColor(i, j, white);
        }
    }

    for (int i = 52; i < 62; ++i)
    {
        for (int j = 41; j < 51; ++j)
        {
            image.SetTgaColor(i, j, red);
        }
    }
    
    IntPoint start = IntPoint(30, 15);
    IntPoint end = IntPoint(60, 85);
    Line line = Line(start, end, green);
    line.Draw(image);

    image.WriteTgaFile("output.tga");
    image.FlipVertically();
    image.WriteTgaFile("outputFlipped.tga");

    return 0;
}

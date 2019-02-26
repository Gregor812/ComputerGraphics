#include "Line.h"

using namespace ComputerGraphics;

const FileFormats::TgaColor white = FileFormats::TgaColor(255, 255, 255, 255);
const FileFormats::TgaColor gray = FileFormats::TgaColor(127, 127, 127, 127);
const FileFormats::TgaColor red = FileFormats::TgaColor(255, 0, 0, 255);
const FileFormats::TgaColor green = FileFormats::TgaColor(0, 255, 0, 255);
const FileFormats::TgaColor blue = FileFormats::TgaColor(0, 0, 255, 255);
const FileFormats::TgaColor magenta = FileFormats::TgaColor(255, 0, 255, 255);

int main(int argc, char **argv)
{
    FileFormats::TgaImage image(100, 100, FileFormats::TgaImage::RGB);
    for(int i = 0; i < 100; ++i)
    {
        for(int j = 0; j < 100; ++j)
        {
            image.SetTgaColor(i, j, gray);
        }
    }
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

    Primitives::IntPoint start = Primitives::IntPoint(20, 55);
    Primitives::IntPoint end = Primitives::IntPoint(70, 65);
    Primitives::Line line = Primitives::Line(start, end, green);
    line.Draw(image);
    
    Primitives::IntPoint start2 = Primitives::IntPoint(55, 20);
    Primitives::IntPoint end2 = Primitives::IntPoint(65, 70);
    line = Primitives::Line(start2, end2, magenta);
    line.Draw(image);

    start2 = Primitives::IntPoint(18, 20);
    end2 = Primitives::IntPoint(18, 70);
    line = Primitives::Line(start2, end2, red);
    line.Draw(image);

    start2 = Primitives::IntPoint(70, 18);
    end2 = Primitives::IntPoint(20, 18);
    line = Primitives::Line(start2, end2, red);
    line.Draw(image);

    image.WriteTgaFile("output.tga");

    line = Primitives::Line(end, start, blue);
    line.Draw(image);
    image.FlipVertically();
    image.WriteTgaFile("outputFlipped.tga");

    return 0;
}

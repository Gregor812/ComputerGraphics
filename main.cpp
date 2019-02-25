#include "TgaImage.h"

const TgaColor white = TgaColor(255, 255, 255, 255);
const TgaColor red = TgaColor(255, 0, 0, 255);

int main(int argc, char** argv)
{
    TgaImage image(100, 100, TgaImage::RGB);
    image.SetTgaColor(10, 10, white);
    image.SetTgaColor(11, 10, white);
    image.SetTgaColor(11, 11, white);
    image.SetTgaColor(10, 11, white);
    image.SetTgaColor(52, 41, red);
    image.FlipVertically();
    image.WriteTgaFile("output.tga");

    return 0;
}

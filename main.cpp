#include "Line.h"
#include "Model.h"
#include "Triangle.h"

using namespace ComputerGraphics;

void Test();
void Test2();
void Test3();

const FileFormats::TgaColor white = FileFormats::TgaColor(255, 255, 255, 255);
const FileFormats::TgaColor gray = FileFormats::TgaColor(127, 127, 127, 127);
const FileFormats::TgaColor red = FileFormats::TgaColor(255, 0, 0, 255);
const FileFormats::TgaColor green = FileFormats::TgaColor(0, 255, 0, 255);
const FileFormats::TgaColor blue = FileFormats::TgaColor(0, 0, 255, 255);
const FileFormats::TgaColor magenta = FileFormats::TgaColor(255, 0, 255, 255);

const int width  = 800;
const int height = 800;

int main(int argc, char **argv)
{
    Test();
    Test2();
    Test3();

    return 0;
}

void Test()
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

    image.WriteTgaFile("test1.tga");

    line = Primitives::Line(end, start, blue);
    line.Draw(image);
    image.FlipVertically();
    image.WriteTgaFile("test1flipped.tga");
}

void Test2()
{
    using Primitives::Vec3f;
    using Models::Model;
    using Primitives::IntPoint;
    using Primitives::Line;
    using FileFormats::TgaImage;
    
    Model* model = new Model("obj/african_head.obj");
    TgaImage image(width, height, TgaImage::RGB);

    for (int i=0; i<model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);

        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);

            int x = (v0.x+1.)*width/2.;
            int y = (v0.y+1.)*height/2.;
            IntPoint start = IntPoint(x, y);

            x = (v1.x+1.)*width/2.;
            y = (v1.y+1.)*height/2.;
            IntPoint end = IntPoint(x, y);

            Line line = Line(start, end, magenta);
            line.Draw(image);
        }
    }

    delete model;

    image.FlipVertically();
    image.WriteTgaFile("test2.tga");
}

void Test3()
{
    using Primitives::Vec2i;
    using Primitives::Triangle;
    using FileFormats::TgaImage;

    TgaImage image = TgaImage(200, 200, TgaImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    Triangle::Draw(t0[0], t0[1], t0[2], image, red);
    Triangle::Draw(t1[0], t1[1], t1[2], image, white);
    Triangle::Draw(t2[0], t2[1], t2[2], image, green);

    image.FlipVertically();
    image.WriteTgaFile("test3.tga");
}

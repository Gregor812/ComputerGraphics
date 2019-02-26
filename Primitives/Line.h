#pragma once

#include <cmath>
#include "IntPoint.h"
#include "TgaImage.h"

class Line
{
private:
    IntPoint _start;
    IntPoint _end;
    TgaColor _color;

public:
    Line(const IntPoint& start, const IntPoint& end, const TgaColor& color)
        : _start(start), _end(end), _color(color)
    { }

    void Draw(TgaImage& image)
    {
        Draw(_start, _end, image, _color);
    }

    static void Draw(IntPoint &start, IntPoint &end, TgaImage &image, const TgaColor &color)
    {
        bool steep = false;

        int dx = start.x - end.x;
        int dy = start.y - end.y;

        if (std::abs(dx) < std::abs(dy))
        {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            steep = true;
        }

        if (start.x > end.x)
        {
            std::swap(start, end);
        }

        int dError = std::abs(dy) * 2;
        int error = 0;

        int y = start.y;

        for (int x = start.x; x <= end.x; ++x)
        {
            if (steep)
            {
                image.SetTgaColor(y, x, color);
            }
            else
            {
                image.SetTgaColor(x, y, color);
            }

            error += dError;

            if (error > dx)
            {
                y += (end.y > start.y) ? 1 : -1;
                error -= dx * 2;
            }
        }
    }
};
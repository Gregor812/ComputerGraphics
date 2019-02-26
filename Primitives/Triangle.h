#pragma once

#include "Line.h"
#include "Geometry.h"

namespace ComputerGraphics
{
    namespace Primitives
    {
        using FileFormats::TgaImage;
        using FileFormats::TgaColor;

        class Triangle
        {
        public:
            static void Draw(const Vec2i& t0, const Vec2i& t1, const Vec2i& t2, TgaImage& image, const TgaColor& color)
            {
                Line::Draw(t0, t1, image, color);
                Line::Draw(t1, t2, image, color);
                Line::Draw(t2, t0, image, color);
            }
        };
    } // namespace Primitives
} // namespace ComputerGraphics

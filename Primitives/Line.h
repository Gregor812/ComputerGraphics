#pragma once

#include <cmath>
#include <iostream>
#include "IntPoint.h"
#include "TgaImage.h"

namespace ComputerGraphics
{
    namespace Primitives
    {
        class Line
        {
        private:
            IntPoint _start;
            IntPoint _end;
            FileFormats::TgaColor _color;

        public:
            Line(const IntPoint& start, const IntPoint& end, const FileFormats::TgaColor& color)
                : _start(start), _end(end), _color(color)
            {
            }

            Line(const Line& other) = default;
            Line& operator=(const Line& other) = default;

            void Draw(FileFormats::TgaImage& image)
            {
                Draw(_start, _end, image, _color);
            }

            static void Draw(
                IntPoint& start, 
                IntPoint& end, 
                FileFormats::TgaImage& image, 
                const FileFormats::TgaColor& color
            )
            {
                bool inverted = false;

                int dx = end.x - start.x;
                int dy = end.y - start.y;

                if (std::abs(dx) < std::abs(dy))
                {
                    std::swap(start.x, start.y);
                    std::swap(end.x, end.y);
                    dx = end.x - start.x;
                    dy = end.y - start.y;
                    inverted = true;
                }

                if (dx < 0)
                {
                    std::swap(start, end);
                    dx = -dx;
                    dy = -dy;
                }

                int normalizedSlope = 0;
                int normalizedError = 0;
                
                if (dx != 0)
                    normalizedSlope = std::abs(dy) * 2;

                int y = start.y;

                for (int x = start.x; x <= end.x; ++x)
                {
                    if (inverted == true)
                    {
                        image.SetTgaColor(y, x, color);
                    }
                    else
                    {
                        image.SetTgaColor(x, y, color);
                    }
                    normalizedError += normalizedSlope;

                    if (normalizedError > dx)
                    {
                        normalizedError -= dx * 2;
                        y += ((dy > 0) ? 1 : -1);
                    }
                }
            }
        };
    } // namespace Primitives
} // namespace ComputerGraphics

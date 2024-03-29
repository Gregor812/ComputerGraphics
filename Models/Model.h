#pragma once

#include <vector>
#include "Geometry.h"

namespace ComputerGraphics
{
    namespace Models
    {
        using Primitives::Vec3f;
        
        class Model
        {
        private:
            std::vector<Vec3f> verts_;
            std::vector<std::vector<int> > faces_;
        public:
            Model(const char *filename);
            ~Model();
            int nverts();
            int nfaces();
            Vec3f vert(int i);
            std::vector<int> face(int idx);
        };
    } // namespace Models
} // namespace ComputerGraphics

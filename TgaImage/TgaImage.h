#pragma once

#include <fstream>

namespace ComputerGraphics
{
    namespace FileFormats
    {
        #pragma pack(push,1)
        struct TgaHeader
        {	
            char  IdLength;
            char  ColorMapType;
            char  DataTypeCode;
            short ColorMapOrigin;
            short ColorMapLength;
            char  ColorMapDepth;

            short XOrigin;
            short YOrigin;
            short Width;
            short Height;
            char  BitsPerPixel;
            char  ImageDescriptor;
        };
        #pragma pack(pop)

        struct TgaColor
        {
            union
            {
                struct
                {
                    unsigned char B;
                    unsigned char G;
                    unsigned char R;
                    unsigned char A;
                };
            
                unsigned char Raw[4];
                unsigned int  Value;
            };

            int BytesPerPixel;

            TgaColor() : Value(0), BytesPerPixel(1)
            { }

            TgaColor
            (
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char a
            )
                : B(b), G(g), R(r), A(a)
            { }
            
            TgaColor(int value, int bytesPerPixel)
                : Value(value), BytesPerPixel(bytesPerPixel) 
            { }

            TgaColor(const TgaColor& other)
                : Value(other.Value), BytesPerPixel(other.BytesPerPixel)
            { }

            TgaColor(const unsigned char* raw, int bytesPerPixel)
                : Value(0), BytesPerPixel(bytesPerPixel)
            {
                for (int i = 0; i < bytesPerPixel; ++i)
                {
                    Raw[i] = raw[i];
                }
            }

            TgaColor& operator=(const TgaColor& other)
            {
                if (this != &other)
                {
                    BytesPerPixel = other.BytesPerPixel;
                    Value = other.Value;
                }
                return *this;
            }
        };

        class TgaImage
        {
        protected:
            unsigned char* data;
            int width;
            int height;
            int bytesPerPixel;

            bool LoadRleData(std::ifstream& in);
            bool UnloadRleData(std::ofstream& out);

        public:
            enum Format
            {
                GrayScale = 1,
                RGB = 3,
                RGBA = 4
            };

            TgaImage();
            TgaImage(int width, int height, int bytesPerPixel);
            TgaImage(const TgaImage& other);

            bool ReadTgaFile(const char* filename);
            bool WriteTgaFile(const char* filename);
            bool WriteTgaFile(const char* filename, bool rle);

            bool FlipHorizontally();
            bool FlipVertically();
            bool Scale(int width, int height);

            TgaColor GetTgaColor(int x, int y);
            bool SetTgaColor(int x, int y, const TgaColor& color);

            TgaImage& operator=(const TgaImage& other);
            ~TgaImage();

            int GetWidth();
            int GetHeight();
            int GetBytesPerPixel();

            unsigned char* GetBuffer();
            void ClearBuffer();
        };
    } // namespace FileFormats
} // namespace ComputerGraphics

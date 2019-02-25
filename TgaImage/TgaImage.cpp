#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "TgaImage.h"

TgaImage::TgaImage()
    : data(nullptr), width(0), height(0), bytesPerPixel(0)
{
}

TgaImage::TgaImage(int width, int height, int bytesPerPixel)
    : data(nullptr), width(width), height(height), bytesPerPixel(bytesPerPixel)
{
    unsigned long nBytes = width * height * bytesPerPixel;
    data = new unsigned char[nBytes];
    memset(data, 0, nBytes);
}

TgaImage::TgaImage(const TgaImage &other)
{
    width = other.width;
    height = other.height;
    bytesPerPixel = other.bytesPerPixel;
    unsigned long nBytes = width * height * bytesPerPixel;
    data = new unsigned char[nBytes];
    memcpy(data, other.data, nBytes);
}

TgaImage::~TgaImage()
{
    if (data)
        delete[] data;
}

TgaImage &TgaImage::operator=(const TgaImage &other)
{
    if (this != &other)
    {
        if (data)
            delete[] data;

        width = other.width;
        height = other.height;
        bytesPerPixel = other.bytesPerPixel;

        unsigned long nBytes = width * height * bytesPerPixel;
        data = new unsigned char[nBytes];
        memcpy(data, other.data, nBytes);
    }

    return *this;
}

bool TgaImage::ReadTgaFile(const char *filename)
{
    if (data)
        delete[] data;

    data = nullptr;

    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }

    TgaHeader header;

    in.read((char*)&header, sizeof(header));
    if (!in.good())
    {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }

    width = header.Width;
    height = header.Height;
    bytesPerPixel = header.BitsPerPixel >> 3;

    if (width <= 0 || height <= 0 || (bytesPerPixel != GrayScale && bytesPerPixel != RGB && bytesPerPixel != RGBA))
    {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }

    unsigned long nBytes = bytesPerPixel * width * height;
    data = new unsigned char[nBytes];

    if (3 == header.DataTypeCode || 2 == header.DataTypeCode)
    {
        in.read((char*)data, nBytes);
        if (!in.good())
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else if (10 == header.DataTypeCode || 11 == header.DataTypeCode)
    {
        if (!LoadRleData(in))
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else
    {
        in.close();
        std::cerr << "unknown file format " << (int)header.DataTypeCode << "\n";
        return false;
    }

    if (!(header.ImageDescriptor & 0x20))
    {
        FlipVertically();
    }

    if (header.ImageDescriptor & 0x10)
    {
        FlipHorizontally();
    }

    std::cerr << width << "x" << height << "/" << bytesPerPixel * 8 << "\n";
    in.close();
    return true;
}

bool TgaImage::LoadRleData(std::ifstream& in)
{
    unsigned long pixelCount = width * height;
    unsigned long currentPixel = 0;
    unsigned long currentByte = 0;
    TgaColor colorBuffer;

    do
    {
        unsigned char chunkHeader = 0;
        chunkHeader = in.get();
        
        if (!in.good())
        {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }

        if (chunkHeader < 128)
        {
            ++chunkHeader;

            for (int i = 0; i < chunkHeader; ++i)
            {
                in.read((char*)colorBuffer.Raw, bytesPerPixel);
                if (!in.good())
                {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }

                for (int t = 0; t < bytesPerPixel; ++t)
                    data[currentByte++] = colorBuffer.Raw[t];
                
                ++currentPixel;
                
                if (currentPixel > pixelCount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
        else
        {
            chunkHeader -= 127;

            in.read((char*)colorBuffer.Raw, bytesPerPixel);
            if (!in.good())
            {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }

            for (int i = 0; i < chunkHeader; ++i)
            {
                for (int t = 0; t < bytesPerPixel; ++t)
                    data[currentByte++] = colorBuffer.Raw[t];
                
                ++currentPixel;

                if (currentPixel > pixelCount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    }
    while (currentPixel < pixelCount);

    return true;
}

bool TgaImage::WriteTgaFile(const char *filename)
{
    this->WriteTgaFile(filename, true);
}

bool TgaImage::WriteTgaFile(const char *filename, bool rle)
{
    unsigned char developerAreaRef[4] = {0, 0, 0, 0};
    unsigned char extensionAreaRef[4] = {0, 0, 0, 0};
    unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
    
    std::ofstream out;

    out.open(filename, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }

    TgaHeader header;

    memset((void*)&header, 0, sizeof(header));
    header.BitsPerPixel = bytesPerPixel << 3;
    header.Width = width;
    header.Height = height;
    header.DataTypeCode = (bytesPerPixel == GrayScale ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.ImageDescriptor = 0x20; // top-left origin

    out.write((char*)&header, sizeof(header));
    if (!out.good())
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle)
    {
        out.write((char*)data, width * height * bytesPerPixel);
        if (!out.good())
        {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    }
    else
    {
        if (!UnloadRleData(out))
        {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }

    out.write((char*)developerAreaRef, sizeof(developerAreaRef));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }

    out.write((char*)extensionAreaRef, sizeof(extensionAreaRef));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }

    out.write((char*)footer, sizeof(footer));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }

    out.close();
    return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TgaImage::UnloadRleData(std::ofstream &out)
{
    const unsigned char maxChunkLength = 128;
    unsigned long nPixels = width * height;
    unsigned long currentPixel = 0;
    
    while (currentPixel < nPixels)
    {
        unsigned long chunkstart = currentPixel * bytesPerPixel;
        unsigned long currentByte = currentPixel * bytesPerPixel;
        unsigned char runLength = 1;
        bool raw = true;

        while (currentPixel + runLength < nPixels && runLength < maxChunkLength)
        {
            bool successfulEquals = true;
            for (int t = 0; successfulEquals && t < bytesPerPixel; ++t)
            {
                successfulEquals = (data[currentByte + t] == data[currentByte + t + bytesPerPixel]);
            }

            currentByte += bytesPerPixel;

            if (1 == runLength)
            {
                raw = !successfulEquals;
            }

            if (raw && successfulEquals)
            {
                --runLength;
                break;
            }

            if (!raw && !successfulEquals)
            {
                break;
            }
            
            ++runLength;
        }

        currentPixel += runLength;

        out.put(raw ? runLength - 1 : runLength + 127);
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }

        out.write((char *)(data + chunkstart), (raw ? runLength * bytesPerPixel : bytesPerPixel));
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }

    return true;
}

TgaColor TgaImage::GetTgaColor(int x, int y)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return TgaColor();
    }
    return TgaColor(data + (x + y * width) * bytesPerPixel, bytesPerPixel);
}

bool TgaImage::SetTgaColor(int x, int y, const TgaColor& color)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return false;
    }
    memcpy(data + (x + y * width) * bytesPerPixel, color.Raw, bytesPerPixel);
    return true;
}

int TgaImage::GetBytesPerPixel()
{
    return bytesPerPixel;
}

int TgaImage::GetWidth()
{
    return width;
}

int TgaImage::GetHeight()
{
    return height;
}

bool TgaImage::FlipHorizontally()
{
    if (!data)
        return false;

    int half = width >> 1;
    for (int i = 0; i < half; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            TgaColor c1 = GetTgaColor(i, j);
            TgaColor c2 = GetTgaColor(width - 1 - i, j);
            SetTgaColor(i, j, c2);
            SetTgaColor(width - 1 - i, j, c1);
        }
    }

    return true;
}

bool TgaImage::FlipVertically()
{
    if (!data)
        return false;

    unsigned long bytesPerLine = width * bytesPerPixel;
    unsigned char *line = new unsigned char[bytesPerLine];
    
    int half = height >> 1;
    for (int j = 0; j < half; ++j)
    {
        unsigned long l1 = j * bytesPerLine;
        unsigned long l2 = (height - 1 - j) * bytesPerLine;

        memmove((void *)line, (void *)(data + l1), bytesPerLine);
        memmove((void *)(data + l1), (void *)(data + l2), bytesPerLine);
        memmove((void *)(data + l2), (void *)line, bytesPerLine);
    }

    delete[] line;
    return true;
}

unsigned char* TgaImage::GetBuffer()
{
    return data;
}

void TgaImage::ClearBuffer()
{
    memset((void *)data, 0, width * height * bytesPerPixel);
}

bool TgaImage::Scale(int w, int h)
{
    if (w <= 0 || h <= 0 || !data)
        return false;

    unsigned char *tdata = new unsigned char[w * h * bytesPerPixel];
    int nScanLine = 0;
    int oScanLine = 0;
    int erry = 0;
    unsigned long nLineBytes = w * bytesPerPixel;
    unsigned long oLineBytes = width * bytesPerPixel;

    for (int j = 0; j < height; ++j)
    {
        int errx = width - w;
        int nx = -bytesPerPixel;
        int ox = -bytesPerPixel;

        for (int i = 0; i < width; ++i)
        {
            ox += bytesPerPixel;
            errx += w;

            while (errx >= (int)width)
            {
                errx -= width;
                nx += bytesPerPixel;

                memcpy(tdata + nScanLine + nx, data + oScanLine + ox, bytesPerPixel);
            }
        }

        erry += h;
        oScanLine += oLineBytes;

        while (erry >= (int)height)
        {
            if (erry >= (int)height << 1) // it means we jump over a scanline
                memcpy(tdata + nScanLine + nLineBytes, tdata + nScanLine, nLineBytes);
                
            erry -= height;
            nScanLine += nLineBytes;
        }
    }

    delete[] data;
    data = tdata;
    width = w;
    height = h;

    return true;
}

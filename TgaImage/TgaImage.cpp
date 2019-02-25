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
    in.read((char *)&header, sizeof(header));
    if (!in.good())
    {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }

    width = header.Width;
    height = header.Height;
    bytesPerPixel = header.BitsPerPixel >> 3;

    if (width <= 0 || height <= 0 ||
        (bytesPerPixel != GrayScale && bytesPerPixel != RGB && bytesPerPixel != RGBA))
    {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }

    unsigned long nBytes = bytesPerPixel * width * height;
    data = new unsigned char[nBytes];
    if (3 == header.DataTypeCode || 2 == header.DataTypeCode)
    {
        in.read((char *)data, nBytes);
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
            chunkHeader++;

            for (int i = 0; i < chunkHeader; ++i)
            {
                in.read((char *)colorBuffer.Raw, bytesPerPixel);
                if (!in.good())
                {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }

                for (int t = 0; t < bytesPerPixel; ++t)
                    data[currentByte++] = colorBuffer.Raw[t];
                
                currentPixel++;
                
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

            in.read((char *)colorBuffer.Raw, bytesPerPixel);
            if (!in.good())
            {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }

            for (int i = 0; i < chunkHeader; ++i)
            {
                for (int t = 0; t < bytesPerPixel; ++t)
                    data[currentByte++] = colorBuffer.Raw[t];
                
                currentPixel++;

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
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
    unsigned char extension_area_ref[4] = {0, 0, 0, 0};
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
    memset((void *)&header, 0, sizeof(header));
    header.BitsPerPixel = bytesPerPixel << 3;
    header.Width = width;
    header.Height = height;
    header.DataTypeCode = (bytesPerPixel == GrayScale ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.ImageDescriptor = 0x20; // top-left origin
    out.write((char *)&header, sizeof(header));
    if (!out.good())
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle)
    {
        out.write((char *)data, width * height * bytesPerPixel);
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
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)footer, sizeof(footer));
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
    const unsigned char max_chunk_length = 128;
    unsigned long npixels = width * height;
    unsigned long curpix = 0;
    while (curpix < npixels)
    {
        unsigned long chunkstart = curpix * bytesPerPixel;
        unsigned long curbyte = curpix * bytesPerPixel;
        unsigned char run_length = 1;
        bool raw = true;
        while (curpix + run_length < npixels && run_length < max_chunk_length)
        {
            bool succ_eq = true;
            for (int t = 0; succ_eq && t < bytesPerPixel; t++)
            {
                succ_eq = (data[curbyte + t] == data[curbyte + t + bytesPerPixel]);
            }
            curbyte += bytesPerPixel;
            if (1 == run_length)
            {
                raw = !succ_eq;
            }
            if (raw && succ_eq)
            {
                run_length--;
                break;
            }
            if (!raw && !succ_eq)
            {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length - 1 : run_length + 127);
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char *)(data + chunkstart), (raw ? run_length * bytesPerPixel : bytesPerPixel));
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

bool TgaImage::SetTgaColor(int x, int y, const TgaColor& c)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return false;
    }
    memcpy(data + (x + y * width) * bytesPerPixel, c.Raw, bytesPerPixel);
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

    unsigned long bytes_per_line = width * bytesPerPixel;
    unsigned char *line = new unsigned char[bytes_per_line];
    int half = height >> 1;
    for (int j = 0; j < half; ++j)
    {
        unsigned long l1 = j * bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;
        memmove((void *)line, (void *)(data + l1), bytes_per_line);
        memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
        memmove((void *)(data + l2), (void *)line, bytes_per_line);
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
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    unsigned long nlinebytes = w * bytesPerPixel;
    unsigned long olinebytes = width * bytesPerPixel;

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
                memcpy(tdata + nscanline + nx, data + oscanline + ox, bytesPerPixel);
            }
        }

        erry += h;
        oscanline += olinebytes;

        while (erry >= (int)height)
        {
            if (erry >= (int)height << 1) // it means we jump over a scanline
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
        }
    }

    delete[] data;
    data = tdata;
    width = w;
    height = h;
    
    return true;
}

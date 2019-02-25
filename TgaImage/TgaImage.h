#pragma once

#include <fstream>

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

    int BytesPP;

    TgaColor() : Value(0), BytesPP(1)
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
    
    TgaColor(int value, int bytesPP)
        : Value(value), BytesPP(bytesPP) 
    { }

    TgaColor(const TgaColor& other)
        : Value(other.Value), BytesPP(other.BytesPP)
    { }

    TgaColor(const unsigned char* raw, int bytesPP)
        : Value(0), BytesPP(bytesPP)
    {
        for (int i = 0; i < bytesPP; ++i)
        {
            Raw[i] = raw[i];
        }
    }

    TgaColor& operator=(const TgaColor& other)
    {
        if (this != &other)
        {
            BytesPP = other.BytesPP;
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
    int bytesPP;

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
    TgaImage(int width, int height, int bytesPP);
    TgaImage(const TgaImage& other);

    bool ReadTgaFile(const char* filename);
    bool WriteTgaFile(const char* filename);
    bool WriteTgaFile(const char* filename, bool rle);

    bool FlipHorizontally();
    bool FlipVertically();
    bool Scale(int width, int height);

    TgaColor GetTgaColor(int x, int y);
    bool SetTgaColor(int x, int y, TgaColor& c);

    TgaImage& operator=(const TgaImage& other);
    ~TgaImage();

    int GetWidth();
    int GetHeight();
    int GetBytesPP();

    unsigned char* GetBuffer();
    void Clear();
};


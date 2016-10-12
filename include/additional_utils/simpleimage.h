#pragma once

#include <memory>
#include <Windows.h>
#include <vector>
#include "simplerect.h"

using namespace std; 

enum FileType { FT_PGM, FT_JPG };

/// Class for loading, writing and storing images
class SimpleImage
{
public:
	SimpleImage() : width(-1), height(-1), data_ptr(NULL), bytes_per_pixel(1), bip(NULL) { }
	SimpleImage(const SimpleImage &si);

	int width, height; 
	unsigned char *data_ptr;
	int bytes_per_pixel;

	void convertToBinary(unsigned char threshold);
	void convertToRGB();
	void cutImage(int x, int y, int w, int h);
	int scaleImage(double scale);

	int readFromFile(const char *filename, FileType file_type);
	int writeToFile(const char *filename, FileType file_type);
	void addImage(SimpleImage &image);

	SimpleImage getChannel(int nc);

	SimpleImage operator-(const SimpleImage &i);
	SimpleImage& operator=(const SimpleImage &si);
	unsigned char* operator[](int row); 

	BITMAPINFO *getBip();
	void prepareBitmapInfo();

	void flipVertically();
	void flipHorizontally();

	SimpleImage& convertToGrayscale();
	void smoothImage();

	void createImage(int _width, int _height, int bpp);

	void drawPixel(int x, int y, int r, int g, int b);
	void drawPixel(int x, int y, int val);
	void drawRectangle(SimpleRect &rect, int r, int g, int b);
	void drawRectangle(SimpleRect &rect, int val);
	void drawRectangle(SimpleRect &rect);

	double getSubpixelImageValue(SimplePoint<double> &p);


	~SimpleImage();
private:
	void setSize(int _width, int _height);
	void zeroVariables();
	void freeMemory();
	unsigned char *scaleImage2(double scale, unsigned char *data, int &iWidth, int &iHeight);
	void toGrayscale(byte *dest, byte *RGBSource, int w, int h);
	BITMAPINFO *bip;
};

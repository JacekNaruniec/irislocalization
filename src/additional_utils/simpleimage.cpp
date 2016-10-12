#include "simpleimage.h"
#include "pgm.h"
#include "file_utils.h"
#include <assert.h>

SimpleImage::SimpleImage(const SimpleImage &si)
{
	zeroVariables();
	*this = si;
}

void SimpleImage::drawRectangle(SimpleRect &rect)
{
	if (bytes_per_pixel==1)
		drawRectangle(rect, 255);
	else if (bytes_per_pixel==3)
		drawRectangle(rect, 0, 0, 255);
}

SimpleImage SimpleImage::getChannel(int cn)
{
	SimpleImage res;
	if (cn >= bytes_per_pixel)
		return res;

	res.createImage(width, height, 1);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			res[j][i] = (*this)[j][i * 3 + cn];


	return res;
}

void SimpleImage::drawPixel(int x, int y, int r, int g, int b)
{
	int l;

	if (x<width && y<height && x>0 && y>0)
	{
		l = (y*width + x) * 3;
		data_ptr[l] = (unsigned char)r;
		data_ptr[l + 1] = (unsigned char)g;
		data_ptr[l + 2] = (unsigned char)b;
	}
}

void SimpleImage::drawPixel(int x, int y, int val)
{
	if (x<width && y<height && x>0 && y>0)
		data_ptr[x + y*width] = (byte)val;
}


void SimpleImage::drawRectangle(SimpleRect &rect, int r, int g, int b)
{
	int i;

	if (bytes_per_pixel != 3)
		return;

	for (i = max(0, rect.getLeft()); i<min((rect.getRight() + 1), width); i++)
	{
		drawPixel(i, rect.getTop(), r, g, b);
		drawPixel(i, rect.getBottom(), r, g, b);
	}

	for (i = max(0, rect.getTop()); i<min((rect.getBottom() + 1), height); i++)
	{
		drawPixel(rect.getLeft(), i,  r, g, b);
		drawPixel(rect.getRight(), i, r, g, b);
	}
}


void SimpleImage::drawRectangle(SimpleRect &rect, int val)
{
	int i;

	if (bytes_per_pixel != 1)
		return;

	for (i = max(0, rect.getLeft()); i<min((rect.getRight() + 1), width); i++)
	{
		drawPixel(i, rect.getTop(), val);
		drawPixel(i, rect.getBottom(),  val);
	}

	for (i = max(0, rect.getTop()); i<min((rect.getBottom() + 1), height); i++)
	{
		drawPixel(rect.getLeft(), i, val);
		drawPixel(rect.getRight(), i, val);
	}
}

void SimpleImage::toGrayscale(byte *dest, byte *RGBSource, int w, int h)
{
	int x, y, diff;
	byte *ptrs, *ptrd;
	for (y = 0; y<h; y++)
	{
		diff = h - y - 1;

		ptrs = RGBSource + y*w * 3;
		ptrd = dest + diff*w;

		for (x = 0; x<w; x++)
		{;
			*ptrd = ((*ptrs) + *(ptrs + 1) + *(ptrs + 2)) / 3;
			ptrs += 3;
			ptrd += 1;
		}
	}

}

void SimpleImage::smoothImage()
{
	if (bytes_per_pixel != 1)
		return;

	int wmax = width - 2;
	int hmax = height - 2;
	int w2 = width * 2;
	unsigned char *temp = new unsigned char[width*height];
	unsigned char *pos_dest, *pos_source;

	memcpy(temp, data_ptr, width*height);

	for (int y = 2; y<hmax; y++)
	{
		pos_source = data_ptr + y*width;
		pos_dest = temp + y*width;
		for (int x = 2; x<wmax; x++)
			pos_dest[x] = (int)((pos_source[x - 2] + 2 * pos_source[x - 1] + 4 * pos_source[x] +
			2 * pos_source[x + 1] + pos_source[x + 2])*0.1f);
	}

	for (int y = 2; y<hmax; y++)
	{
		pos_source = temp + y*width;
		pos_dest = data_ptr + y*width;
		for (int x = 2; x<wmax; x++)
			pos_dest[x] = (unsigned char)((pos_source[x - w2] + 2 * pos_source[x - width] + 4 * pos_source[x] +
			2 * pos_source[x + width] + pos_source[x + w2])*0.1f);
	}

	delete[]temp;
}


SimpleImage& SimpleImage::convertToGrayscale()
{
	if (bytes_per_pixel == 3)
	{

		if (bytes_per_pixel != 3 || width < 0 || height < 0) return *this;

		unsigned char *new_ptr = new unsigned char[width*height];

		toGrayscale(new_ptr, data_ptr, width, height);

		delete[]data_ptr;
		data_ptr = new_ptr;
		bytes_per_pixel = 1;
	}

	return *this;
}

void SimpleImage::addImage(SimpleImage &image)
{
	int new_w, new_h;
	new_w = width + image.width;
	new_h = max(height, image.height);

	unsigned char* new_data = new unsigned char[new_w * new_h];
	memset(new_data, 0, new_w*new_h);

	for (int i=0; i<height; i++)
		memcpy(new_data + i*new_w, data_ptr + i*width, width);

	for (int i=0; i<image.height; i++)
		memcpy(new_data + width + i*new_w, image.data_ptr + i*image.width, image.width);

	width = new_w;
	height = new_h;
	delete []data_ptr;
	data_ptr = new_data;
}


void SimpleImage::flipHorizontally()
{
	SimpleImage img;
	img.createImage(width, height, bytes_per_pixel);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			for (int k = 0; k < bytes_per_pixel; k++)
				img.data_ptr[bytes_per_pixel * ((width - i - 1) + j*width)
				+ k] = data_ptr[bytes_per_pixel * (i + j*width) + k];


	*this = img;
}
void SimpleImage::flipVertically()
{
	unsigned char *tmp_im = new unsigned char[width*height*bytes_per_pixel];

	for (int y=0; y<height; y++)
		memcpy(tmp_im + y*width*bytes_per_pixel, data_ptr + (height - y - 1) * width * bytes_per_pixel,
			width*bytes_per_pixel);

	memcpy(data_ptr, tmp_im, width*height*bytes_per_pixel);
	delete []tmp_im;
}

void SimpleImage::convertToRGB()
{
	if (bytes_per_pixel==3 || width<=0 || height<=0) return;

	unsigned char *new_ptr;
	new_ptr = new unsigned char[width*height*3];
	int counter = 0;
	unsigned char val;

	for (int i=0; i<width*height; i++)
	{
		val = data_ptr[i];
	
		for (int j=0; j<3; j++)
			new_ptr[counter++] = val;
	}

	bytes_per_pixel = 3;
	delete []data_ptr;
	data_ptr = new_ptr;
}


void SimpleImage::convertToBinary(unsigned char threshold)
{
	for (int i=0; i<width*height; i++)
		if (data_ptr[i]>threshold)
			data_ptr[i] = 1;
		else
			data_ptr[i] = 0;
}

void SimpleImage::cutImage(int x, int y, int w, int h)
{
	unsigned char *new_ptr = new unsigned char[w*h*bytes_per_pixel];

	if (width != -1)
	{

		for (int i = 0; i < h; i++)
			memcpy(new_ptr + w*i*bytes_per_pixel, data_ptr + bytes_per_pixel*((y + i)*width + x), w*bytes_per_pixel);

		delete[]data_ptr;
	}

	data_ptr = new_ptr;
	width = w;
	height = h;
	prepareBitmapInfo();
}


int SimpleImage::readFromFile(const char *filename, FileType file_type)
{
	freeMemory();
	zeroVariables();

	if (file_type==FileType::FT_PGM)
	{
		data_ptr = readPGM(filename, width, height, false);
		bytes_per_pixel = 1;
		if (data_ptr!=NULL) return 0;
		else return -1;
	}

	if (file_type==FileType::FT_JPG)
	{
		return -1;
	}

	return -1;
}

SimpleImage SimpleImage::operator-(const SimpleImage &im)
{
	SimpleImage res;
	if (bytes_per_pixel != 1 || im.bytes_per_pixel != 1 ||
		im.width != width || im.height != height)
		return res;

	res.createImage(width, height, bytes_per_pixel);

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			res[j][i] = max(0, data_ptr[j*width + i] - im.data_ptr[j*width + i]);

	return res;
}


int SimpleImage::writeToFile(const char *filename, FileType file_type)
{
	if (file_type==FileType::FT_PGM)
	{
		writePGM(filename, width, height, data_ptr);
	}

	if (file_type==FileType::FT_JPG)
	{
		return -1;
	}

	return 0;
}

unsigned char * SimpleImage::scaleImage2(double scale, unsigned char *data, int &iWidth, int &iHeight)
{
	int iWOrg = iWidth;
	iWidth = (int)((double)iWidth / scale);
	iHeight = (int)((double)iHeight / scale);

	double countX = 0, countY = 0;

	byte *newData = new byte[iWidth*iHeight];
	byte *ptr1, *ptr2;

	ptr1 = newData;
	ptr2 = data;
	for (int j = 0; j<iHeight; j++)
	{
		for (int i = 0; i<iWidth; i++)
		{
			*ptr1 = *ptr2;
			ptr1++;
			ptr2 = data + int(countX) + (int(countY)) * iWOrg;
			countX += scale;
		}
		countY += scale;
		countX = 0;
	}


	//  delete []data;

	return newData;
}

int SimpleImage::scaleImage(double scale)
{
	if (bytes_per_pixel != 1) return -1;
	int nWidth = width; 
	int nHeight = height;
	unsigned char *new_data;

	new_data = scaleImage2(scale, data_ptr, nWidth, nHeight);

	delete[]data_ptr;

	data_ptr = new_data;
	width = nWidth; 
	height = nHeight;
	prepareBitmapInfo();

	return 0;
}

void SimpleImage::zeroVariables()
{
	bip = NULL;
	data_ptr = NULL;
	bytes_per_pixel = 1;
}

void SimpleImage::prepareBitmapInfo()
{
	if (width==-1 || height==-1) return;

	if (bip!=NULL) delete bip;

    bip = (BITMAPINFO*) new unsigned char[sizeof(BITMAPINFOHEADER)];
    bip->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bip->bmiHeader.biWidth = width;
    bip->bmiHeader.biHeight = height;
    bip->bmiHeader.biPlanes = 1;
	bip->bmiHeader.biBitCount = bytes_per_pixel*8;
    bip->bmiHeader.biCompression = BI_RGB;
    bip->bmiHeader.biSizeImage = 0;
    bip->bmiHeader.biXPelsPerMeter = 0;
    bip->bmiHeader.biYPelsPerMeter = 0;
    bip->bmiHeader.biClrUsed = 0;
    bip->bmiHeader.biClrImportant = 0;
}


BITMAPINFO *SimpleImage::getBip()
{
	if (bip==NULL) prepareBitmapInfo();
	return bip;
}
	
unsigned char* SimpleImage::operator[](int row)
{
	return data_ptr + row*width*bytes_per_pixel;
}


SimpleImage& SimpleImage::operator=(const SimpleImage &si)
{
	if (data_ptr!=NULL) delete []data_ptr;
	width = si.width;
	height = si.height;
	bytes_per_pixel = si.bytes_per_pixel;

	if (width!=-1 && height!=-1)
	{
		data_ptr = new unsigned char[width*height*bytes_per_pixel];
		memcpy(data_ptr, si.data_ptr, width*height*bytes_per_pixel);
	}

	prepareBitmapInfo();
	return *this;
}

void SimpleImage::createImage(int _width, int _height, int bpp)
{
	freeMemory();
	setSize(_width, _height);
	data_ptr = new unsigned char[_width*_height*bpp]();
	this->bytes_per_pixel = bpp;
}


void SimpleImage::setSize(int _width, int _height)
{
	width = _width; 
	height = _height; 

	prepareBitmapInfo();
}

double SimpleImage::getSubpixelImageValue(SimplePoint<double> &p)
{
	double x0, y0;
	double x_r, y_r;
	double val = 0.0;
	double diff_x, diff_y;

	if (p.x<0.0 || p.y<0.0 || p.x>(width - 2) || p.y>(height - 2)) return val;

	x0 = floor(p.x);
	y0 = floor(p.y);

	int pos = (unsigned short)x0 + (unsigned short)y0 * width;

	x_r = p.x - x0;
	y_r = p.y - y0;

	diff_x = 1.0 - x_r;
	diff_y = 1.0 - y_r;

	val = diff_x*diff_y*data_ptr[pos] + x_r*diff_y*data_ptr[pos + 1] +
		diff_x*y_r*data_ptr[pos + width] + x_r*y_r*data_ptr[pos + width + 1];

	return val;
}


void SimpleImage::freeMemory()
{
	if (data_ptr!=NULL) 
		delete []data_ptr;
	
	if (bip!=NULL)
		delete bip;

	zeroVariables();
}

SimpleImage::~SimpleImage()
{
	freeMemory();
}


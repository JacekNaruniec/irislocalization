#include "string.h"
#include "stdio.h"
#include "PGM.h"
#include <algorithm>

using namespace std;

int getMagicNumber(int n)
{
	int i=8;
	while (i<n) i+=8;
	return i;
}

bool writePGM(const char *name, int w, int h, int *data)
{
	int max_v = data[0];
	int min_v = data[0];
	double diff;
	double out;

	unsigned char *tmp = new unsigned char[w*h]; 

	for (int i=0; i<w*h; i++)
	{
		max_v = max(data[i], max_v);
		min_v = min(data[i], min_v);
	}

	diff = (double)(max_v - min_v);

	if (diff!=0)
		for (int i=0; i<w*h; i++)
		{
			out = ((data[i]-min_v)/diff)*255.0;
			tmp[i] = (unsigned char)out;
		}
	else
		memset(tmp, 0, w*h);

	writePGM(name, w, h, tmp);

	delete []tmp;
	return true;
}


bool writePGM(const char *name, int w, int h, double *data)
{
	double max_v = data[0];
	double min_v = data[0];
	double diff;
	double out;

	unsigned char *tmp = new unsigned char[w*h]; 

	for (int i=0; i<w*h; i++)
	{
		max_v = max(data[i], max_v);
		min_v = min(data[i], min_v);
	}

	diff = (double)(max_v - min_v);

	if (diff!=0)
		for (int i=0; i<w*h; i++)
		{
			out = ((data[i]-min_v)/diff)*255.0;
			tmp[i] = (unsigned char)out;
		}
	else
		memset(tmp, 0, w*h);

	writePGM(name, w, h, tmp);

	delete []tmp;
	return true;
}

bool writePGM(const char *name, int w, int h, float *data)
{
	float max_v = data[0];
	float min_v = data[0];
	float diff;
	float out;

	unsigned char *tmp = new unsigned char[w*h]; 

	for (int i=0; i<w*h; i++)
	{
		max_v = max(data[i], max_v);
		min_v = min(data[i], min_v);
	}

	diff = (float)(max_v - min_v);

	if (diff!=0)
		for (int i=0; i<w*h; i++)
		{
			out = ((data[i]-min_v)/diff)*255.0f;
			tmp[i] = (unsigned char)out;
		}
	else
		memset(tmp, 0, w*h);

	writePGM(name, w, h, tmp);

	delete []tmp;
	return true;
}


bool writePGM(const char *name, int w, int h, double *data, unsigned char *add_image)
{
	double max_v = data[0];
	double min_v = data[0];
	double diff;
	double out;

	unsigned char *tmp = new unsigned char[w*h]; 

	for (int i=0; i<w*h; i++)
	{
		max_v = max(data[i], max_v);
		min_v = min(data[i], min_v);
	}

	diff = (double)(max_v - min_v);

	if (diff!=0)
		for (int i=0; i<w*h; i++)
		{
			out = ((data[i]-min_v)/diff)*255.0;
			tmp[i] = (unsigned char)((out + add_image[i])/2);
		}
	else
		memset(tmp, 0, w*h);

	writePGM(name, w, h, tmp);

	delete []tmp;
	return true;
}
bool writePGM(const char *name, int w, int h, unsigned char *data)
{
  FILE *f;
  fopen_s(&f, name, "wb");

  if (f==NULL) return false;

  char header[1024];

  sprintf(header, "P5 %d %d %d \0", w, h, 255);
  fwrite(header, 1, strlen(header), f); 

  fwrite(data, w*h, 1, f);

  fclose(f);

  return true;
}
unsigned char *readPGM(const char *name, int &w, int &h, bool adjustToEightMultiple)
{
	FILE *f;
	char head[255], magic_number[2];
	int fields[3];
	unsigned char *image;
	int i, n_read, pos = 0;
	int gray;


	if (fopen_s(&f, name, "rb") != 0) return NULL;

	n_read = 0;
	pos += fread(magic_number, 1, 2, f);
	fgetc(f);

	while (n_read != 3)
	{
		fread(head, 1, 1, f);
		fseek(f, -1, 1);

		if (head[0] == '#')
			fgets(head, 255, f);
		else
		{
			fscanf(f, "%d", fields + (n_read++));
			fgetc(f);
		}
	}

	w = fields[0];
	h = fields[1];
	gray = fields[2];

	if (w < 1 || h < 1 || gray != 255) return NULL;

	image = new unsigned char[w*h];

	fread(image, 1, w*h, f);

	fclose(f);

	if (adjustToEightMultiple)
	{
		// width and height must be 8 bytes multiple (Win bug?)
		int w2, h2;
		w2 = getMagicNumber(w);
		h2 = getMagicNumber(h);

		unsigned char *image2 = new unsigned char[w2*h2];
		memset(image2, 0, w2*h2);

		int j;
		for (i = 0; i < w; i++)
		for (j = 0; j < h; j++)
			image2[i + j*w2] = image[i + j*w];

		w = w2;
		h = h2;
		delete[]image;

		return image2;
	}
	else return image;
}

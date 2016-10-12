#pragma once

#include <vector>
#include "mat.h"
#include "simplepoint.h"
#include "simpleimage.h"
#include <unordered_map>

using namespace std;

struct IrisParameters
{
	double x, y;
	double r;
	bool eyeClosed; 
};

class IrisLocalizator
{
public:
	IrisLocalizator();

	vector<IrisParameters> localizePupilPositions(SimpleImage image, vector<Mat<float>> eyesPoints);

	~IrisLocalizator();
	
private:
	IrisParameters localizeIris(SimpleImage image, Mat<float> eyePoints, float minR, float maxR);
	void createVariables(int imageWidth, int imageHeight);
	void computeEdgesVectorsAndMagnitudes(SimpleImage &image);
	IrisParameters findCenterBounded(SimpleImage &image, float minR, float maxR, Mat<float> eyePoints);
	Mat<unsigned char> findPixelsInBound(int boundWidth, int boundHeight, Mat<float> boundPoints);
	bool inBounds(int x, int y, Mat<float> &eyePoints);
	void setNextCirclePoint(float x0, float y0, float r0, float &x1, float &y1, float r1, float sign);
	void smoothMat(Mat<float> &m, int filterSize);
	bool isEyeClosed(Mat<unsigned char> &acceptedPoints);
	IrisParameters refineResults(SimpleImage &image, float min_r, float max_r, IrisParameters position);

	double calculateEllipseContrast(double r, double xc, double yc, double r_step, SimpleImage &image, int shapeIndex);

	void createCirclesPoints();

	vector<Mat<double>> circleShapes; 
	Mat<float> magnitudeImage, edgesVectors[2], edgesMagnitudes;
};
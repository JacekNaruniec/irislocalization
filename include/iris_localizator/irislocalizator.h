#pragma once

#include <vector>
#include "mat.h"
#include "simplepoint.h"
#include "simpleimage.h"
#include <unordered_map>

using namespace std;

struct IrisParameters
{
	double x, y;		// position of the iris
	double r;			// radius of the iris
	bool eyeClosed;		// true if eye closure is detected
};

class IrisLocalizator
{
public:
	IrisLocalizator();

	/**
	Function for localizing pupil centers. 
	Params:
	image: loaded image (you can create SimpleImage and execute image.readFromFile(filename, FT_PGM) to load PGM files. 
	eyesPosition: matrix defining the position of the eye points. Every row have two columns, corresponding to 
				  separate point coordinates (x, y). There should be 6 points (rows) in the following order:
				  [0] left eye corner [1][2] on the edge of the upper eyelid interpolated between left and right
				  eye corner; [2] should be closer to the left eye corner; [3] right eye corner 
				  [4][5] on the edge of the lower eyelid interpolated between left and right
				  eye corner; [5] should be closer to the right eye corner. This arrangement corresponds
				  to the points 37-41 from http://ibug.doc.ic.ac.uk/resources/facial-point-annotations/.
	**/
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
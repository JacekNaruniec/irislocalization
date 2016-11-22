#include "irislocalizator.h"

#define M_PI	3.14159265359

IrisLocalizator::IrisLocalizator()
{
	createCirclesPoints();
}

IrisLocalizator::~IrisLocalizator()
{
}

void IrisLocalizator::createCirclesPoints()
{
	circleShapes.resize(4);
	int nCirclePoints = 32;
	double angle, maxAngle = M_PI / 5.0;
	int quarterPoints = nCirclePoints / 4;
	double step = maxAngle / (quarterPoints - 1);
	double x, y;

	for (size_t i = 0; i < circleShapes.size(); i++)
		circleShapes[i].setSize(2, nCirclePoints);

	for (int i = 0; i < quarterPoints; i++)
	{
		angle = maxAngle - step*i;
		x = cos(angle);
		y = sqrt(1.0 - x*x);

		circleShapes[0][0][i] = (double)x;
		circleShapes[0][1][i] = (double)y;

		circleShapes[0][0][2 * quarterPoints - i - 1] = (double)x;
		circleShapes[0][1][2 * quarterPoints - i - 1] = (double)-y;

		circleShapes[0][0][nCirclePoints - 1 - i] = (double)-x;
		circleShapes[0][1][nCirclePoints - 1 - i] = (double)y;

		circleShapes[0][0][2 * quarterPoints + i] = (double)-x;
		circleShapes[0][1][2 * quarterPoints + i] = (double)-y;
	}
	for (int i = 0; i < quarterPoints; i++)
	{
		circleShapes[1][0][i] = circleShapes[0][0][i];
		circleShapes[1][1][i] = 0.95*circleShapes[0][1][i];

		circleShapes[2][0][i] = circleShapes[0][0][i];
		circleShapes[2][1][i] = 0.90*circleShapes[0][1][i];

		circleShapes[3][0][i] = circleShapes[0][0][i];
		circleShapes[3][1][i] = 0.85*circleShapes[0][1][i];
	}
}


vector<IrisParameters> IrisLocalizator::localizePupilPositions(SimpleImage image, vector<Mat<float>> eyesPoints)
{
	vector<IrisParameters> retParameters(2);
	image.convertToGrayscale();

	for (int eyeIndex = 0; eyeIndex < 2; eyeIndex++)
	{
		SimpleRect eyeBound;

		int a = (int)(eyesPoints[eyeIndex][3][0] - eyesPoints[eyeIndex][0][0]);
		int b = (int)(eyesPoints[eyeIndex][3][1] - eyesPoints[eyeIndex][0][1]);

		int eyeSize = (int)sqrt(a*a + b*b);

		eyeBound.set((unsigned int)(eyesPoints[eyeIndex][0][0] - 5), (unsigned int)(eyesPoints[eyeIndex][3][0] + 5),
			(unsigned int)(eyesPoints[eyeIndex][1][1] - 5), (unsigned int)(eyesPoints[eyeIndex][5][1] + 5));

		if (eyeBound.getWidth() < 5 || eyeBound.getHeight() < 5 || eyeBound.getTop() < 0 || eyeBound.getLeft() < 0 || eyeBound.getBottom() >= image.height ||
			eyeBound.getRight() >= image.width)
			continue;


		float minIrisSize = eyeSize / 6.0f;
		float maxIrisSize = max(2.0f, eyeSize * 0.4f);

		SimpleImage irisImage = image; 
		irisImage.cutImage(eyeBound.getLeft(), eyeBound.getTop(), eyeBound.getWidth(), eyeBound.getHeight());
		irisImage.smoothImage();

		Mat<float> cuttedImageEyesPoints = eyesPoints[eyeIndex];
		Mat<float> shift(1, 2);

		shift.data[0] = (float)eyeBound.getLeft();
		shift.data[1] = (float)eyeBound.getTop();

		cuttedImageEyesPoints.subtractRow(shift);

		retParameters[eyeIndex] = localizeIris(irisImage, cuttedImageEyesPoints, minIrisSize, maxIrisSize);
		
		retParameters[eyeIndex].x += shift.data[0];
		retParameters[eyeIndex].y += shift.data[1];

	}

	return retParameters;
}

void IrisLocalizator::createVariables(int imageWidth, int imageHeight)
{
	if (magnitudeImage.height != imageHeight)
	{
		magnitudeImage.setSize(imageHeight, imageWidth);
		edgesVectors[0].setSize(imageHeight, imageWidth);
		edgesVectors[1].setSize(imageHeight, imageWidth);
		edgesMagnitudes.setSize(imageHeight, imageWidth);
	}
	else
	{
		magnitudeImage = 0;
		edgesVectors[0] = 0;
		edgesVectors[1] = 0;
		edgesMagnitudes = 0;
	}
}


void IrisLocalizator::computeEdgesVectorsAndMagnitudes(SimpleImage &image)
{
	unsigned char *p_src;
	int w = image.width;
	int h = image.height;

	int sum_hor, sum_ver;
	double mag, dsum_hor, dsum_ver;
	int pos;

	for (int y = 1; y<(h - 1); y++)
	{
		p_src = image.data_ptr + y*w + 1;
		pos = y*w;

		for (int x = 1; x<(w - 1); x++)
		{
			sum_hor = 0;
			sum_ver = 0;

			sum_hor = *(p_src + w) - *(p_src - w);
			sum_ver = *(p_src + 1) - *(p_src - 1);

			dsum_hor = (double)sum_hor;
			dsum_ver = (double)sum_ver;
			mag = sqrt(dsum_hor*dsum_hor + dsum_ver*dsum_ver);

			edgesMagnitudes[y][x] = (float)mag;

			if (mag>0.0)
			{
				edgesVectors[0][y][x] = (float)(-dsum_ver / mag);
				edgesVectors[1][y][x] = (float)(-dsum_hor / mag);
			}

			p_src++;
		}
	}
}

bool IrisLocalizator::inBounds(int x, int y, Mat<float> &eyePoints)
{
	float v1[2], v2[2], dot;

	for (int v = 0; v < 6; v++)
	{
		v1[0] = eyePoints[(v + 1) % 6][0] - eyePoints[v][0];
		v2[0] = x - eyePoints[v][0];

		v1[1] = eyePoints[(v + 1) % 6][1] - eyePoints[v][1];
		v2[1] = y - eyePoints[v][1];

		dot = v1[0] * v2[1] - v1[1] * v2[0];
		if (dot <= 0)
			return false;
	}
	return true;
}

Mat<unsigned char> IrisLocalizator::findPixelsInBound(int boundWidth, int boundHeight, Mat<float> boundPoints)
{
	Mat<unsigned char> isInBounds(boundHeight, boundWidth);

	for (int y = 1; y<(boundHeight - 1); y++)
	for (int x = 1; x < (boundWidth - 1); x++)
	{
		isInBounds[y][x] = (unsigned char)inBounds(x, y, boundPoints);
	}
	return isInBounds;
}


void IrisLocalizator::setNextCirclePoint(float x0, float y0, float r0, float &x1, float &y1, float r1, float sign)
{
	double d = sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
	double a = (r0*r0 - r1*r1 + d*d) / (2.0*d);
	double h = sqrt(r0*r0 - a*a);
	double x2 = x0 + a*(x1 - x0) / d;
	double y2 = y0 + a*(y1 - y0) / d;
	double x3 = x2 + sign*h*(y1 - y0) / d;
	double y3 = y2 - sign*h*(x1 - x0) / d;

	x1 = (float)x3;
	y1 = (float)y3;
}


void IrisLocalizator::smoothMat(Mat<float> &m, int filterSize)
{
	Mat<float> smoothed1(m.height, m.width);

	filterSize = max(1, filterSize);
	int w = m.width;
	int h = m.height;
	float sum;

	for (int x = filterSize; x < (w - filterSize); x++)
	for (int y = filterSize; y < (h - filterSize); y++)
	{
		sum = 0.0;
		for (int i = -filterSize; i <= filterSize; i++)
			sum += m[y + i][x];

		sum /= filterSize * 2;
		smoothed1[y][x] = sum;
	}

	for (int x = filterSize; x < (w - filterSize); x++)
	for (int y = filterSize; y < (h - filterSize); y++)
	{
		sum = 0.0;
		for (int i = -filterSize; i <= filterSize; i++)
			sum += smoothed1[y][x + i];

		sum /= filterSize * 2;
		m[y][x] = sum;
	}

	return;
}



bool IrisLocalizator::isEyeClosed(Mat<unsigned char> &acceptedPoints)
{
	int w = acceptedPoints.width;
	int h = acceptedPoints.height;
	int highest = 0;
	for (int x = 0; x < w; x++)
	{
		int miny = h;
		int maxy = 0;
		for (int y = 0; y < h; y++)
		if (acceptedPoints[y][x]>0)
		{
			miny = y;
			break;
		}

		for (int y = h - 1; y >= 0; y--)
		if (acceptedPoints[y][x]>0)
		{
			maxy = y;
			break;
		}

		highest = max(highest, maxy - miny);
	}

	int threshold = h / 5;
	if (highest < threshold)
		return true;
	else
		return false;
}


IrisParameters IrisLocalizator::findCenterBounded(SimpleImage &image, float minR, float maxR, Mat<float> eyePoints)
{
	int deltaR = (int)((maxR - minR) + 1);
	float r = minR;

	int w = image.width; 
	int h = image.height;

	IrisParameters ret;

	vector<Mat<float>> votesCount(deltaR, Mat<float>(h, w));
	vector<Mat<float>> magnitudesSum(deltaR, Mat<float>(h, w));

	Mat<unsigned char> isInBounds = findPixelsInBound(w, h, eyePoints);

	if (isEyeClosed(isInBounds))
	{
		ret.eyeClosed = true;
		ret.x = eyePoints.subMat(4, 6, 0, 1).mean();
		ret.y = eyePoints.subMat(4, 6, 1, 2).mean();
		ret.r = (minR + maxR) / 2.0f;
		return ret;
	}


	for (int y = 1; y < (h - 1); y++)
	for (int x = 1; x < (w - 1); x++)
	{
		float magnitude = edgesMagnitudes[y][x];

		if (isInBounds[y][x] && magnitude > 1.0)
		{
			r = minR;

			for (int radiusCounter = 0; radiusCounter < deltaR; radiusCounter++)
			{
				int nHalfVotes = (int)(3.14 * (r / 2.0));

				int initialPosX = (int)(edgesVectors[0][y][x] * r + x + 0.5f);
				int initialPosY = (int)(edgesVectors[1][y][x] * r + y + 0.5f);
				float fvotePosX = (float)initialPosX;
				float fvotePosY = (float)initialPosY;

				for (int i = -nHalfVotes; i <= nHalfVotes; i++)
				{
					if (i == 0)
					{
						fvotePosX = (float)initialPosX;
						fvotePosY = (float)initialPosY;
					}

					if (i < 0)
						setNextCirclePoint((float)x, (float)y, r, fvotePosX, fvotePosY, 1.0f, -1.0f);
					else if (i>0)
						setNextCirclePoint((float)x, (float)y, r, fvotePosX, fvotePosY, 1.0f, 1.0f);

					int votePosX = (int)fvotePosX;
					int votePosY = (int)fvotePosY;


					if (votePosX < 0 || votePosY < 0 || votePosX >(w - 1) || votePosY >(h - 1) || !isInBounds[votePosY][votePosX])
						continue;

					votesCount[radiusCounter][votePosY][votePosX] += 1.0;
					magnitudesSum[radiusCounter][votePosY][votePosX] += magnitude;
				}
				r += 1.0f;
			}
		}
	}

	vector<Mat<float>> weights = magnitudesSum;
	Mat<unsigned char> ones(h, w);
	ones = 1;

	for (int radiusCounter = 0; radiusCounter < deltaR; radiusCounter++)
		smoothMat(votesCount[radiusCounter], (int)((minR + radiusCounter) / 2.0));

	for (int y = 1; y < (h - 1); y++)
	for (int x = 1; x < (w - 1); x++)
	{
		r = minR;
		for (int radiusCounter = 0; radiusCounter < deltaR; radiusCounter++)
		{
			if (!isInBounds[y][x])	continue;
			magnitudesSum[radiusCounter][y][x] *= votesCount[radiusCounter][y][x];
			r += 1.0f;
		}
	}

	Mat<float> bestAnswers(deltaR, 4);
	for (int radiusCounter = 0; radiusCounter < deltaR; radiusCounter++)
	{
		r = minR;
		int argmax = magnitudesSum[radiusCounter].argmax();
		float maxval = magnitudesSum[radiusCounter].data[argmax];
		bestAnswers[radiusCounter][0] = (float)(argmax % w);
		bestAnswers[radiusCounter][1] = (float)(argmax / w);
		bestAnswers[radiusCounter][2] = r;
		bestAnswers[radiusCounter][3] = maxval;
		r += 1.0;
	}

	auto meanAnswer = bestAnswers.meanRow();

	ret.eyeClosed = false;
	ret.x = meanAnswer.data[0];
	ret.y = meanAnswer.data[1];
	ret.r = meanAnswer.data[2];

	return ret;
}


double IrisLocalizator::calculateEllipseContrast(double r, double xc, double yc, double r_step, SimpleImage &image, int shapeIndex)
{
	SimplePoint<double> p;
	double sum = 0.0;
	double next_r, prev_r;
	double diff;
	double val1, val2;

	next_r = r + r_step;
	prev_r = r - r_step;
	double sum_diffs = 0;
	int nCirclePoints = circleShapes[shapeIndex].width;

	double *xs = circleShapes[shapeIndex][0];
	double *ys = circleShapes[shapeIndex][1];

	for (int i = 0; i<nCirclePoints; i++)
	{
		p.x = xs[i] * next_r + xc;
		p.y = ys[i] * next_r + yc;
		val1 = image.getSubpixelImageValue(p);

		p.x = xs[i] * prev_r + xc;
		p.y = ys[i] * prev_r + yc;
		val2 = image.getSubpixelImageValue(p);

		diff = val1 - val2;
		sum_diffs += diff;
	}

	sum = sum_diffs;
	return sum;
}


IrisParameters IrisLocalizator::refineResults(SimpleImage &image, float min_r, float max_r, IrisParameters position)
{
	double cx = position.x;
	double cy = position.y;
	double radius = position.r;

	double xStep = 0.5, yStep = 0.5, rStep = 0.5;

	double minx, maxx, miny, maxy, minr, maxr;

	double diff, bestDiff = 0;
	double bestX = 0, bestY = 0, bestR = 0;
	int nShapes = 4, bestE = 0;

	float scale = (max_r + min_r) / 6.0f;
	
	float maxshift = max(1.0f, 1.0f * scale);
	float maxrchange = max(1.0f, 1.0f * scale);

	minx = cx - maxshift;
	maxx = cx + maxshift;
	miny = cy - maxshift;
	maxy = cy + maxshift;

	minr = radius - maxrchange;
	if (minr < min_r) minr = min_r;
	maxr = radius + maxrchange;
	if (maxr > max_r) maxr = max_r;


	for (int shapeIndex = 0; shapeIndex<nShapes; shapeIndex++)
	for (double x = minx; x <= maxx; x += xStep)
	for (double y = miny; y <= maxy; y += yStep)
	for (double r = minr; r <= maxr; r += rStep)
	{
		diff = calculateEllipseContrast(r, x, y, rStep, image, shapeIndex);

		if (diff>bestDiff)
		{
			bestDiff = diff;
			bestX = x;
			bestY = y;
			bestR = r;
			bestE = shapeIndex;
		}
	}

	position.x = (float)bestX;
	position.y = (float)bestY;
	position.r = bestR;
	return position;
}


IrisParameters IrisLocalizator::localizeIris(SimpleImage image, Mat<float> eyePoints, float minR, float maxR)
{
	IrisParameters result;

	createVariables(image.width, image.height);

	computeEdgesVectorsAndMagnitudes(image);

	result = findCenterBounded(image, minR, maxR, eyePoints);
	if (result.eyeClosed == false)
		result = refineResults(image, minR, maxR, result);
	
	return result;
}
/*
	Iris localization

Copyright Jacek Naruniec and Marek Kowalski
Warsaw Univeristy of Technology
Application demostrates iris localization algorithm from our article:

"Webcam based system for video occulography", published in IET Computer Vision, 2016

REQUIREMENTS:
- Visual Studio 2013, 
- BioID database (can be downloaded from https://www.bioid.com/About/BioID-Face-Database
- our facial features landmark set for the BioID database (landmarks included in the package)

No third-party libraries required. 
*/

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include "file_utils.h"
#include "mat.h"
#include "irislocalizator.h"


using namespace std; 

vector<Mat<float>> loadAllPupilsCoordinates(string databaseDirectory, vector<string> &filenames)
{
	vector<Mat<float>> coordinates;
	Mat<float> actualFileCoordinates(2, 2);
	for (auto filename : filenames)
	{
		filename = filename.substr(0, filename.length() - 3);
		filename.append("pts");
		FILE *f = fopen((databaseDirectory + filename).c_str(), "rt");
		if (f == NULL)
			continue; 

		fscanf(f, "%*s %*s %*s %*s %*s %f %f %f %f", 
			&actualFileCoordinates[0][0], &actualFileCoordinates[0][1], &actualFileCoordinates[1][0], &actualFileCoordinates[1][1]);

		coordinates.push_back(actualFileCoordinates);

		fclose(f);
	}

	return coordinates;
}


vector<Mat<float>> loadAllFacialFeaturesCoordinates(string databaseDirectory, vector<string> &filenames)
{
	vector<Mat<float>> coordinates;
	for (auto filename : filenames)
	{
		Mat<float> actualFileCoordinates;
		filename = filename.substr(0, filename.length() - 3);
		filename.append("txt");
		actualFileCoordinates.readFromFile((databaseDirectory+filename).c_str());

		if (actualFileCoordinates.height == 1)
			printf("Error when reading file: %s\n", (databaseDirectory + filename).c_str());

		coordinates.push_back(actualFileCoordinates);
	}

	return coordinates;
}

void printGreetings()
{
	printf("--------------------------------------------------------\n\n");
	printf("Iris localization\n");
	printf("Copyright Jacek Naruniec and Marek Kowalski\n");
	printf("Warsaw Univeristy of Technology\n");
	printf("\nApplication can be used to replicate the results from our article:\n");
	printf("\n\t\"Webcam based system for video occulography\"\n\tpublished in IET Computer Vision, 2016\n");
	printf("\n-------------------------------------------------------\n");
}

void printUsage()
{
	printf("\nUsage:\nIrisLocalization.exe [BIOID database path]\n");
}

vector<Mat<float>> localizePupilsInImages(string databaseDirectory, vector<string> &filenames, vector<Mat<float>> &facialFeaturesCoordinates)
{
	vector<Mat<float>> localizedPupilPositions(filenames.size()); 
	IrisLocalizator irisLocalizator; 
	for (size_t fileIndex = 0; fileIndex < filenames.size(); fileIndex++)
	{
		if (facialFeaturesCoordinates[fileIndex].height == 1)
		{
			printf("No coodrinates, skipping file %s\n", filenames[fileIndex].c_str());
			continue;
		}

		SimpleImage image; 
		image.readFromFile((databaseDirectory + filenames[fileIndex]).c_str(), FT_PGM);
		if (image.width == -1)
		{
			printf("Couldn't load image: %s\n", (databaseDirectory + filenames[fileIndex]).c_str());
			continue;
		}

		vector<Mat<float>> eyesPoints(2);
		eyesPoints[0] = facialFeaturesCoordinates[fileIndex].subMat(36, 42, 0, 2);
		eyesPoints[1] = facialFeaturesCoordinates[fileIndex].subMat(42, 48, 0, 2);

		vector<IrisParameters> positions = irisLocalizator.localizePupilPositions(image, eyesPoints);
		localizedPupilPositions[fileIndex].setSize(2, 2);

		for (int i = 0; i < 2; i++)
		{
			localizedPupilPositions[fileIndex][i][0] = (float)positions[i].x;
			localizedPupilPositions[fileIndex][i][1] = (float)positions[i].y;
		}
		
		printf("\nImage %d processed.", fileIndex + 1);

	}

	return localizedPupilPositions;
}

float getScore(Mat<float> groundTruth, Mat<float> localized)
{
	float DL = sqrt(pow(groundTruth[0][0] - localized[0][0], 2) + pow(groundTruth[0][1] - localized[0][1], 2));
	float DR = sqrt(pow(groundTruth[1][0] - localized[1][0], 2) + pow(groundTruth[1][1] - localized[1][1], 2));
	float D = sqrt(pow(groundTruth[0][0] - groundTruth[1][0], 2) + pow(groundTruth[0][1] - groundTruth[1][1], 2));

	float maxDLDR = max(DL, DR);
	return maxDLDR / D;
}

pair<vector<float>, vector<float>> getAccuracyGraphPoints(vector<float> scores, float start_threshold = 0.005, float step = 0.005, float end_threshold = 0.5)
{
	vector<float> thresholdScores, thresholds;

	int nSteps = (int)((end_threshold - start_threshold) / step + 1);
	float threshold = start_threshold;

	for (int i = 0; i < nSteps; i++)
	{
		float acc = 0;

		for (size_t score = 0; score < scores.size(); score++)
			if (scores[score] < threshold)
				acc++;

		acc /= (float)scores.size();
		thresholdScores.push_back(acc);
		thresholds.push_back(threshold);
		threshold += step;
	}

	return make_pair(thresholds, thresholdScores);
}


void testAccuracy(vector<Mat<float>> localizedPupilCoordinates, vector<Mat<float>> pupilsGroundTruthCoordinates)
{
	size_t nExamples = localizedPupilCoordinates.size();
	vector<float> scores(nExamples);
	double sumScores = 0;
	for (size_t i = 0; i < nExamples; i++)
	{
		scores[i] = getScore(localizedPupilCoordinates[i], pupilsGroundTruthCoordinates[i]);
		printf("File %.4d, score = %f\n", i + 1, scores[i]);
		sumScores += scores[i];
	}


	sumScores /= (double)nExamples;
	printf("\n\nMean score: %f", (float)sumScores);

	pair<vector<float>, vector<float>> accuraciesAndThresholds = getAccuracyGraphPoints(scores);
	vector<float> thresholds = accuraciesAndThresholds.first;
	vector<float> accuracies = accuraciesAndThresholds.second;

	printf("\n Scores at accuracies:\n");

	for (size_t i = 0; i < thresholds.size(); i++)
	{
		printf("\nthreshold: %.4f\taccuracy: %.4f", thresholds[i], accuracies[i]);
	}

}


int main(int argc, char *argv[])
{
	string databaseDirectory;

	printGreetings();
	printUsage();

	if (argc > 1)
		databaseDirectory = argv[1];
	else
	{
		databaseDirectory = "./BioID-FaceDatabase-V1.2/";
		printf("\nSetting default database directory: %s\n", databaseDirectory.c_str());
	}

	vector<string> filenames = loadAllFileNames(databaseDirectory, "pgm", false);
	if (filenames.size() == 0)
	{
		printf("\nNo pgm files in database directory ...");
		return -1;
	}

	vector<Mat<float>> facialFeaturesCoordinates = loadAllFacialFeaturesCoordinates(databaseDirectory, filenames);
	printf("\n%d coordinates loaded.", facialFeaturesCoordinates.size());

	vector<Mat<float>> pupilsGroundTruthCoordinates = loadAllPupilsCoordinates(databaseDirectory, filenames);

	vector<Mat<float>> localizedPupilCoordinates = localizePupilsInImages(databaseDirectory, filenames, facialFeaturesCoordinates);

	testAccuracy(localizedPupilCoordinates, pupilsGroundTruthCoordinates);
	printf("\n");
	return 0;
}
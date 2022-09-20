#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Vector3.h"
#include "Camera.h"
#include "Sphere.h"

using namespace cv;
using namespace std;

int main()
{
	//Camera camera{};
	//Sphere sphere{ { 0, 0, -10 }, 5.0f};

	//Vector3 pixelPosition{ Vector3{0, 0, 0} };
	//Ray currentRay = camera.ScreenToRay(pixelPosition);
	//std::cout << "Intersect : " << sphere.intersect(currentRay);

	const int IMG_SIZE = 400;

	Camera camera{};
	std::vector<Sphere> spheres = {
		Sphere(Vector3{ IMG_SIZE / 2, IMG_SIZE / 2, -10 }, 100.0f),
		Sphere ({ 20, 20, -10 }, 60.0f)
	};

	const float MIN_INTENSITY = 0, MAX_INTENSITY = 255;

	Mat image = Mat::zeros(IMG_SIZE, IMG_SIZE, CV_8UC3);
	image = cv::Scalar(0, 0, 255);

	for (size_t y = 0; y < image.rows; y++)
	{
		for (size_t x = 0; x < image.cols; x++)
		{
			// S'occupe de l'interaction des rayants partants de la caméra vers les formes présentes dans la scene
			Vector3 pixelPosition{ Vector3{(float) x, (float) y, 0.0} };
			Ray ray = camera.ScreenToRay(pixelPosition);

			for (const auto& current : spheres)
			{
				float t (current.intersect(ray));

				// Applique les couleurs selon le degré d'intersection
				Vec3b& color = image.at<Vec3b>(y, x);
				if (t != 0)
				{
					unsigned char intensity(std::clamp(t, MIN_INTENSITY, MAX_INTENSITY));
					color = { intensity, intensity, intensity };
				}
			}
		}
	}

	imshow("Display Window", image);
	//imwrite("C:/Users/wpetit/Desktop/Test.png", image);

	waitKey(0);
	return 0;
}
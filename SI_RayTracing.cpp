#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

#include "Vector3.h"
#include "Camera.h"
#include "Sphere.h"
#include "LightSource.h"
#include "SI_RayTracing.h"

using namespace cv;
using namespace std;

RayHit& GetNearestHit(std::vector<Sphere>& spheres, const Ray& ray)
{
	// Par défaut, le hit touche l'infinie
	RayHit nearestHit ({ std::numeric_limits<float>::max(), std::nullopt });

	for (const auto& current : spheres)
	{
		std::optional<float> t(current.intersect(ray));

		bool bIntersection(t.has_value());
		if (!bIntersection) { continue; }

		if (t.value() < nearestHit.distance)
		{
			nearestHit = { t.value(), &current };
		}
	}

	return nearestHit;
}

float GetOutgoingLightFor(const Sphere& current, const Ray& ray, float distanceWithCamera, LightSource& lightSource)
{
	const float ALBEDO = 0.7f;

	// Point d'intersection
	Vector3 x(ray.origin + distanceWithCamera * ray.direction);

	// Direction entre la lampe et l'intersection
	Vector3 directionLampe(lightSource.position - x);
	float distance(directionLampe.norm());
	Vector3 w0(directionLampe / distance);

	// Normale pour la surface d'intersection du rayon
	Vector3 normal((x - current.center).unitVector());
	
	float f{ normal.dot(w0) / (float) M_PI};
	float cosTeta(std::abs(normal.dot(w0)));

	return (lightSource.intensity * f * ALBEDO * cosTeta) / (distance * distance);
}


int main()
{
	//TODO Convertir en test automatique pour juger l'efficacité de l'intersection
	//OrthogonalCamera camera{};
	//Sphere sphere{ { 0, 0, -10 }, 5.0f};

	//Vector3 pixelPosition{ Vector3{0, 0, 0} };
	//Ray currentRay = camera.ScreenToRay(pixelPosition);
	//std::cout << "Intersect : " << sphere.intersect(currentRay);

	const int IMG_SIZE = 400;

	OrthogonalCamera camera{};
	std::vector<Sphere> spheres
	{
		//// Background
		{ { 200, 200, -100 }, 300 },
		
		// Ceilings / Floor
		{ { 1050, 200, -400 }, 750 },
		{ { -650, 200, -400 }, 750 },
		
		// Walls
		{ { 200, -650, -400 }, 750 },
		{ { 200, 1050, -400 }, 750 },

		 //Objects
		//{ { 200, 170, -10 }, 50.0f },
		{ { 260, 250, -55 }, 50.0f },
		{ { 140, 250, -55 }, 50.0f },
	};
	LightSource lightSource { {200, 120, 0}, 50000000 };

	Mat image = Mat::zeros(IMG_SIZE, IMG_SIZE, CV_8UC3);
	image = Scalar(0, 0, 0);
	const float MIN_INTENSITY = 0, MAX_INTENSITY = 255;
	//float allLightOutgoing[IMG_SIZE][IMG_SIZE];
	for (size_t y = 0; y < IMG_SIZE; y++)
	{
		for (size_t x = 0; x < IMG_SIZE; x++)
		{
			// Rayon pour déterminer la quantité de lumière à afficher par pixel
			Vector3 pixelPosition{ Vector3{(float) x, (float) y, 0.0} };
			Ray ray = camera.ScreenToRay(pixelPosition);
			
			// Récupère l'objet le plus proche afin de ne pas effectuer des calculs sur des éléments invisibles
			RayHit rayHit(GetNearestHit(spheres, ray));

			// Stocke la 
			float lightOutgoing(0);
			if (rayHit.object.has_value())
			{
				lightOutgoing = GetOutgoingLightFor(*rayHit.object.value(), ray, rayHit.distance, lightSource);

				// Applique la lumière dans l'image
				Vec3b& color = image.at<Vec3b>(y, x);
				unsigned char intensity(std::clamp(lightOutgoing, MIN_INTENSITY, MAX_INTENSITY));
				color = { intensity, intensity, intensity };
			}
			//allLightOutgoing[y][x] = lightOutgoing;
		}
	}

	//for (size_t y = 0; y < IMG_SIZE; y++)
	//{
	//	for (size_t x = 0; x < IMG_SIZE; x++)
	//	{
	//		Vec3b& color = image.at<Vec3b>(y, x);
	//		unsigned char intensity(std::clamp(allLightOutgoing[y][x], MIN_INTENSITY, MAX_INTENSITY));
	//		color = { intensity, intensity, intensity };
	//	}
	//}

	imshow("Rendu", image);
	//imwrite("C:/Users/wpetit/Desktop/Test.png", image);

	waitKey(0);
	return 0;
}



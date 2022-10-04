#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>

#include "Vector3.h"
#include "Camera.h"
#include "Sphere.h"
#include "LightSource.h"
#include "Shape.h"
#include "Box.h"
#include "SI_RayTracing.h"

std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray)
{
    std::optional<RayHit> nearestHit(std::nullopt);

    // Par d�faut, le hit touche l'infinie
    float minDistance(std::numeric_limits<float>::max());

    for (const auto& current : shapes)
    {
        std::optional t(current->Intersect(ray));

        const bool bIntersection(t.has_value());
        if (!bIntersection) { continue; }

        if (t.value() < minDistance)
        {
            minDistance = t.value();

            // Point d'intersection et la normale touché par le rayon
            const Vector3 impactPoint(ray.origin + minDistance * ray.direction);
            const Vector3 normal(current->GetNormal(impactPoint));
            nearestHit = { minDistance, current, impactPoint, normal };
        }
    }

    return nearestHit;
}

Vector3 GetLightDirectionFromImpact(const LightSource& lightSource, const RayHit& rayHit)
{
    auto lightDir{ Vector3(lightSource.position - rayHit.impactPoint) };
    return lightDir;
}

float IsVisibleByLight(const RayHit& rayHitWithSurface, const LightSource& lightSource, const std::vector<Shape*>& shapes)
{
    const Vector3 lightDir(GetLightDirectionFromImpact(lightSource, rayHitWithSurface));
    const Vector3 lightDirNormalize(lightDir.unitVector());

    // Rayon entre le point d'intersection et la lumière
    // Décale le point d'impact avec la direction de la lumière pour éviter de redétecter la forme touché par l'impact point
    const Ray lightRay{ rayHitWithSurface.impactPoint + lightDirNormalize, lightDirNormalize };
    const std::optional lightRayHit(GetNearestHit(shapes, lightRay));

    if (lightRayHit.has_value() && lightRayHit.value().distance < lightDir.norm())
    {
        return false;
    }

    return true;
}

float GetOutgoingLightFor(const RayHit& rayHit, const LightSource& lightSource)
{
    const float ALBEDO = 0.7f;

    // Direction entre la lampe et l'intersection
    const Vector3 lightDir(GetLightDirectionFromImpact(lightSource, rayHit));
    const Vector3 w0(lightDir.unitVector());

    // Fonction de transfert de la lumière selon l'angle du rayon
    const float f{ rayHit.normal.dot(w0) / static_cast<float>(M_PI) };
    const float cosTeta(std::fabs(rayHit.normal.dot(w0)));

    return (lightSource.intensity * f * ALBEDO * cosTeta) / (lightDir.normSquared());
}

int main()
{
    //TODO Convertir en test automatique pour juger l'efficacit� de l'intersection
    //OrthogonalCamera camera{};
    //Sphere sphere{ { 0, 0, -10 }, 5.0f};

    //Vector3 pixelPosition{ Vector3{0, 0, 0} };
    //Ray currentRay = camera.ScreenToRay(pixelPosition);
    //std::cout << "Intersect : " << sphere.intersect(currentRay);

    const PerspectiveCamera camera({ 200.0f, 0.0f, 750.0f });

    std::vector<AnyShape<Sphere>> spheres{
        // Background
        { { { 200, 200, -475 }, 300 } },

        // Ceilings / Floor
        { { { 1050, 200, -400 }, 750 } },
        { { { -650, 200, -400 }, 750 } },

        // Walls
        { { { 200, -650, -400 }, 750 } },
        { { { 200, 1050, -400 }, 750 } },

        //Objects
        //{ { { 200, 170, -10 }, 50.0f } },
        { { { 260, 250, -55}, 50.0f } },
        { { { 140, 250, -55}, 50.0f } },
    };

    std::vector<AnyShape<Box>> boundingBoxes{
        { { { 180.0f, 180.f, -50.0f }, { 190.0f, 190.0f, -250.0f } } }
    };

    std::vector<Shape*> shapes;
    for (auto& current : spheres)
        shapes.push_back(&current);

    //for (auto& current : boundingBoxes)
    //    shapes.push_back(&current);

    const LightSource lightSource{ {200, 120, 0}, 50000000 };

    const int IMG_SIZE = 400;
    cv::Mat image = cv::Mat::zeros(IMG_SIZE, IMG_SIZE, CV_8UC3);
    image = cv::Scalar(0, 0, 0);

    const float MIN_INTENSITY = 0, MAX_INTENSITY = 255;
    for (size_t y = 0; y < IMG_SIZE; y++)
    {
        for (size_t x = 0; x < IMG_SIZE; x++)
        {
            // Rayon pour d�terminer la quantit� de lumi�re � afficher par pixel
            Vector3 pixelPosition{ Vector3{static_cast<float>(x), static_cast<float>(y), 0.0} };
            Ray ray = camera.ScreenToRay(pixelPosition);

            // R�cup�re l'objet le plus proche afin de ne pas effectuer des calculs sur des �l�ments invisibles
            std::optional rayHit(GetNearestHit(shapes, ray));
            if (rayHit.has_value() && IsVisibleByLight(rayHit.value(), lightSource, shapes))
            {
                float lightOutgoing(GetOutgoingLightFor(rayHit.value(), lightSource));

                // Applique la lumi�re dans l'image
                cv::Vec3b& color = image.at<cv::Vec3b>(y, x);
                unsigned char intensity(std::clamp(lightOutgoing, MIN_INTENSITY, MAX_INTENSITY));
                color = { intensity, intensity, intensity };
            }
            else
            {
                cv::Vec3b& color = image.at<cv::Vec3b>(y, x);
                color = { 50, 50, 200 };
            }
        }
    }

    imshow("Rendu", image);
    //imwrite("C:/Users/wpetit/Desktop/Test.png", image);

    cv::waitKey(0);
    return 0;
}

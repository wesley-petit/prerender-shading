#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>

#include "World.h"
#include "LightSource.h"
#include "Shape.h"
#include "Sphere.h"
#include "Camera.h"
#include "Vector3.h"
#include "Ray.h"
#include "RayHit.h"
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

std::optional<RayHit> CalculateReflection(const Vector3& origin, const Vector3& destination, const World& world)
{
    const Vector3 reflectionDir(destination - origin);
    const Vector3 reflectionDirNormalize(reflectionDir.unitVector());

    // Rayon entre le point d'intersection et la lumière
    // Décale le point d'impact avec la direction de la lumière pour éviter de redétecter la forme touché par l'impact point
    const Ray ray{ origin + reflectionDirNormalize, reflectionDirNormalize };

    return GetNearestHit(world.shapes, ray);
}

bool IsVisibleByLight(const RayHit& rayHit, const World& world)
{
    const Vector3 reflectionDir(world.lightSource.position - rayHit.impactPoint);
    const Vector3 reflectionDirNormalize(reflectionDir.unitVector());

    // Rayon entre le point d'intersection et la lumière
    // Décale le point d'impact avec la direction de la lumière pour éviter de redétecter la forme touché par l'impact point
    const Ray ray{ rayHit.impactPoint + reflectionDirNormalize, reflectionDirNormalize };
    const std::optional<RayHit> lightRayHit(GetNearestHit(world.shapes, ray));

    if (lightRayHit.has_value() && lightRayHit.value().distance < reflectionDir.norm())
    {
        return false;
    }

    return true;
}

float GetOutgoingLightFor(const RayHit& rayHit, const LightSource& lightSource)
{
    const float ALBEDO = 0.7f;

    // Direction entre la lampe et l'intersection
    const Vector3 lightDir(lightSource.position - rayHit.impactPoint);
    const Vector3 w0(lightDir.unitVector());

    // Fonction de transfert de la lumière selon l'angle du rayon
    const float f{ rayHit.normal.dot(w0) / static_cast<float>(M_PI) };
    const float cosTeta(std::fabs(rayHit.normal.dot(w0)));

    return (lightSource.intensity * f * ALBEDO * cosTeta) / (lightDir.normSquared());
}

Vector3 CalculateReflectDirection(const Vector3& incidentDirection, const Vector3& hitNormal)
{
    //https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
    return incidentDirection - 2.0f * incidentDirection.dot(hitNormal) * hitNormal;
}

Vector3 CalculateRefractedDirection(const Vector3& incidentDirection, const Vector3& hitNormal)
{
    //https://registry.khronos.org/OpenGL-Refpages/gl4/html/refract.xhtml
    return incidentDirection - 2.0f * incidentDirection.dot(hitNormal) * hitNormal;
}

Vector3 GetRadiance(const Ray& ray, const World& world, int& currentDepth)
{
    const Vector3 NO_OBJECT{ 0, 0, 0 };
    const Vector3 SHADOW{ 0, 0, 0 };
    const Vector3 STACK_OVERFLOW_LIMIT{ 200, 50, 50 };
    const Vector3 REFLECTION_NOT_SUPPORTED{ 50, 50, 200 };

    if (10 < currentDepth)
    {
        return STACK_OVERFLOW_LIMIT;
    }

    // ------------------------------------------ Rayon pour déterminer s'il y a un objet ------------------------------------------
    // R�cup�re l'objet le plus proche afin de ne pas effectuer des calculs sur des �l�ments invisibles
    std::optional rayHit(GetNearestHit(world.shapes, ray));
    
    bool bBlockingHit(rayHit.has_value());
    if (!bBlockingHit)
    {
        return NO_OBJECT;
    }

    // ------------------------------------------ Rayon pour déterminer la visibilité par la lumière ------------------------------------------
    const ReflectionType& hitReflection(rayHit.value().object->GetReflectionType());
    if (hitReflection == ReflectionType::Normal)
    {
        // Détermine la visibilité d'un objet par la lumière
        if (IsVisibleByLight(rayHit.value(), world))
        {
            float lightOutgoing(GetOutgoingLightFor(rayHit.value(), world.lightSource));

            // Retourne la lumi�re dans l'image
            return Vector3{ lightOutgoing, lightOutgoing, lightOutgoing };
        }

        return SHADOW;
    }

    // ------------------------------------------ Rayon pour déterminer l'objet réfléchi par le mémoire ------------------------------------------
    if (hitReflection == ReflectionType::Mirror)
    {
        const Vector3 reflectDir(CalculateReflectDirection(ray.direction, rayHit.value().normal));
        const Ray reflectRay{ rayHit.value().impactPoint + reflectDir * 2, reflectDir };
        currentDepth++;

        return GetRadiance(reflectRay, world, currentDepth);
    }

    std::cout << " Reflection Type not supported.\n";
    return REFLECTION_NOT_SUPPORTED;
}

int main()
{
    //TODO Convertir en test automatique pour juger l'efficacit� de l'intersection
    //OrthogonalCamera camera{};
    //Sphere sphere{ { 0, 0, -10 }, 5.0f};

    //Vector3 pixelPosition{ Vector3{0, 0, 0} };
    //Ray currentRay = camera.ScreenToRay(pixelPosition);
    //std::cout << "Intersect : " << sphere.intersect(currentRay);

    std::vector<AnyShape<Sphere>> spheres{
        // Background
        { { { 200, 200, -475 }, 300, ReflectionType::Normal } },

        // Ceilings / Floor
        { { { 200, -650, -400 }, 750, ReflectionType::Normal } },
        { { { 200, 1050, -400 }, 750, ReflectionType::Normal } },

        // Walls
        { { { 1050, 200, -400 }, 750, ReflectionType::Normal } },
        { { { -650, 200, -400 }, 750, ReflectionType::Normal } },

        //Objects
        //{ { { 200, 170, -10 }, 50.0f } },
        { { { 260, 250, -55}, 50.0f, ReflectionType::Mirror } },
        { { { 140, 250, -55}, 50.0f, ReflectionType::Mirror } },
    };

    std::vector<Shape*> shapes;
    for (auto& current : spheres)
        shapes.push_back(&current);
    
    //std::vector<AnyShape<Box>> boundingBoxes{
    //    { { { 180.0f, 180.f, -50.0f }, { 190.0f, 190.0f, -250.0f } } }
    //};
    //for (auto& current : boundingBoxes)
    //    shapes.push_back(&current);

    const World& world(
        {
            shapes,
            LightSource{ {200, 120, 0}, 50000000 }
        });

    const PerspectiveCamera camera({ 200.0f, 0.0f, 750.0f });

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

            int currentDepth(0);
            Vector3 radiance(GetRadiance(ray, world, currentDepth));

            cv::Vec3b& color = image.at<cv::Vec3b>(y, x);
            unsigned char b(std::clamp(radiance.x, MIN_INTENSITY, MAX_INTENSITY));
            unsigned char g(std::clamp(radiance.y, MIN_INTENSITY, MAX_INTENSITY));
            unsigned char r(std::clamp(radiance.z, MIN_INTENSITY, MAX_INTENSITY));
            color = { b, g, r };
        }
    }

    imshow("Rendu", image);
    //imwrite("C:/Users/wpetit/Desktop/Test.png", image);

    cv::waitKey(0);
    return 0;
}

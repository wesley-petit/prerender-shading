#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "World.h"
#include "LightSource.h"
#include "Shape.h"
#include "Sphere.h"
#include "Camera.h"
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
            const glm::vec3 impactPoint(ray.origin + minDistance * ray.direction);
            const glm::vec3 normal(current->GetNormal(impactPoint));
            nearestHit = { minDistance, current, impactPoint, normal };
        }
    }

    return nearestHit;
}

bool IsVisibleByLight(const RayHit& rayHit, const World& world)
{
    const glm::vec3 reflectionDir(world.lightSource.position - rayHit.impactPoint);
    const glm::vec3 reflectionDirNormalize(glm::normalize(reflectionDir));

    // Rayon entre le point d'intersection et la lumière
    // Décale le point d'impact avec la direction de la lumière pour éviter de redétecter la forme touché par l'impact point
    const Ray ray{ rayHit.impactPoint + reflectionDirNormalize, reflectionDirNormalize };
    const std::optional<RayHit> lightRayHit(GetNearestHit(world.shapes, ray));

    if (lightRayHit.has_value() && lightRayHit.value().distance < glm::length(reflectionDir))
    {
        return false;
    }

    return true;
}

float GetOutgoingLightFor(const RayHit& rayHit, const LightSource& lightSource)
{
    const float ALBEDO = 0.7f;

    // Direction entre la lampe et l'intersection
    const glm::vec3 lightDir(lightSource.position - rayHit.impactPoint);
    const glm::vec3 w0(glm::normalize(lightDir));

    // Fonction de transfert de la lumière selon l'angle du rayon
    const float f{ glm::dot(rayHit.normal, w0) / static_cast<float>(M_PI) };
    const float cosTeta(std::fabs(glm::dot(rayHit.normal, w0)));

    return (lightSource.intensity * f * ALBEDO * cosTeta) / (glm::length2(lightDir));
}

Ray GetReflectRay(const glm::vec3& incident, const RayHit& rayHit)
{
    //https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
    const glm::vec3 r(incident - 2.0f * glm::dot(incident, rayHit.normal) * rayHit.normal);
    return { rayHit.impactPoint + r, r };
}

std::optional<glm::vec3> CalculateRefractionDirection(const glm::vec3& incident, const glm::vec3& normal)
{
    //https://registry.khronos.org/OpenGL-Refpages/gl4/html/refract.xhtml
    // Ratio de l'indice de refraction du verre (1.5)
    const float eta(1 / 1.5f);
    const float dotProduct(glm::dot(incident, normal));

    const float k(1.0 - eta * eta * (1.0 - dotProduct * dotProduct));
    if (k < 0.0)
    {
        return std::nullopt;
    }

    return eta * incident - (eta * dotProduct + std::sqrt(k)) * normal;
}

glm::vec3 GetRadiance(const Ray& ray, const World& world, int currentDepth)
{
    const glm::vec3 DEBUG_NO_OBJECT{ 0, 0, 0 };
    const glm::vec3 DEBUG_SHADOW{ 0, 0, 0 };
    const glm::vec3 DEBUG_STACK_OVERFLOW_LIMIT{ 50, 200, 50 };
    const glm::vec3 DEBUG_REFLECTION_NOT_SUPPORTED{ 50, 50, 200 };
    const int STACK_OVERFLOW_LIMIT(10);

    if (STACK_OVERFLOW_LIMIT < currentDepth)
    {
        return DEBUG_STACK_OVERFLOW_LIMIT;
    }

    // ------------------------------------------ Rayon pour déterminer s'il y a un objet ------------------------------------------
    // R�cup�re l'objet le plus proche afin de ne pas effectuer des calculs sur des �l�ments invisibles
    std::optional rayHit(GetNearestHit(world.shapes, ray));
    
    bool bBlockingHit(rayHit.has_value());
    if (!bBlockingHit)
    {
        return DEBUG_NO_OBJECT;
    }

    const RayHit& hit(rayHit.value());

    // ------------------------------------------ Illumination Directe ------------------------------------------
    const ReflectionType& hitReflection(hit.object->GetReflectionType());
    if (hitReflection == ReflectionType::Normal)
    {
        // Détermine la visibilité d'un objet par la lumière
        if (IsVisibleByLight(hit, world))
        {
            float lightOutgoing(GetOutgoingLightFor(hit, world.lightSource));

            if (lightOutgoing <= 0)
            {
                std::cout << "LO négative : " << lightOutgoing << "\n";
            }

            // Retourne la lumi�re dans l'image
            return glm::vec3{ lightOutgoing, lightOutgoing, lightOutgoing };
        }

        return DEBUG_SHADOW;
    }

    // ------------------------------------------ Illumination Indirecte ------------------------------------------
    if (hitReflection == ReflectionType::Mirror)
    {
        const Ray reflectRay(GetReflectRay(ray.direction, hit));
        return GetRadiance(reflectRay, world, currentDepth + 1);
    }

    // ------------------------------------------ Rayon pour déterminer la réfraction dans les surfaces transparentes ------------------------------------------
    if (hitReflection == ReflectionType::Glass)
    {
        const Ray reflectRay(GetReflectRay(ray.direction, hit));
        glm::vec3 reflectedRadiance(GetRadiance(reflectRay, world, currentDepth + 1));

        const auto r(CalculateRefractionDirection(ray.direction, hit.normal));

        if (!r.has_value())
        {
            std::cout << "Valeur nulle pour la réfraction." << "\n";
            return DEBUG_REFLECTION_NOT_SUPPORTED;
        }

        const Ray refractedRay{ hit.impactPoint + r.value(), r.value()};
        glm::vec3 refractedRadiance(GetRadiance(refractedRay, world, currentDepth + 1));

        const glm::vec3 r0Sqrt((reflectedRadiance - refractedRadiance) / (reflectedRadiance + refractedRadiance));
        const glm::vec3 r0(r0Sqrt * r0Sqrt);

        const glm::vec3 lightDir(world.lightSource.position - hit.impactPoint);
        const glm::vec3 w0(glm::normalize(lightDir));
        const float cosTeta(std::fabs(glm::dot(hit.normal, w0)));

        //TODO replace
        //return r0 + (1 - r0) * std::pow((1 - cosTeta), 5);
    }

    //std::cout << "Reflection Type not supported.\n";
    return DEBUG_REFLECTION_NOT_SUPPORTED;
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
        { { { 260, 250, -55}, 50.0f, ReflectionType::Glass } },
        { { { 140, 250, -55}, 50.0f, ReflectionType::Glass } },
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
            glm::vec3 pixelPosition{ glm::vec3{static_cast<float>(x), static_cast<float>(y), 0.0} };
            Ray ray = camera.ScreenToRay(pixelPosition);

            glm::vec3 radiance(GetRadiance(ray, world, 0));

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

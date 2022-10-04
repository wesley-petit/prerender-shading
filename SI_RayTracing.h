#pragma once
#include <optional>

struct Vector3;
struct World;
struct LightSource;
struct RayHit;
struct ReflectionHit;

// S'il existe, retourner l'objet le plus proche touch� par un rayon
std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray);

// Donne la direction de la lumi�re � partir de son point d'impact sur la surface
Vector3 GetDirectionFromImpact(const LightSource& lightSource, const RayHit& rayHit);

// Retourne l'objet touch� par la r�flection de la lumi�re
//ReflectionHit CalculateReflection(const Vector3& origin, const Vector3& destination, const World& world);

// V�rifie si aucun objet bloque la trajectoire de la lumi�re avec la surface
bool IsVisibleByLight(const std::optional<RayHit>& lightRayHit, const float maxDistance);

// Quanit� de lumi�re per�ue par une surface
float GetOutgoingLightFor(const RayHit& ray, const LightSource& lightSource);

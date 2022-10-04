#pragma once
#include <optional>

// S'il existe, retourner l'objet le plus proche touché par un rayon
std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray);

// Donne la direction de la lumière à partir de son point d'impact sur la surface
Vector3 GetLightDirectionFromImpact(const LightSource& lightSource, const RayHit& rayHit);

// Retourne l'objet touché par la réflection de la lumière
ReflectionHit CalculateReflection(const RayHit& rayHitWithSurface, const World& world);

// Vérifie si aucun objet bloque la trajectoire de la lumière avec la surface
bool IsVisibleByLight(const std::optional<RayHit>& lightRayHit, const float maxDistance);

// Quanité de lumière perçue par une surface
float GetOutgoingLightFor(const RayHit& ray, const LightSource& lightSource);

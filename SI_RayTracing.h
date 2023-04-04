#pragma once
#include <optional>

struct Vector3;
struct World;
struct LightSource;
struct RayHit;
struct ReflectionHit;

// S'il existe, retourner l'objet le plus proche touch� par un rayon
std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray);

// V�rifie si aucun objet bloque la trajectoire de la lumi�re avec la surface
bool IsVisibleByLight(const RayHit& rayHit, const World& world);

// Quantit� de lumi�re per�ue par une surface
float GetOutgoingLightFor(const RayHit& ray, const LightSource& lightSource);

// Retourne le rayon r�fl�chis par la surface (externe)
Ray GetReflectRay(const Vector3& incident, const RayHit& rayHit);

// Calcule la direction de r�fraction
std::optional<Vector3> CalculateRefractionDirection(const Vector3& incident, const Vector3& normal);

// R�cup�re l'intensit� de lumi�re � partir d'un rayon
Vector3 GetRadiance(const Ray& ray, const World& world, int& currentDepth);

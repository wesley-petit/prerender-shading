#pragma once
#include <optional>

struct Vector3;
struct World;
struct LightSource;
struct RayHit;
struct ReflectionHit;

// S'il existe, retourner l'objet le plus proche touché par un rayon
std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray);

// Vérifie si aucun objet bloque la trajectoire de la lumière avec la surface
bool IsVisibleByLight(const RayHit& rayHit, const World& world);

// Quantité de lumière perçue par une surface
float GetOutgoingLightFor(const RayHit& ray, const LightSource& lightSource);

// Retourne le rayon réfléchis par la surface (externe)
Ray GetReflectRay(const Vector3& incident, const RayHit& rayHit);

// Calcule la direction de réfraction
std::optional<Vector3> CalculateRefractionDirection(const Vector3& incident, const Vector3& normal);

// Récupère l'intensité de lumière à partir d'un rayon
Vector3 GetRadiance(const Ray& ray, const World& world, int& currentDepth);

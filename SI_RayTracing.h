#pragma once
#include <optional>
#include "Ray.h"
#include "RayHit.h"

// S'il existe, retourner l'objet le plus proche touché par un rayon
std::optional<RayHit> GetNearestHit(const std::vector<Shape*>& shapes, const Ray& ray);

// Donne la direction de la lumière à partir de son point d'impact sur la surface
Vector3 GetLightDirectionFromImpact(const LightSource& lightSource, const RayHit& rayHit);

// Vérifie si aucun objet bloque la trajectoire de la lumière avec la surface
float IsVisibleByLight(const RayHit& rayHitWithSurface, const LightSource& lightSource, const std::vector<Shape*>& shapes);

// Quanité de lumière perçue par une surface
float GetOutgoingLightFor(const RayHit& ray, const LightSource& lightSource);

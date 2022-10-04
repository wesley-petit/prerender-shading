#pragma once

#include <vector>
#include "Shape.h"
#include "LightSource.h"

struct World
{
    const std::vector<Shape*> shapes;
    const LightSource lightSource;
};

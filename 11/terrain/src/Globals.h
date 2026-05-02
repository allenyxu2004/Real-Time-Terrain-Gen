//
// Created by purpl on 12/4/2023.
//
#pragma once
#include "Terrain.hpp"
#include <memory>


struct Global {
    int seed = 12345;
    int octaves = 5;
    int xSegments = 0;
    int zSegments = 0;
    int* heightData;
    int scale = 255;
    Terrain* GlobalTerrain = nullptr;
};

extern Global g;




#include <iostream>
#include <cmath>
#include <cstdlib> // for rand and srand
#include "Globals.h"


// Our perlin noise class
class PerlinNoise
{
private:
    int nOutputWidth;
    int nOutputHeight;
    float *fNoiseSeed2D;
    float *fPerlinNoise2D;

    // initialize values upon creation of a perlin noise generator
public:
    PerlinNoise(int width, int height)
            : nOutputWidth(width), nOutputHeight(height)
    {
        fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
        fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];

        // The seeded random, so the terrain can stay consistent
        srand(g.seed);
        // Initialize the seed array with random values
        for (int i = 0; i < nOutputWidth * nOutputHeight; i++) {
            fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
        }
    }

    // Destructor
    ~PerlinNoise()
    {
        delete[] fNoiseSeed2D;
        delete[] fPerlinNoise2D;
    }

    // Generates the actual perlin noise
    void GeneratePerlinNoise()
    {
        // Generate our 2D perlin noise
        for (int x = 0; x < nOutputWidth; x++)
        {
            for (int y = 0; y < nOutputHeight; y++)
            {
                float fNoise = 0.0f;
                float fScaleAcc = 0.0f;
                float fScale = 1.0f;

                // Number of octaves controls the level of detail
                int nOctaves = g.octaves;

                for (int o = 0; o < nOctaves; o++)
                {
                    int nPitch = nOutputWidth >> o;
                    int nSampleX1 = (x / nPitch) * nPitch;
                    int nSampleY1 = (y / nPitch) * nPitch;

                    int nSampleX2 = (nSampleX1 + nPitch) % nOutputWidth;
                    int nSampleY2 = (nSampleY1 + nPitch) % nOutputHeight;

                    float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
                    float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

                    float fSampleT = (1.0f - fBlendX) * fNoiseSeed2D[nSampleY1 * nOutputWidth + nSampleX1] + fBlendX * fNoiseSeed2D[nSampleY1 * nOutputWidth + nSampleX2];
                    float fSampleB = (1.0f - fBlendX) * fNoiseSeed2D[nSampleY2 * nOutputWidth + nSampleX1] + fBlendX * fNoiseSeed2D[nSampleY2 * nOutputWidth + nSampleX2];

                    fScaleAcc += fScale;
                    fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
                    fScale = fScale / 2.0f; // Scaling factor controls the amplitude
                }

                // Scale our perlin noise values based off the seeded noise
                fPerlinNoise2D[y * nOutputWidth + x] = fNoise / fScaleAcc;
            }
        }
    }

    // Populates the height array from our terrain with the perlin noise generated array
    void PopulatePerlinNoise(int* outputArray)
    {
        // populate the given array
        for (int i = 0; i < nOutputWidth * nOutputHeight; i++)
        {
            // We multiply by 255 because of how we scaled the terrain
            outputArray[i] = static_cast<int>(fPerlinNoise2D[i] * g.scale);
        }
    }
};


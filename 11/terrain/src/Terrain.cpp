#include "Terrain.hpp"
#include "Image.hpp"
#include <cstdlib>  // for rand() and srand()
#include <ctime>    // for time()
#include "PerlinNoise.cpp"



#include <iostream>



// Constructor for our object
// Calls the initialization method
Terrain::Terrain(unsigned int xSegs, unsigned int zSegs, std::string fileName) : 
                m_xSegments(xSegs), m_zSegments(zSegs) {
    std::cout << "(Terrain.cpp) Constructor called \n";

    // Load up some image data
    Image heightMap(fileName);
    heightMap.LoadPPM(true);
    // Set the height data for the image

    float scale = 5.0f; // Note that this scales down the values to make
                        // the image a bit more flat.
    // Create height data
    m_heightData = new int[m_xSegments*m_zSegments];
    // Set the height data equal to the grayscale value of the heightmap
    // Because the R,G,B will all be equal in a grayscale image, then
    // we just grab one of the color components.

    //perlinNoise(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, m_heightData);
    PerlinNoise generator(m_xSegments, m_zSegments);
    generator.GeneratePerlinNoise();
    generator.PopulatePerlinNoise(m_heightData);

    Init();
}

// Destructor
Terrain::~Terrain(){
    // Delete our allocatted higheithmap data
    if(m_heightData!=nullptr){
        delete m_heightData;
    }
}


// Creates a grid of segments
// This article has a pretty handy illustration here:
// http://www.learnopengles.com/wordpress/wp-content/uploads/2012/05/vbo.png
// of what we are trying to do.
void Terrain::Init(){
    // Create the initial grid of vertices.

    // TODO: (Inclass) Build grid of vertices! 
    for(unsigned int z=0; z < m_zSegments; ++z){
        for(unsigned int x =0; x < m_xSegments; ++x){
            float u = 1.0f - ((float)x/(float)m_xSegments);
            float v = 1.0f - ((float)z/(float)m_zSegments);
            // Calculate the correct position and add the texture coordinates
            m_geometry.AddVertex(x,m_heightData[x+z*m_xSegments],z,u,v);
        }
    }



    // Figure out which indices make up each triangle
    // By writing out a few of the indices you can figure out
    // the pattern here. Note there is an offset.
    
    // TODO: (Inclass) Build triangle strip
    for(unsigned int z=0; z < m_zSegments-1; ++z){
        for(unsigned int x =0; x < m_xSegments-1; ++x){
            m_geometry.AddIndex(x+(z*m_zSegments));
            m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments);
            m_geometry.AddIndex(x+(z*m_zSegments+1));

            m_geometry.AddIndex(x+(z*m_zSegments)+1);
            m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments);
            m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments+1);
        }
    }








   // Finally generate a simple 'array of bytes' that contains
   // everything for our buffer to work with.
   m_geometry.Gen();  
   // Create a buffer and set the stride of information
   m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());
}

// Loads an image and uses it to set the heights of the terrain.
void Terrain::LoadHeightMap(Image image){

}

uint8_t* Terrain::GetTerrainDataPtr(){
    uint8_t* pixelData = new uint8_t[m_xSegments * m_zSegments * 3];

    int maxHeight = m_heightData[0];
    for (int i = 1; i < m_xSegments * m_zSegments; ++i) {
        if (m_heightData[i] > maxHeight) {
            maxHeight = m_heightData[i];
        }
    }

    for (unsigned int z = 0; z < m_zSegments; ++z) {
        for (unsigned int x = 0; x < m_xSegments; ++x) {
            int height = m_heightData[x + z * m_xSegments];
            // Calculate the index for the current pixel
            unsigned int index = (x + z * m_xSegments) * 3;

            if (height < 0.65*maxHeight) {
                // fill pixel data with R, G, B value of blue
                pixelData[index] = 0;
                pixelData[index + 1] = 0;
                pixelData[index + 2] = 255;  // Assuming 255 for blue

            } else if (height < 0.7*maxHeight) {
                // fill pixel data with R, G, B value of yellow
                pixelData[index] = 0;  // Assuming 255 for yellow
                pixelData[index + 1] = 255;
                pixelData[index + 2] = 255;

            } else if (height < 0.75*maxHeight) {
                // fill pixel data with R, G, B value of green
                pixelData[index] = 255;
                pixelData[index + 1] = 255;  // Assuming 255 for green
                pixelData[index + 2] = 0;
            }
            else if (height < 0.80*maxHeight) {
                // fill pixel data with R, G, B value of green
                pixelData[index] = 255;
                pixelData[index + 1] = 0;  // Assuming 255 for green
                pixelData[index + 2] = 255;
            }

            else if (height < 0.95*maxHeight) {
                // fill pixel data with R, G, B value of green
                pixelData[index] = 0;
                pixelData[index + 1] = 255;  // Assuming 255 for green
                pixelData[index + 2] = 0;
            }
            else {
                // fill pixel data with R, G, B value of white
                pixelData[index] = 255;
                pixelData[index + 1] = 255;
                pixelData[index + 2] = 255;
            }
        }
    }

    // Reversing the values, may or may not be needed
    uint8_t* copyData = new uint8_t[m_xSegments*m_zSegments*3];
    for(int i =0; i < m_zSegments*m_xSegments*3; ++i){
        copyData[i]=pixelData[i];
    }
    unsigned int pos = (m_xSegments*m_zSegments*3)-1;
    for(int i =0; i < m_xSegments*m_zSegments*3; i+=3){
        pixelData[pos]=copyData[i+2];
        pixelData[pos-1]=copyData[i+1];
        pixelData[pos-2]=copyData[i];
        pos-=3;
    }
    delete[] copyData;

    return pixelData;
}

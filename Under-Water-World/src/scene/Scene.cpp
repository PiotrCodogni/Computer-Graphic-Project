#include "scene/Scene.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <utility>
#include <glm/gtc/constants.hpp>
#include "scene/entity/Stonehenge.h"

// -------------------------------------------------------

// -------------------------------------------------------
// generateRockNormalMap
// Tworzy proceduralnie teksture normalnych dla kamienia:
// losowe nierownomiernosci z warstw sinusoidalnych (FBM).
// Zwraca id tekstury OpenGL.
// -------------------------------------------------------
// Funkcja pomocnicza: prosty szum pseudolosowy dla danej wspolrzednej calkowitoliczbowej
static float rockHash(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// Funkcja pomocnicza: interpolowany szum wartosciowy (Value Noise)
static float rockNoise(float x, float y) {
    int ix = (int)std::floor(x);
    int iy = (int)std::floor(y);
    float fx = x - ix;
    float fy = y - iy;

    float u = fx * fx * (3.0f - 2.0f * fx);
    float v = fy * fy * (3.0f - 2.0f * fy);

    float n00 = rockHash(ix, iy);
    float n10 = rockHash(ix + 1, iy);
    float n01 = rockHash(ix, iy + 1);
    float n11 = rockHash(ix + 1, iy + 1);

    return (n00 * (1.0f - u) + n10 * u) * (1.0f - v) + 
           (n01 * (1.0f - u) + n11 * u) * v;
}

static GLuint generateRockNormalMap(int w, int h)
{
    // Najpierw generujemy heightmape z warstw ostrego szumu
    std::vector<float> hmap(w * h);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float fx = (float)x / (float)w * 25.0f;
            float fy = (float)y / (float)h * 25.0f;
            
            float h_val = 0.0f;
            float amp = 1.0f;
            // 6 oktaw FBM z uzyciem turbulencji (abs) dla ostrych, spękanych krawędzi
            for(int i = 0; i < 6; i++) {
                h_val += amp * std::abs(rockNoise(fx, fy));
                fx *= 2.1f;
                fy *= 2.1f;
                amp *= 0.5f;
            }
            hmap[y * w + x] = -h_val; // odwracamy dla efektu wgłębień
        }
    }

    // Obliczamy normalne z gradientu heightmapy (metoda central differences)
    std::vector<unsigned char> px(w * h * 3);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int xl = (x - 1 + w) % w;
            int xr = (x + 1) % w;
            int yd = (y - 1 + h) % h;
            int yu = (y + 1) % h;

            float dx = (hmap[y * w + xr] - hmap[y * w + xl]);
            float dz = (hmap[yu * w + x] - hmap[yd * w + x]);

            glm::vec3 n = glm::normalize(glm::vec3(-dx, 1.0f, -dz));

            int idx = (y * w + x) * 3;
            px[idx+0] = (unsigned char)((n.x * 0.5f + 0.5f) * 255.0f);
            px[idx+1] = (unsigned char)((n.y * 0.5f + 0.5f) * 255.0f);
            px[idx+2] = (unsigned char)((n.z * 0.5f + 0.5f) * 255.0f);
        }
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return id;
}

Scene::Scene()
{
    sceneTime         = 0.0f;
    seabedShader      = 0;
    algaeShader       = 0;
    godRaysShader     = 0;
    waterSurfaceShader= 0;
    rockNormalMapId   = 0;
    waterNormalMapId  = 0;
    showSkybox        = true;
    showShadows       = true;
}

float getSeabedHeight(float x, float z)
{
    return -6.0f + 0.6f * std::sin(x * 0.08f) * std::cos(z * 0.08f) + 0.2f * std::sin(x * 0.2f + z * 0.2f);
}

bool Scene::init()
{
    if (!fish.init(
        "assets/models/fish1.glb",
        "assets/texture/fish1.png",
        "shaders/fish.vert",
        "shaders/fish.frag"
    ))
    {
        return false;
    }

    school.init(
        "assets/models/fish1.glb",
        "assets/texture/fish1.png",
        "shaders/Fish.vert",
        "shaders/Fish.frag");

    if (!stonehenge.init(
        "assets/models/scene.gltf",
        "shaders/Stonehenge.vert",
        "shaders/Stonehenge.frag"
    ))

    {
        std::cout << "Failed to load Stonehenge" << std::endl;
        return false;
    }

    if (!pearlEnvironmentMap.loadFromFiles({
    "assets/environment/pearl/right.png",
    "assets/environment/pearl/left.png",
    "assets/environment/pearl/top.png",
    "assets/environment/pearl/bottom.png",
    "assets/environment/pearl/front.png",
    "assets/environment/pearl/back.png"
        }))
    {
        std::cout << "Failed to load pearl environment cubemap" << std::endl;
        return false;
    }

    if (!skybox.init({
        "assets/skybox/right.png",
        "assets/skybox/left.png",
        "assets/skybox/top.png",
        "assets/skybox/bottom.png",
        "assets/skybox/front.png",
        "assets/skybox/back.png"
    }))
    {
        std::cout << "Failed to load underwater skybox" << std::endl;
        return false;
    }

    if (!pearl.init(
        "shaders/pearl.vert",
        "shaders/pearl.frag"
    ))
    {
        std::cout << "Failed to init pearl" << std::endl;
        return false;
    }

    glm::vec3 pearlGroundPoint = glm::vec3(-29.33f, -7.32f, -41.83f);
    float pearlRadius = 3.0f;

    glm::vec3 pearlCenter = pearlGroundPoint + glm::vec3(0.0f, pearlRadius, 0.0f);

    pearl.setPosition(pearlCenter);
    pearl.setScale(pearlRadius);

    stonehenge.setPosition(glm::vec3(0.0f, -6.5f, -45.0f));
    stonehenge.setRotation(glm::vec3(-90.0f, 2.5f, 0.0f));
    stonehenge.setScale(0.1f);

    stonehenge.setPivotOffset(glm::vec3(0.0f, 0.0f, 0.0f));




    // Load shaders
    seabedShader = shaderLoader.CreateProgram("shaders/seabed.vert", "shaders/seabed.frag");
    if (seabedShader == 0)
    {
        std::cout << "Failed to load seabed shader" << std::endl;
        return false;
    }

    algaeShader = shaderLoader.CreateProgram("shaders/algae.vert", "shaders/algae.frag");
    if (algaeShader == 0)
    {
        std::cout << "Failed to load algae shader" << std::endl;
        return false;
    }

    godRaysShader = shaderLoader.CreateProgram("shaders/godrays.vert", "shaders/godrays.frag");
    if (godRaysShader == 0)
    {
        std::cout << "Nie udalo sie zaladowac shadera promieni slonecznych" << std::endl;
        return false;
    }

    // Wczytanie tekstury dna - zostaje jako kolor bazowy
    if (!seabedTexture.loadFromFile("assets/texture/seabed.png", true))
    {
        std::cout << "Failed to load seabed texture" << std::endl;
        return false;
    }

    if (!algaeTexture.loadFromFile("assets/texture/algae.png", true))
    {
        std::cout << "Failed to load algae texture" << std::endl;
        return false;
    }

    // Generowanie proceduralnych normalmap dla kamienia
    rockNormalMapId   = generateRockNormalMap(256, 256);
    std::cout << "Generated procedural normal map for rock" << std::endl;

    //normalmapa dla powierchni wody (animowane fale)
     {
        const int W = 512, H = 512;
        std::vector<unsigned char> px(W * H * 3);
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                // Szum wielowarstwowy: plynne, szerokie fale wodne
                float fx = (float)x / (float)W * glm::two_pi<float>() * 6.0f;
                float fy = (float)y / (float)H * glm::two_pi<float>() * 6.0f;

                // Aby tekstura wody byla seamless na calej powierzchni
                float bx = std::sin(fx * 1.0f + fy * 1.0f) * 0.55f
                         + std::sin(fx * 2.0f - fy * 2.0f) * 0.30f
                         + std::sin(fx * 1.0f + fy * 3.0f) * 0.15f;
                float bz = std::cos(fx * 1.0f + fy * 1.0f) * 0.55f
                         + std::cos(fx * 2.0f - fy * 2.0f) * 0.30f
                         + std::cos(fx * 3.0f + fy * 1.0f) * 0.15f;

                
                glm::vec3 n = glm::normalize(glm::vec3(-bx, 1.5f, -bz));

                int idx = (y * W + x) * 3;
                px[idx+0] = (unsigned char)((n.x * 0.5f + 0.5f) * 255.0f);
                px[idx+1] = (unsigned char)((n.y * 0.5f + 0.5f) * 255.0f);
                px[idx+2] = (unsigned char)((n.z * 0.5f + 0.5f) * 255.0f);
            }
        }
        glGenTextures(1, &waterNormalMapId);
        glBindTexture(GL_TEXTURE_2D, waterNormalMapId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "procedural water surface normal map" << std::endl;
    }

    // Shader powierzchni wody
    waterSurfaceShader = shaderLoader.CreateProgram("shaders/water_surface.vert", "shaders/water_surface.frag");
    if (waterSurfaceShader == 0)
    {
        std::cout << "Failed to load water surface shader" << std::endl;
        return false;
    }

     {
        const float WSZ = 6000.0f; 
        const float H   = 450.0f; 
        const int   SEG = 30;
        float step = WSZ / SEG;
        float half = WSZ / 2.0f;

        std::vector<glm::vec3> wpos, wnorm, wtan, wbitan;
        std::vector<glm::vec2> wuv;
        std::vector<unsigned int> wind;

        for (int i = 0; i <= SEG; i++)
        {
            for (int j = 0; j <= SEG; j++)
            {
                float x = -half + i * step;
                float z = -half + j * step;
                wpos.push_back(glm::vec3(x, H, z));
                wnorm.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); 
                wtan.push_back(glm::vec3(1.0f, 0.0f, 0.0f));  
                wbitan.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); 
                
                wuv.push_back(glm::vec2(x / WSZ * 1.0f, z / WSZ * 1.0f));
            }
        }

        for (int i = 0; i < SEG; i++)
        {
            for (int j = 0; j < SEG; j++)
            {
                int r1 = i * (SEG + 1);
                int r2 = (i + 1) * (SEG + 1);
                // Odwrocona kolejnosc wierzcchołkow: widzimy od spodu
                wind.push_back(r1 + j);
                wind.push_back(r1 + j + 1);
                wind.push_back(r2 + j);

                wind.push_back(r1 + j + 1);
                wind.push_back(r2 + j + 1);
                wind.push_back(r2 + j);
            }
        }

        GLuint vao, vboPos, vboNorm, vboUV, vboTan, vboBitan, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        auto upload = [&](GLuint& vbo, int loc, const void* data, size_t bytes, int comp) {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, comp, GL_FLOAT, GL_FALSE, 0, nullptr);
        };

        upload(vboPos,   0, wpos.data(),   wpos.size()   * sizeof(glm::vec3), 3);
        upload(vboNorm,  1, wnorm.data(),  wnorm.size()  * sizeof(glm::vec3), 3);
        upload(vboUV,    2, wuv.data(),    wuv.size()    * sizeof(glm::vec2), 2);
        upload(vboTan,   3, wtan.data(),   wtan.size()   * sizeof(glm::vec3), 3);
        upload(vboBitan, 4, wbitan.data(), wbitan.size() * sizeof(glm::vec3), 3);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, wind.size() * sizeof(unsigned int), wind.data(), GL_STATIC_DRAW);

        waterSurfaceContext.vertexArray       = vao;
        waterSurfaceContext.vertexBuffer      = vboPos;
        waterSurfaceContext.vertexIndexBuffer = ebo;
        waterSurfaceContext.size              = (int)wind.size();

        glBindVertexArray(0);
        
    }

    // 1. Generate Seabed Geometry
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    int numSegments = 100;
    float size = 400.0f;
    float halfSize = size / 2.0f;
    float step = size / numSegments;

    for (int i = 0; i <= numSegments; ++i)
    {
        for (int j = 0; j <= numSegments; ++j)
        {
            float x = -halfSize + i * step;
            float z = -halfSize + j * step;
            float y = getSeabedHeight(x, z);
            positions.push_back(glm::vec3(x, y, z));
            
            // Texture tiling
            uvs.push_back(glm::vec2(x * 0.1f, z * 0.1f));
        }
    }

    // Calculate Normals for the Seabed
    for (int i = 0; i <= numSegments; ++i)
    {
        for (int j = 0; j <= numSegments; ++j)
        {
            float x = -halfSize + i * step;
            float z = -halfSize + j * step;
            
            float delta = 0.1f;
            float y_l = getSeabedHeight(x - delta, z);
            float y_r = getSeabedHeight(x + delta, z);
            float y_d = getSeabedHeight(x, z - delta);
            float y_u = getSeabedHeight(x, z + delta);
            
            glm::vec3 tangentX(2.0f * delta, y_r - y_l, 0.0f);
            glm::vec3 tangentZ(0.0f, y_u - y_d, 2.0f * delta);
            glm::vec3 normal = glm::normalize(glm::cross(tangentZ, tangentX));
            normals.push_back(normal);
        }
    }

    // Generate indices
    for (int i = 0; i < numSegments; ++i)
    {
        for (int j = 0; j < numSegments; ++j)
        {
            int row1 = i * (numSegments + 1);
            int row2 = (i + 1) * (numSegments + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row1 + j + 1);

            indices.push_back(row1 + j + 1);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);
        }
    }

    seabedContext.initFromVectors(positions, normals, uvs, indices);

    // 2. Generate Sphere Geometry (for rocks)
    std::vector<glm::vec3> spherePos;
    std::vector<glm::vec3> sphereNorm;
    std::vector<glm::vec2> sphereUV;
    std::vector<unsigned int> sphereInd;

    int rings = 16;
    int sectors = 16;
    float radius = 1.0f;

    for (int r = 0; r <= rings; ++r)
    {
        float phi = glm::pi<float>() * (float)r / (float)rings;
        for (int s = 0; s <= sectors; ++s)
        {
            float theta = 2.0f * glm::pi<float>() * (float)s / (float)sectors;
            float x = std::cos(theta) * std::sin(phi);
            float y = std::cos(phi);
            float z = std::sin(theta) * std::sin(phi);

            spherePos.push_back(glm::vec3(x, y, z) * radius);
            sphereNorm.push_back(glm::vec3(x, y, z));
            sphereUV.push_back(glm::vec2((float)s / sectors, (float)r / rings));
        }
    }

    for (int r = 0; r < rings; ++r)
    {
        for (int s = 0; s < sectors; ++s)
        {
            int next_r = r + 1;
            int next_s = s + 1;

            sphereInd.push_back(r * (sectors + 1) + s);
            sphereInd.push_back(next_r * (sectors + 1) + s);
            sphereInd.push_back(r * (sectors + 1) + next_s);

            sphereInd.push_back(r * (sectors + 1) + next_s);
            sphereInd.push_back(next_r * (sectors + 1) + s);
            sphereInd.push_back(next_r * (sectors + 1) + next_s);
        }
    }

    rockContext.initFromVectors(spherePos, sphereNorm, sphereUV, sphereInd);

    // 3. Generate Cross-Quad Geometry (for algae)
    std::vector<glm::vec3> algaePos = {
        // Quad 1
        glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(0.5f, 2.0f, 0.0f),  glm::vec3(-0.5f, 2.0f, 0.0f),
        // Quad 2
        glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.5f),
        glm::vec3(0.0f, 2.0f, 0.5f),  glm::vec3(0.0f, 2.0f, -0.5f)
    };
    std::vector<glm::vec3> algaeNorm = {
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)
    };
    std::vector<glm::vec2> algaeUV = {
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)
    };
    std::vector<unsigned int> algaeInd = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7
    };

    algaeContext.initFromVectors(algaePos, algaeNorm, algaeUV, algaeInd);

    // 4. Prostokat na caly ekran do rysowania promieni slonecznych
    //    Wierzcholki w przestrzeni ekranu (-1..1)
    std::vector<glm::vec3> quadPos = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3( 1.0f, -1.0f, 0.0f),
        glm::vec3( 1.0f,  1.0f, 0.0f),
        glm::vec3(-1.0f,  1.0f, 0.0f)
    };
    std::vector<glm::vec3> quadNorm = {
        glm::vec3(0,0,1), glm::vec3(0,0,1),
        glm::vec3(0,0,1), glm::vec3(0,0,1)
    };
    std::vector<glm::vec2> quadUV = {
        glm::vec2(0,0), glm::vec2(1,0),
        glm::vec2(1,1), glm::vec2(0,1)
    };
    std::vector<unsigned int> quadInd = { 0, 1, 2,  0, 2, 3 };
    godRaysQuad.initFromVectors(quadPos, quadNorm, quadUV, quadInd);

    // 5. Generate Random Rock Instances
    std::srand(12345);
    for (int i = 0; i < 35; ++i)
    {
        float x = -150.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 300.0f));
        float z = -150.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 300.0f));
        float y = getSeabedHeight(x, z);

        if (std::sqrt(x*x + z*z) < 10.0f) continue;

        RockInstance rock;
        rock.position = glm::vec3(x, y - 0.2f, z);
        
        float scaleX = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 3.0f));
        float scaleY = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        float scaleZ = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 3.0f));
        rock.scale = glm::vec3(scaleX, scaleY, scaleZ);
        
        rock.rotationY = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 360.0f));

        rocks.push_back(rock);
    }

    // 5. Generate Random Algae Instances
    for (int i = 0; i < 120; ++i)
    {
        float x = -150.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 300.0f));
        float z = -150.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 300.0f));
        float y = getSeabedHeight(x, z);

        if (std::sqrt(x*x + z*z) < 5.0f) continue;

        AlgaeInstance algae;
        algae.position = glm::vec3(x, y, z);
        algae.scale = 0.8f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.5f));

        algaeList.push_back(algae);
    }
    //Inicjalizacja lawicy rybek  (15 ryb w okolicy pozycji 20, -2, 15)
    if (!fishSchool.init(
        "assets/models/fish1.glb",
        "assets/texture/fish1.png",
        "shaders/fish.vert",
        "shaders/fish.frag",
        15,
        glm::vec3(0.0f, -2.0f, -20.0f)
    ))
    {
        std::cout << "Failed to init fish school" << std::endl;
        return false;
    }
    // Pojedyncze/parami rybki rozrzucone po scenie
    struct StraySpawn {
        int count;
        glm::vec3 pos;
    };
    StraySpawn straySpawns[] = {
        { 1, glm::vec3(-25.0f, -1.0f, -10.0f) },
        { 2, glm::vec3( 15.0f,  0.0f, -35.0f) },
        { 1, glm::vec3(-10.0f, -3.0f,  20.0f) },
        { 2, glm::vec3( 30.0f, -2.0f,   5.0f) },
        { 1, glm::vec3(-35.0f,  1.0f, -25.0f) },
        { 2, glm::vec3(  5.0f, -4.0f,  30.0f) },
        { 1, glm::vec3( 40.0f,  0.0f, -15.0f) },
        { 1, glm::vec3(-20.0f, -2.0f, -40.0f) },
    };

    for (const auto& spawn : straySpawns)
    {
        FishSchool stray;
        if (!stray.init(
            "assets/models/fish1.glb",
            "assets/texture/fish1.png",
            "shaders/fish.vert",
            "shaders/fish.frag",
            spawn.count,
            spawn.pos
        ))
        {
            std::cout << "Failed to init stray fish" << std::endl;
            return false;
        }
        strayFish.push_back(std::move(stray));
    }

    return true;
}

void Scene::update(float deltaTime, const Input& input)
{
    sceneTime += deltaTime;
    glm::vec3 camForward = camera.getForwardDirection();
    fish.update(deltaTime, input, camForward);
    fishSchool.update(deltaTime);
    for (auto& stray : strayFish)
        stray.update(deltaTime);
    camera.update(deltaTime, input);
    school.update(fish.getPosition(), deltaTime);
    camera.followTarget(fish.getPosition());
}

void Scene::shutdown()
{
    fish.shutdown();
    fishSchool.shutdown();
    for (auto& stray : strayFish)
        stray.shutdown();
	stonehenge.shutdown();
    seabedTexture.shutdown();
    algaeTexture.shutdown();
    pearl.shutdown();
    pearlEnvironmentMap.shutdown();
    skybox.shutdown();
    

    if (seabedShader != 0)
    {
        shaderLoader.DeleteProgram(seabedShader);
        seabedShader = 0;
    }
    if (algaeShader != 0)
    {
        shaderLoader.DeleteProgram(algaeShader);
        algaeShader = 0;
    }
    if (godRaysShader != 0)
    {
        shaderLoader.DeleteProgram(godRaysShader);
        godRaysShader = 0;
    }
    // Usuniecie proceduralnych normalmap
    if (rockNormalMapId  != 0) { glDeleteTextures(1, &rockNormalMapId);  rockNormalMapId  = 0; }
    if (waterNormalMapId != 0) { glDeleteTextures(1, &waterNormalMapId); waterNormalMapId = 0; }
    if (waterSurfaceShader != 0) { shaderLoader.DeleteProgram(waterSurfaceShader); waterSurfaceShader = 0; }
}

Fish& Scene::getFish()
{
    return fish;
}

Camera& Scene::getCamera()
{
    return camera;
}

Stonehenge& Scene::getStonehenge()
{
    return stonehenge;
}

GLuint Scene::getPearlEnvironmentMapId() const
{
    return pearlEnvironmentMap.getId();
}

Pearl& Scene::getPearl()
{
    return pearl;
}

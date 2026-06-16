#pragma once

#include "graphics/Cubemap.h"
#include "scene/entity/Pearl.h"
#include "core/Input.h"
#include "core/Camera.h"
#include "scene/entity/Fish1.h"
#include "scene/entity/FishSchool.h"
#include "scene/entity/Stonehenge.h"
#include "graphics/Render_Utils.h"
#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include <vector>
#include <glm/glm.hpp>

struct RockInstance
{
    glm::vec3 position;
    glm::vec3 scale;
    float rotationY;
};

struct AlgaeInstance
{
    glm::vec3 position;
    float scale;
};

class Scene
{
public:
    Scene();


    bool init();
    void update(float deltaTime, const Input& input);
    void shutdown();

    Fish& getFish();
    FishSchool& getFishSchool() { return fishSchool; }
    std::vector<FishSchool>& getStrayFish() { return strayFish; }
    Camera& getCamera();
    Stonehenge& getStonehenge();
    Pearl& getPearl();

    const Core::RenderContext& getSeabedContext() const { return seabedContext; }
    const Core::RenderContext& getRockContext() const { return rockContext; }
    const Core::RenderContext& getAlgaeContext() const { return algaeContext; }

    GLuint getSeabedShader()       const { return seabedShader;       }
    GLuint getAlgaeShader()        const { return algaeShader;        }
    GLuint getGodRaysShader()      const { return godRaysShader;      }
    GLuint getRockNormalMapId()    const { return rockNormalMapId;    }
    GLuint getPearlEnvironmentMapId() const;

    const Core::RenderContext& getGodRaysQuad() const { return godRaysQuad; }
    const Core::RenderContext& getWaterSurfaceContext() const { return waterSurfaceContext; }

    GLuint getWaterSurfaceShader()  const { return waterSurfaceShader;  }
    GLuint getWaterNormalMapId()    const { return waterNormalMapId;    }

    const Texture& getSeabedTexture() const { return seabedTexture; }
    const Texture& getAlgaeTexture() const { return algaeTexture; }

    const std::vector<RockInstance>& getRocks() const { return rocks; }
    const std::vector<AlgaeInstance>& getAlgaeList() const { return algaeList; }

    float getSceneTime() const { return sceneTime; }


private:
    Fish fish;
    FishSchool fishSchool;
    std::vector<FishSchool> strayFish;
    Camera camera;
    Stonehenge stonehenge;
    Cubemap pearlEnvironmentMap;
    Pearl pearl;

    Core::RenderContext seabedContext;
    Core::RenderContext rockContext;
    Core::RenderContext algaeContext;
    Core::RenderContext godRaysQuad;
    Core::RenderContext waterSurfaceContext;

    GLuint seabedShader;
    GLuint algaeShader;
    GLuint godRaysShader;
    GLuint waterSurfaceShader;
    // Proceduralna normalmapa dla kamienia generowana w Scene::init()
    GLuint rockNormalMapId;
    GLuint waterNormalMapId;

    Texture seabedTexture;
    Texture algaeTexture;

    Core::Shader_Loader shaderLoader;

    std::vector<RockInstance> rocks;
    std::vector<AlgaeInstance> algaeList;

    float sceneTime;
};
#include "graphics/Renderer.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer()
{
}

bool Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Renderer::beginFrame()
{
    glClearColor(0.0f, 0.25f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Scene& scene)
{
    float aspectRatio = 1.0f;

    glm::mat4 view = scene.getCamera().getViewMatrix();
    glm::mat4 projection = scene.getCamera().getProjectionMatrix(aspectRatio);
    glm::vec3 cameraPos = scene.getCamera().getPosition();
    glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 0.0f);

    // --- 1. RENDER SEABED ---
    GLuint seabedShader = scene.getSeabedShader();
    glUseProgram(seabedShader);

    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(seabedShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(seabedShader, "lightPos"), 1, glm::value_ptr(lightPos));

    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "model"), 1, GL_FALSE, glm::value_ptr(identityModel));

    scene.getSeabedTexture().bind(0);
    glUniform1i(glGetUniformLocation(seabedShader, "colorTexture"), 0);

    Core::DrawContext(const_cast<Core::RenderContext&>(scene.getSeabedContext()));

    // --- 2. RENDER ROCKS ---
    for (const auto& rock : scene.getRocks())
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, rock.position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rock.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, rock.scale);

        glUniformMatrix4fv(glGetUniformLocation(seabedShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        
        Core::DrawContext(const_cast<Core::RenderContext&>(scene.getRockContext()));
    }

    // --- 3. RENDER ALGAE (waving seaweed) ---
    GLuint algaeShader = scene.getAlgaeShader();
    glUseProgram(algaeShader);

    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(algaeShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f(glGetUniformLocation(algaeShader, "time"), scene.getSceneTime());

    scene.getAlgaeTexture().bind(0);
    glUniform1i(glGetUniformLocation(algaeShader, "colorTexture"), 0);

    GLboolean isCullingEnabled = glIsEnabled(GL_CULL_FACE);
    if (isCullingEnabled) glDisable(GL_CULL_FACE);

    for (const auto& algae : scene.getAlgaeList())
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, algae.position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(algae.scale));

        glUniformMatrix4fv(glGetUniformLocation(algaeShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        Core::DrawContext(const_cast<Core::RenderContext&>(scene.getAlgaeContext()));
    }

    if (isCullingEnabled) glEnable(GL_CULL_FACE);

    // --- 4. RENDER PLAYER FISH ---
    scene.getFish().render(view, projection);
}

void Renderer::endFrame()
{
}

void Renderer::shutdown()
{
}
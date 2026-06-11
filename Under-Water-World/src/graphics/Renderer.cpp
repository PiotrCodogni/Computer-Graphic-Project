#include "graphics/Renderer.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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
    glClearColor(0.05f, 0.35f, 0.55f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Scene& scene)
{
    float aspectRatio = 1.0f;

    glm::mat4 view       = scene.getCamera().getViewMatrix();
    glm::mat4 projection = scene.getCamera().getProjectionMatrix(aspectRatio);
    glm::vec3 cameraPos  = scene.getCamera().getPosition();

    // Slonce nad powierzchnia wody
    glm::vec3 lightPos = glm::vec3(0.0f, 25.0f, 0.0f);

    float sceneTime = scene.getSceneTime();

    // ------------------------------------------------------------------
    // RYSOWANIE DNA MORSKIEGO (z sunlightem)
    // ------------------------------------------------------------------
    GLuint seabedShader = scene.getSeabedShader();
    glUseProgram(seabedShader);

    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "view"),       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(seabedShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(seabedShader, "lightPos"),  1, glm::value_ptr(lightPos));
    glUniform1f (glGetUniformLocation(seabedShader, "time"),      sceneTime);   // czas do animacji odblaskow
    
    glm::vec3 fogColor = glm::vec3(0.05f, 0.35f, 0.55f);
    float fogDensity = 0.025f;
    glUniform3fv(glGetUniformLocation(seabedShader, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f (glGetUniformLocation(seabedShader, "fogDensity"), fogDensity);

    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "model"), 1, GL_FALSE, glm::value_ptr(identityModel));

    scene.getSeabedTexture().bind(0);
    glUniform1i(glGetUniformLocation(seabedShader, "colorTexture"), 0);

    Core::DrawContext(const_cast<Core::RenderContext&>(scene.getSeabedContext()));

    // ------------------------------------------------------------------
    //  RYSOWANIE KAMIENI
    // ------------------------------------------------------------------
    for (const auto& rock : scene.getRocks())
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, rock.position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rock.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, rock.scale);

        glUniformMatrix4fv(glGetUniformLocation(seabedShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        Core::DrawContext(const_cast<Core::RenderContext&>(scene.getRockContext()));
    }

    // ------------------------------------------------------------------
    //  RYSOWANIE WODOROSTOW
    // ------------------------------------------------------------------
    GLuint algaeShader = scene.getAlgaeShader();
    glUseProgram(algaeShader);

    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "view"),       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(algaeShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f (glGetUniformLocation(algaeShader, "time"),      sceneTime);

    glUniform3fv(glGetUniformLocation(algaeShader, "fogColor"), 1, glm::value_ptr(glm::vec3(0.05f, 0.35f, 0.55f)));
    glUniform1f (glGetUniformLocation(algaeShader, "fogDensity"), 0.025f);

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

    // ------------------------------------------------------------------
    //  RYSOWANIE RYBKI GRACZA
    // ------------------------------------------------------------------
    scene.getFish().render(view, projection);

    // ------------------------------------------------------------------
    // RYSOWANIE LAWICY RYB
    // ------------------------------------------------------------------
    scene.getFishSchool().render(view, projection, cameraPos, fogColor, fogDensity);

    // ------------------------------------------------------------------
    // RYSOWANIE PROMIENI SLONECZNYCH (addytywne mieszanie)
    //    Rysujemy na koniec zeby promienie nakladaly sie na cala scene.
    // ------------------------------------------------------------------
    GLuint godRaysShader = scene.getGodRaysShader();
    glUseProgram(godRaysShader);

    // Macierze do shadera - viewProj rzutuje slonce na ekran, invViewProj odtwarza kierunek
    glm::mat4 viewProj    = projection * view;
    glm::mat4 invViewProj = glm::inverse(viewProj);

    glUniformMatrix4fv(glGetUniformLocation(godRaysShader, "viewProj"),    1, GL_FALSE, glm::value_ptr(viewProj));
    glUniformMatrix4fv(glGetUniformLocation(godRaysShader, "invViewProj"), 1, GL_FALSE, glm::value_ptr(invViewProj));
    glUniform3fv(glGetUniformLocation(godRaysShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(godRaysShader, "lightPos"),  1, glm::value_ptr(lightPos));
    glUniform1f (glGetUniformLocation(godRaysShader, "time"),      sceneTime);

    // Mieszanie addytywne - promienie rozjasniaja to co jest za nimi
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Nie zapisujemy do bufora glebokosci - promienie nie sa solidna geometria
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    Core::DrawContext(const_cast<Core::RenderContext&>(scene.getGodRaysQuad()));

    // Przywracamy normalne ustawienia renderowania
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Renderer::endFrame()
{
}

void Renderer::shutdown()
{
}
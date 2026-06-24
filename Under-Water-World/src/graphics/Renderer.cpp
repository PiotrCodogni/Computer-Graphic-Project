#include "graphics/Renderer.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Renderer::Renderer()
{
    viewportWidth = 1000;
    viewportHeight = 1000;
    depthMapFBO = 0;
    depthMap = 0;
    shadowDepthShader = 0;
    shadowDepthFishShader = 0;
    shadowDepthAlgaeShader = 0;
}

bool Renderer::init(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;

    glEnable(GL_DEPTH_TEST);

    shadowDepthShader = shaderLoader.CreateProgram("shaders/shadow_depth.vert", "shaders/shadow_depth.frag");
    if (shadowDepthShader == 0)
    {
        std::cout << "Failed to load shadow depth shader" << std::endl;
        return false;
    }

    shadowDepthFishShader = shaderLoader.CreateProgram("shaders/shadow_depth_fish.vert", "shaders/shadow_depth.frag");
    if (shadowDepthFishShader == 0)
    {
        std::cout << "Failed to load fish shadow depth shader" << std::endl;
        return false;
    }

    shadowDepthAlgaeShader = shaderLoader.CreateProgram("shaders/shadow_depth_algae.vert", "shaders/shadow_depth_algae.frag");
    if (shadowDepthAlgaeShader == 0)
    {
        std::cout << "Failed to load algae shadow depth shader" << std::endl;
        return false;
    }

    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Shadow depth framebuffer is incomplete" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void Renderer::beginFrame()
{
    glClearColor(0.10f, 0.45f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Scene& scene)
{
    float aspectRatio = 1.0f;

    if (viewportHeight != 0)
    {
        aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    }

    glm::mat4 view = scene.getCamera().getViewMatrix();
    glm::mat4 projection = scene.getCamera().getProjectionMatrix(aspectRatio);
    glm::vec3 cameraPos = scene.getCamera().getPosition();

    // Slonce nad powierzchnia wody
    glm::vec3 lightPos = glm::vec3(-200.19f, 290.01f, -24.38f);
    glm::vec3 shadowCenter = glm::vec3(0.0f, -6.0f, -45.0f);
    glm::mat4 lightProjection = glm::ortho(-180.0f, 180.0f, -180.0f, 180.0f, 1.0f, 700.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, shadowCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    if (scene.areShadowsEnabled() && depthMapFBO != 0 && shadowDepthShader != 0)
    {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(shadowDepthShader);
        glUniformMatrix4fv(glGetUniformLocation(shadowDepthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        for (const auto& rock : scene.getRocks())
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, rock.position);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rock.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, rock.scale);

            glUniformMatrix4fv(glGetUniformLocation(shadowDepthShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            Core::DrawContext(const_cast<Core::RenderContext&>(scene.getRockContext()));
        }

        scene.getStonehenge().renderDepth(shadowDepthShader, lightSpaceMatrix);
        scene.getPearl().renderDepth(shadowDepthShader, lightSpaceMatrix);
        scene.getCrystal().renderDepth(shadowDepthShader, lightSpaceMatrix);
        scene.getFish().renderDepth(shadowDepthFishShader, lightSpaceMatrix);
        scene.getSchoolOfFish().renderDepth(shadowDepthFishShader, lightSpaceMatrix);
        scene.getVerrucosa().renderDepth(shadowDepthShader, lightSpaceMatrix);
        scene.getFishSchool().renderDepth(shadowDepthFishShader, lightSpaceMatrix);
        for (auto& c : scene.getCorals())
        {
            c.renderDepth(shadowDepthShader, lightSpaceMatrix);
        }
        for (auto& stray : scene.getStrayFish())
        {
            stray.renderDepth(shadowDepthFishShader, lightSpaceMatrix);
        }
        
        glUseProgram(shadowDepthAlgaeShader);
        glUniformMatrix4fv(glGetUniformLocation(shadowDepthAlgaeShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform1f(glGetUniformLocation(shadowDepthAlgaeShader, "time"), scene.getSceneTime());
        scene.getAlgaeTexture().bind(0);
        glUniform1i(glGetUniformLocation(shadowDepthAlgaeShader, "alphaTexture"), 0);

        for (const auto& algae : scene.getAlgaeList())
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, algae.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(algae.scale));

            glUniformMatrix4fv(glGetUniformLocation(shadowDepthAlgaeShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            Core::DrawContext(const_cast<Core::RenderContext&>(scene.getAlgaeContext()));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewportWidth, viewportHeight);
    }

    if (scene.isSkyboxVisible())
    {
        scene.getSkybox().render(view, projection);
    }

    float sceneTime = scene.getSceneTime();

    // ------------------------------------------------------------------
    // RYSOWANIE DNA MORSKIEGO (z sunlightem)
    // ------------------------------------------------------------------
    GLuint seabedShader = scene.getSeabedShader();
    glUseProgram(seabedShader);

    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(seabedShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(seabedShader, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform1i(glGetUniformLocation(seabedShader, "useShadows"), scene.areShadowsEnabled() ? 1 : 0);
    glUniform1f(glGetUniformLocation(seabedShader, "time"), sceneTime);   // czas do animacji odblaskow

    glm::vec3 fogColor = glm::vec3(0.10f, 0.45f, 0.65f);
    float fogDensity = 0.018f;
    glUniform3fv(glGetUniformLocation(seabedShader, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(seabedShader, "fogDensity"), fogDensity);

    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(seabedShader, "model"), 1, GL_FALSE, glm::value_ptr(identityModel));

    scene.getSeabedTexture().bind(0);
    glUniform1i(glGetUniformLocation(seabedShader, "colorTexture"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(seabedShader, "shadowMap"), 5);

    // Ustawienie isRock na false dla dna morskiego
    glUniform1i(glGetUniformLocation(seabedShader, "isRock"), 0);

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

        // Normalmap dla kamienia (slot 1) - inna tekstura normalnych niz dla piasku
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scene.getRockNormalMapId());
        glUniform1i(glGetUniformLocation(seabedShader, "normalMap"), 1);

        // Ustawienie isRock na true dla skal
        glUniform1i(glGetUniformLocation(seabedShader, "isRock"), 1);

        Core::DrawContext(const_cast<Core::RenderContext&>(scene.getRockContext()));
    }

    // ------------------------------------------------------------------
    //  RYSOWANIE WODOROSTOW
    // ------------------------------------------------------------------
    GLuint algaeShader = scene.getAlgaeShader();
    glUseProgram(algaeShader);

    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(algaeShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform3fv(glGetUniformLocation(algaeShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f(glGetUniformLocation(algaeShader, "time"), sceneTime);
    glUniform1i(glGetUniformLocation(algaeShader, "useShadows"), scene.areShadowsEnabled() ? 1 : 0);

    glUniform3fv(glGetUniformLocation(algaeShader, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(algaeShader, "fogDensity"), fogDensity);

    scene.getAlgaeTexture().bind(0);
    glUniform1i(glGetUniformLocation(algaeShader, "colorTexture"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(algaeShader, "shadowMap"), 5);

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

    scene.getPearl().render(
        view,
        projection,
        cameraPos,
        scene.getPearlEnvironmentMapId(),
        sceneTime,
        fogColor,
        fogDensity,
        lightSpaceMatrix,
        depthMap,
        scene.areShadowsEnabled()
    );

    scene.getCrystal().render(view, projection, sceneTime, cameraPos, fogColor, fogDensity, lightPos);

    // ------------------------------------------------------------------
    //  RYSOWANIE RYBKI GRACZA
    // ------------------------------------------------------------------
    scene.getFish().render(view, projection, cameraPos, lightPos, fogColor, fogDensity, lightSpaceMatrix, depthMap, scene.areShadowsEnabled());
    scene.getSchoolOfFish().render(view, projection, cameraPos, lightPos, fogColor, fogDensity, lightSpaceMatrix, depthMap, scene.areShadowsEnabled());


    // ------------------------------------------------------------------
    // 4b. RYSOWANIE STONEHENGE
    // ------------------------------------------------------------------
    scene.getStonehenge().render(view, projection, sceneTime, cameraPos, fogColor, fogDensity, lightPos);

    // ------------------------------------------------------------------
    // RYSOWANIE LAWICY RYB
    // ------------------------------------------------------------------
    scene.getFishSchool().render(view, projection, cameraPos, fogColor, fogDensity, lightPos, lightSpaceMatrix, depthMap, scene.areShadowsEnabled());

    for (auto& c : scene.getCorals())
    {
        c.render(view, projection, sceneTime, cameraPos, fogColor, fogDensity, lightPos);
    }


    scene.getVerrucosa().render(view, projection, sceneTime, cameraPos, fogColor, fogDensity, lightPos);

    
    // ------------------------------------------------------------------
    // RYSOWANIE POJEDYNCZYCH PLYWAJACYCH GDZIE NIEGDZIE RYBEK
    // ------------------------------------------------------------------
    for (auto& stray : scene.getStrayFish()) {
        stray.render(view, projection, cameraPos, fogColor, fogDensity, lightPos, lightSpaceMatrix, depthMap, scene.areShadowsEnabled());
    }
    
    // RYSOWANIE POWIERZCHNI WODY (od spodu, z normal mappingiem)
    // Addytywne blending - woda rozjasnia scene zamiast ja zaslaniac
    
    GLuint waterSurfaceShader = scene.getWaterSurfaceShader();
    glUseProgram(waterSurfaceShader);


    glUniformMatrix4fv(glGetUniformLocation(waterSurfaceShader, "view"),       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(waterSurfaceShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glm::mat4 waterModel = glm::mat4(1.0f);
    waterModel = glm::translate(waterModel, glm::vec3(cameraPos.x, 0.0f, cameraPos.z));
    glUniformMatrix4fv(glGetUniformLocation(waterSurfaceShader, "model"),      1, GL_FALSE, glm::value_ptr(waterModel));
    glUniform3fv(glGetUniformLocation(waterSurfaceShader, "cameraPos"),  1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(waterSurfaceShader, "lightPos"),   1, glm::value_ptr(lightPos));
    glUniform1f (glGetUniformLocation(waterSurfaceShader, "time"),       sceneTime);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene.getWaterNormalMapId());
    glUniform1i(glGetUniformLocation(waterSurfaceShader, "normalMap"), 0);

    // Addytywne blending: woda ROZJASNIA to co za nia - nie blokuje slonca
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // Woda nie blokuje (zapis) głębokości dla promieni godrays
    glDepthMask(GL_FALSE);
    
    Core::DrawContext(const_cast<Core::RenderContext&>(scene.getWaterSurfaceContext()));

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // ------------------------------------------------------------------
    // RYSOWANIE PROMIENI SLONECZNYCH (addytywne mieszanie)
    //    Rysujemy na koncu zeby promienie nakladaly sie na cala scene.
    // ------------------------------------------------------------------
    GLuint godRaysShader = scene.getGodRaysShader();
    glUseProgram(godRaysShader);

    // Macierze do shadera - viewProj rzutuje slonce na ekran, invViewProj odtwarza kierunek
    glm::mat4 viewProj = projection * view;
    glm::mat4 invViewProj = glm::inverse(viewProj);

    glUniformMatrix4fv(glGetUniformLocation(godRaysShader, "viewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
    glUniformMatrix4fv(glGetUniformLocation(godRaysShader, "invViewProj"), 1, GL_FALSE, glm::value_ptr(invViewProj));
    glUniform3fv(glGetUniformLocation(godRaysShader, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(godRaysShader, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform1f(glGetUniformLocation(godRaysShader, "time"), sceneTime);

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

void Renderer::setViewportSize(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;
}

void Renderer::endFrame()
{
}

void Renderer::shutdown()
{
    if (shadowDepthShader != 0)
    {
        shaderLoader.DeleteProgram(shadowDepthShader);
        shadowDepthShader = 0;
    }

    if (shadowDepthFishShader != 0)
    {
        shaderLoader.DeleteProgram(shadowDepthFishShader);
        shadowDepthFishShader = 0;
    }

    if (shadowDepthAlgaeShader != 0)
    {
        shaderLoader.DeleteProgram(shadowDepthAlgaeShader);
        shadowDepthAlgaeShader = 0;
    }

    if (depthMap != 0)
    {
        glDeleteTextures(1, &depthMap);
        depthMap = 0;
    }

    if (depthMapFBO != 0)
    {
        glDeleteFramebuffers(1, &depthMapFBO);
        depthMapFBO = 0;
    }
}

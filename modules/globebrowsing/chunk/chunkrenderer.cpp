/*****************************************************************************************
*                                                                                       *
* OpenSpace                                                                             *
*                                                                                       *
* Copyright (c) 2014-2016                                                               *
*                                                                                       *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
* software and associated documentation files (the "Software"), to deal in the Software *
* without restriction, including without limitation the rights to use, copy, modify,    *
* merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
* permit persons to whom the Software is furnished to do so, subject to the following   *
* conditions:                                                                           *
*                                                                                       *
* The above copyright notice and this permission notice shall be included in all copies *
* or substantial portions of the Software.                                              *
*                                                                                       *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
****************************************************************************************/


#include <modules/globebrowsing/chunk/chunkrenderer.h>
#include <modules/globebrowsing/globes/chunkedlodglobe.h>
#include <modules/globebrowsing/globes/renderableglobe.h>
#include <modules/globebrowsing/tile/layermanager.h>
#include <modules/globebrowsing/tile/chunktile.h>

// open space includes
#include <openspace/engine/wrapper/windowwrapper.h>
#include <openspace/engine/openspaceengine.h> 
#include <openspace/rendering/renderengine.h>

// ghoul includes
#include <ghoul/misc/assert.h>
#include <ghoul/opengl/texture.h>
#include <ghoul/opengl/textureunit.h>

// STL includes
#include <sstream> 

#define _USE_MATH_DEFINES
#include <math.h>

namespace {
    const std::string _loggerCat = "ChunkRenderer";

    const std::string keyFrame = "Frame";
    const std::string keyGeometry = "Geometry";
    const std::string keyShading = "PerformShading";

    const std::string keyBody = "Body";
}

namespace openspace {
namespace globebrowsing {

    ChunkRenderer::ChunkRenderer(
        std::shared_ptr<Grid> grid,
        std::shared_ptr<LayerManager> layerManager)
        : _layerManager(layerManager)
        , _grid(grid)
    {
        _globalRenderingShaderProvider = std::make_shared<LayeredTextureShaderProvider>(
                "GlobalChunkedLodPatch",
                "${MODULE_GLOBEBROWSING}/shaders/globalchunkedlodpatch_vs.glsl",
                "${MODULE_GLOBEBROWSING}/shaders/globalchunkedlodpatch_fs.glsl");

        _localRenderingShaderProvider = std::make_shared<LayeredTextureShaderProvider>(
                "LocalChunkedLodPatch",
                "${MODULE_GLOBEBROWSING}/shaders/localchunkedlodpatch_vs.glsl",
                "${MODULE_GLOBEBROWSING}/shaders/localchunkedlodpatch_fs.glsl");

        _globalGpuLayerManager = std::make_shared<GPULayerManager>();
        _localGpuLayerManager = std::make_shared<GPULayerManager>();

    }

    void ChunkRenderer::renderChunk(const Chunk& chunk, const RenderData& data) {
        // A little arbitrary but it works
        if (chunk.tileIndex().level < 10) {
            renderChunkGlobally(chunk, data);
        }
        else {
            renderChunkLocally(chunk, data);
        }
    }

    void ChunkRenderer::update() {
        // unused atm. Could be used for caching or precalculating
    }

    ProgramObject* ChunkRenderer::getActivatedProgramWithTileData(
        LayeredTextureShaderProvider* layeredTextureShaderProvider,
        GPULayerManager * gpuLayerManager,
        const Chunk& chunk)
    {
        const TileIndex& tileIndex = chunk.tileIndex();

        LayeredTexturePreprocessingData layeredTexturePreprocessingData;
        
        for (size_t i = 0; i < LayerManager::NUM_LAYER_GROUPS; i++) {
            LayeredTextureInfo layeredTextureInfo;
            auto layerGroup = _layerManager->layerGroup(i);
            layeredTextureInfo.lastLayerIdx = layerGroup.activeLayers().size() - 1;
            layeredTextureInfo.layerBlendingEnabled = layerGroup.layerBlendingEnabled();

            layeredTexturePreprocessingData.layeredTextureInfo[i] = layeredTextureInfo;
        }
        
        const auto& generalProps = chunk.owner().generalProperties();
        const auto& debugProps = chunk.owner().debugProperties();
        auto& pairs = layeredTexturePreprocessingData.keyValuePairs;
        
        pairs.push_back(std::make_pair("useAtmosphere", std::to_string(generalProps.atmosphereEnabled)));
        pairs.push_back(std::make_pair("performShading", std::to_string(generalProps.performShading)));
        pairs.push_back(std::make_pair("showChunkEdges", std::to_string(debugProps.showChunkEdges)));
        pairs.push_back(std::make_pair("showHeightResolution", std::to_string(debugProps.showHeightResolution)));
        pairs.push_back(std::make_pair("showHeightIntensities", std::to_string(debugProps.showHeightIntensities)));
        pairs.push_back(std::make_pair("defaultHeight", std::to_string(Chunk::DEFAULT_HEIGHT)));

        // Now the shader program can be accessed
        ProgramObject* programObject =
            layeredTextureShaderProvider->getUpdatedShaderProgram(
                layeredTexturePreprocessingData);
        
        if (layeredTextureShaderProvider->updatedOnLastCall()) {
            gpuLayerManager->updateUniformLocations(programObject, *_layerManager);
        }
        

        // Activate the shader program
        programObject->activate();
        
        gpuLayerManager->setValue(programObject, *_layerManager, tileIndex);

        // The length of the skirts is proportional to its size
        programObject->setUniform("skirtLength", min(static_cast<float>(chunk.surfacePatch().halfSize().lat * 1000000), 8700.0f));
        programObject->setUniform("xSegments", _grid->xSegments());

        if (chunk.owner().debugProperties().showHeightResolution) {
            programObject->setUniform("vertexResolution", glm::vec2(_grid->xSegments(), _grid->ySegments()));
        }       
        
        return programObject;
    }

    void ChunkRenderer::renderChunkGlobally(const Chunk& chunk, const RenderData& data){

        ProgramObject* programObject = getActivatedProgramWithTileData(
            _globalRenderingShaderProvider.get(),
            _globalGpuLayerManager.get(),
            chunk);
        if (programObject == nullptr) {
            return;
        }

        const Ellipsoid& ellipsoid = chunk.owner().ellipsoid();

        if (_layerManager->hasAnyBlendingLayersEnabled()) {
            // Calculations are done in the reference frame of the globe. Hence, the camera
            // position needs to be transformed with the inverse model matrix
            glm::dmat4 inverseModelTransform = chunk.owner().inverseModelTransform();
            glm::dvec3 cameraPosition =
                glm::dvec3(inverseModelTransform * glm::dvec4(data.camera.positionVec3(), 1));
            float distanceScaleFactor = chunk.owner().generalProperties().lodScaleFactor * ellipsoid.minimumRadius();
            programObject->setUniform("cameraPosition", vec3(cameraPosition));
            programObject->setUniform("distanceScaleFactor", distanceScaleFactor);
            programObject->setUniform("chunkLevel", chunk.tileIndex().level);
        }
        
        // Calculate other uniform variables needed for rendering
        Geodetic2 swCorner = chunk.surfacePatch().getCorner(Quad::SOUTH_WEST);
        auto patchSize = chunk.surfacePatch().size();
        
        dmat4 modelTransform = chunk.owner().modelTransform();
        dmat4 viewTransform = data.camera.combinedViewMatrix();
        mat4 modelViewTransform = mat4(viewTransform * modelTransform);
        mat4 modelViewProjectionTransform = data.camera.projectionMatrix() * modelViewTransform;

        // Upload the uniform variables
        programObject->setUniform("modelViewProjectionTransform", modelViewProjectionTransform);
        programObject->setUniform("minLatLon", vec2(swCorner.toLonLatVec2()));
        programObject->setUniform("lonLatScalingFactor", vec2(patchSize.toLonLatVec2()));
        programObject->setUniform("radiiSquared", vec3(ellipsoid.radiiSquared()));

        if (_layerManager->layerGroup(LayerManager::NightTextures).activeLayers().size() > 0 ||
            _layerManager->layerGroup(LayerManager::WaterMasks).activeLayers().size() > 0 ||
            chunk.owner().generalProperties().atmosphereEnabled ||
            chunk.owner().generalProperties().performShading) {
            glm::vec3 directionToSunWorldSpace =
                glm::normalize(-data.modelTransform.translation);
            glm::vec3 directionToSunCameraSpace =
                (viewTransform * glm::dvec4(directionToSunWorldSpace, 0));
            data.modelTransform.translation;
            programObject->setUniform("modelViewTransform", modelViewTransform);
            programObject->setUniform("lightDirectionCameraSpace", -directionToSunCameraSpace);
        }

        // OpenGL rendering settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        

        // render
        _grid->geometry().drawUsingActiveProgram();
        
        _globalGpuLayerManager->deactivate();
    
        // disable shader
        programObject->deactivate();
        
    }



    void ChunkRenderer::renderChunkLocally(const Chunk& chunk, const RenderData& data) {
        
        ProgramObject* programObject = getActivatedProgramWithTileData(
            _localRenderingShaderProvider.get(),
            _localGpuLayerManager.get(),
            chunk);
        if (programObject == nullptr) {
            return;
        }

        using namespace glm;

        const Ellipsoid& ellipsoid = chunk.owner().ellipsoid();


        if (_layerManager->hasAnyBlendingLayersEnabled()) {
            float distanceScaleFactor = chunk.owner().generalProperties().lodScaleFactor * chunk.owner().ellipsoid().minimumRadius();
            programObject->setUniform("distanceScaleFactor", distanceScaleFactor);
            programObject->setUniform("chunkLevel", chunk.tileIndex().level);
        }

        // Calculate other uniform variables needed for rendering
        dmat4 modelTransform = chunk.owner().modelTransform();
        dmat4 viewTransform = data.camera.combinedViewMatrix();
        dmat4 modelViewTransform = viewTransform * modelTransform;

        std::vector<std::string> cornerNames = { "p01", "p11", "p00", "p10" };
        std::vector<Vec3> cornersCameraSpace(4);
        for (int i = 0; i < 4; ++i) {
            Quad q = (Quad)i;
            Geodetic2 corner = chunk.surfacePatch().getCorner(q);
            Vec3 cornerModelSpace = ellipsoid.cartesianSurfacePosition(corner);
            Vec3 cornerCameraSpace = Vec3(dmat4(modelViewTransform) * glm::dvec4(cornerModelSpace, 1));
            cornersCameraSpace[i] = cornerCameraSpace;
            programObject->setUniform(cornerNames[i], vec3(cornerCameraSpace));
        }

        vec3 patchNormalCameraSpace = normalize(
            cross(cornersCameraSpace[Quad::SOUTH_EAST] - cornersCameraSpace[Quad::SOUTH_WEST],
                cornersCameraSpace[Quad::NORTH_EAST] - cornersCameraSpace[Quad::SOUTH_WEST]));

        programObject->setUniform("patchNormalCameraSpace", patchNormalCameraSpace);
        programObject->setUniform("projectionTransform", data.camera.projectionMatrix());

        if (_layerManager->layerGroup(LayerManager::NightTextures).activeLayers().size() > 0 ||
            _layerManager->layerGroup(LayerManager::WaterMasks).activeLayers().size() > 0 ||
            chunk.owner().generalProperties().atmosphereEnabled ||
            chunk.owner().generalProperties().performShading)
        {
            glm::vec3 directionToSunWorldSpace =
                glm::normalize(-data.modelTransform.translation);
            glm::vec3 directionToSunCameraSpace =
                (viewTransform * glm::dvec4(directionToSunWorldSpace, 0));
            data.modelTransform.translation;
            programObject->setUniform("lightDirectionCameraSpace", -directionToSunCameraSpace);
        }


        // OpenGL rendering settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // render
        _grid->geometry().drawUsingActiveProgram();
        
        _localGpuLayerManager->deactivate();

        // disable shader
        programObject->deactivate();
        
        
    }
} // namespace globebrowsing
} // namespace openspace

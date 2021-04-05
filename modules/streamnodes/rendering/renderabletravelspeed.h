/*****************************************************************************************
*                                                                                       *
* OpenSpace                                                                             *
*                                                                                       *
* Copyright (c) 2014-2021                                                               *
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

#ifndef __OPENSPACE_MODULE_STREAMNODES___RENDERABLETRAVELSPEED___H__
#define __OPENSPACE_MODULE_STREAMNODES___RENDERABLETRAVELSPEED___H__

#include <openspace/rendering/renderable.h>
#include <openspace/properties/vector/vec3property.h>
#include <openspace/properties/scalar/intproperty.h>


#include <ghoul/opengl/uniformcache.h>

namespace openspace {

class SceneGraphNode;

namespace documentation { struct Documentation; }

class RenderableTravelSpeed : public Renderable{
public:
    RenderableTravelSpeed(const ghoul::Dictionary& dictionary);

    static documentation::Documentation Documentation();
    void initializeGL() override;
    void initialize() override;
    void deinitializeGL() override;

    bool isReady() const override;

    void render(const RenderData& data, RendererTasks& rendererTask) override;
    void update(const UpdateData& data) override;

private:
    properties::StringProperty _targetName;
    SceneGraphNode* _targetNode = nullptr;
    properties::DoubleProperty _travelSpeed;
    properties::IntProperty _indicatorLength;
    properties::IntProperty _fadeLength;
    properties::FloatProperty _lineWidth;
    properties::FloatProperty _opacity;
    properties::Vec3Property _lineColor;


    //0 at parent, 1 at end of fade, 2 between light and fade, 
    //3 at head of light, 4 at target
    glm::vec3 _verticesPositions[3]; 

    glm::dvec3 _sourcePosition;
    glm::dvec3 _targetPosition;
    double _lightTravelTime;
    glm::dvec3 _directionVector;
    double _initiationTime;
    double _arrivalTime;
    double _timeSinceStart = -1.f;


    ghoul::opengl::ProgramObject* _shaderProgram = nullptr;
    // The vertex attribute location for position
    // must correlate to layout location in vertex shader
    GLuint _vaoId = 0;
    GLuint _vBufferId = 0;

    double calculateLightTravelTime(glm::dvec3 startPosition, glm::dvec3 targetPosition);
    void calculateVerticesPositions();
    void calculateDirectionVector();
    void updateVertexData();
    void reinitiateTravel();
    UniformCache(lineColor, opacity) _uniformCache;
};
} // namespace
#endif //__OPENSPACE_MODULE_STREAMNODES___RENDERABLETRAVELSPEED___H__

/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2020                                                               *
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

#ifndef __OPENSPACE_MODULE_BASE___RenderableTimeVaryingPlaneImageLocal___H__
#define __OPENSPACE_MODULE_BASE___RenderableTimeVaryingPlaneImageLocal___H__

#include <modules/base/rendering/renderableplane.h>

namespace ghoul::filesystem { class File; }
namespace ghoul::opengl { class Texture; }

namespace openspace {

    struct RenderData;
    struct UpdateData;

    namespace documentation { struct Documentation; }

    class RenderableTimeVaryingPlaneImageLocal : public RenderablePlane {
    public:
        RenderableTimeVaryingPlaneImageLocal(const ghoul::Dictionary& dictionary);

        void initializeGL() override;
        void deinitializeGL() override;

        bool isReady() const override;

        void update(const UpdateData& data) override;

        static documentation::Documentation Documentation();

    protected:
        virtual void bindTexture() override;

    private:
        void loadTexture();
        void extractTriggerTimesFromFileNames();
        bool extractMandatoryInfoFromDictionary();
        void updateActiveTriggerTimeIndex(double currenttime);
        void computeSequenceEndTime();
        // Estimated end of sequence.
        double _sequenceEndTime;
        bool _needsUpdate = false;
        std::vector<std::string> _sourceFiles;
        std::vector<double> _startTimes;
        int _activeTriggerTimeIndex = 0;
        // Number of states in the sequence
        size_t _nStates = 273;
        properties::StringProperty _texturePath;
        //properties::StringProperty _timestapsPath;
        ghoul::opengl::Texture* _texture = nullptr;
       // std::unique_ptr<ghoul::opengl::Texture> _texture;
        std::vector<std::unique_ptr<ghoul::opengl::Texture>> _textureFiles;
        std::unique_ptr<ghoul::filesystem::File> _textureFile;
        bool _isLoadingTexture = false;
        bool _isLoadingLazily = false;
        bool _textureIsDirty = false;
    };

} // namespace openspace

#endif // __OPENSPACE_MODULE_BASE___RenderableTimeVaryingPlaneImageLocal___H__

/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2017                                                               *
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

#ifndef __OPENSPACE_MODULE_BASE___SCREENSPACEDASHBOARD___H__
#define __OPENSPACE_MODULE_BASE___SCREENSPACEDASHBOARD___H__

#include <modules/base/rendering/screenspaceframebuffer.h>

namespace ghoul::fontrendering {
    class Font;
    class FontRenderer;
}

namespace openspace {

namespace documentation { struct Documentation; }

class ScreenSpaceDashboard: public ScreenSpaceFramebuffer {
public:
    ScreenSpaceDashboard(const ghoul::Dictionary& dictionary);
    ~ScreenSpaceDashboard();
    
    bool initializeGL() override;
    bool deinitializeGL() override;

    bool isReady() const override;
    void update() override;

    static documentation::Documentation Documentation();

private:
    std::unique_ptr<ghoul::fontrendering::FontRenderer> _fontRenderer;
    
    std::shared_ptr<ghoul::fontrendering::Font> _fontDate;
    std::shared_ptr<ghoul::fontrendering::Font> _fontInfo;
};

} // namespace openspace

#endif // __OPENSPACE_MODULE_BASE___SCREENSPACEDASHBOARD___H__
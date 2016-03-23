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

#ifndef __RAYCASTERMANAGER_H__
#define __RAYCASTERMANAGER_H__

#include <vector>

namespace openspace {

class VolumeRaycaster;
class RaycasterListener;

class RaycasterManager {
public:
    RaycasterManager();
    ~RaycasterManager();
    void attachRaycaster(VolumeRaycaster* raycaster);
    void detachRaycaster(VolumeRaycaster* raycaster);
    bool isAttached(VolumeRaycaster* raycaster);
    const std::vector<VolumeRaycaster*>& raycasters();

    void addListener(RaycasterListener* listener);
    void removeListener(RaycasterListener* listener);
private:
    std::vector<VolumeRaycaster*> _raycasters;
    std::vector<RaycasterListener*> _listeners;
}; // Volume

} // openspace

#endif  // __RAYCASTERMANAGER_H__
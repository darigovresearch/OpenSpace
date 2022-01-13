/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2022                                                               *
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

#ifndef __OPENSPACE_CORE___STRINGLISTPROPERTY___H__
#define __OPENSPACE_CORE___STRINGLISTPROPERTY___H__

#include <openspace/properties/listproperty.h>
#include <string>

namespace openspace::properties {

class StringListProperty : public ListProperty<std::string> {
public:
    StringListProperty(Property::PropertyInfo info,
        std::vector<std::string> values = std::vector<std::string>());

    std::string className() const override;
    int typeLua() const override;

    using TemplateProperty<std::vector<std::string>>::operator std::vector<std::string>;
    using TemplateProperty<std::vector<std::string>>::operator=;

protected:
    std::vector<std::string> fromLuaConversion(lua_State* state,
        bool& success) const override;
    void toLuaConversion(lua_State* state) const override;
    std::string toStringConversion() const override;
};

} // namespace openspace::properties

#endif // __OPENSPACE_CORE___STRINGLISTPROPERTY___H__

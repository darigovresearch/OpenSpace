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

#ifndef __OPENSPACE_UI_LAUNCHER___SGCTEDIT___H__
#define __OPENSPACE_UI_LAUNCHER___SGCTEDIT___H__

#include <QDialog>

#include <sgct/config.h>
#include <QColor>
#include <array>
#include <filesystem>
#include <string>

class DisplayWindowUnion;
class SettingsWidget;
class QBoxLayout;
class QWidget;

class SgctEdit final : public QDialog {
Q_OBJECT
public:
    /**
     * Constructor for SgctEdit class, the underlying class for the full window
     * configuration editor
     *
     * \param parent The Qt QWidget parent object
     * \param userConfigPath A string containing the file path of the user config
     *                       directory where all window configs are stored
     */
    SgctEdit(QWidget* parent, std::string userConfigPath);

    /**
     * Returns the saved filename
     *
     * \return saved filename in std::string
     */
    std::filesystem::path saveFilename() const;

    /**
     * Returns the generated Cluster object.
     * 
     * \return The generated Cluster object
     */
    sgct::config::Cluster cluster() const;

private:
    void createWidgets(const std::vector<QRect>& monitorSizes);
    sgct::config::Cluster generateConfiguration() const;

    void save();
    void apply();

    DisplayWindowUnion* _displayWidget = nullptr;
    SettingsWidget* _settingsWidget = nullptr;
    sgct::config::Cluster _cluster;
    const std::string _userConfigPath;
    const std::array<QColor, 4> _colorsForWindows = {
        QColor(0x2B, 0x9E, 0xC3),
        QColor(0xFC, 0xAB, 0x10),
        QColor(0x44, 0xAF, 0x69),
        QColor(0xF8, 0x33, 0x3C)
    };

    QBoxLayout* _layoutButtonBox = nullptr;
    QPushButton* _saveButton = nullptr;
    QPushButton* _cancelButton = nullptr;
    QPushButton* _applyButton = nullptr;
    std::string _saveTarget;
};

#endif // __OPENSPACE_UI_LAUNCHER___SGCTEDIT___H__

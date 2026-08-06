/*
    Scan Tailor - Interactive post-processing tool for scanned pages.

    Restoration Lab (V1 alpha) - experimental restoration profile editor.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RESTORATION_LAB_DIALOG_H_
#define RESTORATION_LAB_DIALOG_H_

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

class RestorationLabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RestorationLabDialog(QWidget* parent = nullptr);

private slots:
    void loadSettings();
    void saveSettings();
    void resetDefaults();

private:
    QComboBox* m_deskewMethod = nullptr;
    QDoubleSpinBox* m_confidenceThreshold = nullptr;
    QCheckBox* m_backgroundNormalization = nullptr;
    QSpinBox* m_dustSize = nullptr;
    QSpinBox* m_dustSensitivity = nullptr;
    QCheckBox* m_protectPunctuation = nullptr;
    QComboBox* m_binarization = nullptr;
    QSpinBox* m_localRadius = nullptr;
    QDoubleSpinBox* m_minContrast = nullptr;
    QCheckBox* m_protectFineStrokes = nullptr;
    QCheckBox* m_lightRepair = nullptr;
    QCheckBox* m_exportGray = nullptr;
    QCheckBox* m_exportReading = nullptr;
    QCheckBox* m_exportOcr = nullptr;
    QCheckBox* m_exportMask = nullptr;
    QCheckBox* m_exportJson = nullptr;
};

#endif // ifndef RESTORATION_LAB_DIALOG_H_

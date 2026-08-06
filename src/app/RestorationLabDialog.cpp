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

#include "RestorationLabDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace
{
QGroupBox* makeGroup(const QString& title, QLayout* layout)
{
    QGroupBox* box = new QGroupBox(title);
    box->setLayout(layout);
    return box;
}
} // anonymous namespace

RestorationLabDialog::RestorationLabDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Restoration Lab - V1 alpha"));
    resize(620, 560);

    QTabWidget* tabs = new QTabWidget(this);

    QWidget* geometryPage = new QWidget;
    QVBoxLayout* geometryLayout = new QVBoxLayout(geometryPage);
    QFormLayout* geometryForm = new QFormLayout;
    m_deskewMethod = new QComboBox;
    m_deskewMethod->addItem(tr("Combined (text + page edges)"));
    m_deskewMethod->addItem(tr("Text lines"));
    m_deskewMethod->addItem(tr("Page edges"));
    m_confidenceThreshold = new QDoubleSpinBox;
    m_confidenceThreshold->setRange(0.0, 1.0);
    m_confidenceThreshold->setSingleStep(0.05);
    m_confidenceThreshold->setDecimals(2);
    geometryForm->addRow(tr("Deskew method:"), m_deskewMethod);
    geometryForm->addRow(tr("Minimum confidence:"), m_confidenceThreshold);
    geometryLayout->addWidget(makeGroup(tr("Geometry"), geometryForm));
    QLabel* geometryNote = new QLabel(
        tr("V1 stores the restoration profile. The processing engine will be connected in the next alpha.")
    );
    geometryNote->setWordWrap(true);
    geometryLayout->addWidget(geometryNote);
    geometryLayout->addStretch();

    QWidget* cleanupPage = new QWidget;
    QVBoxLayout* cleanupLayout = new QVBoxLayout(cleanupPage);
    QFormLayout* cleanupForm = new QFormLayout;
    m_backgroundNormalization = new QCheckBox(tr("Normalize uneven paper background"));
    m_dustSize = new QSpinBox;
    m_dustSize->setRange(1, 100);
    m_dustSize->setSuffix(tr(" px"));
    m_dustSensitivity = new QSpinBox;
    m_dustSensitivity->setRange(0, 100);
    m_dustSensitivity->setSuffix(tr(" %"));
    m_protectPunctuation = new QCheckBox(tr("Protect punctuation and accents"));
    cleanupForm->addRow(m_backgroundNormalization);
    cleanupForm->addRow(tr("Maximum dust size:"), m_dustSize);
    cleanupForm->addRow(tr("Dust sensitivity:"), m_dustSensitivity);
    cleanupForm->addRow(m_protectPunctuation);
    cleanupLayout->addWidget(makeGroup(tr("Background and dust"), cleanupForm));
    cleanupLayout->addStretch();

    QWidget* inkPage = new QWidget;
    QVBoxLayout* inkLayout = new QVBoxLayout(inkPage);
    QFormLayout* inkForm = new QFormLayout;
    m_binarization = new QComboBox;
    m_binarization->addItem(tr("Otsu"));
    m_binarization->addItem(tr("Adaptive Gaussian"));
    m_binarization->addItem(tr("Sauvola"));
    m_binarization->addItem(tr("Wolf-Jolion"));
    m_localRadius = new QSpinBox;
    m_localRadius->setRange(3, 151);
    m_localRadius->setSingleStep(2);
    m_localRadius->setSuffix(tr(" px"));
    m_minContrast = new QDoubleSpinBox;
    m_minContrast->setRange(0.0, 1.0);
    m_minContrast->setSingleStep(0.01);
    m_minContrast->setDecimals(2);
    m_protectFineStrokes = new QCheckBox(tr("Protect fine strokes and serifs"));
    m_lightRepair = new QCheckBox(tr("Light repair of broken ink strokes"));
    inkForm->addRow(tr("Binarization:"), m_binarization);
    inkForm->addRow(tr("Local radius:"), m_localRadius);
    inkForm->addRow(tr("Minimum contrast:"), m_minContrast);
    inkForm->addRow(m_protectFineStrokes);
    inkForm->addRow(m_lightRepair);
    inkLayout->addWidget(makeGroup(tr("Ink"), inkForm));
    inkLayout->addStretch();

    QWidget* exportPage = new QWidget;
    QVBoxLayout* exportLayout = new QVBoxLayout(exportPage);
    m_exportGray = new QCheckBox(tr("Grayscale preservation master"));
    m_exportReading = new QCheckBox(tr("Enhanced reading image"));
    m_exportOcr = new QCheckBox(tr("Black-and-white OCR image"));
    m_exportMask = new QCheckBox(tr("Removed-pixel mask (PNG)"));
    m_exportJson = new QCheckBox(tr("Processing trace (JSON)"));
    exportLayout->addWidget(m_exportGray);
    exportLayout->addWidget(m_exportReading);
    exportLayout->addWidget(m_exportOcr);
    exportLayout->addWidget(m_exportMask);
    exportLayout->addWidget(m_exportJson);
    exportLayout->addStretch();

    tabs->addTab(geometryPage, tr("Geometry"));
    tabs->addTab(cleanupPage, tr("Cleanup"));
    tabs->addTab(inkPage, tr("Ink"));
    tabs->addTab(exportPage, tr("Export"));

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults
    );
    connect(
        buttons->button(QDialogButtonBox::Save), &QPushButton::clicked,
        this, &RestorationLabDialog::saveSettings
    );
    connect(
        buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
        this, &RestorationLabDialog::reject
    );
    connect(
        buttons->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
        this, &RestorationLabDialog::resetDefaults
    );

    QVBoxLayout* root = new QVBoxLayout(this);
    QLabel* header = new QLabel(tr("Experimental restoration profile for scanned heritage pages."));
    header->setWordWrap(true);
    root->addWidget(header);
    root->addWidget(tabs);
    root->addWidget(buttons);

    loadSettings();
}

void
RestorationLabDialog::resetDefaults()
{
    m_deskewMethod->setCurrentIndex(0);
    m_confidenceThreshold->setValue(0.70);
    m_backgroundNormalization->setChecked(true);
    m_dustSize->setValue(12);
    m_dustSensitivity->setValue(45);
    m_protectPunctuation->setChecked(true);
    m_binarization->setCurrentIndex(2);
    m_localRadius->setValue(31);
    m_minContrast->setValue(0.15);
    m_protectFineStrokes->setChecked(true);
    m_lightRepair->setChecked(false);
    m_exportGray->setChecked(true);
    m_exportReading->setChecked(true);
    m_exportOcr->setChecked(true);
    m_exportMask->setChecked(true);
    m_exportJson->setChecked(true);
}

void
RestorationLabDialog::loadSettings()
{
    QSettings settings;

    settings.beginGroup("restorationLabV1");
    if (!settings.contains("deskewMethod")) {
        settings.endGroup();
        resetDefaults();
        return;
    }
    m_deskewMethod->setCurrentIndex(settings.value("deskewMethod", 0).toInt());
    m_confidenceThreshold->setValue(settings.value("confidenceThreshold", 0.70).toDouble());
    m_backgroundNormalization->setChecked(settings.value("backgroundNormalization", true).toBool());
    m_dustSize->setValue(settings.value("dustSize", 12).toInt());
    m_dustSensitivity->setValue(settings.value("dustSensitivity", 45).toInt());
    m_protectPunctuation->setChecked(settings.value("protectPunctuation", true).toBool());
    m_binarization->setCurrentIndex(settings.value("binarization", 2).toInt());
    m_localRadius->setValue(settings.value("localRadius", 31).toInt());
    m_minContrast->setValue(settings.value("minContrast", 0.15).toDouble());
    m_protectFineStrokes->setChecked(settings.value("protectFineStrokes", true).toBool());
    m_lightRepair->setChecked(settings.value("lightRepair", false).toBool());
    m_exportGray->setChecked(settings.value("exportGray", true).toBool());
    m_exportReading->setChecked(settings.value("exportReading", true).toBool());
    m_exportOcr->setChecked(settings.value("exportOcr", true).toBool());
    m_exportMask->setChecked(settings.value("exportMask", true).toBool());
    m_exportJson->setChecked(settings.value("exportJson", true).toBool());
    settings.endGroup();
}

void
RestorationLabDialog::saveSettings()
{
    QSettings settings;

    settings.beginGroup("restorationLabV1");
    settings.setValue("deskewMethod", m_deskewMethod->currentIndex());
    settings.setValue("confidenceThreshold", m_confidenceThreshold->value());
    settings.setValue("backgroundNormalization", m_backgroundNormalization->isChecked());
    settings.setValue("dustSize", m_dustSize->value());
    settings.setValue("dustSensitivity", m_dustSensitivity->value());
    settings.setValue("protectPunctuation", m_protectPunctuation->isChecked());
    settings.setValue("binarization", m_binarization->currentIndex());
    settings.setValue("localRadius", m_localRadius->value());
    settings.setValue("minContrast", m_minContrast->value());
    settings.setValue("protectFineStrokes", m_protectFineStrokes->isChecked());
    settings.setValue("lightRepair", m_lightRepair->isChecked());
    settings.setValue("exportGray", m_exportGray->isChecked());
    settings.setValue("exportReading", m_exportReading->isChecked());
    settings.setValue("exportOcr", m_exportOcr->isChecked());
    settings.setValue("exportMask", m_exportMask->isChecked());
    settings.setValue("exportJson", m_exportJson->isChecked());
    settings.endGroup();

    accept();
}

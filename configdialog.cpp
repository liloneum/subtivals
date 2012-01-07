#include <QtCore/QSettings>
#include <QtGui/QDesktopWidget>

#include "configdialog.h"
#include "ui_configdialog.h"

#include "script.h"
#include "styleeditor.h"


ConfigDialog::ConfigDialog(Script* script, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ConfigDialog),
    m_styleEditor(new StyleEditor(script))
{
    ui->setupUi(this);
    setFeatures(ConfigDialog::NoDockWidgetFeatures);
    ui->tabStyles->setLayout(m_styleEditor->layout());
    adjustSize();

    QDesktopWidget *dw = QApplication::desktop();
    for(int i = 0; i < dw->screenCount(); i++)
    {
        ui->screens->addItem(QString("Screen %1").arg(i));
    }
    reloadConfig();
    // Connect value changing to apply (live preview)
    connect(ui->x, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->y, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->w, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->h, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->screens, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
}

ConfigDialog::~ConfigDialog()
{
    delete m_styleEditor;
    delete ui;
}

void ConfigDialog::reloadConfig()
{
    QSettings settings;
    settings.beginGroup("SubtitlesForm");
    m_screen = settings.value("screen", 0).toInt();
    m_rect.setX(settings.value("x", 0).toInt());
    m_rect.setY(settings.value("y", 0).toInt());
    m_rect.setWidth(settings.value("w", 0).toInt());
    m_rect.setHeight(settings.value("h", 0).toInt());
    settings.endGroup();
    resetConfig();
}

void ConfigDialog::resetConfig()
{
    m_styleEditor->reset();
    ui->screens->setCurrentIndex(m_screen);
    ui->x->setValue(m_rect.x());
    ui->y->setValue(m_rect.y());
    ui->w->setValue(m_rect.width());
    ui->h->setValue(m_rect.height());
    saveConfig();
}

void ConfigDialog::saveConfig()
{
    m_styleEditor->apply();
    QSettings settings;
    settings.beginGroup("SubtitlesForm");
    settings.setValue("screen", ui->screens->currentIndex());
    settings.setValue("x", ui->x->text());
    settings.setValue("y", ui->y->text());
    settings.setValue("w", ui->w->text());
    settings.setValue("h", ui->h->text());
    settings.endGroup();
    emit configChanged();
}

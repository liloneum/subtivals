#include <QtCore/QSettings>
#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QDesktopWidget>

#include "subtitlesform.h"
#include "ui_subtitlesform.h"
#include "style.h"

SubtitlesForm::SubtitlesForm(QWidget *parent) :
        QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint ),
    ui(new Ui::SubtitlesForm),
    m_maxEvents(2),
    m_visible(true),
    m_resizable(false)
{
    ui->setupUi(this);
    setCursor(QCursor(Qt::BlankCursor));
}

SubtitlesForm::~SubtitlesForm()
{
    delete ui;
}

void SubtitlesForm::addEvent(Event *p_event)
{
    m_currentEvents.append(p_event);
    if(m_currentEvents.size() > m_maxEvents)
    {
        m_currentEvents.removeFirst();
    }
    repaint();
}

void SubtitlesForm::remEvent(Event *p_event)
{
    m_currentEvents.removeOne(p_event);
    repaint();
}

void SubtitlesForm::clearEvents()
{
    m_currentEvents.clear();
    repaint();
}

void SubtitlesForm::toggleHide(bool state)
{
    m_visible = !state;
    repaint();
}

void SubtitlesForm::changeGeometry(int monitor, const QRect& r)
{
    m_screenGeom = QApplication::desktop()->screenGeometry(monitor);
    setGeometry(r.x() + m_screenGeom.x(), r.y() + m_screenGeom.y(), r.width(), r.height());
}

void SubtitlesForm::changeGeometry(const QRect& r)
{
    setGeometry(r);
    emit geometryChanged(QRect(r.x() - m_screenGeom.x(),
                               r.y() - m_screenGeom.y(),
                               r.width(), r.height()));
}

void SubtitlesForm::paintEvent(QPaintEvent*)
{
    QRect bounds(0, 0, width(), height());
    QPainter p(this);
    // Black background
    p.fillRect(bounds, Qt::black);
    // Draw text only if visible
    if (!m_visible) {
        return;
    }
    for(int i = 0; i < m_maxEvents && i < m_currentEvents.size(); i++)
    {
        Event *e = m_currentEvents.at(i);
        e->style()->drawEvent(&p, *e, bounds);
    }
}

void SubtitlesForm::mousePressEvent(QMouseEvent* e)
{
    m_mouseOffset = e->globalPos() - geometry().topLeft();
}

void SubtitlesForm::mouseMoveEvent(QMouseEvent* e)
{
    if (m_mouseOffset.isNull())
        return;

    if (!m_resizable)
        return;

    // Simply move the window on mouse drag
    QRect current = geometry();
    QPoint moveTo = e->globalPos() - m_mouseOffset;
    current.moveTopLeft(moveTo);

    changeGeometry(current);
}

void SubtitlesForm::mouseReleaseEvent(QMouseEvent*)
{
    m_mouseOffset = QPoint();
}

void SubtitlesForm::mouseDoubleClickEvent(QMouseEvent*)
{
    if (!m_resizable)
        return;
    // Fit screen on double-click
    QRect current = geometry();
    if (current.left() != m_screenGeom.left() ||
        current.right() != m_screenGeom.right()) {
        // First fits width
        current.setLeft(m_screenGeom.left());
        current.setRight(m_screenGeom.right());
    }
    else {
        // Second fits top
        current.setTop(m_screenGeom.top());
    }
    changeGeometry(current);
}

void SubtitlesForm::wheelEvent(QWheelEvent* event)
{
    if (!m_resizable)
        return;
    QRect current = geometry();
    int factor = event->delta() / 60;
    if (event->orientation() == Qt::Horizontal ||
        event->modifiers().testFlag(Qt::ControlModifier)) {
        current.setHeight(current.height() + factor);
    } else {
        current.moveLeft(current.left() - factor/2);  // Keep centered
        current.setWidth(current.width() + factor);
    }
    changeGeometry(current);
}

void SubtitlesForm::screenResizable(bool state)
{
    m_resizable = state;
}


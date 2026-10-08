#include "LevelMeter.h"

#include <QPainter>

LevelMeter::LevelMeter(QWidget* pParent)
    : QWidget(pParent)
{
    setMinimumSize(20, 80);
}

void LevelMeter::fnSetLevel(float fLevel)
{
    m_fLevel = qBound(0.0f, fLevel, 1.0f);
    update();
}

float LevelMeter::fnGetLevel() const
{
    return m_fLevel;
}

QSize LevelMeter::sizeHint() const
{
    return QSize(24, 160);
}

void LevelMeter::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect clRect =
        rect().adjusted(4, 4, -4, -4);

    painter.setPen(Qt::NoPen);
    painter.setBrush(
        palette().window().color().darker(130));
    painter.drawRoundedRect(clRect, 4, 4);

    const int iHeight =
        static_cast<int>(
            clRect.height() * m_fLevel);

    if (iHeight > 0) {
        const QRect clFill(
            clRect.left(),
            clRect.bottom() - iHeight + 1,
            clRect.width(),
            iHeight);

        painter.setBrush(palette().highlight());
        painter.drawRoundedRect(clFill, 3, 3);
    }
}

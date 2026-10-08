#include "TransportButton.h"

#include <QPainter>

TransportButton::TransportButton(
    ButtonType eButtonType,
    QWidget* pParent)
    : QPushButton(pParent),
      m_eButtonType(eButtonType)
{
    setMinimumSize(52, 42);
    setFocusPolicy(Qt::StrongFocus);
}

TransportButton::ButtonType
TransportButton::fnGetButtonType() const
{
    return m_eButtonType;
}

void TransportButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(
        QPen(
            palette().mid().color(),
            1));

    painter.setBrush(
        isDown()
            ? palette().mid()
            : palette().button());

    painter.drawRoundedRect(
        QRectF(rect()).adjusted(2, 2, -2, -2),
        6, 6);

    painter.setPen(Qt::NoPen);
    painter.setBrush(
        palette().buttonText());

    if (m_eButtonType ==
        ButtonType::Play) {
        QPolygonF clTriangle;
        clTriangle
            << QPointF(width() * 0.40,
                       height() * 0.28)
            << QPointF(width() * 0.40,
                       height() * 0.72)
            << QPointF(width() * 0.70,
                       height() * 0.50);
        painter.drawPolygon(clTriangle);
    } else if (m_eButtonType ==
               ButtonType::Pause) {
        painter.drawRoundedRect(
            QRectF(
                width() * 0.34,
                height() * 0.28,
                width() * 0.11,
                height() * 0.44),
            2, 2);
        painter.drawRoundedRect(
            QRectF(
                width() * 0.55,
                height() * 0.28,
                width() * 0.11,
                height() * 0.44),
            2, 2);
    } else {
        painter.drawRoundedRect(
            QRectF(
                width() * 0.34,
                height() * 0.34,
                width() * 0.32,
                height() * 0.32),
            2, 2);
    }
}

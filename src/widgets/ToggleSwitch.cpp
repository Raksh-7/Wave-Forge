#include "ToggleSwitch.h"

#include <QMouseEvent>
#include <QPainter>

ToggleSwitch::ToggleSwitch(QWidget* pParent)
    : QWidget(pParent)
{
    setMinimumSize(52, 28);
}

bool ToggleSwitch::fnIsChecked() const
{
    return m_bChecked;
}

void ToggleSwitch::fnSetChecked(bool bChecked)
{
    if (m_bChecked == bChecked)
        return;

    m_bChecked = bChecked;
    update();
    emit fnToggled(m_bChecked);
}

QSize ToggleSwitch::sizeHint() const
{
    return QSize(52, 28);
}

void ToggleSwitch::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF clTrack(
        2,
        5,
        width() - 4,
        height() - 10);

    painter.setPen(Qt::NoPen);
    painter.setBrush(
        m_bChecked
            ? palette().highlight()
            : palette().mid());

    painter.drawRoundedRect(
        clTrack,
        clTrack.height() / 2,
        clTrack.height() / 2);

    const qreal dRadius =
        clTrack.height() / 2 - 2;

    const qreal dX =
        m_bChecked
            ? clTrack.right() - dRadius - 2
            : clTrack.left() + dRadius + 2;

    painter.setBrush(palette().button());
    painter.drawEllipse(
        QPointF(dX, clTrack.center().y()),
        dRadius,
        dRadius);
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
        fnSetChecked(!m_bChecked);
}

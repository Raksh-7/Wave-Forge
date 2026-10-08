#include "Knob.h"

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>

Knob::Knob(QWidget* pParent)
    : QWidget(pParent)
{
    setMinimumSize(64, 64);
    setFocusPolicy(Qt::StrongFocus);
}

int Knob::fnGetValue() const
{
    return m_iValue;
}

void Knob::fnSetValue(int iValue)
{
    const int iNewValue =
        qBound(m_iMinimum, iValue, m_iMaximum);

    if (iNewValue == m_iValue)
        return;

    m_iValue = iNewValue;
    update();
    emit fnValueChanged(m_iValue);
}

QSize Knob::sizeHint() const
{
    return QSize(88, 88);
}

void Knob::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPointF clCenter(
        width() / 2.0,
        height() / 2.0);
    const double dRadius =
        qMin(width(), height()) / 2.0 - 10.0;

    painter.setPen(QPen(
        palette().mid().color(), 3));
    painter.setBrush(palette().button());
    painter.drawEllipse(
        clCenter, dRadius, dRadius);

    const double dProgress =
        static_cast<double>(
            m_iValue - m_iMinimum) /
        static_cast<double>(
            m_iMaximum - m_iMinimum);

    const double dAngle =
        -225.0 + dProgress * 270.0;
    const double dRadians =
        qDegreesToRadians(dAngle);

    const QPointF clEnd(
        clCenter.x() +
            (dRadius - 8.0) * qCos(dRadians),
        clCenter.y() +
            (dRadius - 8.0) * qSin(dRadians));

    painter.setPen(QPen(
        palette().highlight().color(), 4));
    painter.drawLine(clCenter, clEnd);

    painter.setPen(palette().text().color());
    painter.drawText(
        rect(),
        Qt::AlignCenter,
        QString::number(m_iValue) + "%");
}

void Knob::mousePressEvent(QMouseEvent* pEvent)
{
    m_clLastMousePosition = pEvent->pos();
    setFocus();
}

void Knob::mouseMoveEvent(QMouseEvent* pEvent)
{
    const int iDelta =
        m_clLastMousePosition.y() -
        pEvent->pos().y();

    if (iDelta != 0) {
        fnSetValue(m_iValue + iDelta);
        m_clLastMousePosition = pEvent->pos();
    }
}

void Knob::wheelEvent(QWheelEvent* pEvent)
{
    fnSetValue(
        m_iValue +
        pEvent->angleDelta().y() / 120);
    pEvent->accept();
}

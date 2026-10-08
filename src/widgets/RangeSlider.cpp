#include "RangeSlider.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

RangeSlider::RangeSlider(QWidget* pParent)
    : QWidget(pParent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(36);
}

double RangeSlider::fnGetMinimumValue() const
{
    return m_dMinimumValue;
}

double RangeSlider::fnGetMaximumValue() const
{
    return m_dMaximumValue;
}

double RangeSlider::fnGetLowerValue() const
{
    return m_dLowerValue;
}

double RangeSlider::fnGetUpperValue() const
{
    return m_dUpperValue;
}

void RangeSlider::fnSetRange(
    double dMinimumValue,
    double dMaximumValue)
{
    if (dMaximumValue < dMinimumValue)
    {
        std::swap(
            dMinimumValue,
            dMaximumValue);
    }

    m_dMinimumValue = dMinimumValue;
    m_dMaximumValue = dMaximumValue;

    m_dLowerValue =
        fnClampValue(m_dLowerValue);

    m_dUpperValue =
        fnClampValue(m_dUpperValue);

    if (m_dLowerValue > m_dUpperValue)
    {
        m_dLowerValue =
            m_dUpperValue;
    }

    update();

    fnEmitRangeChanged();
}

void RangeSlider::fnSetLowerValue(
    double dValue)
{
    const double dNewValue =
        qBound(
            m_dMinimumValue,
            dValue,
            m_dUpperValue);

    if (qFuzzyCompare(
        m_dLowerValue,
        dNewValue))
    {
        return;
    }

    m_dLowerValue = dNewValue;

    update();

    fnEmitRangeChanged();
}

void RangeSlider::fnSetUpperValue(
    double dValue)
{
    const double dNewValue =
        qBound(
            m_dLowerValue,
            dValue,
            m_dMaximumValue);

    if (qFuzzyCompare(
        m_dUpperValue,
        dNewValue))
    {
        return;
    }

    m_dUpperValue = dNewValue;

    update();

    fnEmitRangeChanged();
}

void RangeSlider::fnSetValues(
    double dLowerValue,
    double dUpperValue)
{
    const double dClampedLower =
        qBound(
            m_dMinimumValue,
            qMin(
                dLowerValue,
                dUpperValue),
            m_dMaximumValue);

    const double dClampedUpper =
        qBound(
            dClampedLower,
            dUpperValue,
            m_dMaximumValue);

    const bool bLowerChanged =
        !qFuzzyCompare(
            m_dLowerValue,
            dClampedLower);

    const bool bUpperChanged =
        !qFuzzyCompare(
            m_dUpperValue,
            dClampedUpper);

    if (!bLowerChanged &&
        !bUpperChanged)
    {
        return;
    }

    m_dLowerValue =
        dClampedLower;

    m_dUpperValue =
        dClampedUpper;

    update();

    fnEmitRangeChanged();
}

QSize RangeSlider::sizeHint() const
{
    return QSize(300, 40);
}

void RangeSlider::paintEvent(
    QPaintEvent*)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing);

    const int iMargin =
        qMax(
            10,
            height() / 3);

    const int iTrackY =
        height() / 2;

    const int iTrackLeft =
        iMargin;

    const int iTrackRight =
        width() - iMargin;

    const int iTrackWidth =
        qMax(
            1,
            iTrackRight - iTrackLeft);

    const int iLowerPosition =
        fnPositionFromValue(
            m_dLowerValue);

    const int iUpperPosition =
        fnPositionFromValue(
            m_dUpperValue);

    painter.setPen(
        Qt::NoPen);

    painter.setBrush(
        palette().mid());

    painter.drawRoundedRect(
        QRect(
            iTrackLeft,
            iTrackY - 3,
            iTrackWidth,
            6),
        3,
        3);

    painter.setBrush(
        palette().highlight());

    painter.drawRoundedRect(
        QRect(
            iLowerPosition,
            iTrackY - 4,
            qMax(
                1,
                iUpperPosition -
                iLowerPosition),
            8),
        4,
        4);

    const int iHandleRadius =
        qMax(
            6,
            height() / 5);

    painter.setBrush(
        palette().highlight());

    painter.drawEllipse(
        QPoint(
            iLowerPosition,
            iTrackY),
        iHandleRadius,
        iHandleRadius);

    painter.drawEllipse(
        QPoint(
            iUpperPosition,
            iTrackY),
        iHandleRadius,
        iHandleRadius);
}

void RangeSlider::mousePressEvent(
    QMouseEvent* pEvent)
{
    if (pEvent->button() !=
        Qt::LeftButton)
    {
        return;
    }

    setFocus();

    m_eActiveHandle =
        fnGetHandleAtPosition(
            pEvent->position().toPoint());

    if (m_eActiveHandle ==
        Handle::None)
    {
        const double dValue =
            fnValueFromPosition(
                pEvent->position().x());

        const double dLowerDistance =
            qAbs(
                dValue -
                m_dLowerValue);

        const double dUpperDistance =
            qAbs(
                dValue -
                m_dUpperValue);

        if (dLowerDistance <
            dUpperDistance)
        {
            m_eActiveHandle =
                Handle::Lower;
        }
        else
        {
            m_eActiveHandle =
                Handle::Upper;
        }
    }

    m_bDragging = true;

    fnSetActiveHandleValue(
        fnValueFromPosition(
            pEvent->position().x()));

    pEvent->accept();
}

void RangeSlider::mouseMoveEvent(
    QMouseEvent* pEvent)
{
    if (!m_bDragging)
    {
        return;
    }

    fnSetActiveHandleValue(
        fnValueFromPosition(
            pEvent->position().x()));

    pEvent->accept();
}

void RangeSlider::mouseReleaseEvent(
    QMouseEvent* pEvent)
{
    if (pEvent->button() ==
        Qt::LeftButton)
    {
        m_bDragging = false;
        m_eActiveHandle =
            Handle::None;
    }

    pEvent->accept();
}

void RangeSlider::keyPressEvent(
    QKeyEvent* pEvent)
{
    if (m_eActiveHandle ==
        Handle::None)
    {
        m_eActiveHandle =
            Handle::Lower;
    }

    const double dRange =
        m_dMaximumValue -
        m_dMinimumValue;

    const double dStep =
        dRange > 0.0
        ? dRange / 100.0
        : 0.01;

    if (pEvent->key() ==
        Qt::Key_Left)
    {
        fnSetActiveHandleValue(
            (m_eActiveHandle ==
                Handle::Lower
                ? m_dLowerValue
                : m_dUpperValue) -
            dStep);

        pEvent->accept();
        return;
    }

    if (pEvent->key() ==
        Qt::Key_Right)
    {
        fnSetActiveHandleValue(
            (m_eActiveHandle ==
                Handle::Lower
                ? m_dLowerValue
                : m_dUpperValue) +
            dStep);

        pEvent->accept();
        return;
    }

    if (pEvent->key() ==
        Qt::Key_Tab)
    {
        m_eActiveHandle =
            m_eActiveHandle ==
            Handle::Lower
            ? Handle::Upper
            : Handle::Lower;

        pEvent->accept();
        return;
    }

    QWidget::keyPressEvent(pEvent);
}

double RangeSlider::fnClampValue(
    double dValue) const
{
    return qBound(
        m_dMinimumValue,
        dValue,
        m_dMaximumValue);
}

double RangeSlider::fnValueFromPosition(
    int iPosition) const
{
    const int iMargin =
        qMax(
            10,
            height() / 3);

    const int iTrackWidth =
        qMax(
            1,
            width() -
            (2 * iMargin));

    const double dNormalized =
        qBound(
            0.0,
            static_cast<double>(
                iPosition - iMargin) /
            iTrackWidth,
            1.0);

    return m_dMinimumValue +
        dNormalized *
        (m_dMaximumValue -
            m_dMinimumValue);
}

int RangeSlider::fnPositionFromValue(
    double dValue) const
{
    const int iMargin =
        qMax(
            10,
            height() / 3);

    const int iTrackWidth =
        qMax(
            1,
            width() -
            (2 * iMargin));

    if (qFuzzyCompare(
        m_dMinimumValue,
        m_dMaximumValue))
    {
        return iMargin;
    }

    const double dNormalized =
        (dValue -
            m_dMinimumValue) /
        (m_dMaximumValue -
            m_dMinimumValue);

    return iMargin +
        qRound(
            dNormalized *
            iTrackWidth);
}

RangeSlider::Handle
RangeSlider::fnGetHandleAtPosition(
    const QPoint& clPosition) const
{
    const int iTrackY =
        height() / 2;

    const int iLowerPosition =
        fnPositionFromValue(
            m_dLowerValue);

    const int iUpperPosition =
        fnPositionFromValue(
            m_dUpperValue);

    const int iHandleRadius =
        qMax(
            8,
            height() / 4);

    const int iLowerDistance =
        qAbs(
            clPosition.x() -
            iLowerPosition);

    const int iUpperDistance =
        qAbs(
            clPosition.x() -
            iUpperPosition);

    if (qAbs(
        clPosition.y() -
        iTrackY) <=
        iHandleRadius)
    {
        if (iLowerDistance <=
            iHandleRadius)
        {
            return Handle::Lower;
        }

        if (iUpperDistance <=
            iHandleRadius)
        {
            return Handle::Upper;
        }
    }

    return Handle::None;
}

void RangeSlider::fnSetActiveHandleValue(
    double dValue)
{
    if (m_eActiveHandle ==
        Handle::Lower)
    {
        fnSetLowerValue(dValue);
    }
    else if (
        m_eActiveHandle ==
        Handle::Upper)
    {
        fnSetUpperValue(dValue);
    }
}

void RangeSlider::fnEmitRangeChanged()
{
    emit fnRangeChanged(
        m_dLowerValue,
        m_dUpperValue);
}

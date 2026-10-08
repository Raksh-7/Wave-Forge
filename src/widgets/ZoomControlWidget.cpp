#include "ZoomControlWidget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

ZoomControlWidget::ZoomControlWidget(QWidget* pParent)
    : QWidget(pParent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumWidth(150);
}

double ZoomControlWidget::fnGetZoomFactor() const
{
    return m_dZoomFactor;
}

void ZoomControlWidget::fnSetZoomFactor(
    double dZoomFactor)
{
    const double dClampedZoom =
        fnClampZoom(dZoomFactor);

    if (qFuzzyCompare(
        m_dZoomFactor,
        dClampedZoom))
    {
        return;
    }

    m_dZoomFactor = dClampedZoom;

    update();

    emit fnZoomFactorChanged(
        m_dZoomFactor);
}

QSize ZoomControlWidget::sizeHint() const
{
    return QSize(170, 36);
}

void ZoomControlWidget::paintEvent(
    QPaintEvent*)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing);

    const int iMargin =
        qMax(
            4,
            height() / 8);

    const int iButtonWidth =
        qMax(
            28,
            height());

    const QRect clMinusRect(
        iMargin,
        iMargin,
        iButtonWidth,
        height() - (2 * iMargin));

    const QRect clPlusRect(
        width() -
        iMargin -
        iButtonWidth,
        iMargin,
        iButtonWidth,
        height() - (2 * iMargin));

    const QRect clValueRect(
        clMinusRect.right() +
        iMargin,
        iMargin,
        qMax(
            1,
            width() -
            (2 * iButtonWidth) -
            (4 * iMargin)),
        height() -
        (2 * iMargin));

    painter.setPen(
        QPen(
            palette().mid().color(),
            1));

    painter.setBrush(
        palette().button());

    painter.drawRoundedRect(
        clMinusRect,
        5,
        5);

    painter.drawRoundedRect(
        clPlusRect,
        5,
        5);

    painter.setPen(
        palette().buttonText().color());

    painter.drawText(
        clMinusRect,
        Qt::AlignCenter,
        QStringLiteral("-"));

    painter.drawText(
        clPlusRect,
        Qt::AlignCenter,
        QStringLiteral("+"));

    painter.setPen(
        palette().text().color());

    painter.drawText(
        clValueRect,
        Qt::AlignCenter,
        QStringLiteral("Zoom %1%")
        .arg(
            qRound(
                m_dZoomFactor *
                100.0)));
}

void ZoomControlWidget::mousePressEvent(
    QMouseEvent* pEvent)
{
    if (pEvent->button() !=
        Qt::LeftButton)
    {
        return;
    }

    setFocus();

    const int iButtonWidth =
        qMax(
            28,
            height());

    const int iMargin =
        qMax(
            4,
            height() / 8);

    if (pEvent->position().x() <=
        iMargin + iButtonWidth)
    {
        fnAdjustZoom(false);
    }
    else if (
        pEvent->position().x() >=
        width() -
        iMargin -
        iButtonWidth)
    {
        fnAdjustZoom(true);
    }
    else
    {
        fnSetZoomFactor(1.0);
    }
}

void ZoomControlWidget::wheelEvent(
    QWheelEvent* pEvent)
{
    if (pEvent->angleDelta().y() == 0)
    {
        return;
    }

    fnAdjustZoom(
        pEvent->angleDelta().y() > 0);

    pEvent->accept();
}

void ZoomControlWidget::keyPressEvent(
    QKeyEvent* pEvent)
{
    if (pEvent->key() == Qt::Key_Plus ||
        pEvent->key() == Qt::Key_Equal)
    {
        fnAdjustZoom(true);
        return;
    }

    if (pEvent->key() == Qt::Key_Minus)
    {
        fnAdjustZoom(false);
        return;
    }

    if (pEvent->key() == Qt::Key_0)
    {
        fnSetZoomFactor(1.0);
        return;
    }

    QWidget::keyPressEvent(pEvent);
}

void ZoomControlWidget::fnAdjustZoom(
    bool bZoomIn)
{
    constexpr double dZoomStep = 1.25;

    if (bZoomIn)
    {
        fnSetZoomFactor(
            m_dZoomFactor *
            dZoomStep);
    }
    else
    {
        fnSetZoomFactor(
            m_dZoomFactor /
            dZoomStep);
    }
}

double ZoomControlWidget::fnClampZoom(
    double dZoomFactor) const
{
    return qBound(
        0.5,
        dZoomFactor,
        8.0);
}

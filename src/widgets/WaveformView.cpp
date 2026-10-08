#include "WaveformView.h"

#include <QHash>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

WaveformView::WaveformView(QWidget* pParent)
    : QWidget(pParent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(180);

    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding);
}

void WaveformView::fnSetPeaks(
    const QVector<float>& aPeaks)
{
    m_aPeaks = aPeaks;

    m_aPeakLevels.clear();

    if (!aPeaks.isEmpty())
    {
        m_aPeakLevels.insert(
            aPeaks.size(),
            aPeaks);
    }

    update();
}

void WaveformView::fnSetPeakLevels(
    const QHash<int, QVector<float>>& aPeakLevels)
{
    m_aPeakLevels = aPeakLevels;

    if (!m_aPeakLevels.isEmpty())
    {
        auto clIterator =
            std::max_element(
                m_aPeakLevels.constBegin(),
                m_aPeakLevels.constEnd(),
                [](const auto& clLeft,
                    const auto& clRight)
                {
                    return clLeft.size() <
                        clRight.size();
                });

        if (clIterator !=
            m_aPeakLevels.constEnd())
        {
            m_aPeaks =
                clIterator.value();
        }
    }

    update();
}

void WaveformView::fnSetDuration(
    double dDurationSeconds)
{
    m_dDurationSeconds =
        qMax(
            0.0,
            dDurationSeconds);

    m_dPlayheadPositionSeconds =
        fnClampTime(
            m_dPlayheadPositionSeconds);

    m_dSelectionStartSeconds =
        fnClampTime(
            m_dSelectionStartSeconds);

    m_dSelectionEndSeconds =
        fnClampTime(
            m_dSelectionEndSeconds);

    if (m_dSelectionStartSeconds >
        m_dSelectionEndSeconds)
    {
        std::swap(
            m_dSelectionStartSeconds,
            m_dSelectionEndSeconds);
    }

    update();
}

void WaveformView::fnSetPlayheadPosition(
    double dPositionSeconds)
{
    m_dPlayheadPositionSeconds =
        fnClampTime(
            dPositionSeconds);

    update();
}

double WaveformView::fnGetPlayheadPosition() const
{
    return m_dPlayheadPositionSeconds;
}

void WaveformView::fnSetZoomFactor(
    double dZoomFactor)
{
    const double dClampedZoom =
        qBound(
            0.5,
            dZoomFactor,
            8.0);

    if (qFuzzyCompare(
        m_dZoomFactor,
        dClampedZoom))
    {
        return;
    }

    m_dZoomFactor =
        dClampedZoom;

    update();

    emit fnZoomFactorChanged(
        m_dZoomFactor);
}

double WaveformView::fnGetZoomFactor() const
{
    return m_dZoomFactor;
}

void WaveformView::fnSetSelection(
    double dStartSeconds,
    double dEndSeconds)
{
    const double dStart =
        fnClampTime(
            qMin(
                dStartSeconds,
                dEndSeconds));

    const double dEnd =
        fnClampTime(
            qMax(
                dStartSeconds,
                dEndSeconds));

    m_dSelectionStartSeconds =
        dStart;

    m_dSelectionEndSeconds =
        dEnd;

    m_bHasSelection =
        dEnd > dStart;

    update();

    emit fnSelectionChanged(
        m_dSelectionStartSeconds,
        m_dSelectionEndSeconds);
}

double WaveformView::fnGetSelectionStart() const
{
    return m_dSelectionStartSeconds;
}

double WaveformView::fnGetSelectionEnd() const
{
    return m_dSelectionEndSeconds;
}

bool WaveformView::fnHasSelection() const
{
    return m_bHasSelection;
}

void WaveformView::fnClearSelection()
{
    m_bHasSelection = false;

    m_dSelectionStartSeconds = 0.0;
    m_dSelectionEndSeconds = 0.0;

    m_eActiveSelectionHandle =
        SelectionHandle::None;

    m_bDraggingSelection = false;

    update();

    emit fnSelectionChanged(
        0.0,
        0.0);
}

void WaveformView::fnClear()
{
    m_aPeaks.clear();
    m_aPeakLevels.clear();

    m_dDurationSeconds = 0.0;
    m_dPlayheadPositionSeconds = 0.0;

    m_dSelectionStartSeconds = 0.0;
    m_dSelectionEndSeconds = 0.0;

    m_bHasSelection = false;
    m_bDraggingSelection = false;

    m_eActiveSelectionHandle =
        SelectionHandle::None;

    update();

    emit fnSelectionChanged(
        0.0,
        0.0);
}

QSize WaveformView::sizeHint() const
{
    return QSize(800, 260);
}

void WaveformView::paintEvent(
    QPaintEvent*)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing);

    painter.fillRect(
        rect(),
        palette().base());

    const int iPlotLeft =
        fnGetPlotLeft();

    const int iPlotRight =
        fnGetPlotRight();

    const int iPlotWidth =
        qMax(
            1,
            iPlotRight -
            iPlotLeft);

    const int iCenterY =
        height() / 2;

    /*
     * Background center line.
     */
    painter.setPen(
        QPen(
            palette().mid().color(),
            1));

    painter.drawLine(
        iPlotLeft,
        iCenterY,
        iPlotRight,
        iCenterY);

    /*
     * Selection background.
     */
    if (m_bHasSelection &&
        m_dDurationSeconds > 0.0)
    {
        const int iSelectionStart =
            fnPositionFromSeconds(
                m_dSelectionStartSeconds);

        const int iSelectionEnd =
            fnPositionFromSeconds(
                m_dSelectionEndSeconds);

        painter.fillRect(
            QRect(
                iSelectionStart,
                0,
                qMax(
                    1,
                    iSelectionEnd -
                    iSelectionStart),
                height()),
            palette().highlight().color());
    }

    /*
     * Waveform.
     */
    const QVector<float>& aCurrentPeaks =
        fnGetCurrentPeaks();

    if (!aCurrentPeaks.isEmpty())
    {
        painter.setPen(
            QPen(
                palette().text().color(),
                1));

        const int iPeakCount =
            aCurrentPeaks.size();

        for (int iPeak = 0;
            iPeak < iPeakCount;
            ++iPeak)
        {
            const double dNormalizedX =
                iPeakCount > 1
                ? static_cast<double>(iPeak) /
                (iPeakCount - 1)
                : 0.0;

            const int iX =
                iPlotLeft +
                qRound(
                    dNormalizedX *
                    iPlotWidth);

            const int iAmplitude =
                qRound(
                    qBound(
                        0.0f,
                        aCurrentPeaks.at(iPeak),
                        1.0f) *
                    (height() * 0.42));

            painter.drawLine(
                iX,
                iCenterY - iAmplitude,
                iX,
                iCenterY + iAmplitude);
        }
    }

    /*
     * Selection handles.
     */
    if (m_bHasSelection)
    {
        const int iStartX =
            fnPositionFromSeconds(
                m_dSelectionStartSeconds);

        const int iEndX =
            fnPositionFromSeconds(
                m_dSelectionEndSeconds);

        painter.setPen(
            QPen(
                palette().highlight().color(),
                2));

        painter.drawLine(
            iStartX,
            0,
            iStartX,
            height());

        painter.drawLine(
            iEndX,
            0,
            iEndX,
            height());

        painter.setBrush(
            palette().highlight());

        painter.drawEllipse(
            QPoint(
                iStartX,
                height() / 2),
            7,
            7);

        painter.drawEllipse(
            QPoint(
                iEndX,
                height() / 2),
            7,
            7);
    }

    /*
     * Playhead.
     */
    if (m_dDurationSeconds > 0.0)
    {
        const int iPlayheadX =
            fnPositionFromSeconds(
                m_dPlayheadPositionSeconds);

        painter.setPen(
            QPen(
                palette().highlight().color(),
                2));

        painter.drawLine(
            iPlayheadX,
            0,
            iPlayheadX,
            height());
    }
}

void WaveformView::mousePressEvent(
    QMouseEvent* pEvent)
{
    if (pEvent->button() !=
        Qt::LeftButton)
    {
        return;
    }

    setFocus();

    const QPoint clPosition =
        pEvent->position().toPoint();

    m_eActiveSelectionHandle =
        fnGetHandleAtPosition(
            clPosition);

    /*
     * Clicking near a handle moves that
     * particular handle.
     */
    if (m_eActiveSelectionHandle !=
        SelectionHandle::None)
    {
        m_bDraggingSelection = true;

        fnSetSelectionHandlePosition(
            clPosition.x());

        pEvent->accept();

        return;
    }

    /*
     * If a selection already exists and
     * the click is not near either handle,
     * seek to that position.
     */
    if (!m_bHasSelection)
    {
        const double dPosition =
            fnSecondsFromPosition(
                clPosition.x());

        m_dPlayheadPositionSeconds =
            dPosition;

        emit fnSeekRequested(
            dPosition);

        update();

        pEvent->accept();

        return;
    }

    /*
     * Clicking inside the selection creates
     * a new selection around the clicked
     * position only when dragging starts.
     */
    const double dClickedTime =
        fnSecondsFromPosition(
            clPosition.x());

    if (dClickedTime >=
        m_dSelectionStartSeconds &&
        dClickedTime <=
        m_dSelectionEndSeconds)
    {
        m_dPlayheadPositionSeconds =
            dClickedTime;

        emit fnSeekRequested(
            dClickedTime);

        update();

        pEvent->accept();

        return;
    }

    /*
     * Outside the current selection:
     * begin a new selection.
     */
    m_dSelectionStartSeconds =
        dClickedTime;

    m_dSelectionEndSeconds =
        dClickedTime;

    m_bHasSelection = true;

    m_eActiveSelectionHandle =
        SelectionHandle::End;

    m_bDraggingSelection = true;

    update();

    emit fnSelectionChanged(
        m_dSelectionStartSeconds,
        m_dSelectionEndSeconds);

    pEvent->accept();
}

void WaveformView::mouseMoveEvent(
    QMouseEvent* pEvent)
{
    if (!m_bDraggingSelection)
    {
        return;
    }

    fnUpdateSelectionFromMouse(
        pEvent->position().x());

    pEvent->accept();
}

void WaveformView::mouseReleaseEvent(
    QMouseEvent* pEvent)
{
    if (pEvent->button() ==
        Qt::LeftButton &&
        m_bDraggingSelection)
    {
        m_bDraggingSelection = false;

        emit fnSelectionFinished(
            m_dSelectionStartSeconds,
            m_dSelectionEndSeconds);

        pEvent->accept();

        return;
    }

    QWidget::mouseReleaseEvent(
        pEvent);
}

void WaveformView::wheelEvent(
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

void WaveformView::keyPressEvent(
    QKeyEvent* pEvent)
{
    const double dStep =
        m_dDurationSeconds > 0.0
        ? m_dDurationSeconds / 100.0
        : 0.01;

    if (pEvent->key() ==
        Qt::Key_Left)
    {
        fnSetPlayheadPosition(
            m_dPlayheadPositionSeconds -
            dStep);

        emit fnSeekRequested(
            m_dPlayheadPositionSeconds);

        return;
    }

    if (pEvent->key() ==
        Qt::Key_Right)
    {
        fnSetPlayheadPosition(
            m_dPlayheadPositionSeconds +
            dStep);

        emit fnSeekRequested(
            m_dPlayheadPositionSeconds);

        return;
    }

    if (pEvent->key() ==
        Qt::Key_Delete)
    {
        fnClearSelection();
        return;
    }

    if (pEvent->key() ==
        Qt::Key_Plus ||
        pEvent->key() ==
        Qt::Key_Equal)
    {
        fnAdjustZoom(true);
        return;
    }

    if (pEvent->key() ==
        Qt::Key_Minus)
    {
        fnAdjustZoom(false);
        return;
    }

    QWidget::keyPressEvent(
        pEvent);
}

int WaveformView::fnGetPlotLeft() const
{
    return 8;
}

int WaveformView::fnGetPlotRight() const
{
    return qMax(
        fnGetPlotLeft() + 1,
        width() - 8);
}

double WaveformView::fnGetVisibleDuration() const
{
    if (m_dZoomFactor <= 0.0)
    {
        return m_dDurationSeconds;
    }

    return m_dDurationSeconds /
        m_dZoomFactor;
}

double WaveformView::fnSecondsFromPosition(
    int iPosition) const
{
    if (m_dDurationSeconds <= 0.0)
    {
        return 0.0;
    }

    const int iPlotLeft =
        fnGetPlotLeft();

    const int iPlotRight =
        fnGetPlotRight();

    const int iPlotWidth =
        qMax(
            1,
            iPlotRight -
            iPlotLeft);

    const double dNormalized =
        qBound(
            0.0,
            static_cast<double>(
                iPosition -
                iPlotLeft) /
            iPlotWidth,
            1.0);

    return fnClampTime(
        dNormalized *
        fnGetVisibleDuration());
}

int WaveformView::fnPositionFromSeconds(
    double dSeconds) const
{
    const int iPlotLeft =
        fnGetPlotLeft();

    const int iPlotRight =
        fnGetPlotRight();

    const int iPlotWidth =
        qMax(
            1,
            iPlotRight -
            iPlotLeft);

    const double dVisibleDuration =
        fnGetVisibleDuration();

    if (dVisibleDuration <= 0.0)
    {
        return iPlotLeft;
    }

    const double dNormalized =
        qBound(
            0.0,
            dSeconds /
            dVisibleDuration,
            1.0);

    return iPlotLeft +
        qRound(
            dNormalized *
            iPlotWidth);
}

WaveformView::SelectionHandle
WaveformView::fnGetHandleAtPosition(
    const QPoint& clPosition) const
{
    if (!m_bHasSelection)
    {
        return SelectionHandle::None;
    }

    const int iStartX =
        fnPositionFromSeconds(
            m_dSelectionStartSeconds);

    const int iEndX =
        fnPositionFromSeconds(
            m_dSelectionEndSeconds);

    const int iHandleRadius =
        qMax(
            10,
            height() / 8);

    if (qAbs(
        clPosition.x() -
        iStartX) <=
        iHandleRadius)
    {
        return SelectionHandle::Start;
    }

    if (qAbs(
        clPosition.x() -
        iEndX) <=
        iHandleRadius)
    {
        return SelectionHandle::End;
    }

    return SelectionHandle::None;
}

void WaveformView::fnSetSelectionHandlePosition(
    int iPosition)
{
    const double dTime =
        fnSecondsFromPosition(
            iPosition);

    if (m_eActiveSelectionHandle ==
        SelectionHandle::Start)
    {
        m_dSelectionStartSeconds =
            qMin(
                dTime,
                m_dSelectionEndSeconds);
    }
    else if (
        m_eActiveSelectionHandle ==
        SelectionHandle::End)
    {
        m_dSelectionEndSeconds =
            qMax(
                dTime,
                m_dSelectionStartSeconds);
    }

    m_bHasSelection =
        m_dSelectionEndSeconds >
        m_dSelectionStartSeconds;

    update();

    emit fnSelectionChanged(
        m_dSelectionStartSeconds,
        m_dSelectionEndSeconds);
}

void WaveformView::fnUpdateSelectionFromMouse(
    int iPosition)
{
    fnSetSelectionHandlePosition(
        iPosition);
}

double WaveformView::fnClampTime(
    double dTimeSeconds) const
{
    return qBound(
        0.0,
        dTimeSeconds,
        m_dDurationSeconds);
}

void WaveformView::fnAdjustZoom(
    bool bZoomIn)
{
    constexpr double dZoomStep = 1.25;

    const double dNewZoom =
        bZoomIn
        ? m_dZoomFactor *
        dZoomStep
        : m_dZoomFactor /
        dZoomStep;

    fnSetZoomFactor(
        dNewZoom);
}

const QVector<float>&
WaveformView::fnGetCurrentPeaks() const
{
    if (m_aPeakLevels.isEmpty())
    {
        return m_aPeaks;
    }

    /*
     * Select a peak level appropriate for
     * the amount of visible waveform.
     */
    const int iTargetPeakCount =
        qMax(
            128,
            qRound(
                width() *
                m_dZoomFactor));

    const QVector<float>* pBestLevel =
        nullptr;

    int iBestDifference =
        std::numeric_limits<int>::max();

    for (auto clIterator =
        m_aPeakLevels.constBegin();
        clIterator !=
        m_aPeakLevels.constEnd();
        ++clIterator)
    {
        const int iDifference =
            qAbs(
                clIterator.value().size() -
                iTargetPeakCount);

        if (iDifference <
            iBestDifference)
        {
            iBestDifference =
                iDifference;

            pBestLevel =
                &clIterator.value();
        }
    }

    if (pBestLevel != nullptr)
    {
        return *pBestLevel;
    }

    return m_aPeaks;
}
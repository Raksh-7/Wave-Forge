#pragma once

#include <QVector>
#include <QWidget>

class WaveformView : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformView(QWidget* pParent = nullptr);

    void fnSetPeaks(const QVector<float>& aPeaks);
    void fnSetPeakLevels(
        const QHash<int, QVector<float>>& aPeakLevels);

    void fnSetDuration(double dDurationSeconds);

    void fnSetPlayheadPosition(
        double dPositionSeconds);

    double fnGetPlayheadPosition() const;

    void fnSetZoomFactor(double dZoomFactor);
    double fnGetZoomFactor() const;

    void fnSetSelection(
        double dStartSeconds,
        double dEndSeconds);

    double fnGetSelectionStart() const;
    double fnGetSelectionEnd() const;

    bool fnHasSelection() const;

    void fnClearSelection();
    void fnClear();

    QSize sizeHint() const override;

signals:
    void fnSeekRequested(double dPositionSeconds);

    void fnSelectionChanged(
        double dStartSeconds,
        double dEndSeconds);

    void fnSelectionFinished(
        double dStartSeconds,
        double dEndSeconds);

    void fnZoomFactorChanged(double dZoomFactor);

protected:
    void paintEvent(QPaintEvent* pEvent) override;

    void mousePressEvent(QMouseEvent* pEvent) override;
    void mouseMoveEvent(QMouseEvent* pEvent) override;
    void mouseReleaseEvent(QMouseEvent* pEvent) override;

    void wheelEvent(QWheelEvent* pEvent) override;

    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    enum class SelectionHandle
    {
        None,
        Start,
        End
    };

    int fnGetPlotLeft() const;
    int fnGetPlotRight() const;

    double fnGetVisibleDuration() const;

    double fnSecondsFromPosition(
        int iPosition) const;

    int fnPositionFromSeconds(
        double dSeconds) const;

    SelectionHandle fnGetHandleAtPosition(
        const QPoint& clPosition) const;

    void fnSetSelectionHandlePosition(
        int iPosition);

    void fnUpdateSelectionFromMouse(
        int iPosition);

    double fnClampTime(
        double dTimeSeconds) const;

    void fnAdjustZoom(bool bZoomIn);

    const QVector<float>& fnGetCurrentPeaks() const;

private:
    QVector<float> m_aPeaks;

    QHash<int, QVector<float>> m_aPeakLevels;

    double m_dDurationSeconds = 0.0;
    double m_dPlayheadPositionSeconds = 0.0;

    double m_dSelectionStartSeconds = 0.0;
    double m_dSelectionEndSeconds = 0.0;

    double m_dZoomFactor = 1.0;

    SelectionHandle m_eActiveSelectionHandle =
        SelectionHandle::None;

    bool m_bDraggingSelection = false;
    bool m_bHasSelection = false;
};

#pragma once

#include <QWidget>

class RangeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit RangeSlider(QWidget* pParent = nullptr);

    double fnGetMinimumValue() const;
    double fnGetMaximumValue() const;

    double fnGetLowerValue() const;
    double fnGetUpperValue() const;

    void fnSetRange(double dMinimumValue,
        double dMaximumValue);

    void fnSetLowerValue(double dValue);
    void fnSetUpperValue(double dValue);

    void fnSetValues(
        double dLowerValue,
        double dUpperValue);

    QSize sizeHint() const override;

signals:
    void fnRangeChanged(double dLowerValue,
        double dUpperValue);

protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void mousePressEvent(QMouseEvent* pEvent) override;
    void mouseMoveEvent(QMouseEvent* pEvent) override;
    void mouseReleaseEvent(QMouseEvent* pEvent) override;
    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    enum class Handle
    {
        None,
        Lower,
        Upper
    };

    double fnClampValue(double dValue) const;

    double fnValueFromPosition(int iPosition) const;

    int fnPositionFromValue(double dValue) const;

    Handle fnGetHandleAtPosition(
        const QPoint& clPosition) const;

    void fnSetActiveHandleValue(
        double dValue);

    void fnEmitRangeChanged();

private:
    double m_dMinimumValue = 0.0;
    double m_dMaximumValue = 1.0;

    double m_dLowerValue = 0.0;
    double m_dUpperValue = 1.0;

    Handle m_eActiveHandle = Handle::None;

    bool m_bDragging = false;
};

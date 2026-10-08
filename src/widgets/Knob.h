#pragma once

#include <QPoint>
#include <QSize>
#include <QWidget>

class Knob : public QWidget
{
    Q_OBJECT

public:
    explicit Knob(QWidget* pParent = nullptr);

    int fnGetValue() const;
    void fnSetValue(int iValue);

    QSize sizeHint() const override;

signals:
    void fnValueChanged(int iValue);

protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void mousePressEvent(QMouseEvent* pEvent) override;
    void mouseMoveEvent(QMouseEvent* pEvent) override;
    void wheelEvent(QWheelEvent* pEvent) override;

private:
    int m_iMinimum = 0;
    int m_iMaximum = 200;
    int m_iValue = 100;
    QPoint m_clLastMousePosition;
};

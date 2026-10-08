#pragma once

#include <QWidget>

class ZoomControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZoomControlWidget(QWidget* pParent = nullptr);

    double fnGetZoomFactor() const;
    void fnSetZoomFactor(double dZoomFactor);

    QSize sizeHint() const override;

signals:
    void fnZoomFactorChanged(double dZoomFactor);

protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void mousePressEvent(QMouseEvent* pEvent) override;
    void wheelEvent(QWheelEvent* pEvent) override;
    void keyPressEvent(QKeyEvent* pEvent) override;

private:
    void fnAdjustZoom(bool bZoomIn);
    double fnClampZoom(double dZoomFactor) const;

private:
    double m_dZoomFactor = 1.0;
};

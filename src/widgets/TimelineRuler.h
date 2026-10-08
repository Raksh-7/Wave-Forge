#pragma once

#include <QWidget>

class TimelineRuler : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineRuler(QWidget* pParent = nullptr);

    void fnSetDurationSeconds(double dDurationSeconds);
    void fnSetCurrentTimeSeconds(double dCurrentTimeSeconds);

    double fnGetDurationSeconds() const;

    QSize sizeHint() const override;

signals:
    void fnSeekRequested(double dTimeSeconds);

protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void mousePressEvent(QMouseEvent* pEvent) override;

private:
    QString fnFormatTime(double dTimeSeconds) const;

    double m_dDurationSeconds = 0.0;
    double m_dCurrentTimeSeconds = 0.0;
};

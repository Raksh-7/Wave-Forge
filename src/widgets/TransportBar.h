#pragma once

#include <QWidget>

class QLabel;
class TransportButton;

class TransportBar : public QWidget
{
    Q_OBJECT

public:
    explicit TransportBar(QWidget* pParent = nullptr);

    void fnSetDuration(double dDurationSeconds);
    void fnSetCurrentTime(double dTimeSeconds);

    QSize sizeHint() const override;

signals:
    void fnPlayRequested();
    void fnPauseRequested();
    void fnStopRequested();

private:
    QString fnFormatTime(double dTimeSeconds) const;

    TransportButton* m_pPlayButton = nullptr;
    TransportButton* m_pPauseButton = nullptr;
    TransportButton* m_pStopButton = nullptr;
    QLabel* m_pTimeLabel = nullptr;

    double m_dDurationSeconds = 0.0;
    double m_dCurrentTimeSeconds = 0.0;
};

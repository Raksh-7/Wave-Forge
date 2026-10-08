#include "TransportBar.h"
#include "TransportButton.h"

#include <QHBoxLayout>
#include <QLabel>

TransportBar::TransportBar(QWidget* pParent)
    : QWidget(pParent)
{
    auto* pLayout = new QHBoxLayout(this);
    pLayout->setContentsMargins(8, 8, 8, 8);

    m_pPlayButton =
        new TransportButton(
            TransportButton::ButtonType::Play,
            this);

    m_pPauseButton =
        new TransportButton(
            TransportButton::ButtonType::Pause,
            this);

    m_pStopButton =
        new TransportButton(
            TransportButton::ButtonType::Stop,
            this);

    m_pTimeLabel =
        new QLabel("00:00.000 / 00:00.000", this);

    pLayout->addWidget(m_pPlayButton);
    pLayout->addWidget(m_pPauseButton);
    pLayout->addWidget(m_pStopButton);
    pLayout->addSpacing(16);
    pLayout->addWidget(m_pTimeLabel);
    pLayout->addStretch();

    connect(
        m_pPlayButton,
        &QPushButton::clicked,
        this,
        &TransportBar::fnPlayRequested);

    connect(
        m_pPauseButton,
        &QPushButton::clicked,
        this,
        &TransportBar::fnPauseRequested);

    connect(
        m_pStopButton,
        &QPushButton::clicked,
        this,
        &TransportBar::fnStopRequested);
}

void TransportBar::fnSetDuration(
    double dDurationSeconds)
{
    m_dDurationSeconds =
        qMax(0.0, dDurationSeconds);
    fnSetCurrentTime(m_dCurrentTimeSeconds);
}

void TransportBar::fnSetCurrentTime(
    double dTimeSeconds)
{
    m_dCurrentTimeSeconds =
        qBound(
            0.0,
            dTimeSeconds,
            m_dDurationSeconds);

    m_pTimeLabel->setText(
        QString("%1 / %2")
            .arg(fnFormatTime(
                m_dCurrentTimeSeconds))
            .arg(fnFormatTime(
                m_dDurationSeconds)));
}

QSize TransportBar::sizeHint() const
{
    return QSize(500, 60);
}

QString TransportBar::fnFormatTime(
    double dTimeSeconds) const
{
    const int iMinutes =
        static_cast<int>(dTimeSeconds) / 60;
    const int iSeconds =
        static_cast<int>(dTimeSeconds) % 60;
    const int iMilliseconds =
        static_cast<int>(
            (dTimeSeconds -
             static_cast<int>(dTimeSeconds)) *
            1000.0);

    return QString("%1:%2.%3")
        .arg(iMinutes, 2, 10, QChar('0'))
        .arg(iSeconds, 2, 10, QChar('0'))
        .arg(iMilliseconds, 3, 10, QChar('0'));
}

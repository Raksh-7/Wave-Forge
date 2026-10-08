#include "TimelineRuler.h"

#include <QMouseEvent>
#include <QPainter>

TimelineRuler::TimelineRuler(QWidget* pParent)
    : QWidget(pParent)
{
    setMinimumHeight(36);
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed);
}

void TimelineRuler::fnSetDurationSeconds(
    double dDurationSeconds)
{
    m_dDurationSeconds = qMax(0.0, dDurationSeconds);
    m_dCurrentTimeSeconds =
        qBound(0.0,
               m_dCurrentTimeSeconds,
               m_dDurationSeconds);
    update();
}

void TimelineRuler::fnSetCurrentTimeSeconds(
    double dCurrentTimeSeconds)
{
    m_dCurrentTimeSeconds =
        qBound(0.0,
               dCurrentTimeSeconds,
               m_dDurationSeconds);
    update();
}

double TimelineRuler::fnGetDurationSeconds() const
{
    return m_dDurationSeconds;
}

QSize TimelineRuler::sizeHint() const
{
    return QSize(600, 36);
}

void TimelineRuler::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().base());

    if (m_dDurationSeconds <= 0.0)
        return;

    painter.setPen(palette().mid().color());

    const int iTickCount =
        qMax(2, width() / 100);

    for (int i = 0; i <= iTickCount; ++i) {
        const double dRatio =
            static_cast<double>(i) /
            static_cast<double>(iTickCount);

        const int iX =
            qRound(dRatio * (width() - 1));

        painter.drawLine(
            iX, height() - 10,
            iX, height() - 1);

        painter.drawText(
            QRect(iX - 40, 2, 80, 18),
            Qt::AlignCenter,
            fnFormatTime(
                dRatio * m_dDurationSeconds));
    }

    const int iPlayheadX =
        qRound(
            (m_dCurrentTimeSeconds /
             m_dDurationSeconds) *
            (width() - 1));

    painter.setPen(QPen(
        palette().highlight().color(), 2));
    painter.drawLine(
        iPlayheadX, 0,
        iPlayheadX, height());
}

void TimelineRuler::mousePressEvent(QMouseEvent* pEvent)
{
    if (m_dDurationSeconds <= 0.0)
        return;

    const double dRatio =
        qBound(
            0.0,
            static_cast<double>(
                pEvent->position().x()) /
            static_cast<double>(
                qMax(1, width() - 1)),
            1.0);

    emit fnSeekRequested(
        dRatio * m_dDurationSeconds);
}

QString TimelineRuler::fnFormatTime(
    double dTimeSeconds) const
{
    const int iMinutes =
        static_cast<int>(dTimeSeconds) / 60;
    const int iSeconds =
        static_cast<int>(dTimeSeconds) % 60;
    const int iMilliseconds =
        static_cast<int>(
            (dTimeSeconds -
             std::floor(dTimeSeconds)) *
            1000.0);

    return QString("%1:%2.%3")
        .arg(iMinutes, 2, 10, QChar('0'))
        .arg(iSeconds, 2, 10, QChar('0'))
        .arg(iMilliseconds, 3, 10, QChar('0'));
}

#include "Toast.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

Toast::Toast(QWidget* pParent)
    : QFrame(pParent)
{
    setWindowFlags(
        Qt::Tool |
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_ShowWithoutActivating);

    auto* pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(14, 10, 14, 10);

    m_pMessageLabel =
        new QLabel(this);

    pLayout->addWidget(
        m_pMessageLabel);

    m_pHideTimer =
        new QTimer(this);

    m_pHideTimer->setSingleShot(true);

    connect(
        m_pHideTimer,
        &QTimer::timeout,
        this,
        &QWidget::hide);
}

void Toast::fnShowMessage(
    const QString& QsMessage,
    int iDurationMilliseconds)
{
    m_pMessageLabel->setText(QsMessage);
    adjustSize();

    if (parentWidget()) {
        const QPoint clPosition =
            parentWidget()->mapToGlobal(
                QPoint(
                    parentWidget()->width() -
                        width() - 24,
                    parentWidget()->height() -
                        height() - 24));

        move(clPosition);
    }

    show();
    raise();

    m_pHideTimer->start(
        qMax(100, iDurationMilliseconds));
}

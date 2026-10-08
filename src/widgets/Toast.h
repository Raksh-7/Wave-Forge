#pragma once

#include <QFrame>

class QLabel;
class QTimer;

class Toast : public QFrame
{
    Q_OBJECT

public:
    explicit Toast(QWidget* pParent = nullptr);

    void fnShowMessage(
        const QString& QsMessage,
        int iDurationMilliseconds = 2500);

private:
    QLabel* m_pMessageLabel = nullptr;
    QTimer* m_pHideTimer = nullptr;
};

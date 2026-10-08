#pragma once

#include <QPushButton>

class TransportButton : public QPushButton
{
    Q_OBJECT

public:
    enum class ButtonType
    {
        Play,
        Pause,
        Stop
    };

    explicit TransportButton(
        ButtonType eButtonType,
        QWidget* pParent = nullptr);

    ButtonType fnGetButtonType() const;

protected:
    void paintEvent(QPaintEvent* pEvent) override;

private:
    ButtonType m_eButtonType;
};

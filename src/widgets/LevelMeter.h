#pragma once

#include <QWidget>

class LevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMeter(QWidget* pParent = nullptr);

    void fnSetLevel(float fLevel);
    float fnGetLevel() const;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* pEvent) override;

private:
    float m_fLevel = 0.0f;
};

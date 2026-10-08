#pragma once

#include <QWidget>

class ToggleSwitch : public QWidget
{
    Q_OBJECT

public:
    explicit ToggleSwitch(QWidget* pParent = nullptr);

    bool fnIsChecked() const;
    void fnSetChecked(bool bChecked);

    QSize sizeHint() const override;

signals:
    void fnToggled(bool bChecked);

protected:
    void paintEvent(QPaintEvent* pEvent) override;
    void mouseReleaseEvent(QMouseEvent* pEvent) override;

private:
    bool m_bChecked = false;
};

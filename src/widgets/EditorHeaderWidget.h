#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class EditorHeaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorHeaderWidget(QWidget* pParent = nullptr);

signals:
    void fnOpenRequested();
    void fnSaveRequested();
    void fnExportRequested();

    void fnTrimRequested();
    void fnCutRequested();
    void fnFadeInRequested();
    void fnFadeOutRequested();
    void fnApplyGainRequested();

    void fnUndoRequested();
    void fnRedoRequested();

    void fnThemeToggleRequested();

private:
    void fnCreateUi();

private:
    QLabel* m_pTitleLabel = nullptr;

    QPushButton* m_pOpenButton = nullptr;
    QPushButton* m_pSaveButton = nullptr;
    QPushButton* m_pExportButton = nullptr;

    QPushButton* m_pTrimButton = nullptr;
    QPushButton* m_pCutButton = nullptr;
    QPushButton* m_pFadeInButton = nullptr;
    QPushButton* m_pFadeOutButton = nullptr;
    QPushButton* m_pApplyGainButton = nullptr;

    QPushButton* m_pUndoButton = nullptr;
    QPushButton* m_pRedoButton = nullptr;

    QPushButton* m_pThemeButton = nullptr;
};

#include "EditorHeaderWidget.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

EditorHeaderWidget::EditorHeaderWidget(
    QWidget* pParent)
    : QWidget(pParent)
{
    fnCreateUi();
}

void EditorHeaderWidget::fnCreateUi()
{
    auto* pLayout =
        new QHBoxLayout(this);

    pLayout->setContentsMargins(
        12,
        8,
        12,
        8);

    pLayout->setSpacing(5);

    m_pTitleLabel =
        new QLabel(
            QStringLiteral("WaveForge"),
            this);

    QFont clFont =
        m_pTitleLabel->font();

    clFont.setPointSize(16);
    clFont.setBold(true);

    m_pTitleLabel->setFont(clFont);

    auto fnCreateButton =
        [this](const QString& QsText)
        {
            auto* pButton =
                new QPushButton(
                    QsText,
                    this);

            pButton->setMinimumHeight(32);

            return pButton;
        };

    m_pOpenButton =
        fnCreateButton(
            QStringLiteral("Open"));

    m_pSaveButton =
        fnCreateButton(
            QStringLiteral("Save"));

    m_pExportButton =
        fnCreateButton(
            QStringLiteral("Export"));

    m_pTrimButton =
        fnCreateButton(
            QStringLiteral("Trim"));

    m_pCutButton =
        fnCreateButton(
            QStringLiteral("Cut"));

    m_pFadeInButton =
        fnCreateButton(
            QStringLiteral("Fade In"));

    m_pFadeOutButton =
        fnCreateButton(
            QStringLiteral("Fade Out"));

    m_pApplyGainButton =
        fnCreateButton(
            QStringLiteral("Apply Gain"));

    m_pUndoButton =
        fnCreateButton(
            QStringLiteral("Undo"));

    m_pRedoButton =
        fnCreateButton(
            QStringLiteral("Redo"));

    m_pThemeButton =
        fnCreateButton(
            QStringLiteral("Theme"));

    pLayout->addWidget(
        m_pTitleLabel);

    pLayout->addStretch();

    pLayout->addWidget(
        m_pOpenButton);

    pLayout->addWidget(
        m_pSaveButton);

    pLayout->addWidget(
        m_pExportButton);

    pLayout->addWidget(
        m_pTrimButton);

    pLayout->addWidget(
        m_pCutButton);

    pLayout->addWidget(
        m_pFadeInButton);

    pLayout->addWidget(
        m_pFadeOutButton);

    pLayout->addWidget(
        m_pApplyGainButton);

    pLayout->addWidget(
        m_pUndoButton);

    pLayout->addWidget(
        m_pRedoButton);

    pLayout->addWidget(
        m_pThemeButton);

    connect(
        m_pOpenButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnOpenRequested);

    connect(
        m_pSaveButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnSaveRequested);

    connect(
        m_pExportButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnExportRequested);

    connect(
        m_pTrimButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnTrimRequested);

    connect(
        m_pCutButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnCutRequested);

    connect(
        m_pFadeInButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnFadeInRequested);

    connect(
        m_pFadeOutButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnFadeOutRequested);

    connect(
        m_pApplyGainButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnApplyGainRequested);

    connect(
        m_pUndoButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnUndoRequested);

    connect(
        m_pRedoButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnRedoRequested);

    connect(
        m_pThemeButton,
        &QPushButton::clicked,
        this,
        &EditorHeaderWidget::fnThemeToggleRequested);
}

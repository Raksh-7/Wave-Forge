#include "ThemeManager.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>

ThemeManager::ThemeManager(QObject* pParent)
    : QObject(pParent)
{
    m_QsCurrentTheme = QStringLiteral("Dark");
}

void ThemeManager::fnApplyDarkTheme()
{
    QApplication* pApplication = qobject_cast<QApplication*>(QApplication::instance());

    if (pApplication == nullptr)
    {
        return;
    }

    QPalette clPalette = pApplication->palette();

    clPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    clPalette.setColor(QPalette::WindowText, QColor(235, 235, 235));
    clPalette.setColor(QPalette::Base, QColor(20, 20, 20));
    clPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    clPalette.setColor(QPalette::Text, QColor(235, 235, 235));
    clPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    clPalette.setColor(QPalette::ButtonText, QColor(235, 235, 235));
    clPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    clPalette.setColor(QPalette::Highlight, QColor(70, 120, 200));
    clPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pApplication->setPalette(clPalette);

    m_QsCurrentTheme = QStringLiteral("Dark");

    emit fnThemeChanged(m_QsCurrentTheme);
}

void ThemeManager::fnApplyLightTheme()
{
    QApplication* pApplication = qobject_cast<QApplication*>(QApplication::instance());

    if (pApplication == nullptr)
    {
        return;
    }

    QPalette clPalette = pApplication->style()->standardPalette();

    pApplication->setPalette(clPalette);

    m_QsCurrentTheme = QStringLiteral("Light");

    emit fnThemeChanged(m_QsCurrentTheme);
}

void ThemeManager::fnApplyTheme(const QString& QsThemeName)
{
    if (QsThemeName.compare(QStringLiteral("Dark"), Qt::CaseInsensitive) == 0)
    {
        fnApplyDarkTheme();
    }
    else if (QsThemeName.compare(QStringLiteral("Light"), Qt::CaseInsensitive) == 0)
    {
        fnApplyLightTheme();
    }
}

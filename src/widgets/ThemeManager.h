#pragma once

#include <QObject>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject* pParent = nullptr);

    bool fnIsDarkTheme() const { return m_QsCurrentTheme.compare(QStringLiteral("Dark"), Qt::CaseInsensitive) == 0; }

    void fnApplyDarkTheme();
    void fnApplyLightTheme();

    void fnApplyTheme(const QString& QsThemeName);

signals:
    void fnThemeChanged(const QString& QsThemeName);

private:
    QString m_QsCurrentTheme;
};

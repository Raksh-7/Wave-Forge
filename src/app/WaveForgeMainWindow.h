#pragma once

#include "../core/AudioEditorDocument.h"

#include <QHash>
#include <QMainWindow>
#include <QVector>

class QAudioOutput;
class QLabel;
class QMediaPlayer;
class QScrollArea;
class QTimer;
class QVBoxLayout;
class QWidget;

class EditorHeaderWidget;
class Knob;
class LevelMeter;
class RangeSlider;
class ThemeManager;
class TimelineRuler;
class Toast;
class TransportBar;
class WaveformView;
class ZoomControlWidget;

class WaveForgeMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaveForgeMainWindow(
        QWidget* pParent = nullptr);

private:
    using PeakLevelMap =
        QHash<int, QVector<float>>;

    void fnCreateMainLayout();
    void fnCreateEditorWidgets();
    void fnCreateAudioPlayback();
    void fnConnectSignals();

    void fnOpenAudioFile();
    void fnSaveAudioFile();
    void fnExportAudioFile();

    void fnPlayAudio();
    void fnPauseAudio();
    void fnStopAudio();
    void fnSeekAudio(
        double dTimeSeconds);

    void fnTrimSelection();
    void fnCutSelection();
    void fnFadeInSelection();
    void fnFadeOutSelection();
    void fnApplyGain();

    void fnUndo();
    void fnRedo();
    void fnToggleTheme();

    bool fnGetSelectedSampleRange(
        int& iStartSample,
        int& iEndSample) const;

    bool fnPreparePlaybackSource();

    void fnStartPeakGeneration();
    void fnUpdateWaveform();

    void fnApplyZoomFactor(
        double dZoomFactor);

    void fnUpdateSelectionFromRangeSlider(
        double dStartSeconds,
        double dEndSeconds);

    void fnUpdatePlaybackPosition(
        double dTimeSeconds);

    void fnUpdateLevelMeter();

    void fnUpdateSelectionLabel();

    void fnUpdatePeakDisplay();

    const QVector<float>&
        fnSelectPeakLevel() const;

private:
    QWidget* m_pCentralWidget = nullptr;

    QVBoxLayout* m_pMainLayout = nullptr;

    QWidget* m_pWaveformContentWidget = nullptr;

    QVBoxLayout*
        m_pWaveformContentLayout = nullptr;

    EditorHeaderWidget*
        m_pEditorHeaderWidget = nullptr;

    WaveformView*
        m_pWaveformView = nullptr;

    TimelineRuler*
        m_pTimelineRuler = nullptr;

    RangeSlider*
        m_pSelectionRangeSlider = nullptr;

    TransportBar*
        m_pTransportBar = nullptr;

    Knob*
        m_pVolumeKnob = nullptr;

    Knob*
        m_pGainKnob = nullptr;

    LevelMeter*
        m_pLevelMeter = nullptr;

    ZoomControlWidget*
        m_pZoomControlWidget = nullptr;

    QLabel*
        m_pSelectionLabel = nullptr;

    QLabel*
        m_pVolumeLabel = nullptr;

    QLabel*
        m_pGainLabel = nullptr;

    QLabel*
        m_pZoomLabel = nullptr;

    QScrollArea*
        m_pWaveformScrollArea = nullptr;

    Toast*
        m_pToast = nullptr;

    QMediaPlayer*
        m_pMediaPlayer = nullptr;

    QAudioOutput*
        m_pAudioOutput = nullptr;

    QTimer*
        m_pLevelMeterTimer = nullptr;

    ThemeManager*
        m_pThemeManager = nullptr;

    AudioEditorDocument
        m_audioEditorDocument;

    PeakLevelMap
        m_aPeakLevels;

    quint64
        m_nPeakGenerationId = 0;

    QString
        m_QsPlaybackFilePath;

    double
        m_dZoomFactor = 1.0;
};

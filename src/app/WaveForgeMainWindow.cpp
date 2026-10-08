#include "WaveForgeMainWindow.h"

#include "../core/WaveformPeakGenerator.h"
#include "../widgets/EditorHeaderWidget.h"
#include "../widgets/Knob.h"
#include "../widgets/LevelMeter.h"
#include "../widgets/RangeSlider.h"
#include "../widgets/ThemeManager.h"
#include "../widgets/TimelineRuler.h"
#include "../widgets/Toast.h"
#include "../widgets/TransportBar.h"
#include "../widgets/WaveformView.h"
#include "../widgets/ZoomControlWidget.h"

#include <QAudioOutput>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QTimer>
#include <QUrl>
#include <QUndoStack>
#include <QVBoxLayout>

#include <QtConcurrent/QtConcurrentRun>

#include <cmath>
#include <limits>

WaveForgeMainWindow::WaveForgeMainWindow(
    QWidget* pParent)
    : QMainWindow(pParent)
{
    setWindowTitle(
        QStringLiteral("WaveForge"));

    resize(
        1300,
        800);

    fnCreateMainLayout();
    fnCreateEditorWidgets();
    fnCreateAudioPlayback();
    fnConnectSignals();

    m_pThemeManager =
        new ThemeManager(this);

    /*
     * Your current ThemeManager API takes
     * no QApplication argument.
     */
    m_pThemeManager->
        fnApplyDarkTheme();
}

void WaveForgeMainWindow::fnCreateMainLayout()
{
    m_pCentralWidget =
        new QWidget(this);

    m_pMainLayout =
        new QVBoxLayout(
            m_pCentralWidget);

    m_pMainLayout->
        setContentsMargins(
            0,
            0,
            0,
            0);

    m_pMainLayout->
        setSpacing(0);

    setCentralWidget(
        m_pCentralWidget);
}

void WaveForgeMainWindow::fnCreateEditorWidgets()
{
    m_pEditorHeaderWidget =
        new EditorHeaderWidget(
            m_pCentralWidget);

    m_pWaveformContentWidget =
        new QWidget(
            m_pCentralWidget);

    m_pWaveformContentLayout =
        new QVBoxLayout(
            m_pWaveformContentWidget);

    m_pWaveformContentLayout->
        setContentsMargins(
            0,
            0,
            0,
            0);

    m_pWaveformContentLayout->
        setSpacing(0);

    m_pTimelineRuler =
        new TimelineRuler(
            m_pWaveformContentWidget);

    m_pWaveformView =
        new WaveformView(
            m_pWaveformContentWidget);

    m_pWaveformContentLayout->
        addWidget(
            m_pTimelineRuler);

    m_pWaveformContentLayout->
        addWidget(
            m_pWaveformView,
            1);

    m_pWaveformScrollArea =
        new QScrollArea(
            m_pCentralWidget);

    m_pWaveformScrollArea->
        setWidget(
            m_pWaveformContentWidget);

    m_pWaveformScrollArea->
        setWidgetResizable(false);

    m_pWaveformScrollArea->
        setHorizontalScrollBarPolicy(
            Qt::ScrollBarAsNeeded);

    m_pWaveformScrollArea->
        setVerticalScrollBarPolicy(
            Qt::ScrollBarAlwaysOff);

    m_pSelectionRangeSlider =
        new RangeSlider(
            m_pCentralWidget);

    m_pTransportBar =
        new TransportBar(
            m_pCentralWidget);

    m_pVolumeKnob =
        new Knob(
            m_pCentralWidget);

    m_pGainKnob =
        new Knob(
            m_pCentralWidget);

    m_pLevelMeter =
        new LevelMeter(
            m_pCentralWidget);

    m_pZoomControlWidget =
        new ZoomControlWidget(
            m_pCentralWidget);

    m_pSelectionLabel =
        new QLabel(
            QStringLiteral(
                "Selection: none"),
            m_pCentralWidget);

    m_pVolumeLabel =
        new QLabel(
            QStringLiteral("Volume"),
            m_pCentralWidget);

    m_pGainLabel =
        new QLabel(
            QStringLiteral("Gain"),
            m_pCentralWidget);

    m_pZoomLabel =
        new QLabel(
            QStringLiteral("View"),
            m_pCentralWidget);

    m_pToast =
        new Toast(this);

    /*
     * Gain starts at 100%.
     */
    m_pGainKnob->
        fnSetValue(100);

    auto* pSelectionLayout =
        new QHBoxLayout();

    pSelectionLayout->
        setContentsMargins(
            8,
            2,
            8,
            2);

    pSelectionLayout->
        addWidget(
            m_pSelectionLabel);

    pSelectionLayout->
        addWidget(
            m_pSelectionRangeSlider,
            1);

    auto* pControlLayout =
        new QHBoxLayout();

    pControlLayout->
        setContentsMargins(
            8,
            6,
            8,
            6);

    pControlLayout->
        setSpacing(8);

    pControlLayout->
        addStretch();

    pControlLayout->
        addWidget(
            m_pVolumeLabel);

    pControlLayout->
        addWidget(
            m_pVolumeKnob);

    pControlLayout->
        addWidget(
            m_pGainLabel);

    pControlLayout->
        addWidget(
            m_pGainKnob);

    pControlLayout->
        addWidget(
            m_pLevelMeter);

    pControlLayout->
        addWidget(
            m_pZoomLabel);

    pControlLayout->
        addWidget(
            m_pZoomControlWidget);

    m_pMainLayout->
        addWidget(
            m_pEditorHeaderWidget);

    m_pMainLayout->
        addWidget(
            m_pWaveformScrollArea,
            1);

    m_pMainLayout->
        addLayout(
            pSelectionLayout);

    m_pMainLayout->
        addWidget(
            m_pTransportBar);

    m_pMainLayout->
        addLayout(
            pControlLayout);
}

void WaveForgeMainWindow::fnCreateAudioPlayback()
{
    m_pAudioOutput =
        new QAudioOutput(this);

    m_pMediaPlayer =
        new QMediaPlayer(this);

    m_pMediaPlayer->
        setAudioOutput(
            m_pAudioOutput);

    m_pAudioOutput->
        setVolume(1.0);

    m_pLevelMeterTimer =
        new QTimer(this);

    m_pLevelMeterTimer->
        setInterval(33);
}

void WaveForgeMainWindow::fnConnectSignals()
{
    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnOpenRequested,
        this,
        &WaveForgeMainWindow::
        fnOpenAudioFile);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnSaveRequested,
        this,
        &WaveForgeMainWindow::
        fnSaveAudioFile);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnExportRequested,
        this,
        &WaveForgeMainWindow::
        fnExportAudioFile);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnTrimRequested,
        this,
        &WaveForgeMainWindow::
        fnTrimSelection);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnCutRequested,
        this,
        &WaveForgeMainWindow::
        fnCutSelection);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnFadeInRequested,
        this,
        &WaveForgeMainWindow::
        fnFadeInSelection);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnFadeOutRequested,
        this,
        &WaveForgeMainWindow::
        fnFadeOutSelection);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnApplyGainRequested,
        this,
        &WaveForgeMainWindow::
        fnApplyGain);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnUndoRequested,
        this,
        &WaveForgeMainWindow::
        fnUndo);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnRedoRequested,
        this,
        &WaveForgeMainWindow::
        fnRedo);

    connect(
        m_pEditorHeaderWidget,
        &EditorHeaderWidget::
        fnThemeToggleRequested,
        this,
        &WaveForgeMainWindow::
        fnToggleTheme);

    connect(
        m_pTransportBar,
        &TransportBar::
        fnPlayRequested,
        this,
        &WaveForgeMainWindow::
        fnPlayAudio);

    connect(
        m_pTransportBar,
        &TransportBar::
        fnPauseRequested,
        this,
        &WaveForgeMainWindow::
        fnPauseAudio);

    connect(
        m_pTransportBar,
        &TransportBar::
        fnStopRequested,
        this,
        &WaveForgeMainWindow::
        fnStopAudio);

    connect(
        m_pWaveformView,
        &WaveformView::
        fnSeekRequested,
        this,
        &WaveForgeMainWindow::
        fnSeekAudio);

    connect(
        m_pTimelineRuler,
        &TimelineRuler::
        fnSeekRequested,
        this,
        &WaveForgeMainWindow::
        fnSeekAudio);

    /*
     * Waveform -> RangeSlider
     */
    connect(
        m_pWaveformView,
        &WaveformView::
        fnSelectionChanged,
        this,
        [this](
            double dStartSeconds,
            double dEndSeconds)
        {
            fnUpdateSelectionLabel();

            QSignalBlocker clBlocker(
                m_pSelectionRangeSlider);

            m_pSelectionRangeSlider->
                fnSetValues(
                    dStartSeconds,
                    dEndSeconds);
        });

    /*
     * RangeSlider -> Waveform
     */
    connect(
        m_pSelectionRangeSlider,
        &RangeSlider::
        fnRangeChanged,
        this,
        &WaveForgeMainWindow::
        fnUpdateSelectionFromRangeSlider);

    /*
     * Zoom control -> waveform area
     */
    connect(
        m_pZoomControlWidget,
        &ZoomControlWidget::
        fnZoomFactorChanged,
        this,
        &WaveForgeMainWindow::
        fnApplyZoomFactor);

    /*
     * Media position -> playhead.
     */
    connect(
        m_pMediaPlayer,
        &QMediaPlayer::
        positionChanged,
        this,
        [this](qint64 nPosition)
        {
            fnUpdatePlaybackPosition(
                static_cast<double>(
                    nPosition) /
                1000.0);
        });

    connect(
        m_pMediaPlayer,
        &QMediaPlayer::
        durationChanged,
        this,
        [this](qint64 nDuration)
        {
            const double dDuration =
                static_cast<double>(
                    nDuration) /
                1000.0;

            m_pTimelineRuler->
                fnSetDurationSeconds(
                    dDuration);

            m_pTransportBar->
                fnSetDuration(
                    dDuration);
        });

    connect(
        m_pMediaPlayer,
        &QMediaPlayer::
        mediaStatusChanged,
        this,
        [this](
            QMediaPlayer::MediaStatus eStatus)
        {
            if (eStatus ==
                QMediaPlayer::EndOfMedia)
            {
                fnStopAudio();
            }
        });

    connect(
        m_pMediaPlayer,
        &QMediaPlayer::
        errorOccurred,
        this,
        [this](
            QMediaPlayer::Error,
            const QString& QsError)
        {
            if (!QsError.isEmpty())
            {
                m_pToast->
                    fnShowMessage(
                        QsError);
            }
        });

    connect(
        m_pVolumeKnob,
        &Knob::
        fnValueChanged,
        this,
        [this](int iValue)
        {
            m_pAudioOutput->
                setVolume(
                    qBound(
                        0.0,
                        static_cast<double>(
                            iValue) /
                        100.0,
                        1.0));
        });

    connect(
        m_pLevelMeterTimer,
        &QTimer::timeout,
        this,
        &WaveForgeMainWindow::
        fnUpdateLevelMeter);
}

void WaveForgeMainWindow::fnOpenAudioFile()
{
    const QString QsFilePath =
        QFileDialog::getOpenFileName(
            this,
            QStringLiteral(
                "Open WAV File"),
            QString(),
            QStringLiteral(
                "WAV Files (*.wav);;"
                "All Files (*)"));

    if (QsFilePath.isEmpty())
    {
        return;
    }

    QString QsErrorMessage;

    if (!m_audioEditorDocument.fnOpenFile(
        QsFilePath,
        QsErrorMessage))
    {
        QMessageBox::critical(
            this,
            QStringLiteral(
                "Open WAV"),
            QsErrorMessage);

        return;
    }

    m_pMediaPlayer->stop();
    m_pLevelMeterTimer->stop();

    m_pLevelMeter->
        fnSetLevel(0.0f);

    m_QsPlaybackFilePath =
        QsFilePath;

    m_pMediaPlayer->
        setSource(
            QUrl::fromLocalFile(
                QsFilePath));

    m_pWaveformView->
        fnClearSelection();

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();

    m_pSelectionRangeSlider->
        fnSetValues(
            0.0,
            0.0);

    fnStartPeakGeneration();

    setWindowTitle(
        QStringLiteral(
            "WaveForge - %1")
        .arg(
            QFileInfo(
                QsFilePath)
            .fileName()));

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "WAV loaded successfully"));
}

void WaveForgeMainWindow::fnSaveAudioFile()
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Open a WAV file first"));

        return;
    }

    const QString QsFilePath =
        QFileDialog::getSaveFileName(
            this,
            QStringLiteral(
                "Save WAV File"),
            QString(),
            QStringLiteral(
                "WAV Files (*.wav)"));

    if (QsFilePath.isEmpty())
    {
        return;
    }

    QString QsErrorMessage;

    if (!m_audioEditorDocument.fnSaveFile(
        QsFilePath,
        QsErrorMessage))
    {
        QMessageBox::critical(
            this,
            QStringLiteral(
                "Save WAV"),
            QsErrorMessage);

        return;
    }

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "WAV saved successfully"));
}

void WaveForgeMainWindow::fnExportAudioFile()
{
    fnSaveAudioFile();
}

bool WaveForgeMainWindow::fnPreparePlaybackSource()
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        return false;
    }

    const QString QsTempFile =
        QDir::tempPath() +
        QStringLiteral(
            "/WaveForge_PlaybackPreview.wav");

    QString QsErrorMessage;

    if (!m_audioEditorDocument.fnSaveFile(
        QsTempFile,
        QsErrorMessage))
    {
        m_pToast->
            fnShowMessage(
                QsErrorMessage);

        return false;
    }

    m_QsPlaybackFilePath =
        QsTempFile;

    m_pMediaPlayer->
        setSource(
            QUrl::fromLocalFile(
                QsTempFile));

    return true;
}

void WaveForgeMainWindow::fnPlayAudio()
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Open a WAV file first"));

        return;
    }

    if (m_pMediaPlayer->
        source()
        .isEmpty() ||
        m_QsPlaybackFilePath
        .contains(
            QStringLiteral(
                "WaveForge_PlaybackPreview")))
    {
        if (!fnPreparePlaybackSource())
        {
            return;
        }
    }

    m_pMediaPlayer->play();

    m_pLevelMeterTimer->
        start();
}

void WaveForgeMainWindow::fnPauseAudio()
{
    m_pMediaPlayer->pause();

    m_pLevelMeterTimer->
        stop();
}

void WaveForgeMainWindow::fnStopAudio()
{
    m_pMediaPlayer->stop();

    m_pLevelMeterTimer->
        stop();

    m_pLevelMeter->
        fnSetLevel(0.0f);

    fnUpdatePlaybackPosition(
        0.0);
}

void WaveForgeMainWindow::fnSeekAudio(
    double dTimeSeconds)
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        return;
    }

    m_pMediaPlayer->
        setPosition(
            static_cast<qint64>(
                dTimeSeconds *
                1000.0));
}

bool WaveForgeMainWindow::
fnGetSelectedSampleRange(
    int& iStartSample,
    int& iEndSample) const
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        return false;
    }

    const double dStart =
        m_pWaveformView->
        fnGetSelectionStart();

    const double dEnd =
        m_pWaveformView->
        fnGetSelectionEnd();

    if (dEnd <= dStart)
    {
        return false;
    }

    const AudioBuffer& clBuffer =
        m_audioEditorDocument.
        fnGetAudioBuffer();

    const int iRate =
        clBuffer.fnGetSampleRate();

    const int iCount =
        clBuffer.fnGetSampleCount();

    iStartSample =
        qBound(
            0,
            static_cast<int>(
                dStart *
                iRate),
            iCount);

    iEndSample =
        qBound(
            0,
            static_cast<int>(
                dEnd *
                iRate),
            iCount);

    return iStartSample <
        iEndSample;
}

void WaveForgeMainWindow::fnTrimSelection()
{
    int iStart = 0;
    int iEnd = 0;

    if (!fnGetSelectedSampleRange(
        iStart,
        iEnd))
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Select a region first"));

        return;
    }

    fnStopAudio();

    if (!m_audioEditorDocument.fnTrim(
        iStart,
        iEnd))
    {
        return;
    }

    m_pWaveformView->
        fnClearSelection();

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Selection trimmed"));
}

void WaveForgeMainWindow::fnCutSelection()
{
    int iStart = 0;
    int iEnd = 0;

    if (!fnGetSelectedSampleRange(
        iStart,
        iEnd))
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Select a region first"));

        return;
    }

    fnStopAudio();

    if (!m_audioEditorDocument.fnCut(
        iStart,
        iEnd))
    {
        return;
    }

    m_pWaveformView->
        fnClearSelection();

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Selection cut"));
}

void WaveForgeMainWindow::fnFadeInSelection()
{
    int iStart = 0;
    int iEnd = 0;

    if (!fnGetSelectedSampleRange(
        iStart,
        iEnd))
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Select a region first"));

        return;
    }

    fnStopAudio();

    if (!m_audioEditorDocument.fnApplyFade(
        iStart,
        iEnd,
        true))
    {
        return;
    }

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Fade in applied"));
}

void WaveForgeMainWindow::fnFadeOutSelection()
{
    int iStart = 0;
    int iEnd = 0;

    if (!fnGetSelectedSampleRange(
        iStart,
        iEnd))
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Select a region first"));

        return;
    }

    fnStopAudio();

    if (!m_audioEditorDocument.fnApplyFade(
        iStart,
        iEnd,
        false))
    {
        return;
    }

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Fade out applied"));
}

void WaveForgeMainWindow::fnApplyGain()
{
    int iStart = 0;
    int iEnd = 0;

    if (!fnGetSelectedSampleRange(
        iStart,
        iEnd))
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Select a region first"));

        return;
    }

    const float fGain =
        static_cast<float>(
            m_pGainKnob->
            fnGetValue()) /
        100.0f;

    fnStopAudio();

    if (!m_audioEditorDocument.fnApplyGain(
        iStart,
        iEnd,
        fGain))
    {
        return;
    }

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Gain applied"));
}

void WaveForgeMainWindow::fnUndo()
{
    QUndoStack* pUndoStack =
        m_audioEditorDocument.
        fnGetUndoStack();

    if (!pUndoStack ||
        !pUndoStack->canUndo())
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Nothing to undo"));

        return;
    }

    fnStopAudio();

    pUndoStack->undo();

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Undo"));
}

void WaveForgeMainWindow::fnRedo()
{
    QUndoStack* pUndoStack =
        m_audioEditorDocument.
        fnGetUndoStack();

    if (!pUndoStack ||
        !pUndoStack->canRedo())
    {
        m_pToast->
            fnShowMessage(
                QStringLiteral(
                    "Nothing to redo"));

        return;
    }

    fnStopAudio();

    pUndoStack->redo();

    m_aPeakLevels.clear();

    ++m_nPeakGenerationId;

    fnUpdateWaveform();
    fnStartPeakGeneration();
    fnPreparePlaybackSource();

    m_pToast->
        fnShowMessage(
            QStringLiteral(
                "Redo"));
}

void WaveForgeMainWindow::fnToggleTheme()
{
    if (m_pThemeManager->
        fnIsDarkTheme())
    {
        m_pThemeManager->
            fnApplyLightTheme();
    }
    else
    {
        m_pThemeManager->
            fnApplyDarkTheme();
    }
}

void WaveForgeMainWindow::fnStartPeakGeneration()
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        return;
    }

    const AudioBuffer clBuffer =
        m_audioEditorDocument.
        fnGetAudioBuffer();

    const quint64 nGenerationId =
        m_nPeakGenerationId;

    const QVector<int>
        aRequestedPeakCounts =
    {
        512,
        2048,
        8192,
        32768,
        65536
    };

    auto* pWatcher =
        new QFutureWatcher<
        QPair<
        quint64,
        PeakLevelMap>>(this);

    connect(
        pWatcher,
        &QFutureWatcher<
        QPair<
        quint64,
        PeakLevelMap>>::
        finished,
        this,
        [this, pWatcher]()
        {
            const auto clResult =
                pWatcher->result();

            pWatcher->deleteLater();

            if (clResult.first !=
                m_nPeakGenerationId)
            {
                return;
            }

            m_aPeakLevels =
                clResult.second;

            fnUpdatePeakDisplay();
        });

    pWatcher->setFuture(
        QtConcurrent::run(
            [clBuffer,
            nGenerationId,
            aRequestedPeakCounts]()
            {
                return qMakePair(
                    nGenerationId,
                    WaveformPeakGenerator::
                    fnGeneratePeakLevels(
                        clBuffer,
                        aRequestedPeakCounts));
            }));
}

void WaveForgeMainWindow::fnUpdateWaveform()
{
    const AudioBuffer& clBuffer =
        m_audioEditorDocument.
        fnGetAudioBuffer();

    const int iSampleCount =
        clBuffer.fnGetSampleCount();

    const int iChannelCount =
        clBuffer.fnGetChannelCount();

    if (iSampleCount <= 0 ||
        iChannelCount <= 0)
    {
        m_pWaveformView->
            fnClear();

        m_pTimelineRuler->
            fnSetDurationSeconds(0.0);

        m_pTransportBar->
            fnSetDuration(0.0);

        m_pSelectionRangeSlider->
            fnSetRange(
                0.0,
                1.0);

        m_pSelectionRangeSlider->
            fnSetValues(
                0.0,
                0.0);

        return;
    }

    const double dDuration =
        clBuffer.fnGetDurationSeconds();

    /*
     * WaveformView uses the entire audio
     * duration for its coordinate system.
     * The actual zoom is achieved by
     * increasing the content width.
     */
    m_pWaveformView->
        fnSetDuration(
            dDuration);

    m_pTimelineRuler->
        fnSetDurationSeconds(
            dDuration);

    m_pTransportBar->
        fnSetDuration(
            dDuration);

    {
        QSignalBlocker clBlocker(
            m_pSelectionRangeSlider);

        m_pSelectionRangeSlider->
            fnSetRange(
                0.0,
                dDuration);
    }

    fnApplyZoomFactor(
        m_dZoomFactor);

    fnUpdateSelectionLabel();
}

void WaveForgeMainWindow::fnApplyZoomFactor(
    double dZoomFactor)
{
    m_dZoomFactor =
        qBound(
            0.5,
            dZoomFactor,
            8.0);

    m_pWaveformView->
        fnSetZoomFactor(
            m_dZoomFactor);

    if (!m_pWaveformScrollArea ||
        !m_pWaveformContentWidget ||
        !m_pWaveformView ||
        !m_pTimelineRuler)
    {
        return;
    }

    const int iViewportWidth =
        m_pWaveformScrollArea->
        viewport()->
        width();

    const int iBaseWidth =
        qMax(
            900,
            iViewportWidth);

    const int iContentWidth =
        qMax(
            iViewportWidth,
            qRound(
                static_cast<double>(
                    iBaseWidth) *
                m_dZoomFactor));

    const int iContentHeight =
        qMax(
            240,
            m_pWaveformScrollArea->
            viewport()->
            height());

    m_pWaveformContentWidget->
        setMinimumSize(
            iContentWidth,
            iContentHeight);

    m_pWaveformContentWidget->
        resize(
            iContentWidth,
            iContentHeight);

    m_pWaveformView->
        setMinimumSize(
            iContentWidth,
            iContentHeight -
            m_pTimelineRuler->
            sizeHint()
            .height());

    m_pWaveformView->
        resize(
            iContentWidth,
            iContentHeight -
            m_pTimelineRuler->
            sizeHint()
            .height());

    m_pTimelineRuler->
        setMinimumWidth(
            iContentWidth);

    m_pTimelineRuler->
        resize(
            iContentWidth,
            m_pTimelineRuler->
            sizeHint()
            .height());

    fnUpdatePeakDisplay();
}

void WaveForgeMainWindow::
fnUpdateSelectionFromRangeSlider(
    double dStartSeconds,
    double dEndSeconds)
{
    /*
     * Prevent a zero-length slider range from
     * being interpreted as an actual selection.
     */
    m_pWaveformView->
        fnSetSelection(
            dStartSeconds,
            dEndSeconds);

    fnUpdateSelectionLabel();
}

void WaveForgeMainWindow::
fnUpdatePlaybackPosition(
    double dTimeSeconds)
{
    m_pWaveformView->
        fnSetPlayheadPosition(
            dTimeSeconds);

    m_pTimelineRuler->
        fnSetCurrentTimeSeconds(
            dTimeSeconds);

    m_pTransportBar->
        fnSetCurrentTime(
            dTimeSeconds);
}

void WaveForgeMainWindow::fnUpdateLevelMeter()
{
    if (!m_audioEditorDocument.fnHasAudio())
    {
        m_pLevelMeter->
            fnSetLevel(0.0f);

        return;
    }

    const AudioBuffer& clBuffer =
        m_audioEditorDocument.
        fnGetAudioBuffer();

    const int iSampleRate =
        clBuffer.fnGetSampleRate();

    const int iSampleCount =
        clBuffer.fnGetSampleCount();

    if (iSampleRate <= 0 ||
        iSampleCount <= 0)
    {
        m_pLevelMeter->
            fnSetLevel(0.0f);

        return;
    }

    const qint64 nCenterSample =
        static_cast<qint64>(
            m_pMediaPlayer->
            position()) *
        iSampleRate /
        1000;

    const int iWindowSize =
        qMax(
            128,
            iSampleRate / 100);

    const qint64 nStart =
        qMax(
            static_cast<qint64>(0),
            nCenterSample -
            iWindowSize / 2);

    const qint64 nEnd =
        qMin(
            static_cast<qint64>(
                iSampleCount),
            nStart +
            iWindowSize);

    float fPeak = 0.0f;

    for (int iChannel = 0;
        iChannel <
        clBuffer.fnGetChannelCount();
        ++iChannel)
    {
        const QVector<float>&
            aSamples =
            clBuffer.
            fnGetChannelSamples(
                iChannel);

        for (qint64 nSample = nStart;
            nSample < nEnd;
            ++nSample)
        {
            const qsizetype iIndex =
                static_cast<qsizetype>(
                    nSample);

            if (iIndex >=
                aSamples.size())
            {
                break;
            }

            fPeak =
                qMax(
                    fPeak,
                    std::abs(
                        aSamples.at(
                            iIndex)));
        }
    }

    m_pLevelMeter->
        fnSetLevel(
            qBound(
                0.0f,
                fPeak,
                1.0f));
}

void WaveForgeMainWindow::
fnUpdateSelectionLabel()
{
    const double dStart =
        m_pWaveformView->
        fnGetSelectionStart();

    const double dEnd =
        m_pWaveformView->
        fnGetSelectionEnd();

    if (dEnd <= dStart)
    {
        m_pSelectionLabel->
            setText(
                QStringLiteral(
                    "Selection: none"));

        return;
    }

    auto fnFormat =
        [](double dTime)
        {
            const int iMinutes =
                static_cast<int>(
                    dTime) /
                60;

            const int iSeconds =
                static_cast<int>(
                    dTime) %
                60;

            const int iMilliseconds =
                static_cast<int>(
                    (dTime -
                        static_cast<int>(
                            dTime)) *
                    1000.0);

            return QStringLiteral(
                "%1:%2.%3")
                .arg(
                    iMinutes,
                    2,
                    10,
                    QChar('0'))
                .arg(
                    iSeconds,
                    2,
                    10,
                    QChar('0'))
                .arg(
                    iMilliseconds,
                    3,
                    10,
                    QChar('0'));
        };

    m_pSelectionLabel->
        setText(
            QStringLiteral(
                "Selection: %1 - %2")
            .arg(
                fnFormat(dStart))
            .arg(
                fnFormat(dEnd)));
}

void WaveForgeMainWindow::
fnUpdatePeakDisplay()
{
    if (m_aPeakLevels.isEmpty())
    {
        m_pWaveformView->
            fnSetPeakLevels({});

        return;
    }

    m_pWaveformView->
        fnSetPeakLevels(
            m_aPeakLevels);
}

const QVector<float>&
WaveForgeMainWindow::
fnSelectPeakLevel() const
{
    static const QVector<float>
        aEmpty;

    if (m_aPeakLevels.isEmpty())
    {
        return aEmpty;
    }

    const int iDesiredPeakCount =
        qBound(
            512,
            m_pWaveformView->
            width() * 2,
            65536);

    int iSelectedKey = -1;

    int iSelectedDistance =
        std::numeric_limits<int>::max();

    for (auto iKey :
        m_aPeakLevels.keys())
    {
        const int iDistance =
            qAbs(
                iKey -
                iDesiredPeakCount);

        if (iDistance <
            iSelectedDistance)
        {
            iSelectedDistance =
                iDistance;

            iSelectedKey =
                iKey;
        }
    }

    if (iSelectedKey < 0)
    {
        return aEmpty;
    }

    return m_aPeakLevels.value(
        iSelectedKey);
}

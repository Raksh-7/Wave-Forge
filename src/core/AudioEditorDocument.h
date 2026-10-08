#pragma once

#include "AudioBuffer.h"
#include "WavFileReader.h"
#include "WavFileWriter.h"

#include <QList>
#include <QUndoStack>
#include <QObject>

class AudioEditorDocument : public QObject
{
    Q_OBJECT

public:
    explicit AudioEditorDocument(QObject* pParent = nullptr);

    bool fnOpenFile(
        const QString& QsFilePath,
        QString& QsErrorMessage);

    bool fnSaveFile(
        const QString& QsFilePath,
        QString& QsErrorMessage);

    bool fnHasAudio() const;

    const AudioBuffer& fnGetAudioBuffer() const;
    AudioBuffer& fnGetAudioBuffer();

    QUndoStack* fnGetUndoStack();

    bool fnTrim(
        int iStartSample,
        int iEndSample);

    bool fnCut(
        int iStartSample,
        int iEndSample);

    bool fnApplyGain(
        int iStartSample,
        int iEndSample,
        float fGain);

    bool fnApplyFade(
        int iStartSample,
        int iEndSample,
        bool bFadeIn);

    void fnClearHistory();

signals:
    void fnAudioChanged();

private:
    AudioBuffer m_audioBuffer;
    WavFileReader m_wavFileReader;
    WavFileWriter m_wavFileWriter;
    QUndoStack m_undoStack;
};

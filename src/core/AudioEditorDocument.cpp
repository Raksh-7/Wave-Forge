#include "AudioEditorDocument.h"

#include <QUndoCommand>

namespace
{
class AudioSnapshotCommand final : public QUndoCommand
{
public:
    AudioSnapshotCommand(
        AudioEditorDocument* pDocument,
        const AudioBuffer& clBefore,
        const AudioBuffer& clAfter,
        const QString& QsText)
        : m_pDocument(pDocument),
          m_clBefore(clBefore),
          m_clAfter(clAfter)
    {
        setText(QsText);
    }

    void undo() override
    {
        m_pDocument->fnGetAudioBuffer() = m_clBefore;
        emit m_pDocument->fnAudioChanged();
    }

    void redo() override
    {
        m_pDocument->fnGetAudioBuffer() = m_clAfter;
        emit m_pDocument->fnAudioChanged();
    }

private:
    AudioEditorDocument* m_pDocument = nullptr;
    AudioBuffer m_clBefore;
    AudioBuffer m_clAfter;
};
}

AudioEditorDocument::AudioEditorDocument(QObject* pParent)
    : QObject(pParent)
{
}

bool AudioEditorDocument::fnOpenFile(
    const QString& QsFilePath,
    QString& QsErrorMessage)
{
    AudioBuffer clLoadedBuffer;

    if (!m_wavFileReader.fnReadFile(
            QsFilePath,
            clLoadedBuffer,
            QsErrorMessage)) {
        return false;
    }

    m_audioBuffer = clLoadedBuffer;
    m_undoStack.clear();

    emit fnAudioChanged();
    return true;
}

bool AudioEditorDocument::fnSaveFile(
    const QString& QsFilePath,
    QString& QsErrorMessage)
{
    return m_wavFileWriter.fnWriteFile(
        QsFilePath,
        m_audioBuffer,
        QsErrorMessage);
}

bool AudioEditorDocument::fnHasAudio() const
{
    return !m_audioBuffer.fnIsEmpty();
}

const AudioBuffer& AudioEditorDocument::fnGetAudioBuffer() const
{
    return m_audioBuffer;
}

AudioBuffer& AudioEditorDocument::fnGetAudioBuffer()
{
    return m_audioBuffer;
}

QUndoStack* AudioEditorDocument::fnGetUndoStack()
{
    return &m_undoStack;
}

bool AudioEditorDocument::fnTrim(
    int iStartSample,
    int iEndSample)
{
    AudioBuffer clBefore = m_audioBuffer;
    AudioBuffer clAfter = m_audioBuffer;

    if (!clAfter.fnTrim(
            iStartSample,
            iEndSample)) {
        return false;
    }

    m_undoStack.push(
        new AudioSnapshotCommand(
            this,
            clBefore,
            clAfter,
            "Trim Selection"));

    return true;
}

bool AudioEditorDocument::fnCut(
    int iStartSample,
    int iEndSample)
{
    AudioBuffer clBefore = m_audioBuffer;
    AudioBuffer clAfter = m_audioBuffer;

    if (!clAfter.fnCut(
            iStartSample,
            iEndSample)) {
        return false;
    }

    m_undoStack.push(
        new AudioSnapshotCommand(
            this,
            clBefore,
            clAfter,
            "Cut Selection"));

    return true;
}

bool AudioEditorDocument::fnApplyGain(
    int iStartSample,
    int iEndSample,
    float fGain)
{
    AudioBuffer clBefore = m_audioBuffer;
    AudioBuffer clAfter = m_audioBuffer;

    if (!clAfter.fnApplyGain(
            iStartSample,
            iEndSample,
            fGain)) {
        return false;
    }

    m_undoStack.push(
        new AudioSnapshotCommand(
            this,
            clBefore,
            clAfter,
            "Change Gain"));

    return true;
}

bool AudioEditorDocument::fnApplyFade(
    int iStartSample,
    int iEndSample,
    bool bFadeIn)
{
    AudioBuffer clBefore = m_audioBuffer;
    AudioBuffer clAfter = m_audioBuffer;

    if (!clAfter.fnApplyFade(
            iStartSample,
            iEndSample,
            bFadeIn)) {
        return false;
    }

    m_undoStack.push(
        new AudioSnapshotCommand(
            this,
            clBefore,
            clAfter,
            bFadeIn ? "Fade In" : "Fade Out"));

    return true;
}

void AudioEditorDocument::fnClearHistory()
{
    m_undoStack.clear();
}

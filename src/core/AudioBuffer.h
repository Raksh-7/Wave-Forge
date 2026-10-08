#pragma once

#include <QVector>

class AudioBuffer
{
public:
    AudioBuffer();

    void fnClear();

    bool fnIsEmpty() const;

    int fnGetSampleRate() const;
    int fnGetChannelCount() const;
    int fnGetSampleCount() const;

    double fnGetDurationSeconds() const;

    const QVector<float>& fnGetChannelSamples(int iChannel) const;
    QVector<float>& fnGetChannelSamples(int iChannel);

    void fnSetFormat(int iSampleRate, int iChannelCount);
    void fnResize(int iSampleCount);

    bool fnTrim(int iStartSample, int iEndSample);
    bool fnCut(int iStartSample, int iEndSample);
    bool fnApplyGain(int iStartSample, int iEndSample, float fGain);
    bool fnApplyFade(int iStartSample, int iEndSample, bool bFadeIn);

private:
    int m_iSampleRate = 44100;
    int m_iChannelCount = 2;
    QVector<QVector<float>> m_aChannelSamples;
};

#include "AudioBuffer.h"

#include <QtGlobal>
#include <algorithm>
#include <cmath>

AudioBuffer::AudioBuffer()
{
    fnSetFormat(44100, 2);
}

void AudioBuffer::fnClear()
{
    m_aChannelSamples.clear();
    m_iSampleRate = 44100;
    m_iChannelCount = 2;
}

bool AudioBuffer::fnIsEmpty() const
{
    return m_aChannelSamples.isEmpty() ||
           m_aChannelSamples.first().isEmpty();
}

int AudioBuffer::fnGetSampleRate() const
{
    return m_iSampleRate;
}

int AudioBuffer::fnGetChannelCount() const
{
    return m_iChannelCount;
}

int AudioBuffer::fnGetSampleCount() const
{
    if (m_aChannelSamples.isEmpty())
        return 0;
    return m_aChannelSamples.first().size();
}

double AudioBuffer::fnGetDurationSeconds() const
{
    if (m_iSampleRate <= 0)
        return 0.0;
    return static_cast<double>(fnGetSampleCount()) /
           static_cast<double>(m_iSampleRate);
}

const QVector<float>& AudioBuffer::fnGetChannelSamples(int iChannel) const
{
    static const QVector<float> aEmpty;
    if (iChannel < 0 || iChannel >= m_aChannelSamples.size())
        return aEmpty;
    return m_aChannelSamples.at(iChannel);
}

QVector<float>& AudioBuffer::fnGetChannelSamples(int iChannel)
{
    Q_ASSERT(iChannel >= 0 && iChannel < m_aChannelSamples.size());
    return m_aChannelSamples[iChannel];
}

void AudioBuffer::fnSetFormat(int iSampleRate, int iChannelCount)
{
    m_iSampleRate = qMax(1, iSampleRate);
    m_iChannelCount = qMax(1, iChannelCount);
    m_aChannelSamples.resize(m_iChannelCount);
}

void AudioBuffer::fnResize(int iSampleCount)
{
    iSampleCount = qMax(0, iSampleCount);
    for (QVector<float>& aChannel : m_aChannelSamples)
        aChannel.resize(iSampleCount);
}

bool AudioBuffer::fnTrim(int iStartSample, int iEndSample)
{
    const int iCount = fnGetSampleCount();
    if (iStartSample < 0 || iEndSample > iCount ||
        iStartSample >= iEndSample)
        return false;

    const int iNewCount = iEndSample - iStartSample;
    for (QVector<float>& aChannel : m_aChannelSamples) {
        const QVector<float> aTrimmed =
            aChannel.mid(iStartSample, iNewCount);
        aChannel = aTrimmed;
    }
    return true;
}

bool AudioBuffer::fnCut(int iStartSample, int iEndSample)
{
    const int iCount = fnGetSampleCount();
    if (iStartSample < 0 || iEndSample > iCount ||
        iStartSample >= iEndSample)
        return false;

    for (QVector<float>& aChannel : m_aChannelSamples)
        aChannel.remove(iStartSample, iEndSample - iStartSample);
    return true;
}

bool AudioBuffer::fnApplyGain(int iStartSample, int iEndSample, float fGain)
{
    const int iCount = fnGetSampleCount();
    if (iStartSample < 0 || iEndSample > iCount ||
        iStartSample >= iEndSample)
        return false;

    fGain = qBound(0.0f, fGain, 2.0f);

    for (QVector<float>& aChannel : m_aChannelSamples) {
        for (int i = iStartSample; i < iEndSample; ++i)
            aChannel[i] = qBound(-1.0f, aChannel[i] * fGain, 1.0f);
    }
    return true;
}

bool AudioBuffer::fnApplyFade(
    int iStartSample,
    int iEndSample,
    bool bFadeIn)
{
    const int iCount = fnGetSampleCount();
    if (iStartSample < 0 || iEndSample > iCount ||
        iStartSample >= iEndSample)
        return false;

    const int iLength = iEndSample - iStartSample;
    if (iLength <= 1)
        return true;

    for (QVector<float>& aChannel : m_aChannelSamples) {
        for (int i = 0; i < iLength; ++i) {
            const float fProgress =
                static_cast<float>(i) /
                static_cast<float>(iLength - 1);
            const float fGain =
                bFadeIn ? fProgress : (1.0f - fProgress);
            aChannel[iStartSample + i] *= fGain;
        }
    }
    return true;
}

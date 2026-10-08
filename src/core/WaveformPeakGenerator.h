#pragma once

#include "AudioBuffer.h"

#include <QHash>
#include <QVector>

class WaveformPeakGenerator
{
public:
    static QHash<int, QVector<float>> fnGeneratePeakLevels(
        const AudioBuffer& clAudioBuffer,
        const QVector<int>& aRequestedPeakCounts);

private:
    static QVector<float> fnGeneratePeaks(
        const QVector<QVector<float>>& aChannelSamples,
        int iPeakCount);
};

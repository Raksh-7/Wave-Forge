#include "WaveformPeakGenerator.h"

#include <QtGlobal>

#include <algorithm>
#include <cmath>

QHash<int, QVector<float>> WaveformPeakGenerator::fnGeneratePeakLevels(
    const AudioBuffer& clAudioBuffer,
    const QVector<int>& aRequestedPeakCounts)
{
    QHash<int, QVector<float>> aLevels;

    if (clAudioBuffer.fnIsEmpty())
    {
        return aLevels;
    }

    QVector<int> aPeakCounts = aRequestedPeakCounts;

    std::sort(
        aPeakCounts.begin(),
        aPeakCounts.end());

    aPeakCounts.erase(
        std::unique(
            aPeakCounts.begin(),
            aPeakCounts.end()),
        aPeakCounts.end());

    const int iSampleCount =
        clAudioBuffer.fnGetSampleCount();

    if (iSampleCount <= 0 || aPeakCounts.isEmpty())
    {
        return aLevels;
    }

    const int iHighestPeakCount =
        qMin(
            aPeakCounts.back(),
            iSampleCount);

    QVector<QVector<float>> aChannelSamples;

    const int iChannelCount =
        clAudioBuffer.fnGetChannelCount();

    aChannelSamples.reserve(iChannelCount);

    for (int iChannel = 0;
        iChannel < iChannelCount;
        ++iChannel)
    {
        aChannelSamples.append(
            clAudioBuffer.fnGetChannelSamples(iChannel));
    }

    const QVector<float> aHighestLevel =
        fnGeneratePeaks(
            aChannelSamples,
            iHighestPeakCount);

    for (const int iRequestedPeakCount : aPeakCounts)
    {
        const int iActualPeakCount =
            qMin(
                iRequestedPeakCount,
                iSampleCount);

        if (iActualPeakCount <= 0)
        {
            continue;
        }

        if (iActualPeakCount == iHighestPeakCount)
        {
            aLevels.insert(
                iRequestedPeakCount,
                aHighestLevel);

            continue;
        }

        QVector<float> aLevel;
        aLevel.resize(iActualPeakCount);

        for (int iPeak = 0;
            iPeak < iActualPeakCount;
            ++iPeak)
        {
            const qint64 nStart =
                static_cast<qint64>(iPeak) *
                aHighestLevel.size() /
                iActualPeakCount;

            const qint64 nEnd =
                static_cast<qint64>(iPeak + 1) *
                aHighestLevel.size() /
                iActualPeakCount;

            float fPeak = 0.0f;

            const qint64 nSafeEnd =
                qMax(
                    nStart + 1,
                    nEnd);

            for (qint64 nIndex = nStart;
                nIndex < nSafeEnd &&
                nIndex < aHighestLevel.size();
                ++nIndex)
            {
                fPeak = qMax(
                    fPeak,
                    aHighestLevel.at(
                        static_cast<qsizetype>(nIndex)));
            }

            aLevel[iPeak] = fPeak;
        }

        aLevels.insert(
            iRequestedPeakCount,
            aLevel);
    }

    return aLevels;
}

QVector<float> WaveformPeakGenerator::fnGeneratePeaks(
    const QVector<QVector<float>>& aChannelSamples,
    int iPeakCount)
{
    QVector<float> aPeaks;

    if (aChannelSamples.isEmpty() ||
        iPeakCount <= 0)
    {
        return aPeaks;
    }

    const int iSampleCount =
        aChannelSamples.first().size();

    if (iSampleCount <= 0)
    {
        return aPeaks;
    }

    const int iActualPeakCount =
        qMin(
            iPeakCount,
            iSampleCount);

    aPeaks.resize(iActualPeakCount);

    for (int iPeak = 0;
        iPeak < iActualPeakCount;
        ++iPeak)
    {
        const qint64 nStart =
            static_cast<qint64>(iPeak) *
            iSampleCount /
            iActualPeakCount;

        const qint64 nEnd =
            static_cast<qint64>(iPeak + 1) *
            iSampleCount /
            iActualPeakCount;

        float fPeak = 0.0f;

        const qint64 nSafeEnd =
            qMax(
                nStart + 1,
                nEnd);

        for (qint64 nSample = nStart;
            nSample < nSafeEnd &&
            nSample < iSampleCount;
            ++nSample)
        {
            const qsizetype iSampleIndex =
                static_cast<qsizetype>(nSample);

            for (const QVector<float>& aChannel :
                aChannelSamples)
            {
                if (iSampleIndex >= aChannel.size())
                {
                    continue;
                }

                fPeak = qMax(
                    fPeak,
                    std::abs(
                        aChannel.at(iSampleIndex)));
            }
        }

        aPeaks[iPeak] =
            qBound(
                0.0f,
                fPeak,
                1.0f);
    }

    return aPeaks;
}

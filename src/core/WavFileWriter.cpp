#include "WavFileWriter.h"

#include <QFile>
#include <QtEndian>
#include <QtGlobal>
#include <cmath>

static void fnAppendUInt16(
    QByteArray& aData,
    quint16 iValue)
{
    const quint16 iLittleEndian =
        qToLittleEndian(iValue);
    aData.append(
        reinterpret_cast<const char*>(&iLittleEndian),
        sizeof(iLittleEndian));
}

static void fnAppendUInt32(
    QByteArray& aData,
    quint32 iValue)
{
    const quint32 iLittleEndian =
        qToLittleEndian(iValue);
    aData.append(
        reinterpret_cast<const char*>(&iLittleEndian),
        sizeof(iLittleEndian));
}

bool WavFileWriter::fnWriteFile(
    const QString& QsFilePath,
    const AudioBuffer& clAudioBuffer,
    QString& QsErrorMessage) const
{
    if (clAudioBuffer.fnIsEmpty()) {
        QsErrorMessage = "There is no audio data to write.";
        return false;
    }

    const int iSampleRate =
        clAudioBuffer.fnGetSampleRate();
    const int iChannelCount =
        clAudioBuffer.fnGetChannelCount();
    const int iSampleCount =
        clAudioBuffer.fnGetSampleCount();

    if (iSampleRate <= 0 ||
        iChannelCount <= 0 ||
        iSampleCount <= 0) {
        QsErrorMessage = "The audio buffer has an invalid format.";
        return false;
    }

    constexpr quint16 iBitsPerSample = 16;
    const quint16 iBlockAlign =
        static_cast<quint16>(
            iChannelCount * 2);
    const quint32 nByteRate =
        static_cast<quint32>(
            iSampleRate * iBlockAlign);
    const quint32 nDataSize =
        static_cast<quint32>(
            static_cast<quint64>(iSampleCount) *
            iBlockAlign);

    QByteArray aData;
    aData.reserve(
        static_cast<int>(
            44 + nDataSize));

    aData.append("RIFF", 4);
    fnAppendUInt32(
        aData,
        36 + nDataSize);
    aData.append("WAVE", 4);

    aData.append("fmt ", 4);
    fnAppendUInt32(aData, 16);
    fnAppendUInt16(aData, 1);
    fnAppendUInt16(
        aData,
        static_cast<quint16>(iChannelCount));
    fnAppendUInt32(
        aData,
        static_cast<quint32>(iSampleRate));
    fnAppendUInt32(
        aData,
        nByteRate);
    fnAppendUInt16(
        aData,
        iBlockAlign);
    fnAppendUInt16(
        aData,
        iBitsPerSample);

    aData.append("data", 4);
    fnAppendUInt32(
        aData,
        nDataSize);

    for (int iSample = 0;
         iSample < iSampleCount;
         ++iSample) {
        for (int iChannel = 0;
             iChannel < iChannelCount;
             ++iChannel) {
            const float fSample =
                qBound(
                    -1.0f,
                    clAudioBuffer
                        .fnGetChannelSamples(iChannel)
                        .at(iSample),
                    1.0f);

            const qint16 iValue =
                static_cast<qint16>(
                    std::lround(
                        fSample * 32767.0f));

            const qint16 iLittleEndian =
                qToLittleEndian(iValue);

            aData.append(
                reinterpret_cast<const char*>(
                    &iLittleEndian),
                sizeof(iLittleEndian));
        }
    }

    QFile clFile(QsFilePath);
    if (!clFile.open(
            QIODevice::WriteOnly |
            QIODevice::Truncate)) {
        QsErrorMessage = clFile.errorString();
        return false;
    }

    if (clFile.write(aData) != aData.size()) {
        QsErrorMessage = clFile.errorString();
        return false;
    }

    return true;
}

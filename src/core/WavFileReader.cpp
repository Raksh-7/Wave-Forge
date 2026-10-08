#include "WavFileReader.h"

#include <QFile>
#include <QtEndian>
#include <cmath>
#include <cstring>

quint16 WavFileReader::fnReadUInt16(
    const QByteArray& aData,
    qsizetype nOffset)
{
    return qFromLittleEndian<quint16>(
        reinterpret_cast<const uchar*>(aData.constData() + nOffset));
}

quint32 WavFileReader::fnReadUInt32(
    const QByteArray& aData,
    qsizetype nOffset)
{
    return qFromLittleEndian<quint32>(
        reinterpret_cast<const uchar*>(aData.constData() + nOffset));
}

float WavFileReader::fnConvertSampleToFloat(
    const char* pSample,
    int iBitsPerSample)
{
    if (iBitsPerSample == 8) {
        const int iValue =
            static_cast<unsigned char>(*pSample) - 128;
        return static_cast<float>(iValue) / 128.0f;
    }

    if (iBitsPerSample == 16) {
        const qint16 iValue =
            qFromLittleEndian<qint16>(
                reinterpret_cast<const uchar*>(pSample));
        return static_cast<float>(iValue) / 32768.0f;
    }

    if (iBitsPerSample == 24) {
        const auto* p = reinterpret_cast<const uchar*>(pSample);
        qint32 iValue =
            static_cast<qint32>(p[0]) |
            (static_cast<qint32>(p[1]) << 8) |
            (static_cast<qint32>(p[2]) << 16);
        if (iValue & 0x00800000)
            iValue |= 0xFF000000;
        return static_cast<float>(iValue) / 8388608.0f;
    }

    if (iBitsPerSample == 32) {
        const qint32 iValue =
            qFromLittleEndian<qint32>(
                reinterpret_cast<const uchar*>(pSample));
        return static_cast<float>(iValue) / 2147483648.0f;
    }

    return 0.0f;
}

bool WavFileReader::fnReadFile(
    const QString& QsFilePath,
    AudioBuffer& clAudioBuffer,
    QString& QsErrorMessage) const
{
    QFile clFile(QsFilePath);
    if (!clFile.open(QIODevice::ReadOnly)) {
        QsErrorMessage = clFile.errorString();
        return false;
    }

    const QByteArray aData = clFile.readAll();
    if (aData.size() < 44 ||
        aData.mid(0, 4) != "RIFF" ||
        aData.mid(8, 4) != "WAVE") {
        QsErrorMessage = "The file is not a valid RIFF/WAVE file.";
        return false;
    }

    qsizetype nPosition = 12;
    quint16 iAudioFormat = 0;
    quint16 iChannelCount = 0;
    quint32 iSampleRate = 0;
    quint16 iBitsPerSample = 0;
    qsizetype nDataOffset = -1;
    quint32 nDataSize = 0;

    while (nPosition + 8 <= aData.size()) {
        const QByteArray aChunkId =
            aData.mid(nPosition, 4);
        const quint32 nChunkSize =
            fnReadUInt32(aData, nPosition + 4);
        const qsizetype nChunkData =
            nPosition + 8;

        if (nChunkData + nChunkSize > aData.size())
            break;

        if (aChunkId == "fmt ") {
            if (nChunkSize < 16) {
                QsErrorMessage = "Invalid WAV format chunk.";
                return false;
            }

            iAudioFormat =
                fnReadUInt16(aData, nChunkData);
            iChannelCount =
                fnReadUInt16(aData, nChunkData + 2);
            iSampleRate =
                fnReadUInt32(aData, nChunkData + 4);
            iBitsPerSample =
                fnReadUInt16(aData, nChunkData + 14);
        } else if (aChunkId == "data") {
            nDataOffset = nChunkData;
            nDataSize = nChunkSize;
        }

        nPosition = nChunkData + nChunkSize;
        if (nChunkSize & 1)
            ++nPosition;
    }

    if (iAudioFormat != 1 ||
        iChannelCount == 0 ||
        iSampleRate == 0 ||
        nDataOffset < 0 ||
        nDataSize == 0) {
        QsErrorMessage =
            "WaveForge currently supports uncompressed PCM WAV files.";
        return false;
    }

    if (iBitsPerSample != 8 &&
        iBitsPerSample != 16 &&
        iBitsPerSample != 24 &&
        iBitsPerSample != 32) {
        QsErrorMessage =
            "Unsupported PCM bit depth. Supported: 8, 16, 24 and 32 bit.";
        return false;
    }

    const int iBytesPerSample =
        iBitsPerSample / 8;
    const int iBlockAlign =
        iChannelCount * iBytesPerSample;

    if (iBlockAlign <= 0 ||
        nDataSize < static_cast<quint32>(iBlockAlign)) {
        QsErrorMessage = "Invalid WAV sample data.";
        return false;
    }

    const int iSampleCount =
        static_cast<int>(
            nDataSize /
            static_cast<quint32>(iBlockAlign));

    clAudioBuffer.fnClear();
    clAudioBuffer.fnSetFormat(
        static_cast<int>(iSampleRate),
        static_cast<int>(iChannelCount));
    clAudioBuffer.fnResize(iSampleCount);

    const char* pData =
        aData.constData() + nDataOffset;

    for (int iSample = 0;
         iSample < iSampleCount;
         ++iSample) {
        for (int iChannel = 0;
             iChannel < iChannelCount;
             ++iChannel) {
            const qsizetype nOffset =
                static_cast<qsizetype>(iSample) *
                    iBlockAlign +
                static_cast<qsizetype>(iChannel) *
                    iBytesPerSample;

            clAudioBuffer.fnGetChannelSamples(iChannel)[iSample] =
                qBound(
                    -1.0f,
                    fnConvertSampleToFloat(
                        pData + nOffset,
                        iBitsPerSample),
                    1.0f);
        }
    }

    return true;
}

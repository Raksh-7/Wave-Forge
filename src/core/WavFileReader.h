#pragma once

#include "AudioBuffer.h"

#include <QString>

class WavFileReader
{
public:
    bool fnReadFile(
        const QString& QsFilePath,
        AudioBuffer& clAudioBuffer,
        QString& QsErrorMessage) const;

private:
    static quint16 fnReadUInt16(
        const QByteArray& aData,
        qsizetype nOffset);

    static quint32 fnReadUInt32(
        const QByteArray& aData,
        qsizetype nOffset);

    static float fnConvertSampleToFloat(
        const char* pSample,
        int iBitsPerSample);
};

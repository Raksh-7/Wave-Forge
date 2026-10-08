#pragma once

#include "AudioBuffer.h"

#include <QString>

class WavFileWriter
{
public:
    bool fnWriteFile(
        const QString& QsFilePath,
        const AudioBuffer& clAudioBuffer,
        QString& QsErrorMessage) const;
};

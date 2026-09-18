#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <QString>

class ZipExtractor
{
public:
    bool extract(
        const QString &zipPath,
        const QString &destinationPath
    );
};

#endif
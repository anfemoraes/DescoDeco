#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <QString>
#include <QStringList>

class PdfGenerator
{
public:
    bool generate(
        const QStringList &files,
        const QString &outputPath
    );
};

#endif

#include "ZipExtractor.h"

#include <zip.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>

bool ZipExtractor::extract(
    const QString &zipPath,
    const QString &destinationPath
)
{
    int error = 0;

    zip_t *archive = zip_open(
        zipPath.toUtf8().constData(),
        0,
        &error
    );

    if (!archive)
    {
        return false;
    }

    zip_int64_t totalFiles =
        zip_get_num_entries(archive, 0);

    for (zip_int64_t i = 0; i < totalFiles; ++i)
    {
        const char *name =
            zip_get_name(archive, i, 0);

        if (!name)
        {
            continue;
        }

        QString fileName =
            QString::fromUtf8(name);

        QString outputPath =
            destinationPath + "/" + fileName;

        QFileInfo info(outputPath);

        // É uma pasta
        if (fileName.endsWith("/"))
        {
            QDir().mkpath(outputPath);
            continue;
        }

        QDir().mkpath(info.absolutePath());

        zip_file_t *file =
            zip_fopen_index(archive, i, 0);

        if (!file)
        {
            zip_close(archive);
            return false;
        }

        QFile outputFile(outputPath);

        if (!outputFile.open(QIODevice::WriteOnly))
        {
            zip_fclose(file);
            zip_close(archive);
            return false;
        }

        char buffer[4096];
        zip_int64_t bytesRead;

        while ((bytesRead = zip_fread(
            file,
            buffer,
            sizeof(buffer)
        )) > 0)
        {
            outputFile.write(buffer, bytesRead);
        }

        outputFile.close();
        zip_fclose(file);
    }

    zip_close(archive);

    return true;
}
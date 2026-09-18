#include "PdfGenerator.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFontMetrics>
#include <QImage>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryDir>

namespace
{
bool isImageFile(const QString &filePath)
{
    const QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp";
}

bool isPdfFile(const QString &filePath)
{
    const QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "pdf";
}

QString findPdfUnite()
{
#ifdef Q_OS_WIN
    const QString bundledPath = QCoreApplication::applicationDirPath()
        + QDir::separator() + "pdfunite.exe";
    if (QFileInfo::exists(bundledPath))
    {
        return bundledPath;
    }
#endif

    return QStandardPaths::findExecutable("pdfunite");
}

QVector<QString> wrapText(const QString &text, const QFontMetrics &fontMetrics, qreal maxWidth)
{
    QVector<QString> lines;

    if (text.isEmpty())
    {
        return lines;
    }

    const QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (words.isEmpty())
    {
        return lines;
    }

    QString currentLine;

    for (const QString &word : words)
    {
        if (fontMetrics.horizontalAdvance(word) > maxWidth && !currentLine.isEmpty())
        {
            lines << currentLine;
            currentLine.clear();
        }

        if (fontMetrics.horizontalAdvance(word) <= maxWidth)
        {
            const QString candidate = currentLine.isEmpty() ? word : currentLine + " " + word;
            if (fontMetrics.horizontalAdvance(candidate) <= maxWidth)
            {
                currentLine = candidate;
                continue;
            }
        }

        if (!currentLine.isEmpty())
        {
            lines << currentLine;
            currentLine.clear();
        }

        if (fontMetrics.horizontalAdvance(word) > maxWidth)
        {
            QString fragment;
            for (const QChar ch : word)
            {
                const QString trial = fragment + ch;
                if (!fragment.isEmpty() && fontMetrics.horizontalAdvance(trial) > maxWidth)
                {
                    lines << fragment;
                    fragment = ch;
                }
                else
                {
                    fragment = trial;
                }
            }

            if (!fragment.isEmpty())
            {
                currentLine = fragment;
            }
        }
        else
        {
            currentLine = word;
        }
    }

    if (!currentLine.isEmpty())
    {
        lines << currentLine;
    }

    return lines;
}

void renderTextContent(QPdfWriter &writer, QPainter &painter, const QString &text)
{
    const QRectF pageRect = QRectF(0, 0, writer.width(), writer.height());
    const qreal leftMargin = 40.0;
    const qreal rightMargin = 40.0;
    const qreal topMargin = 40.0;
    const qreal bottomMargin = 40.0;
    const qreal lineHeight = 18.0;
    const qreal maxWidth = pageRect.width() - leftMargin - rightMargin;
    const qreal maxHeight = pageRect.height() - topMargin - bottomMargin;

    const QVector<QString> lines = wrapText(text, painter.fontMetrics(), maxWidth);
    if (lines.isEmpty())
    {
        return;
    }

    qreal y = topMargin;
    for (const QString &line : lines)
    {
        if (y + lineHeight > maxHeight + topMargin)
        {
            writer.newPage();
            y = topMargin;
        }

        painter.drawText(
            QRectF(leftMargin, y, maxWidth, lineHeight),
            line
        );
        y += lineHeight + 2.0;
    }
}

void renderImageContent(QPdfWriter &writer, QPainter &painter, const QString &imagePath)
{
    QImage image(imagePath);
    if (image.isNull())
    {
        return;
    }

    const QRectF pageRect = QRectF(0, 0, writer.width(), writer.height());
    const qreal leftMargin = 40.0;
    const qreal rightMargin = 40.0;
    const qreal topMargin = 40.0;
    const qreal bottomMargin = 40.0;
    const qreal maxWidth = pageRect.width() - leftMargin - rightMargin;
    const qreal maxHeight = pageRect.height() - topMargin - bottomMargin;

    qreal scale = 1.0;
    const qreal availableScaleX = maxWidth / static_cast<qreal>(image.width());
    const qreal availableScaleY = maxHeight / static_cast<qreal>(image.height());
    scale = qMin(availableScaleX, availableScaleY);
    if (scale > 1.0)
    {
        scale = 1.0;
    }

    const qreal targetWidth = image.width() * scale;
    const qreal targetHeight = image.height() * scale;
    const qreal x = (pageRect.width() - targetWidth) / 2.0;
    const qreal y = topMargin + (maxHeight - targetHeight) / 2.0;

    painter.drawImage(
        QRectF(x, y, targetWidth, targetHeight),
        image
    );
}

}

bool PdfGenerator::generate(const QStringList &files, const QString &outputPath)
{
    if (files.isEmpty() || outputPath.isEmpty())
    {
        return false;
    }

    const QFileInfo outputInfo(outputPath);
    QDir outputDir(outputInfo.absolutePath());
    if (!outputDir.exists() && !outputDir.mkpath("."))
    {
        return false;
    }

    // Linux uses poppler-utils; Windows can bundle pdfunite.exe beside the app.
    const QString pdfunitePath = findPdfUnite();
    if (pdfunitePath.isEmpty())
    {
        return false;
    }

    QTemporaryDir temporaryDirectory;
    if (!temporaryDirectory.isValid())
    {
        return false;
    }

    QStringList pdfParts;
    int partIndex = 0;

    for (const QString &filePath : files)
    {
        const QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable())
        {
            return false;
        }

        const QString extension = fileInfo.suffix().toLower();
        if (extension == "pdf")
        {
            pdfParts << fileInfo.absoluteFilePath();
            continue;
        }

        if (extension != "txt" && !isImageFile(filePath))
        {
            continue;
        }

        const QString partPath = temporaryDirectory.path()
            + QString("/part-%1.pdf").arg(partIndex++);
        QPdfWriter writer(partPath);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setPageMargins(QMarginsF(20, 20, 20, 20));
        writer.setResolution(300);

        QPainter painter;
        if (!painter.begin(&writer))
        {
            return false;
        }

        painter.setPen(Qt::black);
        painter.setFont(QFont("Sans Serif", 11));

        if (extension == "txt")
        {
            QFile inputFile(filePath);
            if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                painter.end();
                return false;
            }

            const QString text = QString::fromUtf8(inputFile.readAll());
            inputFile.close();
            renderTextContent(writer, painter, text);
        }
        else
        {
            QImage image(filePath);
            if (image.isNull())
            {
                painter.end();
                return false;
            }

            renderImageContent(writer, painter, filePath);
        }

        painter.end();
        pdfParts << partPath;
    }

    if (pdfParts.isEmpty())
    {
        return false;
    }

    const QString mergedPath = temporaryDirectory.path() + "/merged.pdf";
    QStringList arguments = pdfParts;
    arguments << mergedPath;

    QProcess mergeProcess;
    mergeProcess.start(pdfunitePath, arguments);
    if (!mergeProcess.waitForFinished() || mergeProcess.exitStatus() != QProcess::NormalExit
        || mergeProcess.exitCode() != 0 || !QFileInfo::exists(mergedPath))
    {
        return false;
    }

    QFile::remove(outputPath);
    return QFile::copy(mergedPath, outputPath);
}

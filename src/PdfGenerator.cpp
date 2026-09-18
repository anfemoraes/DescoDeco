#include "PdfGenerator.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFontMetrics>
#include <QImage>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QRegularExpression>

namespace
{
bool isTextFile(const QString &filePath)
{
    const QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "txt";
}

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

void renderPdfInfoPage(QPdfWriter &writer, QPainter &painter, const QString &filePath)
{
    const QString fileName = QFileInfo(filePath).fileName();
    const QString text =
        "Arquivo PDF encontrado:\n\n"
        + fileName + "\n\n"
        + "O conteúdo deste PDF foi identificado pelo DescoDeco,\n"
        + "mas a incorporação das páginas será implementada posteriormente.";

    renderTextContent(writer, painter, text);
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

    QPdfWriter writer(outputPath);
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

    bool firstFile = true;
    for (const QString &filePath : files)
    {
        if (!QFileInfo(filePath).exists())
        {
            continue;
        }

        if (!firstFile)
        {
            writer.newPage();
        }
        firstFile = false;

        const QString extension = QFileInfo(filePath).suffix().toLower();

        if (extension == "txt")
        {
            QFile inputFile(filePath);
            if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                continue;
            }

            const QString text = QString::fromUtf8(inputFile.readAll());
            inputFile.close();
            renderTextContent(writer, painter, text);
        }
        else if (isImageFile(filePath))
        {
            renderImageContent(writer, painter, filePath);
        }
        else if (extension == "pdf")
        {
            renderPdfInfoPage(writer, painter, filePath);
        }
    }

    painter.end();
    return true;
}

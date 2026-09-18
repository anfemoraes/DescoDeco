#include "MainWindow.h"
#include "ZipExtractor.h"

#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("DescoDeco");
    resize(800, 500);

    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

    botaoSelecionar = new QPushButton(
        "Selecionar arquivo ZIP",
        this
    );

    labelArquivo = new QLabel(
        "Nenhum arquivo selecionado.",
        this
    );

    layout->addWidget(botaoSelecionar);
    layout->addWidget(labelArquivo);

    setCentralWidget(centralWidget);

    connect(
        botaoSelecionar,
        &QPushButton::clicked,
        this,
        &MainWindow::selecionarArquivo
    );
}

void MainWindow::selecionarArquivo()
{
    QString arquivo = QFileDialog::getOpenFileName(
        this,
        "Selecionar arquivo ZIP",
        QString(),
        "Arquivos ZIP (*.zip)"
    );

    if (arquivo.isEmpty())
    {
        return;
    }

    labelArquivo->setText(
        "Arquivo selecionado:\n" + arquivo
    );

    QString pastaDestino =
        QDir::tempPath() + "/DescoDeco";

    QDir().mkpath(pastaDestino);

    ZipExtractor extractor;

    bool sucesso = extractor.extract(
        arquivo,
        pastaDestino
    );

    if (sucesso)
    {
        QString arquivosExtraidos;
        QDirIterator iterator(
            pastaDestino,
            QDir::Files,
            QDirIterator::Subdirectories
        );

        while (iterator.hasNext())
        {
            QString caminhoArquivo = iterator.next();
            arquivosExtraidos += caminhoArquivo + "\n";
        }

        if (arquivosExtraidos.isEmpty())
        {
            QMessageBox::information(
                this,
                "DescoDeco",
                "Nenhum arquivo encontrado."
            );
        }
        else
        {
            QMessageBox::information(
                this,
                "DescoDeco",
                "Arquivos extraídos:\n\n" +
                arquivosExtraidos.trimmed()
            );
        }
    }
    else
    {
        QMessageBox::critical(
            this,
            "Erro",
            "Não foi possível descompactar o arquivo ZIP."
        );
    }
}
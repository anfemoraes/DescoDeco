#include "MainWindow.h"

#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("DescoDeco");
    resize(800, 500);

    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

    botaoSelecionar = new QPushButton("Selecionar arquivo ZIP", this);
    labelArquivo = new QLabel("Nenhum arquivo selecionado.", this);

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

    if (!arquivo.isEmpty())
    {
        labelArquivo->setText(arquivo);
    }
}
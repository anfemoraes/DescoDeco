# DescoDeco

DescoDeco é um aplicativo desktop para Windows que pega um arquivo ZIP, extrai o conteúdo, busca todos os arquivos dentro da pasta e gera um único PDF com o resultado final.

Ele foi pensado para quem precisa transformar rapidamente um lote de imagem, texto ou PDF em um documento único para envio, organização ou impressão.

## O que o programa faz

O fluxo principal é:

1. O usuário seleciona um arquivo ZIP.
2. O programa extrai o conteúdo do ZIP para uma pasta temporária.
3. Ele procura recursivamente por arquivos dentro da pasta extraída.
4. Os arquivos suportados são convertidos em páginas de PDF.
5. O usuário escolhe o nome e a localização do PDF final.
6. O programa gera um único arquivo PDF consolidado.

### Formatos suportados

Atualmente, a aplicação trabalha com:

- imagens: PNG, JPG, JPEG, BMP
- texto: TXT
- documentos PDF: PDF

### O que acontece por trás

A lógica do programa está dividida em algumas partes:

- `src/MainWindow.cpp`: interface principal do app e fluxo da operação
- `src/ZipExtractor.cpp`: descompacta o ZIP escolhido
- `src/PdfGenerator.cpp`: converte os arquivos em PDF e junta tudo em um único arquivo
- `src/MainWindow.h`, `src/ZipExtractor.h`, `src/PdfGenerator.h`: definições das classes

O app usa Qt 6 para a interface e libzip para leitura dos arquivos ZIP.

## Como instalar para um usuário comum

### Opção 1: usar a versão portátil

Se você recebeu o pacote ZIP pronto para distribuição:

1. Faça o download do ZIP do projeto.
2. Extraia a pasta em qualquer lugar do computador.
3. Abra a pasta extraída.
4. Clique no arquivo `DescoDeco.exe`.
5. Se aparecer uma mensagem pedindo a instalação de dependências, mantenha todos os arquivos da pasta juntos.

Importante:
- O executável precisa ficar junto com as DLLs e com os arquivos de suporte da pasta.
- Não mova apenas o `DescoDeco.exe` para outro local sem levar junto o restante da pasta.

### Opção 2: compilar a partir do código-fonte

Se quiser gerar o programa no próprio PC:

1. Instale o MSYS2.
2. Abra o terminal MSYS2 ucrt64.
3. Instale os pacotes necessários:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-libzip
```

4. Abra o PowerShell na pasta do projeto.
5. Execute:

```powershell
./build-win.ps1
```

6. O executável será gerado em uma pasta de build.

## Como usar

1. Abra o programa.
2. Clique em `Selecionar arquivo ZIP`.
3. Escolha o arquivo ZIP desejado.
4. O programa vai extrair o conteúdo para uma pasta temporária.
5. Ao final, o programa pedirá onde salvar o PDF final.
6. Escolha o nome e o local.
7. O arquivo PDF será gerado.

## Requisitos do sistema

- Windows 10 ou Windows 11
- Arquitetura 64 bits
- Permissão para salvar arquivos em uma pasta local

## Observações importantes

- O programa depende de bibliotecas do Qt e do `libzip`.
- Em ambiente Windows, a distribuição portátil precisa manter todas as DLLs junto ao executável.
- Para a conversão final em PDF, o aplicativo também depende de um utilitário de PDF (como `pdfunite`), que precisa estar disponível no ambiente.
- O programa é feito para uso local e offline.

## Estrutura do projeto

```text
DescoDeco/
├── CMakeLists.txt
├── build-win.ps1
├── README.md
├── src/
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── PdfGenerator.cpp
│   ├── PdfGenerator.h
│   ├── ZipExtractor.cpp
│   ├── ZipExtractor.h
│   └── main.cpp
└── ...
```

## Resumo em uma frase

O DescoDeco serve para transformar um pacote ZIP com imagens, textos e PDFs em um único documento PDF organizado e pronto para uso.

---

Se você quiser, também posso criar uma segunda versão do README mais enxuta, para publicação no GitHub, com uma aparência mais profissional e pronta para o repositório.

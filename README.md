# Desafio163 - Ferramenta de Recuperação de Chave Privada Bitcoin

Uma ferramenta de alta performance para encontrar a chave privada de Bitcoin do desafio163. O aplicativo foi escrito em C++.

Se você conseguir resolver o desafio163 utilizando este aplicativo, por favor me conte, ficarei feliz em saber.

## Autor
- **Roberto Dutra**
  - GitHub: [@dutraroberto](https://github.com/dutraroberto)

## Visão Geral
Este aplicativo é apenas um estudo desenvolvido para ser usado no Desafio163, um desafio criado pelo canal "Investidor Internacional". Para mais informações sobre o desafio, visite o canal no YouTube: [Investidor Internacional](https://www.youtube.com/@InvestidorInternacional).

## Funcionalidades
- Busca de chave multithread com desempenho otimizado
- Operações de curva elíptica Secp256k1
- Geração de endereço Bitcoin
- Codificação Base58
- Funções hash SHA256 e RIPEMD160
- Cache de contexto local por thread
- Métricas de desempenho em tempo real

## Requisitos
- Windows 10 ou superior (64-bit)

## Instalação e Execução

Existem duas formas de executar o programa:

### Método 1: Download do Programa Compilado

1. **Download**:
   - Baixe o arquivo `desafio163.ZIP` na página de releases do GitHub: [Desafio163 Releases](https://github.com/dutraroberto/desafio163/releases/tag/v0.2-beta)
   - Extraia o conteúdo para um diretório de sua escolha.

2. **Executar o Aplicativo**:
   - Navegue até o diretório extraído.
   - Execute o arquivo `Desafio163.exe` localizado na pasta `build/Release`.

### Método 2: Compilação do Projeto

1. **Requisitos**:
   - Visual Studio 2022
   - CMake

2. **Preparação**:
   - Faça um clone do repositório: `git clone https://github.com/dutraroberto/desafio163`
   - Abra o projeto no Visual Studio Code

3. **Compilação**:
   - Execute o arquivo `build.bat` para iniciar a compilação.
   
   **Nota Importante**: Devido a um erro conhecido, o projeto precisa ser compilado duas vezes para garantir que todos os arquivos necessários sejam gerados corretamente. Se a primeira compilação falhar, execute o `build.bat` novamente.

## Resultados
Os resultados serão exibidos no CMD e salvos na pasta `resultados` na raiz do projeto.

## Problemas Conhecidos

Durante a primeira tentativa de compilação, pode ocorrer um erro de linkagem devido à falta de alguns arquivos necessários. No entanto, esses arquivos são gerados durante a primeira compilação. Para resolver este problema, basta compilar o projeto novamente, e a compilação será concluída com sucesso.

## Licença
Este projeto é de uso exclusivo do Desafio163 - veja o arquivo [LICENÇA](LICENÇA) para detalhes.

## Contribuição
Contribuições são bem-vindas! Por favor, sinta-se à vontade para enviar uma Solicitação de Pull.

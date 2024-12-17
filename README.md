# Busca de Chaves para o Desafio163

Se você conseguir resolver o desafio163 utilizando este aplicativo, por favor me conte, ficarei feliz em saber.

Este aplicativo é apenas um estudo desenvolvido para ser usado no Desafio163, um desafio criado pelo canal "Investidor Internacional". Para mais informações sobre o desafio, visite o canal no YouTube: [Investidor Internacional](https://www.youtube.com/@InvestidorInternacional).

Desenvolvido por:
- **Roberto Dutra / ChatGPT**
  - GitHub: [@dutraroberto](https://github.com/dutraroberto)

Colaboradores:
- **JefinhoCPS**
  - GitHub: [@JefinhoCPS](https://github.com/jmr2704)

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
   - CMake (versão 3.10 ou superior)
   - OpenSSL v3.x para Windows (64-bit)

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

## Funcionalidades
- Busca de chave multithread com desempenho otimizado
- Operações de curva elíptica Secp256k1
- Geração de endereço Bitcoin
- Codificação Base58
- Funções hash SHA256 e RIPEMD160
- Cache de contexto local por thread
- Métricas de desempenho em tempo real
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

Para instalar e executar o aplicativo, siga as etapas abaixo:

1. **Download e Extração**:
   - Baixe o arquivo zip do aplicativo a partir do repositório.
   - Extraia o conteúdo para um diretório de sua escolha.

2. **Executar o Aplicativo**:
   - Navegue até o diretório extraído.
   - Execute o arquivo `Desafio163.exe` localizado na pasta `build/Release`. (Ou utilize o atalho no diretório raiz do projeto.)

3. **Configuração de Teste**:
   - Use a seguinte carteira para testes: `19YW4vDiNZzMzyb1gKYGsqM2KM7bSmEnCH`
   - Para a chave privada parcial, use: `C17248365038AB57B7E3A92FE375661B35D8147B9C205FA91EDB3AA0x5xBxDxD`
     - Nota: Você pode alterar os caracteres revelados por "x" para aumentar a dificuldade do desafio.

## Resultados
Os resultados serão exibidos no CMD e salvados na pasta `resultados` na raiz do projeto.

## Problemas Conhecidos

Durante a primeira tentativa de compilação, pode ocorrer um erro de linkagem devido à falta de alguns arquivos necessários. No entanto, esses arquivos são gerados durante a primeira compilação. Para resolver este problema, basta compilar o projeto novamente, e a compilação será concluída com sucesso.

## Licença
Este projeto é de uso exclusivo do Desafio163 - veja o arquivo [LICENÇA](LICENÇA) para detalhes.

## Contribuição
Contribuições são bem-vindas! Por favor, sinta-se à vontade para enviar uma Solicitação de Pull.

# Desafio163 - Ferramenta de Recuperação de Chave Privada Bitcoin

Uma ferramenta de alta performance para encontrar chaves privadas de Bitcoin escrita em C++.

## Autor
- **Roberto Dutra**
  - GitHub: [@dutraroberto](https://github.com/dutraroberto)

## Visão Geral
Esta ferramenta é projetada para recuperar chaves privadas de Bitcoin usando técnicas criptográficas avançadas e multithreading otimizado.

## Funcionalidades
- Busca de chave multithread com desempenho otimizado
- Operações de curva elíptica Secp256k1
- Geração de endereço Bitcoin
- Codificação Base58
- Funções hash SHA256 e RIPEMD160
- Cache de contexto local por thread
- Métricas de desempenho em tempo real

## Requisitos
- Windows 64-bit
- Visual Studio 2022
- CMake 3.10+
- OpenSSL v3.x

## Compilação
1. Instale as dependências:
   - Visual Studio 2022 com ferramentas de desenvolvimento C++
   - CMake 3.10 ou superior
   - OpenSSL v3.x (versão Win64)

2. Clone o repositório:
   ```bash
   git clone https://github.com/dutraroberto/desafio163.git
   cd desafio163
   ```

3. Compile o projeto:
   ```bash
   build.bat
   ```

## Uso
O executável estará localizado em `build/Release/Desafio163.exe`

Opções de linha de comando:
- `--address`: Endereço Bitcoin alvo
- `--key`: Chave privada parcial
- `--threads`: Número de threads a usar
- `--benchmark`: Executar teste de desempenho
- `--help`: Mostrar informações de uso

## Licença
Este projeto é licenciado sob a Licença MIT - veja o arquivo [LICENÇA](LICENÇA) para detalhes.

## Otimizações de Desempenho
- Algoritmo de busca multithread
- Cache de contexto local por thread
- Pré-computação de curva elíptica
- Minimização de alocações de memória
- Operações criptográficas otimizadas

## Contribuição
Contribuições são bem-vindas! Por favor, sinta-se à vontade para enviar uma Solicitação de Pull.

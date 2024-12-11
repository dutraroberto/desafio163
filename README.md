# Desafio163 - Bitcoin Private Key Finder

Este programa foi desenvolvido para resolver o Desafio163, que consiste em descobrir uma chave privada de Bitcoin parcialmente revelada. O programa utiliza processamento multi-thread e operações criptográficas otimizadas para maximizar a velocidade de busca.

## Características

- Implementação em C++ com otimizações de baixo nível
- Suporte completo à curva elíptica secp256k1 do Bitcoin
- Processamento multi-thread automático
- Interface de linha de comando com várias opções
- Modo benchmark para testes de performance
- Feedback em tempo real do progresso
- Otimizações criptográficas com cache de objetos

## Requisitos

- Visual Studio 2019 ou 2022 com suporte a C++
- CMake 3.10 ou superior
- OpenSSL v3.x
- Windows 64-bit

## Instalação

1. Clone o repositório:
```bash
git clone https://github.com/dutraroberto/desafio163.git
cd desafio163
```

2. Instale as dependências:
   - Visual Studio 2022 Community Edition com:
     - "Desenvolvimento para desktop com C++"
     - "Ferramentas do CMake"
   - OpenSSL v3.x para Windows 64-bit de: https://slproweb.com/products/Win32OpenSSL.html

3. Compile o projeto:
```bash
build.bat
```

## Uso

### Uso Básico
```bash
Desafio163.exe
```

### Opções Disponíveis
```bash
Desafio163.exe --help
```

### Especificar Endereço e Chave
```bash
Desafio163.exe --address <endereço_bitcoin> --key <chave_parcial>
```

### Ajustar Número de Threads
```bash
Desafio163.exe --threads 8
```

### Executar Benchmark
```bash
Desafio163.exe --benchmark
```

## Exemplos

Buscar uma chave específica:
```bash
Desafio163.exe --address 1Hoyt6UBzwL5vvUSTLMQC2mwvvE5PpeSC --key 4x3x3x4xcxfx6x9xfx3xaxcx5x0x4xbxbx7x2x6x8x7x8xax4x0x8x3x3x3x7x3x
```

## Estrutura do Projeto

- `src/` - Arquivos fonte
  - `main.cpp` - Ponto de entrada e interface do usuário
  - `bitcoin_utils.cpp` - Implementação das operações Bitcoin
  - `key_finder.cpp` - Lógica de busca da chave
- `include/` - Arquivos de cabeçalho
- `CMakeLists.txt` - Configuração do CMake
- `build.bat` - Script de compilação para Windows

## Otimizações

- Cache de objetos EC_GROUP e BN_CTX
- Contextos thread-local para operações criptográficas
- Pré-computação de tabelas para operações na curva elíptica
- Minimização de alocações de memória
- Otimização das operações de hash

## Licença

MIT License - veja o arquivo LICENSE para detalhes.

## Autor

Roberto Dutra

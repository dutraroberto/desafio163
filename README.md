# Desafio163 - Ferramenta de Recuperação de Chave Privada Bitcoin

Uma ferramenta para encontrar a chave privada de Bitcoin do desafio163, desenvolvida com C++.

## Autor
- **Roberto Dutra**
  - GitHub: [@dutraroberto](https://github.com/dutraroberto)
  - Canal do YouTube do desafio163: [Investidor Internacional](https://www.youtube.com/@investidorint)
  - Meu e-mail: [rd.robertodutra@gmail.com](mailto:rd.robertodutra@gmail.com)

## ⚠️ Aviso Importante
Este software é fornecido exclusivamente para participação no Desafio163. É estritamente proibido o uso deste código para fins comerciais, ataques ou busca de carteiras privadas de Bitcoin. O uso é permitido apenas para fins educacionais e de participação no desafio.

## Visão Geral
Esta ferramenta é projetada para recuperar chaves privadas de Bitcoin usando técnicas criptográficas avançadas, processamento GPU e multithreading otimizado.

## Funcionalidades
- Processamento GPU otimizado para NVIDIA RTX 2060
- Busca de chave multithread com desempenho otimizado
- Operações de curva elíptica Secp256k1
- Geração de endereço Bitcoin
- Codificação Base58
- Funções hash SHA256 e RIPEMD160
- Cache de contexto local por thread
- Métricas de desempenho em tempo real
- Fallback automático para CPU se GPU não disponível

## Requisitos
### Hardware
- NVIDIA RTX 2060 ou superior (recomendado)
- 8GB RAM mínimo
- CPU compatível com SSE4.1

### Software
- Windows 64-bit
- Visual Studio 2022
- CMake 3.10+
- OpenSSL v3.x (versão Win64)
- NVIDIA CUDA Toolkit 11.x ou superior
- Driver NVIDIA atualizado (versão 470 ou superior)

## Instalação
1. Instale as dependências:
   - Visual Studio 2022 com ferramentas de desenvolvimento C++
   - CMake 3.10 ou superior
   - OpenSSL v3.x (versão Win64)
   - [NVIDIA CUDA Toolkit](https://developer.nvidia.com/cuda-downloads)

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

### Opções de linha de comando:
- `--address`: Endereço Bitcoin alvo
- `--key`: Chave privada parcial
- `--threads`: Número de threads CPU a usar
- `--gpu`: Usar processamento GPU (padrão: ativado)
- `--gpu-blocks`: Número de blocos GPU (padrão: 1024)
- `--gpu-threads`: Threads por bloco GPU (padrão: 256)
- `--benchmark`: Executar teste de desempenho
- `--help`: Mostrar informações de uso

### Exemplos de uso:
```bash
# Usar GPU (recomendado)
Desafio163.exe --address 1abcd... --key "XXXX..." --gpu

# Usar apenas CPU
Desafio163.exe --address 1abcd... --key "XXXX..." --gpu=false

# Configurar blocos e threads GPU
Desafio163.exe --address 1abcd... --key "XXXX..." --gpu-blocks=2048 --gpu-threads=512
```

## Otimizações de Desempenho
- Processamento paralelo em GPU NVIDIA
- Algoritmo de busca multithread para CPU
- Cache de contexto local por thread
- Pré-computação de curva elíptica
- Minimização de alocações de memória
- Operações criptográficas otimizadas
- Balanceamento automático de carga CPU/GPU

## Benchmarks
Testes realizados em uma RTX 2060:
- GPU: ~1.7 milhões de chaves/segundo
- CPU (12 threads): ~170 mil chaves/segundo
- Modo híbrido (GPU + CPU): ~1.85 milhões de chaves/segundo

## Contribuição
Contribuições são bem-vindas! Por favor, sinta-se à vontade para enviar uma Solicitação de Pull.

## Licença
Este projeto é licenciado sob uma licença personalizada - veja o arquivo [LICENÇA](LICENÇA) para detalhes.

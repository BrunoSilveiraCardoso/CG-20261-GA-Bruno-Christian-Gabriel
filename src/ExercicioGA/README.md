# Leitor e Visualizador de Cenas 3D com OpenGL Moderna

**Trabalho Pratico - Grau A | Processamento Grafico: Computacao Grafica e Aplicacoes**
**UNISINOS 2026/1**

**Integrantes:**
- Bruno Silveira da Silva
- Christian Aguiar Plentz

---

## Descricao

Leitor e visualizador de cenas 3D utilizando OpenGL Moderna (3.3 Core Profile), com suporte a multiplos objetos, camera sintetica FPS, transformacoes geometricas e iluminacao de Phong.

---

## Requisitos para Compilacao

- **Compilador:** GCC via MSYS2 (ucrt64) ou equivalente com suporte a C++17
- **CMake:** 3.10 ou superior
- **Dependencias (instaladas automaticamente via CMake FetchContent):**
  - GLFW 3.4
  - GLM
  - Assimp
  - stb_image

### Windows (MSYS2)

```bash
# Instale o MSYS2 e execute no terminal ucrt64:
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake

# Clone o repositorio
git clone https://github.com/Csatinapppple/CG-20261.git
cd CG-20261

# Gere os arquivos de build
cmake -S . -B build -G "MinGW Makefiles"

# Compile
cd build
mingw32-make -j4
```

### Executando

**IMPORTANTE:** O executavel deve ser rodado a partir da pasta raiz do projeto (onde estao as pastas `assets/` e `shaders/`):

```bash
# A partir da raiz do projeto:
build\ExercicioGA.exe

# Ou use o script na area de trabalho:
RodarOpenGL.bat
```

---

## Funcionalidades Implementadas

### 1. Processamento de Geometria
- Leitura de arquivos `.obj` via biblioteca **Assimp**
- Recuperacao de vertices, faces (triangulos), normais e coordenadas de textura
- Gerenciamento correto de **VAO**, **VBO** e **EBO**

### 2. Visualizacao e Controle
- Exibicao simultanea de multiplos objetos na cena
- Selecao de objeto via teclado
- Transformacoes geometricas: translacao, rotacao e escala (por eixo e uniforme)
- Camera sintetica FPS com teclado e mouse
- Troca entre projecao **perspectiva** e **ortografica**

### 3. Iluminacao e Materiais
- Modelo de iluminacao de **Phong** completo (Ambiente + Difusa + Especular) implementado nos shaders GLSL
- Fonte de luz pontual configuravel
- Propriedades de material (`ka`, `kd`, `ks`, `shininess`) ajustaveis em tempo real via teclado
- Valores exibidos em tempo real no titulo da janela
- Modo **wireframe** sobreposto a geometria solida

---

## Controles

### Camera
| Tecla | Acao |
|-------|------|
| W / S | Mover frente / tras |
| A / D | Mover esquerda / direita |
| Mouse | Rotacionar camera |
| Scroll | Zoom (FOV) |

### Selecao e Operacoes
| Tecla | Acao |
|-------|------|
| NumPad + | Selecionar proximo objeto |
| NumPad - | Selecionar objeto anterior |
| NumPad 7 | Modo: Translacao |
| NumPad 4 | Modo: Rotacao |
| NumPad 1 | Modo: Escala por eixo |
| NumPad 0 | Modo: Escala uniforme |
| NumPad . | Reset escala e rotacao do objeto atual |

### Transformacao (eixos)
| Tecla | Acao |
|-------|------|
| NumPad 8 / 9 | Eixo X: diminuir / aumentar |
| NumPad 5 / 6 | Eixo Y: diminuir / aumentar |
| NumPad 2 / 3 | Eixo Z: diminuir / aumentar |

### Visualizacao
| Tecla | Acao |
|-------|------|
| NumPad / | Toggle wireframe sobreposto |
| P | Alternar perspectiva / ortografica |

### Material Phong (ajuste em tempo real)
| Tecla | Acao |
|-------|------|
| I / K | ka (ambiente) aumentar / diminuir |
| O / L | kd (difusa) aumentar / diminuir |
| U / J | ks (especular) aumentar / diminuir |
| N / M | shininess aumentar / diminuir |

> Os valores atuais de ka, kd, ks e shininess sao exibidos no titulo da janela em tempo real.

### Sistema
| Tecla | Acao |
|-------|------|
| ESC | Fechar aplicacao |

---

## Estrutura do Projeto

```
CG-20261/
├── assets/
│   └── Modelos3D/       # Arquivos .obj e texturas
├── common/
│   └── glad.c           # Loader OpenGL
├── include/
│   ├── glad/            # Headers GLAD
│   ├── camera.hpp       # Camera FPS
│   ├── mesh.hpp         # Mesh com VAO/VBO/EBO
│   ├── model.hpp        # Carregamento de modelos via Assimp
│   ├── shader.hpp       # Compilacao e uso de shaders
│   └── fileReader.hpp
├── shaders/
│   ├── vertex.glsl      # Vertex shader (posicao, normal, texcoord)
│   ├── geometry.glsl    # Geometry shader (wireframe baricentrino)
│   └── fragment.glsl    # Fragment shader (Phong completo)
├── src/
│   └── ExercicioGA/
│       └── main.cpp     # Programa principal
└── CMakeLists.txt
```

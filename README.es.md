<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Velot8

Emulador educativo de CHIP-8 en C11. La primera iteración incluye núcleo desacoplado de SDL2, ventana SDL2 y una demo CHIP-8 original animada.

Ejecuta `make test` para las pruebas sin interfaz, `make headless` para el binario sin SDL2 y `make run` para abrir la demo gráfica. SDL2 y `pkg-config` son necesarios solo para `make run`, `make debug` o `make release`. En macOS con Homebrew: `brew install sdl2 pkg-config`.

La implementación aún es parcial y no declara compatibilidad general con ROMs. Incluye buzzer SDL básico, perfiles de quirks por línea de comandos y pruebas SDL de mapeo de teclas con `make test-sdl`. El proyecto se distribuye bajo [Apache License 2.0](LICENSE); consulta [DEPENDENCIES.md](DEPENDENCIES.md) para dependencias y [README.md](README.md) para controles, limitaciones y uso detallado.

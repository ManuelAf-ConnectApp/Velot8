<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# PROMPT MAESTRO — Velot8, emulador Open Source en C11 con frontend gráfico SDL2

## 0. Rol y forma de trabajo
Actúa como **ingeniero principal de software** especializado en sistemas embebidos, arquitectura de computadores, emulación de hardware, C de alto rendimiento, portabilidad y software libre. Tu misión es **diseñar e implementar de forma incremental** un emulador educativo, robusto y mantenible en **C11** (o C99 si existe una justificación explícita).

Trabaja como responsable técnico del proyecto: entrega código concreto y compilable, pruebas automatizadas, decisiones arquitectónicas fundamentadas y documentación. **No simules haber compilado, ejecutado o probado código si no lo has hecho.** Si tienes acceso al repositorio, examínalo antes de introducir cambios, conserva los componentes válidos y explica las modificaciones.

## 1. Parámetros del proyecto
- **Nombre de trabajo**: **Velot8**. Usar `velot8` en ejecutables, carpetas, rutas de inclusión, nombres de biblioteca y documentación. La búsqueda pública preliminar no garantiza disponibilidad jurídica de marca; comprobar antes de publicar.
- **Sistema emulado**: **CHIP-8** como objetivo inicial obligatorio del MVP gráfico; **Game Boy clásica** solamente como proyecto/etapa futura y núcleo independiente, tras completar y validar CHIP-8.
- **Selección por defecto si no se especifica**: **CHIP-8** como MVP inicial. No mezclar las especificaciones de CHIP-8 y Game Boy en un solo núcleo genérico prematuro.
- **Lenguaje**: C11 puro para el núcleo, sin C++.
- **Sistemas anfitriones objetivo**: Linux, Windows y macOS.
- **Compiladores**: GCC y Clang; MSVC puede incorporarse mediante adaptación adicional si es viable.
- **Frontend**: **SDL2 obligatorio en el ejecutable gráfico MVP** para ventana, píxeles, teclado, eventos y sonido opcional; aislarlo del núcleo. El ejecutable headless debe funcionar sin SDL2. SDL3 o raylib son alternativas futuras solo con justificación.
- **Licencia propuesta del código propio**: Apache-2.0 o MIT; solicitar o explicitar la decisión antes de publicar y verificar compatibilidad de dependencias.
- **Restricciones**: no distribuir ROMs comerciales, BIOS/boot ROMs ni otros archivos con copyright sin autorización; emplear ROMs de prueba propias, sintéticas o con licencia verificable.

## 2. Objetivos y criterios de diseño
1. Emulación funcional, correcta y verificable antes de optimizaciones prematuras.
2. Núcleo desacoplado de sistema operativo, gráficos, audio, entrada, filesystem y reloj de pared.
3. Diseño modular con interfaces internas explícitas y mínima cantidad de estado global mutable.
4. Aritmética de ancho fijo (`stdint.h`), gestión segura de límites y memoria, errores trazables y pruebas deterministas.
5. Rendimiento estable con perfiles Release/Debug y profiling reproducible.
6. Buena experiencia de desarrollador: construcción simple, logs y herramientas de inspección.
7. Portabilidad y accesibilidad del frontend: escalado entero configurable, controles remapeables y pausa.

Define además **no objetivos** del MVP (por ejemplo, rewind, netplay, shaders, soporte de decenas de sistemas) para evitar complejidad innecesaria.

## 3. Investigación de especificación y decisiones previas
Antes de codificar, presenta una tabla con características de **la máquina seleccionada** y referencias técnicas fiables (sin inventar citas). Identifica incertidumbres y variantes de comportamiento.

### Si se elige CHIP-8
- Memoria de 4096 bytes; 16 registros generales `V[16]`, `I`, `PC`, pila y `SP`; temporizadores de 60 Hz; teclado hexadecimal de 16 teclas; pantalla base de 64×32 monocroma.
- Carga de programas habitualmente a `0x200`; definir área reservada y fuente de sprites de forma documentada.
- Especificar semántica y quirks configurables: desplazamientos `8xy6/8xyE`, `Fx55/Fx65`, salto `Bnnn`, borrado de `VF` en operaciones lógicas cuando corresponda, espera de tecla `Fx0A`, dibujado y colisiones.
- Manejar explícitamente fin de memoria, desbordamiento de pila, opcodes no válidos y tamaños de ROM.
- **No incorporar interrupciones de Game Boy a CHIP-8**; los temporizadores y el sonido de CHIP-8 son mecanismos distintos.
- Considerar extensiones SCHIP/XO-CHIP únicamente como futuras variantes separadas, no como MVP automático.

### Si se elige Game Boy clásica (DMG)
- CPU tipo LR35902: registros, flags, instrucciones normales y prefijo CB; modelos precisos de ciclos de máquina y tiempos de instrucción.
- Mapa de memoria y bus: ROM, VRAM, WRAM, OAM, HRAM, I/O y regiones reservadas.
- Cartuchos y **MBC**: comenzar con ROM-only, ampliar a controladores relevantes más adelante; no asumir que todas las ROMs son compatibles.
- Interrupciones: IME, IE/IF, prioridades, EI diferido, HALT/STOP y particularidades documentadas.
- Temporizador DIV/TIMA/TMA/TAC, PPU y modos de renderizado, DMA/OAM, joypad y APU por fases.
- Resolución 160×144, frecuencia de CPU y calendario de PPU/frames documentados según especificación DMG; el host FPS no equivale a frecuencia de CPU.
- Boot ROM opcional solo si el usuario proporciona una copia autorizada; alternativa de inicialización posarranque documentada.
- Identificar qué precisión es necesaria para juegos reales vs. tests de ciclo, y evitar prometer compatibilidad universal en el MVP.

## 4. Arquitectura y estructura del repositorio
Propón y justifica un árbol como punto de partida, adaptado al sistema seleccionado:

```text
velot8/
├── .github/workflows/ci.yml
├── include/velot8/          # API pública agnóstica de SDL
│   ├── emulator.h
│   ├── cpu.h
│   └── types.h
├── src/
│   ├── core/                 # CPU, bus/memoria, temporizadores, máquina
│   │   ├── cpu.c
│   │   ├── memory.c
│   │   ├── timers.c
│   │   └── emulator.c
│   ├── frontend/sdl2/        # Ventana, controles, gráficos y audio
│   │   ├── frontend.c
│   │   └── audio.c
│   └── main.c
├── tests/                     # Test unitarios, ROMs de prueba redistribuibles
├── tools/                     # Debugger, trazas y utilidades opcionales
├── docs/                      # Arquitectura, instrucciones, quirks, decisiones
├── examples/
├── assets/                    # Solo recursos con licencia apropiada
├── Makefile
├── README.md
├── README.es.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── LICENSE
├── NOTICE
└── .gitignore
```

No crear archivos vacíos sin propósito. Separar cabeceras públicas y privadas; evitar inclusiones cíclicas y capas artificiales. Recomendar si conviene biblioteca `libvelot8` + ejecutable frontend.

## 5. Modelo de estado y API interna
Proporciona definiciones reales de cabeceras C y tipos de datos para el sistema elegido:
- Estado CPU: registros y flags (exactos de la máquina), PC/SP, estado detenido/esperando tecla, ciclos acumulados cuando proceda.
- Memoria/bus con lectura-escritura centralizadas, comprobaciones de rango y mapeos dependientes de plataforma.
- Temporizadores, pantalla/framebuffer, entrada, buzzer/audio y configuración.
- Estructura de contexto `Emulator` que posea el estado; `init/reset/load/step/destroy`, sin recursos globales ocultos.
- `enum` de códigos de error y mecanismo para recuperar errores sin abortos arbitrarios.
- Reglas de propiedad y vida de buffers, carga de ROM con tamaño explícito, bytes const y validación.
- Endianness, desplazamientos de bits sin comportamiento indefinido y tipos enteros (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`) correctos.

Usar funciones `static` donde corresponda y evitar exponer detalles de SDL en `include/velot8`.

## 6. Ejecución y temporización
Explica el ciclo completo **fetch → decode → execute**, qué muta, cómo se contabilizan ciclos y qué invariantes deben mantenerse. Incluye diagrama Mermaid.

- El **reloj de la máquina emulada** debe distinguirse del reloj real del ordenador anfitrión.
- Proponer acumulador temporal con reloj monotónico y `delta_time` acotado, preservando precisión al convertir entre segundos y ciclos.
- Evitar que el renderizado gobierne directamente la lógica de CPU; renderizar a cadencia independiente cuando sea apropiado.
- CHIP-8: velocidad de instrucciones configurable (porque las implementaciones históricas varían) y decremento de temporizadores a **60 Hz** independiente de esa velocidad.
- Game Boy: planificador guiado por ciclos/t-cycles que avance CPU, PPU, APU y temporizadores; documentar estados HALT/STOP, interrupciones y sincronización aproximada de 59,7 FPS sin confundir unidades.
- Evitar `sleep` activo y acumulación descontrolada de lag; definir catch-up máximo, pausa, reanudación, escalado de velocidad y ejecución frame-step / instruction-step.
- Explicar qué partes se ejecutan en el núcleo y cuáles pertenecen al frontend.

## 7. Descodificación y corrección
- Comparar `switch`/máscaras versus tabla de despacho en términos de claridad y benchmarking; no asumir automáticamente que una técnica será más rápida.
- Definir orden de operaciones y tratamiento preciso de flags y PC (incluyendo opcodes de longitud variable si aplica).
- Proporcionar implementaciones representativas y **probadas conceptualmente** de varias instrucciones básicas, pero no presentar un conjunto parcial como completo.
- Implementar manejo claro de instrucción ilegal, lectura fuera de rango y fallos de cartucho.
- Diseñar registro opcional de trazas con PC, opcode, registros, ciclos y desensamblado sin penalizar el hot loop de Release.

## 8. Frontend, controles y audio
- SDL2 como capa intercambiable de ventana, input y salida de audio; explicar instalación/enlace en Linux, macOS y Windows.
- Escalado entero con relación de aspecto correcta; modo fullscreen opcional; orientación del framebuffer.
- Mapeo de teclas documentado y remapeable. Gamepad opcional para fase posterior.
- CHIP-8: señal de sonido simple asociada a `sound_timer`, sin bloquear el hilo principal.
- Game Boy: empezar con interfaz APU clara y priorizar precisión incremental; documentar si el MVP aún no genera audio real.
- Gestión explícita de eventos, quit, pausa y errores de SDL; sin dependencia del frontend en tests del núcleo.

## 8.1. REQUISITO PRIORITARIO — Aplicación gráfica visible y utilizable desde el primer MVP
**No basta con una CPU que ejecute ROMs en terminal.** Implementa y entrega un ejecutable gráfico `velot8` que, al abrirse, muestre una ventana SDL2 realmente funcional con contenido visible incluso si el usuario no dispone de ninguna ROM externa. El modo `velot8 --headless` y los tests no dependerán de SDL2.

### Flujo de inicio e interacción
1. `./velot8` sin argumentos: abrir ventana **Velot8 — CHIP-8 Emulator** de tamaño inicial **960×480** o escala entera equivalente, fondo oscuro, área de display CHIP-8 **64×32**, estado visible de `NO ROM`/`DEMO` y breve leyenda de controles; no mostrar ventana vacía ni terminar inmediatamente.
2. `./velot8 --demo`: ejecutar una **demostración original y redistribuible** incluida en el repositorio, generada en C o como bytes CHIP-8 creados para el proyecto, que dibuje una figura o animación simple en el framebuffer. Documentar explícitamente si es demostración de frontend o programa CHIP-8 real. **Preferencia:** ROM CHIP-8 original con opcodes ya implementados y tests de su ejecución. Si esos opcodes aún no existen, usar una animación de framebuffer claramente etiquetada como `frontend demo`, no fingir emulación.
3. `./velot8 --rom ruta/al/archivo.ch8`: cargar ROM desde disco mediante loader validado, ejecutar y mostrar su framebuffer cuando exista soporte suficiente.
4. Botón de cierre, tecla Escape y eventos SDL de salida deben terminar el proceso limpiamente; no ejecutar bucle infinito sin procesar eventos.
5. Teclas `Space` (pausar/continuar), `R` (reiniciar), `N` (avanzar una instrucción estando pausado) y `F11` (pantalla completa) si se encuentran implementadas; documentar cuáles están funcionales. Mostrar título de ventana con estado de pausa y ROM actual. Evitar controles anunciados que todavía no hagan nada.
6. Mapeo del teclado CHIP-8 estándar y verificable: teclado físico `1 2 3 4 / Q W E R / A S D F / Z X C V` → matriz hexadecimal CHIP-8 `1 2 3 C / 4 5 6 D / 7 8 9 E / A 0 B F`, con opción de configurar más adelante. Como `R` coincide con una tecla de la matriz, asignar la acción de **reinicio a Ctrl+R**, no a `R` sola; la entrada CHIP-8 tiene prioridad en las teclas sin modificador. Definir explícitamente esta precedencia y comprobarla con tests.
7. Permitir el escalado entero mediante `--scale 8` (predeterminado, por ejemplo), preservar aspecto **2:1** de CHIP-8 64×32, centrar el contenido con letterboxing y gestionar redimensionado de ventana sin deformar píxeles.

### Implementación de renderizado SDL2
- `SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)` y audio cuando se implemente; crear `SDL_Window` y `SDL_Renderer` comprobando errores y con **fallback a renderer software** si falla el acelerado.
- Construir una textura streaming **RGBA8888** de **64×32**, o textura equivalente de formato explícito; convertir el framebuffer monocromo del núcleo a píxeles sin exponer SDL en las cabeceras del core. Elegir un orden de bytes consistente con el formato SDL elegido y probarlo.
- Actualizar la textura cuando el framebuffer cambie (o cada frame inicialmente si es más simple), hacer `SDL_RenderClear`, `SDL_RenderCopy` con rectángulo de destino de escala entera y `SDL_RenderPresent`. No crear/destruir texturas en cada frame ni hacer asignaciones evitables en el bucle caliente.
- Mantener los temporizadores CHIP-8 a 60 Hz e instrucciones por segundo configurables, **sin ejecutar una instrucción por frame de SDL**. Usar `SDL_GetPerformanceCounter`/`SDL_GetPerformanceFrequency` para un reloj monotónico del frontend, `dt` acotado y acumuladores temporales independientes.
- Procesar `SDL_PollEvent` cada iteración y manejar teclado, resizing, foco, cierre y pausa. Ante pérdida de foco, evitar teclas atascadas; nunca hacer busy-wait agresivo.
- Integrar un tema visual sencillo y propio: fondo azul noche `#101827`, píxeles cian `#44E3F5` y detalles discretos; evitar fuentes o assets de terceros sin atribución. Texto de estado opcional mediante `SDL_SetWindowTitle` para **no introducir SDL_ttf** en el MVP. Un HUD legible dentro de la ventana es mejora posterior si exige dependencias nuevas.
- En caso de inicialización SDL fallida, presentar error descriptivo y código de salida no cero, liberando recursos parcialmente creados.

### Estructura adicional solicitada
```text
src/frontend/sdl2/
  app.c          # Inicialización, shutdown y bucle de ventana
  app.h          # API interna frontend (no expuesta en core)
  video.c        # Textura, escala entera, renderizado
  video.h
  input.c        # Mapeo y eventos de teclado
  input.h
src/demo/
  demo_rom.c     # ROM de prueba original o demo framebuffer claramente etiquetada
  demo_rom.h
```

### Makefile y distribución
- `make release` construye el ejecutable gráfico enlazado con SDL2; `make run` ejecuta `./velot8 --demo` y debe abrir una ventana con imagen observable. `make run-rom ROM=...` ejecuta una ROM de usuario, validando ausencia de ruta.
- `make headless` compila sin la dependencia SDL2 y `make test` compila y ejecuta tests del núcleo sin acceso a un display.
- Detectar SDL2 con `pkg-config --cflags --libs sdl2` en plataformas compatibles; documentar `brew install sdl2 pkg-config`, instalación MSYS2/MinGW y paquetes de desarrollo Linux sin presuponer idénticos nombres en todas las distribuciones.
- No afirmar compatibilidad Windows/macOS/Linux si no se ha construido/probado en ellas; documentar los pasos y limitaciones.

### Pruebas de aceptación gráficas obligatorias
1. `make run` abre una ventana y **muestra píxeles que cambian** o un patrón original explícitamente denominado DEMO; pantalla negra permanente = fallo.
2. La ventana responde a cerrar/Escape y no deja recursos SDL sin liberar.
3. La imagen mantiene proporción 64:32 al redimensionar; no hay píxeles deformados ni textura recreada cada fotograma.
4. Mapeo de las 16 teclas validado; comprobar conflicto con Ctrl+R.
5. El framebuffer renderizado coincide con valores esperados en prueba automatizada sin depender de captura de pantalla, cuando resulte viable.
6. Tests de core/headless ejecutan sin entorno gráfico.
7. Temporizadores a 60 Hz independientemente del FPS de pantalla.
8. El README incluye captura real **solo si se ha generado y verificado**, y explica cómo ejecutar la demo sin descargar ROMs externas.

**Prioridad de entrega:** primera iteración funcional con ventana SDL2 + demo visual propia + cierre correcto + build/ejecución documentados, y evolución progresiva del núcleo CHIP-8; nunca sacrificar código compilable por un esqueleto demasiado amplio.

## 9. Makefile, herramientas y portabilidad
Entrega un `Makefile` **real y coherente** con el árbol, con:
- `make`, `make debug`, `make release`, `make test`, `make clean`, `make run`, `make format` (si la herramienta existe) y `make sanitize` cuando sea soportado.
- GCC y Clang seleccionables mediante `CC`, uso correcto de `CPPFLAGS`, `CFLAGS`, `LDFLAGS`, `LDLIBS`, y `.PHONY`.
- Compilación estricta: `-std=c11 -Wall -Wextra -Werror -pedantic` y otros warnings compatibles **configurados por compilador**; flags de Debug `-O0 -g` y Release `-O2`/`-O3` justificadas.
- Archivos de objetos en carpetas separadas por configuración y generación automática de dependencias con `-MMD -MP` si es compatible.
- Detección de SDL2 vía `pkg-config` cuando sea viable; describir la ruta Windows (MSYS2/MinGW, etc.), sin prometer portabilidad de Makefiles POSIX a MSVC.
- Sanitizers ASan/UBSan para GCC/Clang en plataformas compatibles y ausencia de `-march=native` por defecto en binarios redistribuibles.
- Alternativa CMake futura solo si facilita de verdad el empaquetado multiplataforma.

## 10. Calidad y pruebas automatizadas
Diseña una matriz de pruebas específica:
1. Tests de CPU para cada familia de opcodes y flags, PC y SP.
2. Tests de memoria: límites, carga de ROM, lectura-escritura, mapeo.
3. Tests de temporización con reloj simulado (sin esperar tiempo real).
4. Tests gráficos con framebuffer o hashes de referencia de creación propia.
5. Tests de entrada y audio mediante adaptadores fake.
6. ROMs de pruebas públicas y redistribuibles: documentar autor, licencia, origen y resultados; no incluir ROMs comerciales.
7. Tests de regresión, integración y comportamiento ante errores/archivos malformados.
8. Fuzzing opcional para loader/decode y sanitizers.
9. Benchmarks reproducibles por instrucciones/s, latencia de frame, memoria y consumo CPU, con configuración de hardware documentada.
10. CI para Linux, Windows y macOS según disponibilidad y presupuesto de mantenimiento.

Separa claramente **tests que se pueden ejecutar ya** de **tests planificados**; reporta fallos reales y cobertura pendiente.

## 11. Seguridad, aspectos legales y distribución Open Source
- No cargar ROM sin validar tamaño, extensión no confiable y límites de direccionamiento.
- Tratar ROMs y save files como entradas no confiables, evitando lecturas fuera de rango y enteros desbordados.
- Evitar suposiciones sobre permisos de archivos o rutas; documentar manejo de errores.
- No incluir ROMs/BIOS comerciales ni logotipos de fabricantes sin permiso.
- Generar documentación `README.md` (inglés), `README.es.md` (español), `CHANGELOG.md` (Keep a Changelog), `CONTRIBUTING.md`, `LICENSE` (texto oficial de la licencia seleccionada) y `NOTICE` si corresponde; no inventar atribuciones o titulares.
- README con instalación, compilación, uso, controles, compatibilidad, estado de funcionalidades, pruebas, limitaciones, capturas (solo si existen), licencias y roadmap.
- Reconocer marcas de terceros únicamente con atribuciones informativas apropiadas; proyecto no afiliado al fabricante.

## 12. Hoja de ruta por fases y criterios de aceptación
**Fase 0 — Especificación, ventana gráfica y demo**: CHIP-8, árbol, build, SDL2 inicializado, render de 64×32 escalado, demo propia visible, entrada y cierre de ventana; tests de humo sin ROM. Criterio: `make run` abre una ventana con contenido visible y responde a Escape; `make headless` funciona sin SDL2.

**Fase 1 — Núcleo mínimo verificable**: estado de máquina, ROM loader, CPU, memoria y fetch/decode/execute con subconjunto probado. Criterio: tests unitarios reproducibles, errores controlados.

**Fase 2 — Emulación básica observable**: video, teclado, temporizadores, scheduling y sonido mínimo en CHIP-8; para Game Boy, PPU/timers/interrupciones y componentes del bus por subfases. Criterio: tests de integración y programas de prueba propios.

**Fase 3 — Compatibilidad y depuración**: ampliar opcodes, quirks/MBC, trace/debugger, casos de regresión, ROMs de pruebas con permiso. Criterio: matriz de compatibilidad documentada, sin afirmaciones exageradas.

**Fase 4 — Estabilidad y distribución**: análisis de rendimiento, sanitizers, CI de plataformas, binarios y documentación completa. Criterio: build reproducible, sin errores conocidos de memoria y release documentada.

Incluye dependencias, riesgos, esfuerzo relativo y definición de terminado (DoD) por fase. Si el sistema elegido es Game Boy, desglosa de forma realista PPU/APU/MBC e interrupciones: no equiparar su alcance al de CHIP-8.

## 13. Entregables concretos de tu primera respuesta
**No respondas únicamente con teoría ni entregues un emulador supuestamente completo de una sola vez.** Presenta, en este orden:
1. Resumen ejecutivo de la plataforma seleccionada, alcance MVP, supuestos y no objetivos.
2. Comparación de decisiones arquitectónicas y justificación de tradeoffs.
3. Diagrama Mermaid del flujo y mapa de módulos.
4. Árbol de carpetas propuesto.
5. Código compilable inicial para `src/main.c`, `include/velot8/cpu.h`, `src/core/cpu.c`, `include/velot8/emulator.h`, `src/core/emulator.c`, y el mínimo adicional necesario; no dejar símbolos referenciados sin implementar.
6. `Makefile` completo que construya **obligatoriamente el frontend gráfico SDL2**, incluya `make run` con demo propia observable y permita modo headless separado para tests sin SDL2.
7. Tests iniciales de reset/estado, ROM loader e instrucción representativa si ya existe soporte; no escribir tests ficticios.
8. Comandos exactos para compilar, probar y **abrir la ventana con la demo visible** en entornos soportados; indicaciones si dependen de paquetes instalados.
9. Roadmap incremental con criterios de aceptación verificables.
10. Lista de decisiones pendientes y riesgos conocidos, priorizados.

**Regla de calidad:** cada archivo debe tener nombre y ruta claros, incluir el contenido íntegro, compilar con los demás archivos presentados y seguir C11 estricto; si falta una dependencia o parte del código, señalarlo expresamente. No mezclar fragmentos incompatibles ni pseudocódigo con código supuestamente ejecutable.

## 14. Criterio final de revisión
Antes de cerrar cada entrega, verifica y declara:
- ¿Compila en los compiladores realmente disponibles? ¿Qué comandos ejecutaste?
- ¿Hay advertencias con opciones estrictas?
- ¿Se puede ejecutar headless y probar la lógica sin interfaz gráfica?
- ¿Los ciclos y timers usan unidades correctas para la máquina elegida?
- ¿Existen rutas de error seguras para ROMs inválidas y operaciones fuera de rango?
- ¿Está realmente desacoplado el núcleo de SDL?
- ¿Documentación y licencia coinciden con el estado real del proyecto?
- ¿Qué queda pendiente, sin presentar lo incompleto como implementado?

Empieza con **CHIP-8** y el frontend gráfico SDL2 y desarrolla únicamente una primera iteración pequeña, funcional y comprobable. Si el usuario elige **Game Boy clásica**, cambia toda la especificación, estructuras y roadmap de forma consistente, sin trasplantar supuestos de CHIP-8.

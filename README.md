# py-sintax — Analizador Sintáctico Predictivo No Recursivo LL(1)

Analizador léxico y sintáctico predictivo no recursivo ($LL(1)$) desarrollado en **C++17** para validar un subconjunto del lenguaje **Python**, enfocado en el uso de las funciones de orden superior `map`, `filter` y `reduce`, expresiones lambda, definición de funciones (`def`), estructuras iterables (`list`, `tuple`, `range`) y expresiones aritméticas/relacionales con jerarquía de precedencia.

---

## Tabla de Contenidos
1. [Alcance del Lenguaje](#1-alcance-del-lenguaje)
2. [Arquitectura y Estructura del Proyecto](#2-arquitectura-y-estructura-del-proyecto)
3. [Especificación Gramatical LL(1)](#3-especificación-gramatical-ll1)
4. [Compilación y Uso](#4-compilación-y-uso)
5. [Salidas del Programa](#5-salidas-del-programa)
6. [Casos de Prueba](#6-casos-de-prueba)
7. [Créditos](#7-créditos)

---

## 1. Alcance del Lenguaje

El analizador valida archivos fuente `.py` comprobando su estructura léxica y sintáctica línea por línea lógica (incluyendo control de sangría mediante tokens sintéticos `INDENT`, `DEDENT` y `NEWLINE`).

### Sentencias Reconocidas
| Tipo de Sentencia | Forma Sintáctica | Ejemplo |
| :--- | :--- | :--- |
| **Asignación** | `id = valor` (lista, tupla, expresión o llamada) | `datos = [1, 2, 3]` |
| **Llamada funcional** | `map(f, it)` \| `filter(f, it)` \| `reduce(f, it[, init])` | `map(lambda x: x * 2, datos)` |
| **Definición (`def`)** | `def id(params):` + bloque indentado (`return expr` \| `pass`) | `def cuadrado(x):`<br>`    return x ** 2` |

* **Argumento función (`Fn`):** Expresión `lambda params: expr` o un identificador (`id`).
* **Iterables (`I`):** Identificadores (`id`), listas (`[...]`), tuplas (`(...)`) o `range(expr)`.

### Operadores y Precedencia (de menor a mayor)
1. **Relacionales (`rel`):** `==`, `!=`, `<`, `>`, `<=`, `>=`
2. **Aditivos:** `+`, `-` (asociativos por la izquierda)
3. **Multiplicativos:** `*`, `/`, `//`, `%` (asociativos por la izquierda)
4. **Unario:** `-` (menos unario, recursivo por la derecha)
5. **Potencia:** `**` (asociativo por la derecha, permite expresiones como `-x ** 2` y `2 ** -1`)

### Supuestos y Límites
* **Análisis estrictamente sintáctico:** No realiza verificaciones semánticas (aridad de funciones, existencia previa de identificadores o chequeo de tipos).
* **Control de sangría y saltos:** Ignora comentarios (`#`) y líneas en blanco. Los saltos de línea dentro de `(...)` o `[...]` no cierran la sentencia lógica. Mezclar tabuladores y espacios dispara un error léxico.
* **Exclusiones del subconjunto:** Se rechazan construcciones fuera de la gramática como `if`, `for`, `import`, `print`, cadenas de texto, booleanos (`True`, `False`, `None`), operador `+` unario, llamadas a funciones de usuario `f(x)` o llamadas anidadas como iterable.

---

## 2. Arquitectura y Estructura del Proyecto

El código es modular y *header-only* en su lógica interna, separando el vocabulario, el escaneo léxico, la definición gramatical, el motor de análisis y la generación de reportes:

```text
.
├── casos/                  # Batería de pruebas (5 casos aceptados, 12 rechazados, 1 traza)
│   ├── A1_ejemplo.py
│   ├── A2_asignaciones.py
│   ├── A3_def.py
│   ├── A4_aritmetica.py
│   ├── A5_vacio.py
│   ├── R1_falta_dosp.py ... R12_llamada_propia.py
│   └── T1_traza.py
├── dir/                    # Directorio de pruebas rápidas
│   ├── prog.py
│   └── prog.csv
├── headers/                # Módulos del analizador (C++17)
│   ├── tokens.h            # Vocabulario terminal (29 terminales + T_ERROR) y struct Token
│   ├── lexer.h             # AFD con tabla de transición + Scanner (INDENT/DEDENT/NEWLINE)
│   ├── gramatica.h         # 25 no terminales, 56 producciones LL(1) y tabla predictiva M
│   ├── parser.h            # Algoritmo predictivo no recursivo con pila explícita
│   └── informe.h           # Exportación de traza CSV (RFC 4180), errores en consola y tabla M
├── output/                 # Binarios compilados
│   └── main
├── main.cpp                # Punto de entrada y CLI
└── README.md
```

### Flujo de Ejecución
`Archivo .py` $\longrightarrow$ **`Scanner`** (`lexer.h`) $\longrightarrow$ `std::vector<Token>` (`tokens.h`) $\longrightarrow$ **`analizar()`** (`parser.h` + `gramatica.h`) $\longrightarrow$ `Resultado` $\longrightarrow$ **`informe.h`** (Consola + Traza `.csv`).

---

## 3. Especificación Gramatical LL(1)

La gramática libre de contexto fue transformada eliminando la recursividad por la izquierda y factorizando prefijos comunes. Consta de **25 símbolos no terminales**, **29 símbolos terminales** (incluyendo `$`) y **56 producciones**, generando una tabla predictiva $M$ con **114 celdas ocupadas y 0 conflictos LL(1)**.

<details>
<summary><b>Ver las 56 producciones de la Gramática LL(1)</b></summary>

1. `Prog -> Sent Prog`
2. `Prog -> eps`
3. `Sent -> id = Val NEWLINE`
4. `Sent -> Llam NEWLINE`
5. `Sent -> def id ( P ) : NEWLINE INDENT Bloque NEWLINE DEDENT`
6. `Val -> [ L ]`
7. `Val -> Llam`
8. `Val -> E`
9. `Llam -> map ( Fn , I )`
10. `Llam -> filter ( Fn , I )`
11. `Llam -> reduce ( Fn , I O )`
12. `O -> , E`
13. `O -> eps`
14. `Fn -> lambda P : E`
15. `Fn -> id`
16. `P -> id P'`
17. `P -> eps`
18. `P' -> , P`
19. `P' -> eps`
20. `I -> id`
21. `I -> [ L ]`
22. `I -> range ( E )`
23. `I -> ( Par`
24. `L -> E L'`
25. `L -> eps`
26. `L' -> , L`
27. `L' -> eps`
28. `E -> A E'`
29. `E' -> rel A E'`
30. `E' -> eps`
31. `A -> T A'`
32. `A' -> + T A'`
33. `A' -> - T A'`
34. `A' -> eps`
35. `T -> U T'`
36. `T' -> * U T'`
37. `T' -> / U T'`
38. `T' -> // U T'`
39. `T' -> % U T'`
40. `T' -> eps`
41. `U -> - U`
42. `U -> Pot`
43. `Pot -> F Pot'`
44. `Pot' -> ** U`
45. `Pot' -> eps`
46. `F -> ( Par`
47. `F -> id`
48. `F -> num`
49. `Par -> )`
50. `Par -> E Cola`
51. `Cola -> )`
52. `Cola -> , Resto`
53. `Resto -> )`
54. `Resto -> E Cola`
55. `Bloque -> return E`
56. `Bloque -> pass`

</details>

---

## 4. Compilación y Uso

### Requisitos
* Compilador C++ con soporte para **C++17** (`g++`, `clang++` o MSVC).
* Sin dependencias externas (100% biblioteca estándar).

### Compilar
En **Linux / macOS**:
```bash
g++ -std=c++17 -Wall -Wextra -o ./output/main main.cpp
```

En **Windows (MinGW)**:
```bash
g++ -std=c++17 -Wall -Wextra -o ./output/main.exe main.cpp
```

### Modos de Ejecución
1. **Analizar un archivo `.py`** (genera automáticamente el `.csv` junto al archivo de entrada):
   ```bash
   ./output/main casos/A1_ejemplo.py
   ```
2. **Especificar ruta de salida para la traza CSV:**
   ```bash
   ./output/main casos/A1_ejemplo.py salida_traza.csv
   ```
3. **Incluir todos los tokens restantes en la columna `entrada` del CSV** (por defecto se trunca a 60 tokens):
   ```bash
   ./output/main --entrada-completa casos/T1_traza.py
   ```
4. **Imprimir la tabla de análisis sintáctico $M$ en consola:**
   ```bash
   ./output/main --tabla
   ```

### Códigos de Salida (Exit Codes)
* `0`: Análisis exitoso (programa sintácticamente válido o ejecución de `--tabla`).
* `1`: Error léxico o sintáctico detectado en el código fuente.
* `2`: Error de uso en línea de comandos o archivo de entrada inaccesible/binario.

---

## 5. Salidas del Programa

### 1. Veredicto en Consola
Si el archivo es válido:
```text
Analisis exitoso: 'casos/A1_ejemplo.py' es sintacticamente valido.
```
Si ocurre un error léxico o sintáctico, indica línea, columna, causa, tokens esperados y señala el punto exacto con `^`:
```text
Error sintactico en la linea 1, columna 14: no se esperaba el identificador 'x'; se esperaba una de estas opciones: ')', ',', ':'
  1 | map(lambda x x * 2, lista)
    |              ^
```

### 2. Traza de Ejecución CSV
El archivo `.csv` generado contiene 5 columnas (`paso,linea,pila,entrada,accion`) siguiendo el formato clásico de análisis $LL(1)$ con el tope de la pila a la derecha:

| paso | linea | pila | entrada | accion |
| :---: | :---: | :--- | :--- | :--- |
| 1 | 1 | `$ Prog` \vert{} `map ( lambda id : id * num , id ) NEWLINE $` | `Prog -> Sent Prog` |
| 2 | 1 | `$ Prog Sent` | `map ( lambda id : id * num , id ) NEWLINE $` | `Sent -> Llam NEWLINE` |
| 3 | 1 | `$ Prog NEWLINE Llam` | `map ( lambda id : id * num , id ) NEWLINE $` \vert{} `Llam -> map ( Fn , I )` \vert{} \vert{} 4 \vert{} 1 \vert{} `$ Prog NEWLINE ) I , Fn ( map` | `map ( lambda id : id * num , id ) NEWLINE $` | *(empareja `map`)* |

---

## 6. Casos de Prueba

El directorio `casos/` incluye los scripts de validación del analizador:

### Casos Aceptados (`A1` – `A5`)
* **`A1_ejemplo.py`**: Ejemplo base con lista, `map`, `filter`, `reduce` y `def` con `**` (198 pasos).
* **`A2_asignaciones.py`**: Listas y tuplas con coma final, comentarios, `//`, `%`, `==`, `range` y `reduce` con valor inicial asignados a variables (276 pasos).
* **`A3_def.py`**: Definiciones de funciones con cuerpo `return` y `pass` (55 pasos).
* **`A4_aritmetica.py`**: Precedencia y asociatividad en `-x ** 2`, `2 ** -1`, paréntesis, `//`, `%` y `>=` (149 pasos).
* **`A5_vacio.py`**: Archivo válido compuesto únicamente por comentarios y saltos de línea (1 paso).
* **`T1_traza.py`**: Sentencia única `map(lambda x: x * 2, lista)` para inspección de traza completa (35 pasos).

### Casos Rechazados (`R1` – `R12`)
| Archivo | Prueba de Fallo | Tipo de Error | Ubicación |
| :--- | :--- | :---: | :---: |
| `R1_falta_dosp.py` | Falta `:` en expresión `lambda` | Sintáctico | Línea 1, Col 14 |
| `R2_if.py` | Uso de estructura `if` fuera del subconjunto | Sintáctico | Línea 2, Col 4 |
| `R3_simbolo.py` | Carácter `@` no reconocido | Léxico | Línea 1, Col 7 |
| `R4_numero.py` | Literal numérico mal formado (`3.`) | Léxico | Línea 1, Col 5 |
| `R5_def_sin_return.py` | Cuerpo de `def` sin `return` ni `pass` | Sintáctico | Línea 2, Col 5 |
| `R6_parentesis.py` | Paréntesis de llamada sin cerrar | Sintáctico | Línea 1, Col 27 |
| `R7_sangria.py` | Nivel de sangría inesperado | Sintáctico | Línea 2, Col 1 |
| `R8_mezcla.py` | Mezcla de tabuladores y espacios en sangría | Léxico | Línea 3, Col 2 |
| `R9_mas_unario.py` | Operador `+` unario no soportado | Sintáctico | Línea 1, Col 5 |
| `R10_coma_doble.py` | Coma doble consecutiva `[1,, 2]` | Sintáctico | Línea 1, Col 13 |
| `R11_incompleto.py` | Expresión aritmética incompleta (`a = 1 +`) | Sintáctico | Línea 1, Col 8 |
| `R12_llamada_propia.py` | Llamada directa a función propia `cuadrado(3)` | Sintáctico | Línea 1, Col 21 |

---

## 7. Créditos

Proyecto académico desarrollado para la asignatura de **Lenguajes y Autómatas I** en el **Tecnológico Nacional de México — Instituto Tecnológico de Tuxtla Gutiérrez** (Ingeniería en Sistemas Computacionales).

* **Autores:**
  * Balderas Osorio Rafael de Jesús
  * Morales Jiménez Alexis Emiliano
  * Soancatl Niño Adrik Said

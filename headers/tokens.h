#pragma once
// tokens.h - Vocabulario terminal de la gramatica.
// Lo comparten el lexer (que produce tokens) y la gramatica/analizador (que los consumen).
// Todo texto que el programa imprime es ASCII puro (sin acentos ni simbolos especiales).

#include <cstddef>
#include <cstdint>
#include <string>

/// @brief Terminales de la gramatica. Son las columnas de la tabla M.
enum Term : uint8_t
{
    T_MAP,
    T_FILTER,
    T_REDUCE,
    T_LAMBDA,
    T_RANGE,
    T_DEF,
    T_RETURN,
    T_PASS,    // Agregado por el documento para "BLOQUE_FUNC -> pass"
    T_ID,
    T_NUM,
    T_LP,      // (
    T_RP,      // )
    T_LB,      // [
    T_RB,      // ]
    T_COMA,    // ,
    T_DOSP,    // :
    T_ASIG,    // =
    T_MAS,     // +
    T_MENOS,   // -
    T_POR,     // *
    T_DIV,     // /
    T_IDIV,    // //
    T_MOD,     // %
    T_POT,     // **
    T_REL,     // == != < > <= >=
    T_NEWLINE, // fin de una línea lógica (\n)
    T_INDENT,  // aumento de sangría (\t)
    T_DEDENT,  // disminución de sangría
    T_EOF,     // $
    T_ERROR,   // error lexico (lo produce el lexer; no esta en la gramatica)
    N_TERM
};

/// @brief Nombre de cada terminal tal como aparece en la traza y en el reporte.
inline constexpr const char *NOMBRE_T[N_TERM] = {
    "map", "filter", "reduce", "lambda", "range", "def", "return", "pass", "id", "num",
    "(", ")", "[", "]", ",", ":", "=", "+", "-", "*", "/", "%", "rel",
    "NEWLINE", "INDENT", "DEDENT", "$", "ERROR"};

/// @brief Descripcion para mensajes de error dirigidos al usuario.
inline constexpr const char *LEGIBLE_T[N_TERM] = {
    "'map'", "'filter'", "'reduce'", "'lambda'", "'range'", "'def'", "'return'", "'pass'",
    "un identificador", "un numero",
    "'('", "')'", "'['", "']'", "','", "':'", "'='", "'+'", "'-'", "'*'", "'/'", "'//'", "'%'", "'**'",
    "un operador de comparacion",
    "el fin de la linea", "una sangria", "el fin del bloque", "el fin del archivo",
    "un simbolo no valido"};

/// @brief Un token con su posicion en el archivo (linea y columna, base 1).
struct Token
{
    Term tipo;
    std::string lexema;
    std::size_t linea;
    std::size_t col;
    std::string mensaje; // solo para T_ERROR: descripcion del problema lexico
};
#pragma once
// tokens.h — Vocabulario terminal de la gramática.
// Lo comparten el lexer (que produce tokens) y la gramática/analizador (que los consumen).

#include <cstddef>
#include <cstdint>
#include <string>

/// @brief Terminales de la gramática. Son las columnas de la tabla M.
enum Term : uint8_t
{
    T_MAP,
    T_FILTER,
    T_REDUCE,
    T_LAMBDA,
    T_RANGE,
    T_DEF,
    T_RETURN,
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
    T_MOD,     // %
    T_REL,     // == != < > <= >=
    T_NEWLINE, // fin de una línea lógica
    T_INDENT,  // aumento de sangría
    T_DEDENT,  // disminución de sangría
    T_EOF,     // $
    T_ERROR,   // error léxico (lo produce el lexer; no está en la gramática)
    N_TERM
};

/// @brief Nombre de cada terminal tal como aparece en la traza y en el reporte.
inline constexpr const char *NOMBRE_T[N_TERM] = {
    "map", "filter", "reduce", "lambda", "range", "def", "return", "id", "num",
    "(", ")", "[", "]", ",", ":", "=", "+", "-", "*", "/", "%", "rel",
    "NEWLINE", "INDENT", "DEDENT", "$", "ERROR"};

/// @brief Descripción para mensajes de error dirigidos al usuario.
inline constexpr const char *LEGIBLE_T[N_TERM] = {
    "'map'", "'filter'", "'reduce'", "'lambda'", "'range'", "'def'", "'return'",
    "un identificador", "un numero",
    "'('", "')'", "'['", "']'", "','", "':'", "'='", "'+'", "'-'", "'*'", "'/'", "'%'",
    "un operador de comparación",
    "el fin de la línea", "una sangría", "el fin del bloque", "el fin del archivo",
    "un símbolo no válido"};

/// @brief Un token con su posición en el archivo (línea y columna, base 1).
struct Token
{
    Term tipo;
    std::string lexema;
    std::size_t linea;
    std::size_t col;
    std::string mensaje; // solo para T_ERROR: descripción del problema léxico
};
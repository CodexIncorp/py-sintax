#pragma once
// tokens.h - Vocabulario terminal de la gramática.
// Compartido por el lexer y el analizador sintáctico. Salida en ASCII puro.

#include <cstddef>
#include <cstdint>
#include <string>

// Terminales de la gramática (columnas de la tabla M).
enum Term : uint8_t
{
    T_MAP,
    T_FILTER,
    T_REDUCE,
    T_LAMBDA,
    T_RANGE,
    T_DEF,
    T_RETURN,
    T_PASS,    // 'pass' (cuerpo de función vacío)
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
    T_NEWLINE, // fin de una línea lógica
    T_INDENT,  // aumento de sangría
    T_DEDENT,  // disminución de sangría
    T_EOF,     // $
    T_ERROR,   // error léxico (interno del lexer; no está en la gramática)
    N_TERM
};

// Nombre de cada terminal para la traza y el reporte.
inline constexpr const char *NOMBRE_T[N_TERM] = {
    "map", "filter", "reduce", "lambda", "range", "def", "return", "pass", "id", "num",
    "(", ")", "[", "]", ",", ":", "=", "+", "-", "*", "/", "//", "%", "**", "rel",
    "NEWLINE", "INDENT", "DEDENT", "$", "ERROR"};

// Descripciones legibles para mensajes de error.
inline constexpr const char *LEGIBLE_T[N_TERM] = {
    "'map'", "'filter'", "'reduce'", "'lambda'", "'range'", "'def'", "'return'", "'pass'",
    "un identificador", "un numero",
    "'('", "')'", "'['", "']'", "','", "':'", "'='", "'+'", "'-'", "'*'", "'/'", "'//'", "'%'", "'**'",
    "un operador de comparacion",
    "el fin de la linea", "una sangria", "el fin del bloque", "el fin del archivo",
    "un simbolo no valido"};

// Token con su posición en el archivo fuente (base 1).
struct Token
{
    Term tipo;
    std::string lexema;
    std::size_t linea;
    std::size_t col;
    std::string mensaje; // Solo para T_ERROR: descripción del problema léxico
};
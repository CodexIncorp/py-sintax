#pragma once
// tokens.h — Vocabulario terminal de la gramática.
// Lo comparten el lexer (que produce tokens) y la gramática/parser (que los consume).

#include <cstddef>
#include <cstdint>
#include <string>

/// @brief Terminales de la gramática. Son las columnas de la tabla M del parser.
enum Term : uint8_t
{
    T_MAP,
    T_FILTER,
    T_REDUCE,
    T_LAMBDA,
    T_RANGE,
    T_ID,
    T_NUM,
    T_LP,    // (
    T_RP,    // )
    T_LB,    // [
    T_RB,    // ]
    T_COMA,  // ,
    T_DOSP,  // :
    T_MAS,   // +
    T_MENOS, // -
    T_POR,   // *
    T_DIV,   // /
    T_MOD,   // %
    T_REL,   // == != < > <= >=
    T_EOF,   // $
    T_ERROR, // símbolo inválido (lo produce el lexer; no está en la gramática)
    N_TERM
};

/// @brief Nombre imprimible de cada terminal (para la traza y los mensajes).
inline constexpr const char *NOMBRE_T[N_TERM] = {
    "map", "filter", "reduce", "lambda", "range", "id", "num",
    "(", ")", "[", "]", ",", ":", "+", "-", "*", "/", "%",
    "rel", "$", "ERROR"};

/// @brief Un token: tipo, texto original y columna (base 1) para mensajes de error.
struct Token
{
    Term tipo;
    std::string lexema;
    std::size_t col;
};
#pragma once
// gramatica.h — La gramática LL(1) ya sin recursividad por la izquierda:
// no terminales, producciones numeradas (1-32) y tabla de análisis M.
// Es la transcripción directa de las actividades 4.1 y 4.3 del reporte.

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "tokens.h"

/// @brief No terminales. Son las filas de la tabla M.
enum NoTerm : uint8_t
{
    NT_S,
    NT_O,
    NT_FN,
    NT_P,
    NT_PP, // P'
    NT_I,
    NT_L,
    NT_LP, // L'
    NT_E,
    NT_EP, // E'
    NT_A,
    NT_AP, // A'
    NT_T,
    NT_TP, // T'
    NT_F,
    N_NOTERM
};

inline constexpr const char *NOMBRE_NT[N_NOTERM] = {
    "S", "O", "Fn", "P", "P'", "I", "L", "L'",
    "E", "E'", "A", "A'", "T", "T'", "F"};

/// @brief Símbolo de una producción: terminal o no terminal.
struct Simbolo
{
    bool esTerm;
    uint8_t id; // Term si esTerm, NoTerm si no
};

/// @brief Producción izq → der. Una parte derecha vacía es ε.
struct Produccion
{
    NoTerm izq;
    std::vector<Simbolo> der;
};

constexpr Simbolo term(Term t) { return {true, t}; }
constexpr Simbolo nt(NoTerm n) { return {false, n}; }

/// @brief Producciones. El índice es el número de la producción en el reporte (1-32).
/// El índice 0 es un relleno: en M, el valor 0 significa "celda vacía" (error).
inline const std::vector<Produccion> PROD = {
    /*  0 */ {NT_S, {}}, // relleno, no es una producción real
    /*  1 */ {NT_S, {term(T_MAP), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), term(T_RP)}},
    /*  2 */ {NT_S, {term(T_FILTER), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), term(T_RP)}},
    /*  3 */ {NT_S, {term(T_REDUCE), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), nt(NT_O), term(T_RP)}},
    /*  4 */ {NT_O, {term(T_COMA), nt(NT_E)}},
    /*  5 */ {NT_O, {}},
    /*  6 */ {NT_FN, {term(T_LAMBDA), nt(NT_P), term(T_DOSP), nt(NT_E)}},
    /*  7 */ {NT_FN, {term(T_ID)}},
    /*  8 */ {NT_P, {term(T_ID), nt(NT_PP)}},
    /*  9 */ {NT_PP, {term(T_COMA), term(T_ID), nt(NT_PP)}},
    /* 10 */ {NT_PP, {}},
    /* 11 */ {NT_I, {term(T_ID)}},
    /* 12 */ {NT_I, {term(T_LB), nt(NT_L), term(T_RB)}},
    /* 13 */ {NT_I, {term(T_RANGE), term(T_LP), nt(NT_E), term(T_RP)}},
    /* 14 */ {NT_L, {nt(NT_E), nt(NT_LP)}},
    /* 15 */ {NT_L, {}},
    /* 16 */ {NT_LP, {term(T_COMA), nt(NT_E), nt(NT_LP)}},
    /* 17 */ {NT_LP, {}},
    /* 18 */ {NT_E, {nt(NT_A), nt(NT_EP)}},
    /* 19 */ {NT_EP, {term(T_REL), nt(NT_A), nt(NT_EP)}},
    /* 20 */ {NT_EP, {}},
    /* 21 */ {NT_A, {nt(NT_T), nt(NT_AP)}},
    /* 22 */ {NT_AP, {term(T_MAS), nt(NT_T), nt(NT_AP)}},
    /* 23 */ {NT_AP, {term(T_MENOS), nt(NT_T), nt(NT_AP)}},
    /* 24 */ {NT_AP, {}},
    /* 25 */ {NT_T, {nt(NT_F), nt(NT_TP)}},
    /* 26 */ {NT_TP, {term(T_POR), nt(NT_F), nt(NT_TP)}},
    /* 27 */ {NT_TP, {term(T_DIV), nt(NT_F), nt(NT_TP)}},
    /* 28 */ {NT_TP, {term(T_MOD), nt(NT_F), nt(NT_TP)}},
    /* 29 */ {NT_TP, {}},
    /* 30 */ {NT_F, {term(T_LP), nt(NT_E), term(T_RP)}},
    /* 31 */ {NT_F, {term(T_ID)}},
    /* 32 */ {NT_F, {term(T_NUM)}},
};

/// @brief Tabla de análisis: M[no terminal][terminal] = número de producción (0 = error).
using TablaM = std::array<std::array<uint8_t, N_TERM>, N_NOTERM>;

/// @brief Llena M igual que la tabla de la actividad 4.3: una línea por celda agrupada.
inline TablaM crearTablaM()
{
    TablaM M{}; // todo en 0 = celda vacía
    auto fila = [&M](NoTerm A, std::initializer_list<Term> columnas, uint8_t produccion)
    {
        for (Term a : columnas)
            M[A][a] = produccion;
    };

    // Inicios de una expresión: lo que puede empezar E, A, T o F
    const auto INICIO_EXPR = {T_ID, T_NUM, T_LP};
    // Lo que puede seguir a una expresión completa: SIGUIENTE(E)
    const auto FIN_EXPR = {T_COMA, T_RP, T_RB};

    fila(NT_S, {T_MAP}, 1);
    fila(NT_S, {T_FILTER}, 2);
    fila(NT_S, {T_REDUCE}, 3);

    fila(NT_O, {T_COMA}, 4);
    fila(NT_O, {T_RP}, 5);

    fila(NT_FN, {T_LAMBDA}, 6);
    fila(NT_FN, {T_ID}, 7);

    fila(NT_P, {T_ID}, 8);

    fila(NT_PP, {T_COMA}, 9);
    fila(NT_PP, {T_DOSP}, 10);

    fila(NT_I, {T_ID}, 11);
    fila(NT_I, {T_LB}, 12);
    fila(NT_I, {T_RANGE}, 13);

    fila(NT_L, INICIO_EXPR, 14);
    fila(NT_L, {T_RB}, 15);

    fila(NT_LP, {T_COMA}, 16);
    fila(NT_LP, {T_RB}, 17);

    fila(NT_E, INICIO_EXPR, 18);

    fila(NT_EP, {T_REL}, 19);
    fila(NT_EP, FIN_EXPR, 20);

    fila(NT_A, INICIO_EXPR, 21);

    fila(NT_AP, {T_MAS}, 22);
    fila(NT_AP, {T_MENOS}, 23);
    fila(NT_AP, {T_REL, T_COMA, T_RP, T_RB}, 24);

    fila(NT_T, INICIO_EXPR, 25);

    fila(NT_TP, {T_POR}, 26);
    fila(NT_TP, {T_DIV}, 27);
    fila(NT_TP, {T_MOD}, 28);
    fila(NT_TP, {T_MAS, T_MENOS, T_REL, T_COMA, T_RP, T_RB}, 29);

    fila(NT_F, {T_LP}, 30);
    fila(NT_F, {T_ID}, 31);
    fila(NT_F, {T_NUM}, 32);

    return M;
}

inline const TablaM M = crearTablaM();
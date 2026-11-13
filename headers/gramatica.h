#pragma once
// gramatica.h — La gramática LL(1) ya sin recursividad por la izquierda:
// no terminales, producciones numeradas (1-54) y tabla de análisis M.
// Es la transcripción directa de las actividades 4.1 y 4.3 del reporte.
// (Generado a partir de la gramática y verificado contra un cálculo independiente
//  de PRIMERO/SIGUIENTE: 112 celdas, 0 conflictos LL(1).)

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "tokens.h"

/// @brief No terminales. Son las filas de la tabla M.
enum NoTerm : uint8_t
{
    NT_PROG,
    NT_SENT,
    NT_VAL,
    NT_LLAM,
    NT_O,
    NT_FN,
    NT_P,
    NT_PP,
    NT_I,
    NT_L,
    NT_LP,
    NT_E,
    NT_EP,
    NT_A,
    NT_AP,
    NT_T,
    NT_TP,
    NT_U,
    NT_POT,
    NT_POTP,
    NT_F,
    NT_PAR,
    NT_COLA,
    NT_RESTO,
    N_NOTERM
};

inline constexpr const char *NOMBRE_NT[N_NOTERM] = {
    "Prog", "Sent", "Val", "Llam", "O", "Fn", "P", "P'", "I", "L", "L'", "E", "E'", "A", "A'", "T", "T'", "U", "Pot", "Pot'", "F", "Par", "Cola", "Resto"};

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

/// @brief Producciones. El índice es el número de la producción en el reporte (1-54).
/// El índice 0 es un relleno: en M, el valor 0 significa "celda vacía" (error).
inline const std::vector<Produccion> PROD = {
    /*  0 */ {NT_PROG, {}}, // relleno, no es una producción real
    /*  1 */ {NT_PROG, {nt(NT_SENT), nt(NT_PROG)}},
    /*  2 */ {NT_PROG, {}},
    /*  3 */ {NT_SENT, {term(T_ID), term(T_ASIG), nt(NT_VAL), term(T_NEWLINE)}},
    /*  4 */ {NT_SENT, {nt(NT_LLAM), term(T_NEWLINE)}},
    /*  5 */ {NT_SENT, {term(T_DEF), term(T_ID), term(T_LP), nt(NT_P), term(T_RP), term(T_DOSP), term(T_NEWLINE), term(T_INDENT), term(T_RETURN), nt(NT_E), term(T_NEWLINE), term(T_DEDENT)}},
    /*  6 */ {NT_VAL, {term(T_LB), nt(NT_L), term(T_RB)}},
    /*  7 */ {NT_VAL, {nt(NT_LLAM)}},
    /*  8 */ {NT_VAL, {nt(NT_E)}},
    /*  9 */ {NT_LLAM, {term(T_MAP), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), term(T_RP)}},
    /* 10 */ {NT_LLAM, {term(T_FILTER), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), term(T_RP)}},
    /* 11 */ {NT_LLAM, {term(T_REDUCE), term(T_LP), nt(NT_FN), term(T_COMA), nt(NT_I), nt(NT_O), term(T_RP)}},
    /* 12 */ {NT_O, {term(T_COMA), nt(NT_E)}},
    /* 13 */ {NT_O, {}},
    /* 14 */ {NT_FN, {term(T_LAMBDA), nt(NT_P), term(T_DOSP), nt(NT_E)}},
    /* 15 */ {NT_FN, {term(T_ID)}},
    /* 16 */ {NT_P, {term(T_ID), nt(NT_PP)}},
    /* 17 */ {NT_P, {}},
    /* 18 */ {NT_PP, {term(T_COMA), nt(NT_P)}},
    /* 19 */ {NT_PP, {}},
    /* 20 */ {NT_I, {term(T_ID)}},
    /* 21 */ {NT_I, {term(T_LB), nt(NT_L), term(T_RB)}},
    /* 22 */ {NT_I, {term(T_RANGE), term(T_LP), nt(NT_E), term(T_RP)}},
    /* 23 */ {NT_I, {term(T_LP), nt(NT_PAR)}},
    /* 24 */ {NT_L, {nt(NT_E), nt(NT_LP)}},
    /* 25 */ {NT_L, {}},
    /* 26 */ {NT_LP, {term(T_COMA), nt(NT_L)}},
    /* 27 */ {NT_LP, {}},
    /* 28 */ {NT_E, {nt(NT_A), nt(NT_EP)}},
    /* 29 */ {NT_EP, {term(T_REL), nt(NT_A), nt(NT_EP)}},
    /* 30 */ {NT_EP, {}},
    /* 31 */ {NT_A, {nt(NT_T), nt(NT_AP)}},
    /* 32 */ {NT_AP, {term(T_MAS), nt(NT_T), nt(NT_AP)}},
    /* 33 */ {NT_AP, {term(T_MENOS), nt(NT_T), nt(NT_AP)}},
    /* 34 */ {NT_AP, {}},
    /* 35 */ {NT_T, {nt(NT_U), nt(NT_TP)}},
    /* 36 */ {NT_TP, {term(T_POR), nt(NT_U), nt(NT_TP)}},
    /* 37 */ {NT_TP, {term(T_DIV), nt(NT_U), nt(NT_TP)}},
    /* 38 */ {NT_TP, {term(T_IDIV), nt(NT_U), nt(NT_TP)}},
    /* 39 */ {NT_TP, {term(T_MOD), nt(NT_U), nt(NT_TP)}},
    /* 40 */ {NT_TP, {}},
    /* 41 */ {NT_U, {term(T_MENOS), nt(NT_U)}},
    /* 42 */ {NT_U, {nt(NT_POT)}},
    /* 43 */ {NT_POT, {nt(NT_F), nt(NT_POTP)}},
    /* 44 */ {NT_POTP, {term(T_POT), nt(NT_U)}},
    /* 45 */ {NT_POTP, {}},
    /* 46 */ {NT_F, {term(T_LP), nt(NT_PAR)}},
    /* 47 */ {NT_F, {term(T_ID)}},
    /* 48 */ {NT_F, {term(T_NUM)}},
    /* 49 */ {NT_PAR, {term(T_RP)}},
    /* 50 */ {NT_PAR, {nt(NT_E), nt(NT_COLA)}},
    /* 51 */ {NT_COLA, {term(T_RP)}},
    /* 52 */ {NT_COLA, {term(T_COMA), nt(NT_RESTO)}},
    /* 53 */ {NT_RESTO, {term(T_RP)}},
    /* 54 */ {NT_RESTO, {nt(NT_E), nt(NT_COLA)}},
};

/// @brief Tabla de análisis: M[no terminal][terminal] = número de producción (0 = error).
using TablaM = std::array<std::array<uint8_t, N_TERM>, N_NOTERM>;

/// @brief Llena M igual que la tabla de la actividad 4.3: cada línea es "no terminal,
/// terminales de entrada, producción a aplicar". Un ε se aplica con SIGUIENTE.
inline TablaM crearTablaM()
{
    TablaM M{}; // todo en 0 = celda vacía
    auto fila = [&M](NoTerm A, std::initializer_list<Term> columnas, uint8_t produccion)
    {
        for (Term a : columnas)
            M[A][a] = produccion;
    };

    fila(NT_PROG, {T_MAP, T_FILTER, T_REDUCE, T_DEF, T_ID}, 1);
    fila(NT_PROG, {T_EOF}, 2);
    fila(NT_SENT, {T_ID}, 3);
    fila(NT_SENT, {T_MAP, T_FILTER, T_REDUCE}, 4);
    fila(NT_SENT, {T_DEF}, 5);
    fila(NT_VAL, {T_LB}, 6);
    fila(NT_VAL, {T_MAP, T_FILTER, T_REDUCE}, 7);
    fila(NT_VAL, {T_ID, T_NUM, T_LP, T_MENOS}, 8);
    fila(NT_LLAM, {T_MAP}, 9);
    fila(NT_LLAM, {T_FILTER}, 10);
    fila(NT_LLAM, {T_REDUCE}, 11);
    fila(NT_O, {T_COMA}, 12);
    fila(NT_O, {T_RP}, 13);
    fila(NT_FN, {T_LAMBDA}, 14);
    fila(NT_FN, {T_ID}, 15);
    fila(NT_P, {T_ID}, 16);
    fila(NT_P, {T_RP, T_DOSP}, 17);
    fila(NT_PP, {T_COMA}, 18);
    fila(NT_PP, {T_RP, T_DOSP}, 19);
    fila(NT_I, {T_ID}, 20);
    fila(NT_I, {T_LB}, 21);
    fila(NT_I, {T_RANGE}, 22);
    fila(NT_I, {T_LP}, 23);
    fila(NT_L, {T_ID, T_NUM, T_LP, T_MENOS}, 24);
    fila(NT_L, {T_RB}, 25);
    fila(NT_LP, {T_COMA}, 26);
    fila(NT_LP, {T_RB}, 27);
    fila(NT_E, {T_ID, T_NUM, T_LP, T_MENOS}, 28);
    fila(NT_EP, {T_REL}, 29);
    fila(NT_EP, {T_RP, T_RB, T_COMA, T_NEWLINE}, 30);
    fila(NT_A, {T_ID, T_NUM, T_LP, T_MENOS}, 31);
    fila(NT_AP, {T_MAS}, 32);
    fila(NT_AP, {T_MENOS}, 33);
    fila(NT_AP, {T_RP, T_RB, T_COMA, T_REL, T_NEWLINE}, 34);
    fila(NT_T, {T_ID, T_NUM, T_LP, T_MENOS}, 35);
    fila(NT_TP, {T_POR}, 36);
    fila(NT_TP, {T_DIV}, 37);
    fila(NT_TP, {T_IDIV}, 38);
    fila(NT_TP, {T_MOD}, 39);
    fila(NT_TP, {T_RP, T_RB, T_COMA, T_MAS, T_MENOS, T_REL, T_NEWLINE}, 40);
    fila(NT_U, {T_MENOS}, 41);
    fila(NT_U, {T_ID, T_NUM, T_LP}, 42);
    fila(NT_POT, {T_ID, T_NUM, T_LP}, 43);
    fila(NT_POTP, {T_POT}, 44);
    fila(NT_POTP, {T_RP, T_RB, T_COMA, T_MAS, T_MENOS, T_POR, T_DIV, T_IDIV, T_MOD, T_REL, T_NEWLINE}, 45);
    fila(NT_F, {T_LP}, 46);
    fila(NT_F, {T_ID}, 47);
    fila(NT_F, {T_NUM}, 48);
    fila(NT_PAR, {T_RP}, 49);
    fila(NT_PAR, {T_ID, T_NUM, T_LP, T_MENOS}, 50);
    fila(NT_COLA, {T_RP}, 51);
    fila(NT_COLA, {T_COMA}, 52);
    fila(NT_RESTO, {T_RP}, 53);
    fila(NT_RESTO, {T_ID, T_NUM, T_LP, T_MENOS}, 54);

    return M;
}

inline const TablaM M = crearTablaM();
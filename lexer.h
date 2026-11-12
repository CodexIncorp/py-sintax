#pragma once
// lexer.h — Analizador léxico (autómata finito con tabla de transición).
// Header-only: se incluye únicamente desde parser.cpp.

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include "tokens.h"

namespace lex_impl
{
    /// @brief Clases de carácter; cada valor es una columna de TABLA.
    enum Clase : uint8_t
    {
        LETRA,
        DIGITO,
        PARENI,
        PARENF,
        CORCHI,
        CORCHD,
        COMA,
        DOSP,
        MAS,
        MENOS,
        POR,
        DIV,
        MOD,
        IGUAL,
        ADMIR,
        MENOR,
        MAYOR,
        PUNTO,
        ESPACIO,
        FIN,
        OTRO,
        N_CLASE
    };

    /// Estados finales: >= 500.
    ///   500 + t : token t reconocido; el último carácter leído NO es parte del token (se retrocede)
    ///   600 + t : token t reconocido; el último carácter leído SÍ es parte del token
    constexpr int16_t FIN_MIN = 500;
    constexpr int16_t BASE_CONS = 600;

    constexpr int16_t RETRO(Term t) { return 500 + t; }
    constexpr int16_t CONS(Term t) { return BASE_CONS + t; }

    /// Estados no finales
    enum Estado : int16_t
    {
        E_INICIO = 0,
        E_ID = 1,     // identificador o palabra reservada
        E_ENTERO = 2, // dígitos
        E_PUNTO = 3,  // dígitos '.'  (falta al menos un dígito)
        E_DECIMAL = 4,
        E_IGUAL = 5, // '='  (solo vale "==")
        E_ADMIR = 6, // '!'  (solo vale "!=")
        E_MENOR = 7, // '<'  ("<" o "<=")
        E_MAYOR = 8, // '>'  (">" o ">=")
        N_ESTADO
    };

    using Tabla = std::array<std::array<int16_t, N_CLASE>, N_ESTADO>;

    /// @brief Función de transición: TABLA[estado][clase] = estado siguiente.
    constexpr Tabla crearTabla()
    {
        Tabla t{};

        // Valores por defecto de cada fila
        for (auto &x : t[E_INICIO])
            x = CONS(T_ERROR); // carácter inválido: se consume
        for (auto &x : t[E_ID])
            x = RETRO(T_ID);
        for (auto &x : t[E_ENTERO])
            x = RETRO(T_NUM);
        for (auto &x : t[E_PUNTO])
            x = RETRO(T_ERROR); // "3." sin dígitos después
        for (auto &x : t[E_DECIMAL])
            x = RETRO(T_NUM);
        for (auto &x : t[E_IGUAL])
            x = RETRO(T_ERROR); // '=' solo, no pertenece a la gramática
        for (auto &x : t[E_ADMIR])
            x = RETRO(T_ERROR);
        for (auto &x : t[E_MENOR])
            x = RETRO(T_REL);
        for (auto &x : t[E_MAYOR])
            x = RETRO(T_REL);

        // Estado inicial
        t[E_INICIO][LETRA] = E_ID;
        t[E_INICIO][DIGITO] = E_ENTERO;
        t[E_INICIO][PARENI] = CONS(T_LP);
        t[E_INICIO][PARENF] = CONS(T_RP);
        t[E_INICIO][CORCHI] = CONS(T_LB);
        t[E_INICIO][CORCHD] = CONS(T_RB);
        t[E_INICIO][COMA] = CONS(T_COMA);
        t[E_INICIO][DOSP] = CONS(T_DOSP);
        t[E_INICIO][MAS] = CONS(T_MAS);
        t[E_INICIO][MENOS] = CONS(T_MENOS);
        t[E_INICIO][POR] = CONS(T_POR);
        t[E_INICIO][DIV] = CONS(T_DIV);
        t[E_INICIO][MOD] = CONS(T_MOD);
        t[E_INICIO][IGUAL] = E_IGUAL;
        t[E_INICIO][ADMIR] = E_ADMIR;
        t[E_INICIO][MENOR] = E_MENOR;
        t[E_INICIO][MAYOR] = E_MAYOR;
        t[E_INICIO][ESPACIO] = E_INICIO; // los espacios se saltan
        t[E_INICIO][FIN] = RETRO(T_EOF); // no avanza: pedir más tokens sigue dando $

        // Identificadores: letras, dígitos y '_'
        t[E_ID][LETRA] = E_ID;
        t[E_ID][DIGITO] = E_ID;

        // Números
        t[E_ENTERO][DIGITO] = E_ENTERO;
        t[E_ENTERO][PUNTO] = E_PUNTO;
        t[E_PUNTO][DIGITO] = E_DECIMAL;
        t[E_DECIMAL][DIGITO] = E_DECIMAL;

        // Operadores relacionales de dos caracteres
        t[E_IGUAL][IGUAL] = CONS(T_REL);
        t[E_ADMIR][IGUAL] = CONS(T_REL);
        t[E_MENOR][IGUAL] = CONS(T_REL);
        t[E_MAYOR][IGUAL] = CONS(T_REL);

        return t;
    }

    inline constexpr Tabla TABLA = crearTabla();

    /// @brief Construye en compilación la tabla que traduce cada byte (0-255) a su clase.
    constexpr std::array<uint8_t, 256> crearClases()
    {
        std::array<uint8_t, 256> t{};
        for (auto &x : t)
            x = OTRO;
        for (int c = 'a'; c <= 'z'; ++c)
            t[c] = t[c - 32] = LETRA;
        t['_'] = LETRA;
        for (int c = '0'; c <= '9'; ++c)
            t[c] = DIGITO;
        t['('] = PARENI;
        t[')'] = PARENF;
        t['['] = CORCHI;
        t[']'] = CORCHD;
        t[','] = COMA;
        t[':'] = DOSP;
        t['+'] = MAS;
        t['-'] = MENOS;
        t['*'] = POR;
        t['/'] = DIV;
        t['%'] = MOD;
        t['='] = IGUAL;
        t['!'] = ADMIR;
        t['<'] = MENOR;
        t['>'] = MAYOR;
        t['.'] = PUNTO;
        t[' '] = t['\t'] = t['\r'] = t['\n'] = ESPACIO;
        t[0] = FIN;
        return t;
    }

    inline constexpr auto CLASE = crearClases();

    /// Palabras reservadas: un identificador con este lexema cambia de tipo.
    inline constexpr std::array<std::pair<std::string_view, Term>, 5> RESERVADAS = {{
        {"map", T_MAP},
        {"filter", T_FILTER},
        {"reduce", T_REDUCE},
        {"lambda", T_LAMBDA},
        {"range", T_RANGE},
    }};
} // namespace lex_impl

/// @brief Analizador léxico.
/// No copia el texto: la cadena original debe seguir viva mientras se use.
class Scanner
{
    std::string_view str;
    std::size_t pos = 0;

public:
    explicit Scanner(std::string_view s) : str(s) {}

    /// @brief Reconoce el siguiente token a partir de la posición actual.
    /// @return Token con tipo, lexema y columna. Al terminar la entrada devuelve T_EOF
    ///         las veces que se pida; un carácter inválido devuelve T_ERROR y se avanza.
    Token next()
    {
        using namespace lex_impl;

        int16_t estado = E_INICIO;
        std::size_t ini = pos;
        do
        {
            unsigned char c = pos < str.size() ? str[pos] : 0;
            estado = TABLA[estado][CLASE[c]];
            ++pos;
            if (estado == E_INICIO)
                ini = pos; // seguimos en blanco: el token aún no empieza
        } while (estado < FIN_MIN);

        Term tipo;
        if (estado >= BASE_CONS)
            tipo = static_cast<Term>(estado - BASE_CONS);
        else
        {
            tipo = static_cast<Term>(estado - FIN_MIN);
            --pos; // el carácter que cerró el token no se consume
        }

        std::string_view lex = str.substr(ini, pos - ini);

        if (tipo == T_ID)
            for (const auto &[palabra, t] : RESERVADAS)
                if (lex == palabra)
                {
                    tipo = t;
                    break;
                }

        return {tipo, std::string(lex), ini + 1};
    }
};
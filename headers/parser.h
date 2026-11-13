#pragma once
// parser.h — Análisis sintáctico predictivo no recursivo (LL(1)).
//
// Esta capa solo calcula: recibe los tokens y devuelve un Resultado (veredicto, error con
// su posición y la traza completa). No lee archivos ni imprime nada; de eso se encarga
// informe.h, y de conectar las etapas, main.cpp.

#include <cstddef>
#include <string>
#include <vector>

#include "gramatica.h"
#include "tokens.h"

/// @brief Un paso de la traza (una fila de la tabla PILA / ENTRADA / ACCIÓN).
struct Paso
{
    std::size_t numero;
    std::size_t linea;  // línea del token de entrada en ese paso
    std::string pila;   // de abajo hacia arriba: "$ Prog" (el tope queda a la derecha)
    std::size_t indice; // índice en el vector de tokens del primer símbolo de ENTRADA
    std::string accion; // formato diapositiva 32: "A → α" al expandir, vacío al emparejar
};

enum class TipoError
{
    Ninguno,
    Lexico,
    Sintactico
};

struct Resultado
{
    bool ok = false;
    TipoError tipo = TipoError::Ninguno;
    std::size_t linea = 0, col = 0; // posición del error
    std::string mensaje;
    std::vector<Paso> traza;
};

namespace analisis_impl
{
    inline std::string nombre(Simbolo s)
    {
        return s.esTerm ? NOMBRE_T[s.id] : NOMBRE_NT[s.id];
    }

    inline std::string pilaStr(const std::vector<Simbolo> &pila)
    {
        std::string s;
        for (const Simbolo &x : pila)
            s += (s.empty() ? "" : " ") + nombre(x);
        return s;
    }
} // namespace analisis_impl

/// @brief La produccion p escrita como "A -> alfa" ("eps" si la parte derecha es vacia). Solo ASCII.
inline std::string produccionStr(int p)
{
    std::string s = std::string(NOMBRE_NT[PROD[p].izq]) + " -> ";
    if (PROD[p].der.empty())
        return s + "eps";
    for (std::size_t k = 0; k < PROD[p].der.size(); ++k)
        s += (k ? " " : "") + analisis_impl::nombre(PROD[p].der[k]);
    return s;
}

namespace analisis_impl
{
    /// Terminales que tienen producción en la fila de A (incluye los que habilita un ε):
    /// es exactamente lo que el analizador habría aceptado en ese punto.
    inline std::string esperados(NoTerm A)
    {
        std::vector<std::string> v;
        for (int a = 0; a < T_ERROR; ++a)
            if (M[A][a])
                v.push_back(LEGIBLE_T[a]);
        if (v.size() == 1)
            return v[0];
        std::string s = "una de estas opciones: ";
        for (std::size_t k = 0; k < v.size(); ++k)
            s += (k ? ", " : "") + v[k];
        return s;
    }

    /// Cómo nombrar al token encontrado en un mensaje de error.
    inline std::string descripcion(const Token &t)
    {
        switch (t.tipo)
        {
        case T_ID:
            return "el identificador '" + t.lexema + "'";
        case T_NUM:
            return "el numero '" + t.lexema + "'";
        case T_REL:
            return "el operador '" + t.lexema + "'";
        case T_NEWLINE:
        case T_INDENT:
        case T_DEDENT:
        case T_EOF:
            return LEGIBLE_T[t.tipo];
        default:
            return "'" + t.lexema + "'";
        }
    }
} // namespace analisis_impl

/// @brief Analiza la secuencia de tokens (el último debe ser T_EOF).
/// Se detiene en el primer error, léxico o sintáctico, tal como aparecen en el archivo.
inline Resultado analizar(const std::vector<Token> &toks)
{
    using namespace analisis_impl;

    Resultado R;
    std::vector<Simbolo> pila{{true, T_EOF}, {false, NT_PROG}};
    std::size_t i = 0;

    // Registra la fila del paso actual: la pila y la entrada ANTES de ejecutar la acción.
    auto paso = [&](const std::string &accion)
    {
        R.traza.push_back({R.traza.size() + 1, toks[i].linea, pilaStr(pila), i, accion});
    };
    auto fallo = [&](TipoError tipo, const Token &a, const std::string &msg)
    {
        paso("error: " + msg);
        R.ok = false;
        R.tipo = tipo;
        R.linea = a.linea;
        R.col = a.col;
        R.mensaje = msg;
    };

    while (!pila.empty())
    {
        const Simbolo X = pila.back();
        const Token &a = toks[i];

        if (a.tipo == T_ERROR) // el lexer ya encontró un problema en este punto
        {
            fallo(TipoError::Lexico, a, a.mensaje);
            return R;
        }

        if (X.esTerm)
        {
            if (X.id != a.tipo)
            {
                fallo(TipoError::Sintactico, a,
                      std::string("se esperaba ") + LEGIBLE_T[X.id] + " y se encontro " + descripcion(a));
                return R;
            }
            if (X.id == T_EOF)
            {
                // El análisis concluye en la última reducción a ε cuando queda $ y $
                R.ok = true;
                return R;
            }
            // Emparejar terminal: la columna de acción/salida queda vacía
            paso("");
            pila.pop_back();
            ++i;
        }
        else
        {
            const int p = M[X.id][a.tipo];
            if (p == 0)
            {
                fallo(TipoError::Sintactico, a,
                      "no se esperaba " + descripcion(a) + "; se esperaba " + esperados(static_cast<NoTerm>(X.id)));
                return R;
            }
            // Derivar no terminal: muestra únicamente la producción "A → α"
            paso(produccionStr(p));
            pila.pop_back();
            for (auto it = PROD[p].der.rbegin(); it != PROD[p].der.rend(); ++it)
                pila.push_back(*it); // se apila de derecha a izquierda
        }
    }
    return R; // no se alcanza: la pila siempre termina en $
}

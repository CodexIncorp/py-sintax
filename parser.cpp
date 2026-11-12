// parser.cpp — Analizador sintáctico predictivo no recursivo (LL(1))
// para las instrucciones map, filter y reduce de Python.
//
// Compilar:  g++ -std=c++17 -Wall -Wextra -o parser parser.cpp
// Usar:      ./parser                     (modo interactivo, línea vacía para salir)
//            ./parser "map(f, xs)"        (una sola cadena)
//            ./parser -q "map(f, xs)"     (sin traza, solo el veredicto)
//            ./parser --tabla             (imprime la tabla M, útil para el reporte)

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "gramatica.h"
#include "lexer.h"

using namespace std;

// ---------------------------------------------------------------------------
// Utilidades de impresión
// ---------------------------------------------------------------------------

static string nombre(Simbolo s)
{
    return s.esTerm ? NOMBRE_T[s.id] : NOMBRE_NT[s.id];
}

/// Pila de abajo hacia arriba ($ a la izquierda, tope a la derecha), como en la diapositiva 17.
static string pilaStr(const vector<Simbolo> &pila)
{
    string s;
    for (const Simbolo &x : pila)
        s += (s.empty() ? "" : " ") + nombre(x);
    return s;
}

/// Lo que falta por leer, a partir del token i.
static string entradaStr(const vector<Token> &toks, size_t i)
{
    string s;
    for (; i < toks.size(); ++i)
        s += (s.empty() ? "" : " ") + string(NOMBRE_T[toks[i].tipo]);
    return s;
}

static string produccionStr(int p)
{
    string s = string(NOMBRE_NT[PROD[p].izq]) + " -> ";
    if (PROD[p].der.empty())
        return s + "epsilon";
    for (size_t k = 0; k < PROD[p].der.size(); ++k)
        s += (k ? " " : "") + nombre(PROD[p].der[k]);
    return s;
}

/// Terminales que tienen producción en la fila de A: lo que el parser habría aceptado.
static string esperados(NoTerm A)
{
    string s;
    for (int a = 0; a < T_ERROR; ++a)
        if (M[A][a])
            s += (s.empty() ? "" : ", ") + string("'") + NOMBRE_T[a] + "'";
    return s;
}

static string descripcion(const Token &t)
{
    return t.tipo == T_EOF ? "fin de la entrada" : "'" + t.lexema + "'";
}

// ---------------------------------------------------------------------------
// Algoritmo de análisis sintáctico predictivo no recursivo (diapositiva 14)
// ---------------------------------------------------------------------------

/// @param toks  Tokens de la entrada; el último debe ser T_EOF.
/// @param traza Si es true, imprime PILA / ENTRADA / SALIDA en cada paso.
/// @return true si la cadena pertenece al lenguaje.
static bool analizar(const vector<Token> &toks, bool traza)
{
    const int ANCHO_PILA = 38, ANCHO_ENTRADA = 46;

    vector<Simbolo> pila{{true, T_EOF}, {false, NT_S}};
    size_t i = 0;

    if (traza)
        cout << left << setw(ANCHO_PILA) << "PILA" << setw(ANCHO_ENTRADA) << "ENTRADA"
             << "SALIDA\n"
             << string(ANCHO_PILA + ANCHO_ENTRADA + 20, '-') << "\n";

    while (!pila.empty())
    {
        Simbolo X = pila.back();
        const Token &a = toks[i];

        if (traza)
            cout << left << setw(ANCHO_PILA) << pilaStr(pila)
                 << setw(ANCHO_ENTRADA) << entradaStr(toks, i);

        if (X.esTerm)
        {
            if (X.id != a.tipo)
            {
                if (traza)
                    cout << "ERROR\n";
                cout << "Error sintactico en la columna " << a.col << ": se esperaba "
                     << "'" << NOMBRE_T[X.id] << "' y se encontro " << descripcion(a) << ".\n";
                return false;
            }
            pila.pop_back();
            if (X.id == T_EOF)
            {
                if (traza)
                    cout << "aceptar\n";
                return true;
            }
            if (traza)
                cout << "\n"; // coincidencia de terminal: sin salida, como en la diapositiva 17
            ++i;
        }
        else
        {
            int p = M[X.id][a.tipo];
            if (p == 0)
            {
                if (traza)
                    cout << "ERROR\n";
                cout << "Error sintactico en la columna " << a.col << ": no se esperaba "
                     << descripcion(a) << ". Con " << NOMBRE_NT[X.id]
                     << " en el tope se esperaba: " << esperados(static_cast<NoTerm>(X.id)) << ".\n";
                return false;
            }
            if (traza)
                cout << produccionStr(p) << "\n";
            pila.pop_back();
            for (auto it = PROD[p].der.rbegin(); it != PROD[p].der.rend(); ++it)
                pila.push_back(*it); // se apila de derecha a izquierda
        }
    }
    return false; // no se alcanza: la pila siempre termina en $
}

// ---------------------------------------------------------------------------
// Programa principal
// ---------------------------------------------------------------------------

/// Tokeniza toda la línea con el lexer; devuelve false si hay un error léxico.
static bool tokenizar(const string &linea, vector<Token> &toks)
{
    Scanner scan(linea);
    Token t;
    do
    {
        t = scan.next();
        if (t.tipo == T_ERROR)
        {
            cout << "Error lexico en la columna " << t.col << ": '" << t.lexema
                 << "' no es un simbolo valido.\n";
            return false;
        }
        toks.push_back(t);
    } while (t.tipo != T_EOF);
    return true;
}

static bool procesar(const string &linea, bool traza)
{
    vector<Token> toks;
    if (!tokenizar(linea, toks))
        return false;
    bool ok = analizar(toks, traza);
    cout << (ok ? "=> Cadena ACEPTADA\n" : "=> Cadena RECHAZADA\n");
    return ok;
}

/// Imprime M agrupada por no terminal: "terminal → producción".
static void imprimirTabla()
{
    for (int A = 0; A < N_NOTERM; ++A)
    {
        cout << NOMBRE_NT[A] << ":\n";
        for (int a = 0; a < T_ERROR; ++a)
            if (M[A][a])
                cout << "    " << setw(7) << left << NOMBRE_T[a] << " → " << setw(2) << right
                     << int(M[A][a]) << "   (" << produccionStr(M[A][a]) << ")\n";
    }
}

int main(int argc, char **argv)
{
    bool traza = true, hayEntrada = false;
    string entrada;

    for (int k = 1; k < argc; ++k)
    {
        string arg = argv[k];
        if (arg == "--tabla")
        {
            imprimirTabla();
            return 0;
        }
        if (arg == "-q")
            traza = false;
        else
        {
            entrada = arg;
            hayEntrada = true;
        }
    }

    if (hayEntrada)
        return procesar(entrada, traza) ? 0 : 1;

    for (;;)
    {
        cout << "Ingrese instruccion (ENTER para salir): ";
        string linea;
        if (!getline(cin, linea) || linea.empty())
            break;
        procesar(linea, traza);
        cout << "\n";
    }
    return 0;
}
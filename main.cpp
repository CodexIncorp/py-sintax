// main.cpp — Analizador sintáctico predictivo no recursivo (LL(1)) para un subconjunto de Python:
// asignaciones de listas y tuplas, map/filter/reduce y definición de funciones.
//
// Flujo:  archivo .py  →  Scanner (lexer.h)  →  analizar (parser.h)  →  informe.h
//
// Compilar:  (Linux) g++ -std=c++17 -Wall -Wextra -o ./build/parser main.cpp
//           (Windows) g++ -o ./build/parser main.cpp                
// Usar:      ./build/parser programa.py [traza.csv]   (si no se da el .csv: programa.csv)
//            ./build/parser --entrada-completa programa.py
//                  la columna ENTRADA del CSV muestra todo lo que falta por leer (por defecto,
//                  los próximos 60 tokens; el CSV completo crece con el cuadrado del archivo)
//            ./build/parser --tabla                   (imprime la tabla M para el reporte)

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "./headers/parser.h"
#include "./headers/informe.h"
#include "./headers/lexer.h"

using namespace std;

/// Lee el archivo completo. Quita el BOM UTF-8 si lo trae.
static bool leerArchivo(const string &ruta, string &texto)
{
    ifstream f(ruta, ios::binary);
    if (!f)
        return false;
    ostringstream ss;
    ss << f.rdbuf();
    texto = ss.str();
    if (texto.compare(0, 3, "\xEF\xBB\xBF") == 0)
        texto.erase(0, 3);
    return true;
}

/// "dir/prog.py" → "dir/prog.csv"
static string rutaCsvPorDefecto(const string &ruta)
{
    size_t punto = ruta.find_last_of('.');
    size_t barra = ruta.find_last_of("/\\");
    if (punto == string::npos || (barra != string::npos && punto < barra))
        return ruta + ".csv";
    return ruta.substr(0, punto) + ".csv";
}

int main(int argc, char **argv)
{
    bool entradaCompleta = false;
    vector<string> rutas;
    for (int k = 1; k < argc; ++k)
    {
        const string arg = argv[k];
        if (arg == "--tabla")
        {
            imprimirTabla(cout);
            return 0;
        }
        if (arg == "--entrada-completa")
            entradaCompleta = true;
        else
            rutas.push_back(arg);
    }
    if (rutas.empty() || rutas.size() > 2)
    {
        cerr << "Uso: " << argv[0] << " [--entrada-completa] archivo.py [traza.csv]\n"
             << "     " << argv[0] << " --tabla\n";
        return 2;
    }

    const string ruta = rutas[0];
    const string rutaCsv = rutas.size() == 2 ? rutas[1] : rutaCsvPorDefecto(ruta);

    string fuente;
    if (!leerArchivo(ruta, fuente))
    {
        cerr << "No se pudo abrir '" << ruta << "'.\n";
        return 2;
    }

    if (fuente.find('\0') != string::npos)
    {
        cerr << "'" << ruta << "' contiene bytes nulos: no parece un archivo de texto.\n";
        return 2;
    }

    // 1. Lexer: todo el archivo a tokens (termina siempre en T_EOF)
    vector<Token> tokens;
    Scanner scan(fuente);
    Token t;
    do
    {
        t = scan.next();
        tokens.push_back(t);
    } while (t.tipo != T_EOF);

    // 2. Análisis sintáctico (no imprime nada)
    const Resultado resultado = analizar(tokens);

    // 3. Salidas: la traza al CSV y el veredicto a consola
    if (!escribirCsv(rutaCsv, resultado, tokens, entradaCompleta ? 0 : 60))
        cerr << "Aviso: no se pudo escribir '" << rutaCsv << "'.\n";
    informarConsola(resultado, ruta, fuente);

    return resultado.ok ? 0 : 1;
}
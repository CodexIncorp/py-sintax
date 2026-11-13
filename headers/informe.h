#pragma once
// informe.h — Componentes de reporte:
// - escribirCsv: exporta la traza (PILA / ENTRADA / ACCIÓN).
// - informarConsola: veredicto y errores sintácticos apuntados.
// - imprimirTabla: volcado de tabla M para debug.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"

namespace informe_impl
{
    // Campo CSV (RFC 4180): entre comillas, con las comillas internas duplicadas.
    inline std::string campo(const std::string &s)
    {
        std::string out = "\"";
        for (char c : s)
        {
            if (c == '"') out += '"';
            out += c;
        }
        return out + "\"";
    }

    // Cadena con los próximos tokens en el stream (hasta 'limite').
    inline std::string entrada(const std::vector<Token> &toks, std::size_t i, std::size_t limite)
    {
        std::string s;
        for (std::size_t n = 0; i < toks.size(); ++i, ++n)
        {
            if (limite && n == limite)
            {
                s += " ... (+" + std::to_string(toks.size() - i) + ")";
                break;
            }
            s += (s.empty() ? "" : " ") + std::string(NOMBRE_T[toks[i].tipo]);
        }
        return s;
    }

    // Reemplaza por '?' los bytes de control y no-ASCII (mantiene la alineación para el puntero del error).
    inline std::string soloAscii(std::string s)
    {
        for (char &c : s)
            if (static_cast<unsigned char>(c) >= 0x80 || (static_cast<unsigned char>(c) < 0x20 && c != '\t'))
                c = '?';
        return s;
    }

    // Extrae la línea n (base 1) del fuente omitiendo el salto de carro al final.
    inline std::string lineaDeTexto(const std::string &fuente, std::size_t n)
    {
        std::size_t ini = 0;
        for (std::size_t l = 1; l < n; ++l)
        {
            ini = fuente.find('\n', ini);
            if (ini == std::string::npos) return "";
            ++ini;
        }
        std::size_t fin = fuente.find('\n', ini);
        std::string s = fuente.substr(ini, fin == std::string::npos ? std::string::npos : fin - ini);
        while (!s.empty() && s.back() == '\r') s.pop_back();
        return s;
    }
}

// Escribe la traza en CSV.
inline bool escribirCsv(const std::string &ruta, const Resultado &r, const std::vector<Token> &toks,
                        std::size_t maxEntrada = 60)
{
    using namespace informe_impl;
    std::ofstream f(ruta, std::ios::binary);
    if (!f) return false;
    
    f << "paso,linea,pila,entrada,accion\n";
    for (const Paso &p : r.traza)
        f << p.numero << ',' << p.linea << ',' << campo(p.pila) << ','
          << campo(entrada(toks, p.indice, maxEntrada)) << ',' << campo(p.accion) << '\n';
    return static_cast<bool>(f);
}

// Imprime el veredicto en consola, incluyendo el puntero al error si ocurre un fallo.
inline void informarConsola(const Resultado &r, const std::string &archivo, const std::string &fuente)
{
    using namespace informe_impl;
    if (r.ok)
    {
        std::cout << "Analisis exitoso: '" << archivo << "' es sintacticamente valido.\n";
        return;
    }

    std::cerr << (r.tipo == TipoError::Lexico ? "Error lexico" : "Error sintactico")
              << " en la linea " << r.linea << ", columna " << r.col << ": " << r.mensaje << "\n";

    const std::string texto = soloAscii(lineaDeTexto(fuente, r.linea));
    const std::string num = std::to_string(r.linea);
    std::cerr << "  " << num << " | " << texto << "\n";

    std::string relleno;
    for (std::size_t k = 0; k + 1 < r.col && k < texto.size(); ++k)
        relleno += (texto[k] == '\t' ? '\t' : ' ');
    for (std::size_t k = texto.size(); k + 1 < r.col; ++k)
        relleno += ' ';
    std::cerr << "  " << std::string(num.size(), ' ') << " | " << relleno << "^\n";
}

// Imprime la tabla M agrupada por no terminal.
inline void imprimirTabla(std::ostream &os)
{
    for (int A = 0; A < N_NOTERM; ++A)
    {
        os << NOMBRE_NT[A] << ":\n";
        for (int a = 0; a < T_ERROR; ++a)
            if (M[A][a])
                os << "    " << std::setw(8) << std::left << NOMBRE_T[a] << " -> " << std::setw(2)
                   << std::right << int(M[A][a]) << "   (" << produccionStr(M[A][a]) << ")\n";
    }
}
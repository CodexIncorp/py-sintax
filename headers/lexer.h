#pragma once
// lexer.h — Analizador léxico de un archivo .py (subconjunto).
// Dos capas:
//   1. Autómata finito que reconoce tokens por línea.
//   2. Clase Scanner (cuenta líneas, maneja sangría vía pila y cierra EOF).

#include <array>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "tokens.h"

namespace lex_impl
{
    // Clases de carácter (columnas de TABLA).
    enum Clase : uint8_t
    {
        LETRA, DIGITO, PARENI, PARENF, CORCHI, CORCHD, COMA, DOSP, MAS, MENOS, 
        POR, DIV, MOD, IGUAL, ADMIR, MENOR, MAYOR, PUNTO, ESPACIO, SALTO, COMENT, 
        FIN, OTRO, N_CLASE
    };

    // Estados finales (>= 500):
    // 500+t: Token t reconocido. El último caracter NO es parte del token (retrocede).
    // 600+t: Token t reconocido. El último caracter SÍ es parte del token.
    constexpr int16_t FIN_MIN = 500;
    constexpr int16_t BASE_CONS = 600;

    constexpr int16_t RETRO(Term t) { return 500 + t; }
    constexpr int16_t CONS(Term t) { return BASE_CONS + t; }

    enum Estado : int16_t
    {
        E_INICIO = 0, E_ID = 1, E_ENTERO = 2, E_PUNTO = 3, E_DECIMAL = 4,
        E_IGUAL = 5, E_ADMIR = 6, E_MENOR = 7, E_MAYOR = 8, E_COMENT = 9,
        E_POR = 10, E_DIV = 11, N_ESTADO
    };

    using Tabla = std::array<std::array<int16_t, N_CLASE>, N_ESTADO>;

    // Función de transición: TABLA[estado][clase] = estado_siguiente.
    constexpr Tabla crearTabla()
    {
        Tabla t{};

        for (auto &x : t[E_INICIO]) x = CONS(T_ERROR); 
        for (auto &x : t[E_ID])     x = RETRO(T_ID);
        for (auto &x : t[E_ENTERO]) x = RETRO(T_NUM);
        for (auto &x : t[E_PUNTO])  x = RETRO(T_ERROR); // "3." sin dígitos después
        for (auto &x : t[E_DECIMAL])x = RETRO(T_NUM);
        for (auto &x : t[E_IGUAL])  x = RETRO(T_ASIG);
        for (auto &x : t[E_ADMIR])  x = RETRO(T_ERROR);
        for (auto &x : t[E_MENOR])  x = RETRO(T_REL);
        for (auto &x : t[E_MAYOR])  x = RETRO(T_REL);
        for (auto &x : t[E_COMENT]) x = E_COMENT; 
        for (auto &x : t[E_POR])    x = RETRO(T_POR);
        for (auto &x : t[E_DIV])    x = RETRO(T_DIV);

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
        t[E_INICIO][POR] = E_POR;
        t[E_INICIO][DIV] = E_DIV;
        t[E_INICIO][MOD] = CONS(T_MOD);
        t[E_INICIO][IGUAL] = E_IGUAL;
        t[E_INICIO][ADMIR] = E_ADMIR;
        t[E_INICIO][MENOR] = E_MENOR;
        t[E_INICIO][MAYOR] = E_MAYOR;
        t[E_INICIO][ESPACIO] = E_INICIO;
        t[E_INICIO][SALTO] = CONS(T_NEWLINE);
        t[E_INICIO][COMENT] = E_COMENT;
        t[E_INICIO][FIN] = RETRO(T_EOF);

        t[E_ID][LETRA] = E_ID;
        t[E_ID][DIGITO] = E_ID;
        t[E_ENTERO][DIGITO] = E_ENTERO;
        t[E_ENTERO][PUNTO] = E_PUNTO;
        t[E_PUNTO][DIGITO] = E_DECIMAL;
        t[E_DECIMAL][DIGITO] = E_DECIMAL;

        t[E_IGUAL][IGUAL] = CONS(T_REL);
        t[E_ADMIR][IGUAL] = CONS(T_REL);
        t[E_MENOR][IGUAL] = CONS(T_REL);
        t[E_MAYOR][IGUAL] = CONS(T_REL);
        
        t[E_POR][POR] = CONS(T_POT);
        t[E_DIV][DIV] = CONS(T_IDIV);

        t[E_COMENT][SALTO] = CONS(T_NEWLINE);
        t[E_COMENT][FIN] = RETRO(T_NEWLINE);

        return t;
    }

    inline constexpr Tabla TABLA = crearTabla();

    // Mapeo en tiempo de compilación de cada byte (0-255) a su clase léxica.
    constexpr std::array<uint8_t, 256> crearClases()
    {
        std::array<uint8_t, 256> t{};
        for (auto &x : t) x = OTRO;
        for (int c = 'a'; c <= 'z'; ++c) t[c] = t[c - 32] = LETRA;
        t['_'] = LETRA;
        for (int c = '0'; c <= '9'; ++c) t[c] = DIGITO;
        t['('] = PARENI; t[')'] = PARENF; t['['] = CORCHI; t[']'] = CORCHD;
        t[','] = COMA; t[':'] = DOSP; t['+'] = MAS; t['-'] = MENOS;
        t['*'] = POR; t['/'] = DIV; t['%'] = MOD; t['='] = IGUAL;
        t['!'] = ADMIR; t['<'] = MENOR; t['>'] = MAYOR; t['.'] = PUNTO;
        t[' '] = t['\t'] = t['\r'] = ESPACIO;
        t['\n'] = SALTO; t['#'] = COMENT; t[0] = FIN;
        return t;
    }

    inline constexpr auto CLASE = crearClases();

    // Formatea caracteres UTF-8 no ASCII (ej. "U+00E9") para evitar problemas en consola.
    inline std::string codigoUnicode(std::string_view s)
    {
        const unsigned char b0 = static_cast<unsigned char>(s[0]);
        std::size_t extra = 0;
        uint32_t cp = 0;
        if (b0 >= 0xF0 && b0 < 0xF8) { cp = b0 & 0x07; extra = 3; }
        else if (b0 >= 0xE0) { cp = b0 & 0x0F; extra = 2; }
        else if (b0 >= 0xC0) { cp = b0 & 0x1F; extra = 1; }
        
        if (extra == 0 || s.size() != extra + 1) return "byte invalido";
        for (std::size_t k = 1; k <= extra; ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[k]) & 0x3F);
        
        char buf[16];
        std::snprintf(buf, sizeof buf, "U+%04X", static_cast<unsigned>(cp));
        return buf;
    }

    inline constexpr std::array<std::pair<std::string_view, Term>, 8> RESERVADAS = {{
        {"map", T_MAP}, {"filter", T_FILTER}, {"reduce", T_REDUCE},
        {"lambda", T_LAMBDA}, {"range", T_RANGE}, {"def", T_DEF},
        {"return", T_RETURN}, {"pass", T_PASS},
    }};
}

// Analizador léxico de archivo completo (string_view sobre el texto original).
// Incluye tokens estructurales sintéticos (NEWLINE, INDENT, DEDENT).
class Scanner
{
    std::string_view str;
    std::size_t pos = 0;
    std::size_t linea = 1;       
    std::size_t inicioLinea = 0; 
    std::size_t ultimaLinea = 1; 
    std::size_t ultimaFin = 1;
    std::size_t prof = 0; // profundidad de paréntesis/corchetes abiertos
    bool alMargen = true;
    bool lineaConContenido = false;
    char tipoSangria = 0;             
    std::vector<std::size_t> pila{0}; // niveles de sangría abiertos
    std::deque<Token> cola;           // tokens pendientes

    void registrar(const Token &t)
    {
        ultimaLinea = t.linea;
        ultimaFin = t.col + t.lexema.size();
    }

    Token sacar()
    {
        Token t = std::move(cola.front());
        cola.pop_front();
        registrar(t);
        return t;
    }

    // Evalúa la sangría saltando líneas en blanco y comentarios.
    void margen()
    {
        for (;;)
        {
            const std::size_t ini = pos;
            std::size_t q = pos, tabs = 0, esp = 0;
            while (q < str.size() && (str[q] == ' ' || str[q] == '\t'))
            {
                (str[q] == '\t' ? tabs : esp)++;
                ++q;
            }
            std::size_t r = q;
            while (r < str.size() && str[r] == '\r') ++r;

            if (r >= str.size()) // fin del archivo
            {
                pos = str.size();
                alMargen = false;
                return;
            }
            if (str[r] == '\n' || str[r] == '#') // ignorar blanco/comentario
            {
                if (str[r] == '#') {
                    while (r < str.size() && str[r] != '\n') ++r;
                }
                if (r < str.size()) {
                    ++r;
                    ++linea;
                }
                pos = r;
                inicioLinea = pos;
                continue;
            }

            pos = q;
            alMargen = false;
            const std::size_t ancho = q - ini;
            auto error = [&](const std::string &msg) {
                cola.clear();
                cola.push_back({T_ERROR, "", linea, ancho + 1, msg});
                lineaConContenido = true;
            };

            if (tabs && esp) return error("la sangria mezcla tabuladores y espacios");
            
            if (ancho > 0)
            {
                const char k = tabs ? '\t' : ' ';
                if (!tipoSangria) tipoSangria = k;
                else if (tipoSangria != k) return error("el archivo mezcla tabuladores y espacios en la sangria");
            }

            if (ancho > pila.back())
            {
                pila.push_back(ancho);
                cola.push_back({T_INDENT, "", linea, 1, ""});
            }
            else if (ancho < pila.back())
            {
                while (pila.back() > ancho)
                {
                    pila.pop_back();
                    cola.push_back({T_DEDENT, "", linea, 1, ""});
                }
                if (pila.back() != ancho)
                {
                    error("la sangria no coincide con ningun nivel anterior");
                    pila.push_back(ancho);
                }
            }
            return;
        }
    }

    // Cierra la última línea y bloques abiertos antes del EOF.
    Token cierre()
    {
        if (lineaConContenido)
        {
            lineaConContenido = false;
            return {T_NEWLINE, "", ultimaLinea, ultimaFin, ""};
        }
        if (pila.size() > 1)
        {
            pila.pop_back();
            return {T_DEDENT, "", ultimaLinea, ultimaFin, ""};
        }
        return {T_EOF, "", ultimaLinea, ultimaFin, ""};
    }

public:
    explicit Scanner(std::string_view s) : str(s) {}

    Token next()
    {
        using namespace lex_impl;

        if (!cola.empty()) return sacar();
        if (alMargen && prof == 0)
        {
            margen();
            if (!cola.empty()) return sacar();
        }

        Term tipo;
        std::size_t ini;
        for (;;)
        {
            int16_t estado = E_INICIO;
            ini = pos;
            do
            {
                unsigned char c = pos < str.size() ? str[pos] : 0;
                estado = TABLA[estado][CLASE[c]];
                ++pos;
                if (estado == E_INICIO) ini = pos;
            } while (estado < FIN_MIN);

            if (estado >= BASE_CONS)
                tipo = static_cast<Term>(estado - BASE_CONS);
            else
            {
                tipo = static_cast<Term>(estado - FIN_MIN);
                --pos;
            }

            if (tipo == T_NEWLINE && prof > 0 && pos > 0 && str[pos - 1] == '\n')
            {
                ++linea;
                inicioLinea = pos;
                continue;
            }
            break;
        }

        if (tipo == T_ERROR && pos - ini == 1 && static_cast<unsigned char>(str[ini]) >= 0xC0)
            while (pos < str.size() && (static_cast<unsigned char>(str[pos]) & 0xC0) == 0x80)
                ++pos;

        const std::string_view lex = str.substr(ini, pos - ini);
        Token t{tipo, std::string(lex), linea, ini - inicioLinea + 1, ""};

        if (tipo == T_EOF) return cierre();

        if (tipo == T_ID)
            for (const auto &[palabra, r] : RESERVADAS)
                if (lex == palabra) { t.tipo = r; break; }

        if (tipo == T_ERROR)
        {
            if (!lex.empty() && static_cast<unsigned char>(lex[0]) >= 0x80)
                t.mensaje = "caracter no ASCII no valido (" + codigoUnicode(lex) + ")";
            else if (!lex.empty() && lex.back() == '.' && lex[0] >= '0' && lex[0] <= '9')
                t.mensaje = "numero mal formado '" + t.lexema + "'";
            else
                t.mensaje = "simbolo no valido '" + t.lexema + "'";
        }

        if (tipo == T_LP || tipo == T_LB) ++prof;
        else if ((tipo == T_RP || tipo == T_RB) && prof > 0) --prof;

        if (tipo == T_NEWLINE)
        {
            if (!lex.empty() && lex.back() == '\n')
            {
                ++linea;
                inicioLinea = pos;
            }
            alMargen = true;
            lineaConContenido = false;
        }
        else lineaConContenido = true;

        registrar(t);
        return t;
    }
};
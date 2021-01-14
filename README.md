# Kompilator 

## Dane autora
- <b>Imię i nazwisko:</b> Tomasz Janik
- <b>Nr indeksu:</b> 244926

## Opis źródeł
- `Makefile` - plik kompilujący kompilator
- `lexer.l` - plik zawierający definicje analizatora leksykalnego generowanego za pomocą programu <b>FLEX</b>
- `parser.y` - plik zawierający definicje analizatora składniowego generowanego za pomocą programu <b>Bison</b>
- `compiler.cpp` i `compiler.hpp` - pliki, w których znajdują się deklaracje i definicje funkcji służące do generowania poszczególnych komend
- `symbols/` - katalog zawierający pliki, w których zdefiniowana jest tabla symboli, dzięki której możliwa jest kontrola zmiennych podczas kompilacji
- `values/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują wartości oraz funkcje generujące instrukcje potrzebne do operacji na nich
- `expressions/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują całe wyrażenia oraz funkcje potrzebne do ich wyliczania
- `conditions/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują warunki oraz funkcje potrzebne do ich sprawdzania
- `utils/` - katalog zawierający pliki, w których znajdują się inne funkcje pomocnicze

Wszystkie pliki źródłowe znajdują się w katalogu `src/`.


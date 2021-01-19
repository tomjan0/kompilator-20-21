# Kompilator - 2020/2021
Repozytorium zawiera kod źródłowy kompilatora napisanego w ramach kursu <b>Języki Formalne i Teoria Translacji</b> na kierunku informatyka - WPPT Politechnika Wrocławska.

## Środowisko i wykorzystane narzędzia
- Ubutnu (WSL) `20.04`
- Bison `3.5.1`
- Flex `2.6.4`
- g++ `9.3.0`
- cln-dev `1.3.6`
- make `4.2.1`


## Opis źródeł
Pliki źródłowe kompliatora znajdują się w katalogu `src/`:
- `Makefile` - plik kompilujący kompilator
- `lexer.l` - plik zawierający definicje analizatora leksykalnego generowanego za pomocą programu <b>FLEX</b>
- `parser.y` - plik zawierający definicje analizatora składniowego generowanego za pomocą programu <b>Bison</b>
- `compiler.cpp` i `compiler.hpp` - pliki, w których znajdują się deklaracje i definicje funkcji służące do generowania poszczególnych komend
- `symbols/` - katalog zawierający pliki, w których zdefiniowana jest tabela symboli, która przechowuje metadane o każej ze zmiennych takie jak nazwa, adres pamięci, czy informację na temat inicjalizacji oraz w przypadku tablic indeks startowy i długość
- `values/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują wartości (np. a, 51, b(1), c(a)) oraz funkcje generujące instrukcje potrzebne do operacji na nich
- `expressions/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują wyrażenia (np. a + b, 5 * 4) oraz funkcje generujące instrukcje potrzebne do ich wyliczania
- `conditions/` - katalog zawierający pliki, w których znajdują się definicje obiektów, które reprezentują warunki (np. a > 2, b(a) <= 3) oraz funkcje generujące instrukcje potrzebne do ich sprawdzania
- `utils/` - katalog zawierający pliki, w których znajdują się inne funkcje pomocnicze

Natomiast w katalogu `maszyna_wirtualna/` znajdują się pliki źródłowe maszyny wirtualnej, na której można uruchomić skompilowany program. Jej autorem jest prowadzący kurs <b>dr Maciej Gębala</b>.

## Jak używać
Na początku należy zbudować kompilator i maszynę wirtualną. Aby tego dokonać należy wykonać polecenie `make` odpowiednio w katalogach `src/` oraz `maszyna_wirtualna/`. Zostaną w nich wygenerowane pliki wykonywalne `kompilator` oraz `maszyna_wirtualna`.

Następnie należy dokonać kompilacji poleceniem `src/kompilator <plik_wejściowy> <plik_wyjściowy>` i skompilowny program uruchomić na maszynie wirtualnej poleceniem `maszyna_wirtualna/maszyna_wirtualna <skompilowany_plik>.`

## Przykładowy program
```
[ Rozkład liczby na czynniki pierwsze ]
DECLARE
    n, m, reszta, potega, dzielnik
BEGIN
    READ n;
    dzielnik := 2;
    m := dzielnik * dzielnik;
    WHILE n >= m DO
        potega := 0;
        reszta := n % dzielnik;
        WHILE reszta = 0 DO
            n := n / dzielnik;
            potega := potega + 1;
            reszta := n % dzielnik;
        ENDWHILE
        IF potega > 0 THEN [ czy znaleziono dzielnik ]
            WRITE dzielnik;
            WRITE potega;
        ELSE
            dzielnik := dzielnik + 1;
            m := dzielnik * dzielnik;
        ENDIF
    ENDWHILE
    IF n != 1 THEN [ ostatni dzielnik ]
        WRITE n;
        WRITE 1;
    ENDIF
END
```

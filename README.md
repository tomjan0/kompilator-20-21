# Compiler - 2020/2021
This repository contains the source code for a compiler developed as part of the **Formal Languages and Translation Theory** course in Computer Science at the Faculty of Fundamental Problems of Technology (WPPT), Wroclaw University of Science and Technology.

## Environment and Tools Used
* Ubuntu (WSL) `20.04`
* Bison `3.5.1`
* Flex `2.6.4`
* g++ `9.3.0`
* cln-dev `1.3.6`
* make `4.2.1`

## Source Code Description
The compiler's source files are located in the `src/` directory:

* `Makefile` - The file used to compile the compiler itself.
* `lexer.l` - Contains the definitions for the **lexical analyzer**, generated using **FLEX**.
* `parser.y` - Contains the definitions for the **parser**, generated using **Bison**.
* `compiler.cpp` and `compiler.hpp` - These files contain the declarations and definitions of functions used to generate individual commands.
* `symbols/` - This directory contains files that define the **symbol table**, which stores metadata for each variable, such as its name, memory address, initialization status, and for arrays, their starting index and length.
* `values/` - This directory contains files with definitions for objects that represent **values** (e.g., `a`, `51`, `b(1)`, `c(a)`) and functions that generate the necessary instructions for operations on them.
* `expressions/` - This directory contains files with definitions for objects that represent **expressions** (e.g., `a + b`, `5 * 4`) and functions that generate the necessary instructions for their evaluation.
* `conditions/` - This directory contains files with definitions for objects that represent **conditions** (e.g., `a > 2`, `b(a) <= 3`) and functions that generate the necessary instructions for checking them.
* `utils/` - This directory contains other **helper functions**.

Meanwhile, the `maszyna_wirtualna/` directory contains the source files for the **virtual machine** on which the compiled program can be run. Its author is the course lecturer, **Phd Maciej Gębala**.

## How to Use
First, you need to build the compiler and the virtual machine. To do this, run the `make` command in the `src/` and `maszyna_wirtualna/` directories, respectively. This will generate the executable files `kompilator` (compiler) and `maszyna_wirtualna` (virtual machine) in their respective directories.

Next, compile your program using the command `src/kompilator <input_file> <output_file>`, and then run the compiled program on the virtual machine with `maszyna_wirtualna/maszyna_wirtualna <compiled_file>`.

## Example Program
```
[ Prime factorization ]
DECLARE
    n, m, remainder, power, divisor
BEGIN
    READ n;
    divisor := 2;
    m := divisor * divisor;
    WHILE n >= m DO
        power := 0;
        remainder := n % divisor;
        WHILE remainder = 0 DO
            n := n / divisor;
            power := power + 1;
            remainder := n % divisor;
        ENDWHILE
        IF power > 0 THEN [ divisor found ]
            WRITE divisor;
            WRITE power;
        ELSE
            divisor := divisor + 1;
            m := divisor * divisor;
        ENDIF
    ENDWHILE
    IF n != 1 THEN [ last divisor ]
        WRITE n;
        WRITE 1;
    ENDIF
END
```

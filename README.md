# C-preprocessor

Se rezolva directivele:
- #define, inclusiv cele recursive
- #include
- #ifdef, #ifndef

Proiectul este atat pentru Linux, cat si pentru Windows, existand doua Makefile-uri:
- GNUmakefile pentru Linux,
- Makefile pentru Windows.

Hashmap-ul este folosit pentru a retine maparile din directiva define, iar linked list-ul
pentru a retine datele din fisier (linie cu linie). Acestea sunt implementari proprii pentru
cele doua structuri de date.

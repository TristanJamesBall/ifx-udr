# ifx-udr
Informix UDR's ( C Extensions / Stored Procs )

## realtime 

Simple wrappers around `clock_gettime(2)` which gets the current time in UTC or Localtime

 - By default returns a `datetime year to fraction(5)`
 - Provides a new timestamp (if the time has changed!) every call, even from within a stored proc or transaction, unlike `current` or `sysdate`

## bitops

Alternative bitwise operators, provides bitwise functions - particularly shifts, that operate more like the C versions

Also some helpers for alternate hex printing and bit strings



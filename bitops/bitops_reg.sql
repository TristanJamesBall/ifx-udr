

create function if not exists bitprint4(bigint) returns lvarchar(85) external name '$INFORMIXDIR/extend/bitops/bitops.bld(bitprint4)' language c not variant;
create function if not exists hexprint4(bigint) returns lvarchar(20) external name '$INFORMIXDIR/extend/bitops/bitops.bld(hexprint4)' language c not variant;
create function if not exists hexprint(bigint) returns lvarchar(20) external name '$INFORMIXDIR/extend/bitops/bitops.bld(hexprint)' language c not variant;
create function if not exists bitprint(bigint) returns lvarchar(85) external name '$INFORMIXDIR/extend/bitops/bitops.bld(bitprint)' language c not variant;

create function if not exists bop_not(smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_not_small)' language c not variant;
create function if not exists bop_not(integer)  returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_not_int)'   language c not variant;
create function if not exists bop_not(bigint)   returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_not_big)'   language c not variant;

create function if not exists bop_xor(smallint   ,smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_xor_small)'   language c not variant;
create function if not exists bop_xor(integer    ,integer)  returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_xor_int)'     language c not variant;
create function if not exists bop_xor(bigint     ,bigint)   returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_xor_big)'     language c not variant;
create function if not exists bop_and(smallint   ,smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_and_small)'   language c not variant;
create function if not exists bop_and(integer    ,integer)  returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_and_int)'     language c not variant;
create function if not exists bop_and(bigint     ,bigint)   returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_and_big)'     language c not variant;
create function if not exists bop_or(smallint    ,smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_or_small)'    language c not variant;
create function if not exists bop_or(integer     ,integer)  returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_or_int)'      language c not variant;
create function if not exists bop_or(bigint      ,bigint)   returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_or_big)'      language c not variant;
create function if not exists bop_left(smallint  ,smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_left_small)'  language c not variant;
create function if not exists bop_left(integer   ,smallint) returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_left_int)'    language c not variant;
create function if not exists bop_left(bigint    ,smallint) returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_left_big)'    language c not variant;
create function if not exists bop_right(smallint ,smallint) returns smallint external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_right_small)' language c not variant;
create function if not exists bop_right(integer  ,smallint) returns integer  external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_right_int)'   language c not variant;
create function if not exists bop_right(bigint   ,smallint) returns bigint   external name '$INFORMIXDIR/extend/bitops/bitops.bld(bop_right_big)'   language c not variant;

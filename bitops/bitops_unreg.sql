

drop function if exists bitprint(bigint);
drop function if exists hexprint(bigint);
drop function if exists hexprint4(bigint);
drop function if exists bitprint4(bigint);

drop function if exists bop_xor(  integer  ,integer);
drop function if exists bop_xor(  bigint   ,bigint);

drop function if exists bop_or(  smallint ,smallint);
drop function if exists bop_or(  integer  ,integer);
drop function if exists bop_or(  bigint   ,bigint);

drop function if exists bop_and(  smallint ,smallint);
drop function if exists bop_and(  integer  ,integer);
drop function if exists bop_and(  bigint   ,bigint);

drop function if exists bop_not(  smallint );
drop function if exists bop_not(  integer  );
drop function if exists bop_not(  bigint   );



drop function if exists bop_left(  smallint ,smallint);
drop function if exists bop_left(  integer  ,smallint);
drop function if exists bop_left(  bigint   ,smallint);

drop function if exists bop_right( smallint ,smallint);
drop function if exists bop_right( integer  ,smallint);
drop function if exists bop_right( bigint   ,smallint);

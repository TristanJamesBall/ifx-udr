

 -- 
 --   NOTE
 -- 
 --   Current Informix (12,10.x, 14,10.x, 15.0.2) have a quirk where by variant functions or synonyms in a sub query are 
 --   called multiple times if they're referred to multiple times in  the enclosing query.
 -- 
 --   eg, in the following, utc_realtime_ns() is called *4* times, not 2, and the 'latency' calculation 
 --   will appear wrong, because the  'ts1 - ts2' calculation is using a second set of function calls 
 --   compared to thn the ts1 and ts2 fields
 --
 -- 
 --   select
 --      ts1,
 --      ts2,
 --      ts2 - ts1 as latency
 --   from (
 -- 	select
 -- 		utc_realtime_ns() as ts1,
 -- 		utc_realtime_ns() as ts2
 -- 	from
 -- 		sysmaster:sysdual
 --   )
 -- 
 --  WORKAROUNDS:
 --
 --   - You can also force the sub query to fully matarialize, by either sorting or using a 'first ..' 
 --   - In some case marking the function as variant or not variant ( confusingly, either way ) may help
 --
 --  I have a case open with IBM about the above
 --
 --
 --  In a "normal" query where we're doing somethig like...
 --
 --    insert into table(...) values( realtime_dt() ... ) 
 --  
 --  ... then the above issue wont matte anyway



create function if not exists realtime_dt() returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;


create function if not exists utc_realtime_dt() returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;



-- 
--  ANOTHER NOTE: 
--
--  Due to DBFLTMASK, dbaccess will only show 5 decimal places in it's default output
--  Using 'unload to' or another query tool will show the full 9 places
--


create function if not exists utc_realtime_dec() returns decimal(32,9)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;


create function if not exists utc_realtime_frac5() returns decimal(32,5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists utc_realtime_usec() returns bigint
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists utc_realtime_msec() returns bigint
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists utc_realtime_sec() returns bigint
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists get_nsec() returns bigint
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists usec_to_datetime(bigint) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;
create function if not exists msec_to_datetime(bigint) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;
create function if not exists dec_to_datetime(decimal) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

create function if not exists usec_to_utc_datetime(bigint) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;
create function if not exists msec_to_utc_datetime(bigint) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;
create function if not exists dec_to_utc_datetime(decimal) returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	variant;

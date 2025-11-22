

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
 --   - Mark the functions not variant. Despite the documentation, I've not seen much evidence that this results
 --     in any caching of the values, but it does mean utc_realtime_ns() is only called twice in the above
 --
 --   - You can also force the sub query to fully matarialize, by either sorting or using a 'first ..' limit
 --     but that requires everyone to know that.. sowe're going the 'not variant' path
 --
 --  I have a case open with IBM about the above
 --


create function if not exists realtime_dt() returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	not variant;


create function if not exists utc_realtime_dt() returns datetime year to fraction(5)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	not variant;



-- 
--  ANOTHER NOTE: 
--
--  Due to DBFLTMASK, dbaccess will only show 5 decimal places in it's default output
--  Using 'unload to' or another query tool will show the full 9 places
--


create function if not exists utc_realtime_dec() returns decimal(32,9)
	external name '$INFORMIXDIR/extend/realtime/realtime.bld' 
	language c  
	not variant;

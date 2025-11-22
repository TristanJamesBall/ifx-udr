
select 
	35752 as value,
	bitprint(  35752 )  as bitprint,
	bitprint4( 35752 )  as bitprint4,
	hexprint(  35752 )  as hexprint, 
	hexprint4( 35752 )  as hexprint4 
from 
	sysmaster:sysdual 
union all
select 
	357523 as value,
	bitprint(  357523 )  as bitprint,
	bitprint4( 357523 )  as bitprint4,
	hexprint(  357523 )  as hexprint, 
	hexprint4( 357523 )  as hexprint4 
from 
	sysmaster:sysdual 
union all
select 
	3575253 as value,
	bitprint(  3575253 )  as bitprint,
	bitprint4( 3575253 )  as bitprint4,
	hexprint(  3575253 )  as hexprint, 
	hexprint4( 3575253 )  as hexprint4 
from 
	sysmaster:sysdual 
order by 1 asc

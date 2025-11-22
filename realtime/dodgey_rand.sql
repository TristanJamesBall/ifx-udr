
{
   not saying this is a good idea, but this generates "fairly" random numbers
   at a rate of about 500k/sec on my laptop

  for comparison:

  while echo "$RANDOM$RANDOM"; do true ; done >out

  -	under ksh, this generates about 1.1M/sec
  - under bash, this generates about 400kM/sec

   Any kind of good native PRNG will do 10x these numbers tho! :-)

}
select
	bop_xor (
		(mod(get_nsec(),1000)*1000000 )  
		+ (mod(get_nsec(),1000)*1000  )  
		+  mod(get_nsec(),1000)
	   , dbinfo('sessionid')  
	) as rand
from
			   table( list{1,2,3,4,5,6,7,8,9,9} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	cross join table( list{1,2,3,4,5,6,7,8,9,0} )



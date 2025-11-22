#include <mi.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


/*
 * ( with thanks Daniel Stenberg, daniel@haxx.se of curl fame, I'm re-using his license notice )
 *
 *
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 2025, Tristan Ball, tristan@itcynic.com 
 * 
 * All rights reserved.
 * 
 * Permission to use, copy, modify, and distribute this software for any purpose with or without 
 * fee is hereby granted, provided that the above copyright notice and this permission notice 
 * appear in all copies.
 *
 * But I'd appreciate it you drop me a note, I'd get buzz from that.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of a copyright holder shall not be 
 * used in advertising or otherwise to promote the sale, use or other dealings in 
 * this Software without prior written authorization of the copyright holder.
 *
 *
 */


/*
 *
 * print the binary value of an integer as a string.
 * 
 * Digits are grouped by 4, and the minimum number of groups are used
 * 
 * eg: bitprint(357)
 *
 * 0001_0110_0101
 *
 * Mostly a debugging aid, I can't really think of a sensible 
 * production use for this
 *
 */

mi_lvarchar*
bitprint( mi_bigint *val ) {

        char buf[120];
		short int off;
        mi_lvarchar *ret;
		uint64_t tmp,t;

		off=0; 
		tmp = (uint64_t)*val;

		for ( t = 60; t > 0 ; t=t-4 ) {
				
			if( tmp >= ( (uint64_t)0x1 << t ) ) {
		
				snprintf( &buf[off],6, "%ld%ld%ld%ld_", 
						( (tmp>>t) >> 3 ) & 1,
						( (tmp>>t) >> 2 ) & 1,
						( (tmp>>t) >> 1 ) & 1,
						( (tmp>>t) >> 0 ) & 1 );

				off = off+5;
			}
		}

		snprintf( &buf[off],6, "%ld%ld%ld%ld_", 
				( (tmp) >> 3 ) & 1,
				( (tmp) >> 2 ) & 1,
				( (tmp) >> 1 ) & 1,
				( (tmp) >> 0 ) & 1 );
		off = off+5;

        ret = mi_new_var( strlen(buf)-1 );
        mi_set_vardata( ret, buf );

        return( ret );
}

mi_lvarchar*
bitprint4( mi_bigint *val ) {

        char buf[120];
		short int off;
        mi_lvarchar *ret;
		uint64_t tmp,t;

		off=0; 
		tmp = (uint64_t)*val;

		for ( t = 48; t > 0 ; t=t-16 ) {
				
			if( tmp >= ( (uint64_t)0x1 << t ) ) {
		
				/* ok fine, not my finest work */

				snprintf( &buf[off],21, "%ld%ld%ld%ld_%ld%ld%ld%ld_%ld%ld%ld%ld_%ld%ld%ld%ld_", 
						( (tmp>>t) >>15 ) & 1,
						( (tmp>>t) >>14 ) & 1,
						( (tmp>>t) >>13 ) & 1,
						( (tmp>>t) >>12 ) & 1,
						( (tmp>>t) >>11 ) & 1,
						( (tmp>>t) >>10 ) & 1,
						( (tmp>>t) >> 9 ) & 1,
						( (tmp>>t) >> 8 ) & 1,
						( (tmp>>t) >> 7 ) & 1,
						( (tmp>>t) >> 6 ) & 1,
						( (tmp>>t) >> 5 ) & 1,
						( (tmp>>t) >> 4 ) & 1, 
						( (tmp>>t) >> 3 ) & 1,
						( (tmp>>t) >> 2 ) & 1,
						( (tmp>>t) >> 1 ) & 1,
						( (tmp>>t) >> 0 ) & 1 );

				off = off+20;
			}
		}

		snprintf( &buf[off],21, "%ld%ld%ld%ld_%ld%ld%ld%ld_%ld%ld%ld%ld_%ld%ld%ld%ld_", 
				( (tmp>>t) >>15 ) & 1,
				( (tmp>>t) >>14 ) & 1,
				( (tmp>>t) >>13 ) & 1,
				( (tmp>>t) >>12 ) & 1, 
				( (tmp>>t) >>11 ) & 1,
				( (tmp>>t) >>10 ) & 1,
				( (tmp>>t) >> 9 ) & 1,
				( (tmp>>t) >> 8 ) & 1, 
				( (tmp>>t) >> 7 ) & 1,
				( (tmp>>t) >> 6 ) & 1,
				( (tmp>>t) >> 5 ) & 1,
				( (tmp>>t) >> 4 ) & 1,
				( (tmp>>t) >> 3 ) & 1,
				( (tmp>>t) >> 2 ) & 1,
				( (tmp>>t) >> 1 ) & 1,
				( (tmp>>t) >> 0 ) & 1 );
		off = off+20;

        ret = mi_new_var( strlen(buf)-1 );
        mi_set_vardata( ret, buf );

        return( ret );
}



/*
 *
 * print the hex value of an integer as a string.
 * 
 * unlike the builtin hex(), we don't prefix with a 0x and variable number of 0x
 * 
 * eg: hexprint(357357)
 * 
 * 573ed
 *
 * ... Because I almost never actually want the 0x000... prefix, and end up having
 * to do silly string maniplation to trim it
 * 
 *
 */

mi_lvarchar*
hexprint( mi_bigint *val ) {

        char buf[90];
		short int off;
        mi_lvarchar *ret;
		uint64_t tmp,t;

		off=0; 
		tmp = (uint64_t)*val;

		for ( t = 60; t > 0 ; t=t-4 ) {
				
			if( tmp >= ( (uint64_t)0x1 << t ) ) {
		
				snprintf( &buf[off],3, "%lx",(tmp>>t)  & 0xf );

				off = off+1;
			}
		}

		snprintf( &buf[off],3, "%lx",(tmp>>t)  & 0xf );
		off = off+1;

        ret = mi_new_var( strlen(buf) );
        mi_set_vardata( ret, buf );

        return( ret );
}


/*
 *
 * print the hex value of an integer as a string, but print in groups of 4
 * ( 32 bits ), using the mininup number of groups.
 * 
 * This version does 0 prefix, but only to pad the left most group of 4, and
 * it doesn't add the occursed 0x prefix
 * 
 * eg: hexprint4( 357357 )
 * 
 * 000573ed
 *
 * eg: hexprint4( 35735 )
 * 
 * 8b97
 */

mi_lvarchar*
hexprint4( mi_bigint *val ) {

        char buf[90];
		short int off;
        mi_lvarchar *ret;
		uint64_t tmp,t;

		off=0; 
		tmp = (uint64_t)*val;

		for ( t = 48; t > 0 ; t=t-16 ) {
				
			if( tmp >= ( (uint64_t)0x1 << t ) ) {
		
				snprintf( &buf[off],6, "%04lx",(tmp>>t)  & 0xffff );

				off = off+4;
			}
		}

		snprintf( &buf[off],6, "%04lx",(tmp>>t)  & 0xffff );
		off = off+4;

        ret = mi_new_var( strlen(buf) );
        mi_set_vardata( ret, buf );

        return( ret );
}
mi_smallint  bop_left_small( mi_smallint val, mi_smallint shift, MI_FPARAM *fParam) {

	return( (abs(shift) >= 15) ? 0 : (val << abs(shift) ) & (0xffff >> 1 ) );
}

mi_integer bop_left_int( mi_integer val, mi_smallint shift, MI_FPARAM *fParam) {

	return( (abs(shift) >= 31) ? 0 : (val << abs(shift) ) & (0xffffffff >> 1 ) );
}

mi_bigint* bop_left_big( mi_bigint *val, mi_smallint shift, MI_FPARAM *fParam) {

	*val = ( (abs(shift) >= 63) ? 0 : (*val << abs(shift) ) & (0xffffffffffffffff >> 1 ) );
	return(val);
}

mi_smallint  bop_right_small( mi_integer val, mi_smallint shift, MI_FPARAM *fParam) {

	return( (abs(shift) >= 15) ? 0 : (val >> abs(shift) ) & (0xffff >> 1 ) );
}

mi_integer bop_right_int( mi_integer val, mi_smallint shift, MI_FPARAM *fParam) {

	return( (abs(shift) >= 31) ? 0 : (val >> abs(shift) ) & (0xffffffff >> 1 ) );
}

mi_bigint* bop_right_big( mi_bigint *val, mi_smallint shift, MI_FPARAM *fParam) {

	*val = ( (abs(shift) >= 63) ? 0 : (*val >> abs(shift) ) & (0xffffffffffffffff >> 1 ) );
	return(val);
}



mi_smallint  bop_and_small( mi_smallint v1, mi_smallint v2, MI_FPARAM *fParam) {
	return( (v1 & v2 ) & (0xffff >> 1 ) );
}


mi_integer  bop_and_int( mi_integer v1, mi_integer v2, MI_FPARAM *fParam) {
	return( (v1 & v2 ) & (0xffffffff >> 1 ) );
}

mi_bigint* bop_and_big( mi_bigint *v1, mi_bigint *v2, MI_FPARAM *fParam) {
	*v1 = ( (*v1 & *v2 ) & (0xffffffffffffffff >> 1 ) );
	return(v1);
}


mi_smallint  bop_or_small( mi_smallint v1, mi_smallint v2, MI_FPARAM *fParam) {
	return( (v1 | v2 ) & (0xffff >> 1 ) );
}


mi_integer  bop_or_int( mi_integer v1, mi_integer v2, MI_FPARAM *fParam) {
	return( (v1 | v2 ) & (0xffffffff >> 1 ) );
}

mi_bigint* bop_or_big( mi_bigint *v1, mi_bigint *v2, MI_FPARAM *fParam) {
	*v1 = ( (*v1 | *v2 ) & (0xffffffffffffffff >> 1 ) );
	return(v1);
}


mi_smallint  bop_xor_small( mi_smallint v1, mi_smallint v2, MI_FPARAM *fParam) {
	return( (v1 ^ v2 ) & (0xffff >> 1 ) );
}


mi_integer  bop_xor_int( mi_integer v1, mi_integer v2, MI_FPARAM *fParam) {
	return( (v1 ^ v2 ) & (0xffffffff >> 1 ) );
}

mi_bigint* bop_xor_big( mi_bigint *v1, mi_bigint *v2, MI_FPARAM *fParam) {
	*v1 = ( (*v1 ^ *v2 ) & (0xffffffffffffffff >> 1 ) );
	return(v1);
}


mi_smallint  bop_not_small( mi_smallint v1, MI_FPARAM *fParam) {
	return( ~v1  & (0xffff >> 1 ) );
}


mi_integer  bop_not_int( mi_integer v1, MI_FPARAM *fParam) {
	return( ~v1  & (0xffffffff >> 1 ) );
}

mi_bigint* bop_not_big( mi_bigint *v1, MI_FPARAM *fParam) {
	*v1 = ( ~(*v1)  & (0xffffffffffffffff >> 1 ) );
	return(v1);
}

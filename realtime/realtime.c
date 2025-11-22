#include <stdio.h>
#include <mi.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>

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
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of a copyright holder shall not be used in advertising 
 * or otherwise to promote the sale, use or other dealings in this Software without prior written 
 * authorization of the copyright holder.
 *
 *
 */


/*
 *
 * gettimeofday() was depreciated in POSIX from 2001, and removed 2024, so we're using 
 * clock_gettime() like the new-fangled space cadets we are. 
 *
 * clock_gettime() requires specifying _which_ clock to check though...
 *
 * CLOCK_TAI seems slighly faster on my fedora laptop, and the manpage notes that CLOCK_REALTIME:
 *
 *		"This clock is affected by discontinuous jumps in the system time (e.g., if the system 
 *		 administrator manually changes the clock), and by frequency adjustments performed by NTP
 *		 and similar applications..."
 *
 * However: the other informix date/time functoins use CLOCK_REALTIME ( or gettimeofday ) which
 * will return slightly different times to CLOCK_TAI
 *
 * And CLOCK_REALTIME is Posix, so we'll go with that..
 */


#define USE_CLOCK CLOCK_REALTIME

#define NSEC   (long)1000000000
#define USEC   (long)1000000
#define MSEC   (long)1000
#define FRAC5  (long)100000
#define FRAC4  (long)10000

/* 
 * LINUX NOTE:
 * 
 * On any remotely modern linux ( since kernel 2.6 on x86.. ) both gettimeofday and clock_gettime 
 * are VDSO calls, that is, they don't require the switch to kernel mode that most syscalls require.
 * So they're very very fast. 
 * 
 * Regretably, we lose most of tha in the overheads of the Informix UDR manager but at least we're 
 * not paying extra for the syscall as well. 
 *
 * Even so, this usually returns < 10 microseconds, although the first 1-2 calls in a transaction
 * may be a little slower, and of course is MASSIVELY subject to the load on your Informix Server
 *
 */ 

/* 
 * Informix Virtual Processors:
 * 
 * While there are good arguements for running custom C udr's on a custom VP, it adds quite a bit
 * of latency, 10 microsecons becomes pretty much the best case rather than worst case, at least
 * on my test box
 *
 * But we don't block, the only syscall we make is a VDSO, and we're careful with memory
 *
 * So, I'm running it on the CPU VP's but you do you.
 *
 */

typedef struct timespec		timespec_t;

#define int16_to_dec( int_val, dec_ptr )		deccvint(    int_val, dec_ptr )
#define int32_to_dec( int_val, dec_ptr )		deccvlong(   int_val, dec_ptr )
#define int64_to_dec( int_val, dec_ptr )		biginttodec( int_val, dec_ptr )
#define dec_to_int16( dec_ptr, int_ptr )		dectoint(    dec_ptr, int_ptr )
#define dec_to_int32( dec_ptr, int_ptr )		dectolong(   dec_ptr, int_ptr )
#define dec_to_int64( dec_ptr, int_ptr )		bigintcvdec( dec_ptr, int_ptr )


/*
 * Lots of duplication below.. 
 *
 * I may bother to do a single parameterised version at some point, my original
 * thinking was that I didn't want the extra oveheads of dealing with that
 * in a routine that's supposed to be fast
 *
 * But it almost certainly wouldn't make a difference compared to the general
 * overheads of the UDR handling...
 *
 * ... so #laziness,  not #engineering it is...
 *
 */ 



/* 
 * Datetime in local time
 *
 * Return as a datetime year to fraction(5)
 *
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * If you don't have the TZ variable exported in the environment
 * before you start Informix, then localtime() does extra work
 * running stat(2) on /etc/localtime 
 *
 * For something that might be called thousands or millions of times a
 * second, that will matter ( and it's a syscall we dont want
 * running here in particular!! )
 *
 * Either:
 *  - Setup /etc/localtime and export TZ=:/dev/localtime
 *  - export TZ=your_time_zone
 *
 * Friends don't let friends leave TZ blank.. well, my friends would
 * but hopefully you have nicer ones.
 *
 */

mi_datetime*
realtime_dt( MI_FPARAM *fParam ) {

    mi_datetime *ret, *dt;
    timespec_t ts;
    struct tm tm;
    char buf[40];

    clock_gettime(USE_CLOCK, &ts);

	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in realtime_dt()" );
       return(ret);

    }

    localtime_r( &ts.tv_sec, &tm );
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",(ts.tv_nsec/10000) % FRAC5 ); 

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}



/* 
 * Datetime in UTC/GMT0
 *
 * Return as a datetime year to fraction(5)
 *
 */

mi_datetime*
utc_realtime_dt( MI_FPARAM *fParam ) {

    mi_datetime *ret, *dt;
    timespec_t ts;
    struct tm tm;
    char buf[40];

    clock_gettime(USE_CLOCK, &ts);

	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dt()" );
       return(ret);

    }

    gmtime_r( &ts.tv_sec, &tm );
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",(ts.tv_nsec/10000) % FRAC5 ); 

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}



/* 
 *
 * Nanoseconds since the epoch, as a decimal 
 *
 * Returns seconds.nanoseconds
 *
 */

mi_decimal*
utc_realtime_dec( MI_FPARAM *fParam ) {

    mi_decimal* ret;
    timespec_t ts;
    mi_decimal dec_part, nsec;


    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_decimal *)mi_dalloc(sizeof(mi_decimal), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dec()" );
       return(ret);

    }

	int64_to_dec( NSEC, &nsec );
    int64_to_dec( ts.tv_sec, ret );
    int64_to_dec( ts.tv_nsec, &dec_part );

	decdiv( &dec_part, &nsec, &dec_part );
    decadd( ret, &dec_part, ret );

    return( ret );

}


/* 
 *
 * Seconds to Fraction(5) Since the Epoch, as a decimal
 * 
 *
 * Returns seconds.fffff
 *
 */

mi_decimal*
utc_realtime_frac5( MI_FPARAM *fParam ) {

    mi_decimal* ret;
    timespec_t ts;
    mi_decimal dec_part, frac4;
	/* frac4 here beccause 4+5 == 9 */

    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_decimal *)mi_dalloc(sizeof(mi_decimal), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dec()" );
       return(ret);

    }

	int64_to_dec( FRAC4, &frac4 );
    int64_to_dec( ts.tv_sec, ret );
    int64_to_dec( ts.tv_nsec, &dec_part );

	decdiv( &dec_part, &frac4, &dec_part );
    decadd( ret, &dec_part, ret );

    return( ret );

}



/* 
 *
 * Microseconds since the epoch, as a bigint whole number.
 * This is fairly safe given the wrap times:
 *
 * $ printf " %ld Years\n" $(( 0x7fffffffffffffff / (24 * 60 * 60 * 365 * 1000 * 1000) ))
 *
 *   292471 Years
 *
 * Returns microseconds
 *
 */

mi_bigint*
utc_realtime_msec( MI_FPARAM *fParam ) {

    mi_bigint* ret;
    timespec_t ts;


    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_bigint *)mi_dalloc(sizeof(mi_bigint), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dec()" );
       return(ret);

    }

    *ret = ts.tv_sec * MSEC;
	*ret += ts.tv_nsec / USEC;

    return( ret );

}


/* 
 *
 * Milliseconds since the epoch, as a bigint whole number.
 * Wrap times: 
 *
 * $  printf " %ld Years\n" $(( 0x7fffffffffffffff / (24 * 60 * 60 * 365 * 1000) ))
 *
 *   292471208 Years
 *
 * Returns milliseconds
 *
 */

mi_bigint*
utc_realtime_usec( MI_FPARAM *fParam ) {

    mi_bigint* ret;
    timespec_t ts;


    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_bigint *)mi_dalloc(sizeof(mi_bigint), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dec()" );
       return(ret);

    }

    *ret = ts.tv_sec * USEC;
	*ret += ts.tv_nsec / MSEC;

    return( ret );

}



/* 
 *
 * Seconds since the epoch, as a bigint whole number.
 *
 * Looking in sysmaster:sshmvals will be faster than this, but gives as slightly different
 * timestamp, so including this just for consistancy with the interface
 *
 * Returns seconds
 *
 */

mi_bigint*
utc_realtime_sec( MI_FPARAM *fParam ) {

    mi_bigint* ret;
    timespec_t ts;


    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_bigint *)mi_dalloc(sizeof(mi_bigint), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in utc_realtime_dec()" );
       return(ret);

    }

    *ret = ts.tv_sec;

    return( ret );

}

mi_datetime*
msec_to_datetime( bigint *msec,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in msec_to_datetime()" );
       return(ret);

    }
	int_part = (time_t)( *msec / MSEC );
	dec_part = (int64_t)( *msec % MSEC );
    localtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],6,".%03ld",dec_part % MSEC);

    dt = mi_string_to_datetime(buf, "datetime year to fraction(3)");
    *ret = *dt;

    return(ret);
}



mi_datetime*
msec_to_utc_datetime( bigint *msec,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in msec_to_datetime()" );
       return(ret);

    }
	int_part = (time_t)( *msec / MSEC );
	dec_part = (int64_t)( *msec % MSEC );
    gmtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],6,".%03ld",dec_part % MSEC);

    dt = mi_string_to_datetime(buf, "datetime year to fraction(3)");
    *ret = *dt;

    return(ret);
}

mi_datetime*
usec_to_datetime( bigint *usec,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in usec_to_datetime()" );
       return(ret);

    }
	int_part = (time_t)( *usec / USEC );
	dec_part = (int64_t)( (*usec % USEC)/10 );
    localtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",dec_part % FRAC5);

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}

mi_datetime*
usec_to_utc_datetime( bigint *usec,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in usec_to_datetime()" );
       return(ret);

    }
	int_part = (time_t)( *usec / USEC );
	dec_part = (int64_t)( (*usec % USEC)/10 );
    gmtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",dec_part % FRAC5);

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}

mi_datetime*
dec_to_datetime( mi_decimal *in,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	mi_decimal dec_int_part,dec_dec_part,dec_nsec;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in usec_to_datetime()" );
       return(ret);

    }
	/* what a palaval... */

	int64_to_dec( NSEC, &dec_nsec );

    deccopy( in, &dec_int_part );
    dectrunc( &dec_int_part,0 );
    decsub( in, &dec_int_part, &dec_dec_part );
	decmul( &dec_dec_part, &dec_nsec, &dec_dec_part );

	dec_to_int64(&dec_int_part,&int_part);
	dec_to_int64(&dec_dec_part,&dec_part);
	/* 
	 * At this point int_part holds our seconds, as a whole number
	 * while dec_part holds 'nanoseconds', again as a while number.
	 * If the incoming decimal only had say 2 decimal places, 
	 * our nanosecond will loook like nn0000000
	 */

    localtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",(dec_part / FRAC4)%FRAC5 );

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}



mi_datetime*
dec_to_utc_datetime( mi_decimal *in,MI_FPARAM *fParam  ) {

    mi_datetime *ret,*dt;
    char buf[60];
    struct tm tm;
	mi_decimal dec_int_part,dec_dec_part,dec_nsec;
	time_t int_part;
	int64_t dec_part;


	if ( ( ret = (mi_datetime *)mi_dalloc(sizeof(mi_datetime), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in dec_to_utc_datetime)" );
       return(ret);

    }
	/* what see localtime version above for comments */

	int64_to_dec( NSEC, &dec_nsec );

    deccopy( in, &dec_int_part );
    dectrunc( &dec_int_part,0 );
    decsub( in, &dec_int_part, &dec_dec_part );
	decmul( &dec_dec_part, &dec_nsec, &dec_dec_part );

	dec_to_int64(&dec_int_part,&int_part);
	dec_to_int64(&dec_dec_part,&dec_part);

    gmtime_r( &int_part, &tm );

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    snprintf(&buf[19],8,".%05ld",(dec_part / FRAC4)%FRAC5 );

    dt = mi_string_to_datetime(buf, "datetime year to fraction(5)");
    *ret = *dt;

    return(ret);
}




/* 
 *
 * Nanosecond value for the current second, as a bigint whole number
 *
 * NOT A FULL TIME
 *
 * Returns nanoseconds
 *
 */

mi_bigint*
get_nsec( MI_FPARAM *fParam ) {

    mi_bigint* ret;
    timespec_t ts;


    clock_gettime(USE_CLOCK, &ts);


	if ( ( ret = (mi_bigint *)mi_dalloc(sizeof(mi_bigint), PER_COMMAND) ) == NULL ) {

       mi_fp_setreturnisnull(fParam, 0, MI_TRUE);
       mi_db_error_raise(NULL, MI_EXCEPTION, "Memory allocation failure in get_nsec()" );
       return(ret);

    }

    *ret = ts.tv_nsec;

    return( ret );

}

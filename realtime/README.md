
# Table of contents

- [realtime.udr](#realtimeudr)
  - [Outstanding Issues](#outstanding-issues)
  - [Example Use](#example-use)
  - [Example Result](#example-result)
- [Performanance](#performance)
  - [Test Notes](#test-notes)
  - [Test Results](#test-results)
- [Installing](#installing)

# realtime.udr - Informix Extension

A C User Defined Routine wrapper around `clock_gettime(2)` which gets the current time in UTC or Localtime

 - By default returns a `datetime year to fraction(5)`
 - Provides a new timestamp (if the time has changed!) every call, 
   - **Even when called multiple times from within a stored proc or transaction** unlike `current` or `sysdate`
 - Fairly quick - unlikely to be noticible tbh
 - Safe, to the best of my knowledge, although I"ve only tested on Linux
 - Extra bits in the code not shown here:
   - Functions to return the time as decimal to Nanosecond resolution 
   - Millisecond/Microsecond since Unix epoch as time as bigint whole numbers
   - Conversion functions to convert either  these back to datetimes, with some loss of precsision

## Outstanding Issues

 - C code has a lot of duplication betweeen the various functions, could be cleaned up a lot

## Example Use

```
select * from (
    select
        sysmaster:yieldn(1) as yield,
        current             as current,
        sysdate             as sysdate,
        realtime_dt()       as realtime_dt
    from
        sysmaster:sysdual
    union all
    select
        sysmaster:yieldn(1) as yield,
        current             as current,
        sysdate             as sysdate,
        realtime_dt()       as realtime_dt
    from
        sysmaster:sysdual
    union all
    select
        sysmaster:yieldn(1) as yield,
        current             as current,
        sysdate             as sysdate,
        realtime_dt()       as realtime_dt
    from
        sysmaster:sysdual
    order by 4
);
```

## Example Result 

Note the `realtime_dt` values change, the current and sysdate lines don't

```
yield        1
current      2025-11-22 19:59:11.383
sysdate      2025-11-22 19:59:11.38320
realtime_dt  2025-11-22 19:59:11.47913

yield        1
current      2025-11-22 19:59:11.383
sysdate      2025-11-22 19:59:11.38320
realtime_dt  2025-11-22 19:59:12.47930

yield        1
current      2025-11-22 19:59:11.383
sysdate      2025-11-22 19:59:11.38320
realtime_dt  2025-11-22 19:59:13.47943
```


# Performance 

## Test Notes

`utc_realtime_dec()` has been cast to vharchar to avoid dbaccess DBFLTMASK limits on decimal sizes

These are microbencmarks, take with salt!

mostly they're jus showing this is not 'wildly worse', althought tome of my tests have 'current' being notable faster (eg to a temp table)....
But it still has the limitation of only providing a single timestamp for the entire query, so that's not particularly meaningful to me.

The following tests show Lines (records)/sec for a dbaccess unload query, that only calls the timestamp in question, that is, it's a 10M row single field query.

## Test Results
```
============== realtime_demo.sh ==================

================== OUTPUT DEMO ==================

Before_Date       2025-11-22 19:28:16.580

current           2025-11-22 19:28:16.586
sysdate           2025-11-22 19:28:16.58600
realtime_dt       2025-11-22 19:28:16.58590
utc_realtime_dt   2025-11-22 08:28:16.58592
utc_realtime_dec  1763800096.585946392

After_Date        2025-11-22 19:28:16.586

================== Fixed String Rate Tests ==================

'current' Lines/Sec ............... - Rate: [1.34M/s]  Count: 10.0M  Elapsed: 0:00:07
'sysdate' Lines/Sec ............... - Rate: [1.30M/s]  Count: 10.0M  Elapsed: 0:00:07
'realtime_dt()' Lines/Sec ......... - Rate: [2.02M/s]  Count: 10.0M  Elapsed: 0:00:04
'utc_realtime_dt()' Lines/Sec ..... - Rate: [1.99M/s]  Count: 10.0M  Elapsed: 0:00:05
'utc_realtime_dec()' Lines/Sec .... - Rate: [1.96M/s]  Count: 10.0M  Elapsed: 0:00:05


================== Raw Rate Tests ==================

'current' Lines/Sec ............... - Rate: [1.30M/s]  Count: 10.0M  Elapsed: 0:00:07
'sysdate' Lines/Sec ............... - Rate: [1.33M/s]  Count: 10.0M  Elapsed: 0:00:07
'realtime_dt()' Lines/Sec ......... - Rate: [1.48M/s]  Count: 10.0M  Elapsed: 0:00:06
'utc_realtime_dt()' Lines/Sec ..... - Rate: [1.48M/s]  Count: 10.0M  Elapsed: 0:00:06
'utc_realtime_dec()' Lines/Sec .... - Rate: [2.86M/s]  Count: 10.0M  Elapsed: 0:00:03
```


# Installing

- EDIT THE MAKEFILE!!
- You'll need to set a target DB, and probably change the RUNAS

```
su - informix
make
make install
```
**Make sure you copy `$INFORMIXDIR/extend/realtime` folder to add db servers in your cluster**

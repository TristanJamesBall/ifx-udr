
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
)

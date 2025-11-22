##
## With thanks too Paul Watson
## Email: paul@oninit.com
##


default : bitops.bld

PROJECT = bitops
DBNAME = tjb
RUNAS = sudo -u informix 


CC = gcc
CFLAGS = -Wall -pedantic -O3
CBLDINCLS = -I${INFORMIXDIR}/incl/public -I${INFORMIXDIR}/incl/esql -I${INFORMIXDIR}/incl
CBLDFLAGS = -DMI_SERVBUILD -D_GNU_SOURCE -fPIC -shared
LDBLDFLAGS = -shared -u _etext

RM = rm -f
MKDIR= mkdir -p

.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	${CC} ${CFLAGS} ${CBLDFLAGS} ${CBLDINCLS} -c $<

FILES.c = ${PROJECT}.c

FILES.o = ${FILES.c:.c=.o}

${PROJECT}.bld: ${FILES.o}
	${LD} ${LDBLDFLAGS} -o $@ ${FILES.o}

clean:
	${RM} ${FILES.o} ${PROJECT}.bld

install: ${PROJECT}.bld
	${RUNAS} ${MKDIR} ${INFORMIXDIR}/extend/${PROJECT}
	${RUNAS} chmod 775 ${INFORMIXDIR}/extend/${PROJECT}
	${RUNAS} cp -f ${PROJECT}.bld      ${INFORMIXDIR}/extend/${PROJECT}
	${RUNAS} cp -f ${PROJECT}*reg.sql  ${INFORMIXDIR}/extend/${PROJECT}
	${RUNAS} chmod 664  ${INFORMIXDIR}/extend/${PROJECT}/*
	${RUNAS} dbaccess -a ${DBNAME} ${PROJECT}_reg.sql 2>&1 

uninstall: ${PROJECT}.bld
	${RUNAS} dbaccess -a ${DBNAME} ${PROJECT}_unreg.sql 2>&1 
	${RUNAS} ${RM} ${INFORMIXDIR}/extend/${PROJECT}/*
	${RUNAS} rmdir ${INFORMIXDIR}/extend/${PROJECT}


	LOC Data_Segment
	GREG @
ARG	OCTA 0
	OCTA 3
	OCTA 3
	OCTA 5

	LOC #100
Main	IS @
	LDA $5,ARG
	SETL $1,1*8  // account for data width
	SETL $2,2*8
	SETL $3,3*8

	LDO $6,$5
	LDO $7,$5,$1
	LDO $8,$5,$2
	LDO $9,$5,$3

	CMP $11,$6,$8
	CSZ $21,$11,1
	CMP $12,$7,$8
	CMP $13,$9,$6
	ADD $14,$12,$13
	SETL $15,2
	CMP $16,$14,$15
	BNZ $16,9F
	SETL $21,1
9H	TRAP 0,Halt,0

	LOC Data_Segment
	; this line fixes: no base address is close enough to the address A!
	GREG @  
Array	OCTA 12
	OCTA 4
	OCTA 8
	OCTA 9
	OCTA 42

	LOC #100
Main	IS @
	SETL $3,0
	LDA $2,Array
	SETL $1,0
1H	LDO $4,$2,$3
	CMP $5,$1,$4
	BP $5,2F
	SET $1,$4
2H	SET $6,8
	ADD $3,$3,$6
	CMP $7,$3,5*8
	BN $7,1B
	SET $8,$1
	TRAP 0,Halt,0

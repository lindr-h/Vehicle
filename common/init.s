;=========================================
; NAME: init.s
; DESC: C start up codes
;       Configure memory, ISR ,stacks
;	Initialize C-variables
; HISTORY:
; 2002.02.25:kwtark: ver 0.0
; 2002.03.20:purnnamu: Add some functions for testing STOP,POWER_OFF mode
; 2002.04.10:SJS:sub interrupt disable 0x3ff -> 0x7ff
;=========================================

	GET memcfg.s
	GET s3c2410x.s

BIT_SELFREFRESH EQU	(1<<22)

;Pre-defined constants
USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0

;The location of stacks
UserStack	EQU	(_STACK_BASEADDRESS-0x3800)		;0x33ff4800 ~
SVCStack    EQU	(_STACK_BASEADDRESS-0x2800)		;0x33ff5800 ~
UndefStack	EQU	(_STACK_BASEADDRESS-0x2400)		;0x33ff5c00 ~
AbortStack	EQU	(_STACK_BASEADDRESS-0x2000)		;0x33ff6000 ~
IRQStack    EQU	(_STACK_BASEADDRESS-0x1000)		;0x33ff7000 ~
FIQStack	EQU	(_STACK_BASEADDRESS-0x0)		;0x33ff8000 ~

;Check if tasm.exe(armasm -16 ...@ADS 1.0) is used.
	GBLL    THUMBCODE
	[ {CONFIG} = 16
THUMBCODE SETL  {TRUE}
		CODE32
	|
THUMBCODE SETL  {FALSE}
	]

	MACRO
	MOV_PC_LR
	[ THUMBCODE
		bx lr
	|
		mov	pc,lr
	]
	MEND

	MACRO
	MOVEQ_PC_LR
	[ THUMBCODE
		bxeq lr
	|
		moveq pc,lr
	]
	MEND

	MACRO
$HandlerLabel HANDLER $HandleLabel

$HandlerLabel
	sub	sp,sp,#4				;decrement sp(to store jump address)
	stmfd	sp!,{r0}			;PUSH the work register to stack(lr does not push because it return to original address)
	ldr     r0,=$HandleLabel	;load the address of HandleXXX to r0
	ldr     r0,[r0]				;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]			;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}			;POP the work register and pc(jump to ISR)
	MEND


	IMPORT  |Image$$RO$$Limit|  ; End of ROM code (=start of ROM data)
	IMPORT  |Image$$RO$$Base|
	IMPORT  |Image$$RW$$Limit|
	IMPORT  |Image$$RW$$Base|   ; Base of RAM to initialise
	IMPORT  |Image$$ZI$$Base|   ; Base and limit of area
	IMPORT  |Image$$ZI$$Limit|  ; to zero initialise
	IMPORT  ISRVector;


	AREA  Init,CODE,READONLY
	ENTRY



	;1)The code, which converts to Big-endian, should be in little endian code.
	;2)The following little endian code will be compiled in Big-Endian mode.
	;  The code byte order should be changed as the memory bus width.
	;3)The pseudo instruction,DCD can not be used here because the linker generates error.

	ASSERT	:DEF:ENDIAN_CHANGE
	[ ENDIAN_CHANGE
		ASSERT  :DEF:ENTRY_BUS_WIDTH
		[ ENTRY_BUS_WIDTH=32
			b	ChangeBigEndian	    ;DCD 0xea000007
		]

		[ ENTRY_BUS_WIDTH=16
			andeq	r14,r7,r0,lsl #20   ;DCD 0x0007ea00
		]

		[ ENTRY_BUS_WIDTH=8
			streq	r0,[r0,-r10,ror #1] ;DCD 0x070000ea
		]
	|
		b	ResetHandler
	]
	b	HandlerUndef	;handler for Undefined mode
	b	HandlerSWI		;handler for SWI interrupt
	b	HandlerPabort	;handler for PAbort
	b	HandlerDabort	;handler for DAbort
	b	.				;reserved
	b	HandlerIRQ		;handler for IRQ interrupt
	b	HandlerFIQ		;handler for FIQ interrupt


;@0x20
	b EnterPWDN

ChangeBigEndian
;@0x24
	[ ENTRY_BUS_WIDTH=32
		DCD	0xee110f10	;0xee110f10 => mrc p15,0,r0,c1,c0,0
		DCD	0xe3800080	;0xe3800080 => orr r0,r0,#0x80;  //Big-endian
		DCD	0xee010f10	;0xee010f10 => mcr p15,0,r0,c1,c0,0
	]
	[ ENTRY_BUS_WIDTH=16
		DCD 0x0f10ee11
		DCD 0x0080e380
		DCD 0x0f10ee01
	]
	[ ENTRY_BUS_WIDTH=8
		DCD 0x100f11ee
		DCD 0x800080e3
		DCD 0x100f01ee
	]
	DCD 0xffffffff		;swinv 0xffffff is similar with NOP and run well in both endian mode.
	DCD 0xffffffff
	DCD 0xffffffff
	DCD 0xffffffff
	DCD 0xffffffff
	b ResetHandler

;Function for entering power down mode
; 1. SDRAM should be in self-refresh mode.
; 2. All interrupt should be maksked for SDRAM/DRAM self-refresh.
; 3. LCD controller should be disabled for SDRAM/DRAM self-refresh.
; 4. The I-cache may have to be turned on.
; 5. The location of the following code may have not to be changed.

;void EnterPWDN(int CLKCON);
EnterPWDN
	mov r2,r0			;r2=rCLKCON
	tst r0,#0x8			;POWER_OFF mode?
	bne ENTER_POWER_OFF

ENTER_STOP
	ldr r0,=REFRESH
	ldr r3,[r0]			;r3=rREFRESH
	mov r1, r3
	orr r1, r1, #BIT_SELFREFRESH
	str r1, [r0]		;Enable SDRAM self-refresh

	mov r1,#16			;wait until self-refresh is issued. may not be needed.
0
	subs r1,r1,#1
	bne %B0

	ldr r0,=CLKCON		;enter STOP mode.
	str r2,[r0]

	mov r1,#32
0
	subs r1,r1,#1		;1) wait until the STOP mode is in effect.
	bne %B0				;2) Or wait here until the CPU&Peripherals will be turned-off
						;   Entering POWER_OFF mode, only the reset by wake-up is available.

	ldr r0,=REFRESH		;exit from SDRAM self refresh mode.
	str r3,[r0]

	MOV_PC_LR

ENTER_POWER_OFF
	;NOTE.
	;1) rGSTATUS3 should have the return address after wake-up from POWER_OFF mode.

	ldr r0,=REFRESH
	ldr r1,[r0]							;r1=rREFRESH
	orr r1, r1, #BIT_SELFREFRESH
	str r1, [r0]						;Enable SDRAM self-refresh

	mov r1,#16			;Wait until self-refresh is issued,which may not be needed.
0
	subs r1,r1,#1
	bne %B0

	ldr 	r1,=MISCCR
	ldr	r0,[r1]
	orr	r0,r0,#(7<<17)	;Make sure that SCLK0:SCLK->0, SCLK1:SCLK->0, SCKE=L during boot-up
	str	r0,[r1]

	ldr r0,=CLKCON
	str r2,[r0]

	b .					;CPU will die here.


WAKEUP_POWER_OFF
	;Release SCLKn after wake-up from the POWER_OFF mode.
	ldr r1,=MISCCR
	ldr	r0,[r1]
	bic	r0,r0,#(7<<17)	;SCLK0:0->SCLK, SCLK1:0->SCLK, SCKE:L->H
	str	r0,[r1]

	;Set memory control registers
	ldr	r0,=SMRDATA
	ldr	r1,=BWSCON		;BWSCON Address
	add	r2, r0, #52		;End address of SMRDATA
0
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	cmp	r2, r0
	bne	%B0

	mov r1,#256
0
	subs r1,r1,#1		;1) wait until the SelfRefresh is released.
	bne %B0

	ldr r1,=GSTATUS3	;GSTATUS3 has the start address just after POWER_OFF wake-up
	ldr r0,[r1]
	mov pc,r0


	LTORG
HandlerFIQ		HANDLER HandleFIQ
HandlerIRQ		HANDLER HandleIRQ
HandlerUndef	HANDLER HandleUndef
HandlerSWI		HANDLER HandleSWI
HandlerDabort	HANDLER HandleDabort
HandlerPabort	HANDLER HandlePabort

	EXPORT IsrIRQ
IsrIRQ
	sub	sp,sp,#4		;reserved for PC
	stmfd	sp!,{r8-r9}

	ldr	r9,=INTOFFSET
	ldr	r9,[r9]
	ldr	r8,=ISRVector
	add	r8,r8,r9,lsl #2
	ldr	r8,[r8]
	str	r8,[sp,#8]
	mov r0,r9
	ldmfd	sp!,{r8-r9,pc}

;=======
; ENTRY
;=======
ResetHandler
	;// 禁止看门狗
	ldr	r0,=WTCON
	ldr	r1,=0x0
	str	r1,[r0]

	;// 禁止所有的中断
	ldr	r0,=INTMSK
	ldr	r1,=0xffffffff
	str	r1,[r0]

	;// 禁止所有的子中断
	ldr	r0,=INTSUBMSK
	ldr	r1,=0x7ff
	str	r1,[r0]

	;// 调整锁相环，设置系统工作频率
	ldr	r0,=LOCKTIME
	ldr	r1,=0xffffff
	str	r1,[r0]

	;// 设置CPU工作频率 Fin = 12MHz, Fout = 202.80MHz
	LDR R0,=MPLLCON
	LDR R1,=0X70022
	STR R1,[R0]

	;// 设置时钟对所有的片内外设有效
	LDR	    r0,=CLKCON
	LDR	    r1,= 0x7FFF0
	STR	    r1,[r0]

	;// 关闭MMU
	MOV R0,#0
	MCR p15,0,R0,c1,c0,0

	;// 检查启动原因
	ldr	r1,=GSTATUS2
	ldr	r0,[r1]
	tst	r0,#0x2

	;// 如果是 POWER_OFF 模式，启动 POWER_OFF_WAKEUP 处理.
	bne	WAKEUP_POWER_OFF

	EXPORT StartPointAfterPowerOffWakeUp
StartPointAfterPowerOffWakeUp

	;// 设置内存控制寄存器，配置内存参数，初始化内存
	LDR R0,=BWSCON
	LDR R1,=0X22111110
	STR R1,[R0]
	LDR R0,=BANKCON0
	LDR R1,=0X700
	STR R1,[R0]
	LDR R0,=BANKCON6
	LDR R1,=0X18005
	STR R1,[R0]
	LDR R0,=BANKCON7
	LDR R1,=0x18005
	STR R1,[R0]
	LDR R0,=REFRESH
	LDR R1,=0X8E0459
	STR R1,[R0]
	LDR R0,=BANKSIZE
	LDR R1,=0X32
	STR R1,[R0]
	LDR R0,=MRSRB6
	LDR R1,=0X30
	STR R1,[R0]
	LDR R0,=MRSRB7
	LDR R1,=0X30
	STR R1,[R0]

	;// 复制 RO, RW，设置 ZI 段
	LDR    r0,=|Image$$RO$$Limit|		;// Get the start of the
	LDR    r1,=|Image$$RW$$Base|		;// Get the start of the
	LDR    r3,=|Image$$ZI$$Base|		;// Pickup the start of
	LDR    r4,=|Image$$RO$$Base|
	LDR    r7,=|Image$$RW$$Limit|
	LDR    r5,=0x20000
	;LDR    r5,=0x8000000

	CMP    r4,pc
	BCC    INT_RAM_COPY2

INT_ROM_COPY
	CMP    r4,r0
	BEQ    INT_RAM_COPY
	LDR    r6,[r5],#4
	STR    r6,[r4],#4
	B      INT_ROM_COPY

INT_RAM_COPY
	CMP    r1,r3
	BEQ    INT_BSS_Clear
	LDR    r6,[r5],#4
	STR    r6,[r1],#4
	B      INT_RAM_COPY


INT_RAM_COPY2
	CMP    r0,r1
	BEQ    INT_BSS_Clear

INT_ROM_Vars_Copy
	CMP    r1,r3
	BEQ    INT_BSS_Clear				;// Check to set flags
	LDR    r6, [r0], #4					;// Get value from ROM
	STR    r6, [r1], #4					;// Put value in RAM
	B      INT_ROM_Vars_Copy			;// Continue

INT_BSS_Clear
	LDR    r1,=|Image$$ZI$$Limit|		;// Pickup the end of the BSS area
	MOV    r2,#0						;// Clear value in r2

INT_BSS_Clear_Loop
	CMP    r3,r1						;// Are the start and end equal?
	STRCC  r2,[r3],#4					;// Clear a word
	BCC    INT_BSS_Clear_Loop			;// If so, continue with BSS clear

	MOV r3,r3,LSR#12
	ADD r3,r3,#1
	MOV r3,r3,LSL#12

;function initializing stacks
InitStacks
	;Do not use DRAM,such as stmfd,ldmfd......
	;SVCstack is initialized before
	;Under toolkit ver 2.5, 'msr cpsr,r1' can be used instead of 'msr cpsr_cxsf,r1'
	mrs	r0,cpsr
	bic	r0,r0,#MODEMASK
	orr	r1,r0,#FIQMODE|NOINT
	msr	cpsr_cxsf,r1		;FIQMode
	add r3,r3,#0x1000
	mov sp,r3

	orr	r1,r0,#IRQMODE|NOINT
	msr	cpsr_cxsf,r1		;IRQMode
	add r3,r3,#0x1000
	mov sp,r3

	orr	r1,r0,#ABORTMODE|NOINT
	msr	cpsr_cxsf,r1		;AbortMode
	add r3,r3,#0x400
	mov sp,r3

	orr	r1,r0,#UNDEFMODE|NOINT
	msr	cpsr_cxsf,r1		;UndefMode
	add r3,r3,#0x400
	mov sp,r3

	bic	r0,r0,#MODEMASK|NOINT
	orr	r1,r0,#SVCMODE
	msr	cpsr_cxsf,r1		;SVCMode
	add r3,r3,#0x1000
	mov sp,r3

	IMPORT main

	ldr pc,=main

	LTORG
SMRDATA DATA
	; Memory configuration should be optimized for best performance
	; The following parameter is not optimized.
	; Memory access cycle parameter strategy
	; 1) The memory settings is  safe parameters even at HCLK=75Mhz.
	; 2) SDRAM refresh period is for HCLK=75Mhz.
	[ :DEF:_XCHG_CGSx
		DCD (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
	|
		DCD (8+(B1_BWSCON<<2)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
	]
	DCD ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))   ;GCS0
	DCD ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))   ;GCS1
	DCD ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))   ;GCS2
	DCD ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))   ;GCS3
	DCD ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))   ;GCS4
	DCD ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))   ;GCS5
	DCD ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))    ;GCS6
	DCD ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))    ;GCS7
	DCD ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)

	DCD 0x32            ;SCLK power saving mode, BANKSIZE 128M/128M

	DCD 0x30            ;MRSR6 CL=3clk
	DCD 0x30            ;MRSR7

	ALIGN


	EXPORT Handle
Handle DATA

HandleUndef 	DCD   4
HandleSWI   	DCD   4
HandlePabort    DCD   4
HandleDabort    DCD   4
HandleReserved  DCD   4
HandleIRQ   	DCD   4
HandleFIQ   	DCD   4

	END

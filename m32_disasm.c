
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>
#include <inttypes.h>

#include "mips32.h"

#define DISASM
#define EMULATOR


static void cleanup();

FILE	*outfile;
uint8_t *buffer;

enum {
	MIPS32_ZERO,MIPS32_AT,
	MIPS32_V0,MIPS32_V1,
	MIPS32_A0,MIPS32_A1,MIPS32_A2,MIPS32_A3,
	MIPS32_T0,MIPS32_T1,MIPS32_T2,MIPS32_T3,MIPS32_T4,MIPS32_T5,MIPS32_T6,MIPS32_T7,
	MIPS32_S0,MIPS32_S1,MIPS32_S2,MIPS32_S3,MIPS32_S4,MIPS32_S5,MIPS32_S6,MIPS32_S7,
	MIPS32_T8,MIPS32_T9,
	MIPS32_K0,MIPS32_K1,
	MIPS32_GP,
	MIPS32_SP,
	MIPS32_FP,
	MIPS32_RA,
};

//	names of the registers as strings 
char *regnames[]={
		"$zero",
		"$at",
		"$v0","$v1",
		"$a0","$a1","$a2","$a3",
		"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
		"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
		"$t8","$t9",
		"$k0","$k1",
		"$gp",
		"$sp",
		"$fp",
		"$ra"};


MIPS32 mcpu;
uint8_t mcpu_ram[65536];	//	64kb max for now

uint8_t read8(uint32_t address)
{
//	printf("read from ram %x\n",address);
	return mcpu_ram[address&0xffff];
}

void write8(uint32_t address,uint8_t data)
{
//	printf("write to ram %x\n",address);
	mcpu_ram[address&0xffff]=data;
}

uint16_t read16(uint32_t address)
{
uint32_t vad = address&0xffff;

	uint16_t val = mcpu.read8(vad+1) << 8 |
								 mcpu.read8(vad);
	printf("read %08x from  %x\n",val,address);
	return val;
}

uint32_t read32(uint32_t address)
{
uint32_t vad = address&0xffff;
	uint32_t val = 	mcpu.read8(vad+3) << 24|
									mcpu.read8(vad+2) << 16 |
									mcpu.read8(vad+1) << 8 |
									mcpu.read8(vad);
	printf("read %08x from  %x\n",val,address);
	return val;
}

void write16(uint32_t address,uint16_t data)
{
uint32_t vad = address&0xffff;	

	printf("write %08x to ram %x\n",data,address);

	mcpu.write8(vad,data&0xff);
	mcpu.write8(vad+1,(data>>8)&0xff);
}

void write32(uint32_t address,uint32_t data)
{
uint32_t vad = address&0xffff;	

	printf("write %08x to ram %x\n",data,address);

	mcpu.write8(vad,data&0xff);
	mcpu.write8(vad+1,(data>>8)&0xff);
	mcpu.write8(vad+2,(data>>16)&0xff);
	mcpu.write8(vad+3,(data>>24)&0xff);
}

void mips_reset()
{
	memset(mcpu_ram,0,65536);
	for (int q=0;q<32;q++)
		mcpu.Registers[q]=0;
	mcpu.PC = 0;
	mcpu.read8 = read8;
	mcpu.write8 = write8;
}

void mips_dumpram()
{
FILE *fp = fopen("dump.bin","wb");
	fwrite(mcpu_ram,1,65536,fp);
	fclose(fp);
}

void mips_display()
{
	fprintf(outfile,"PC:%08X\n",mcpu.PC);
	for (int q=0;q<32;q++)
	{
		fprintf(outfile,"%s=%016X\t",regnames[q],mcpu.Registers[q]);
		if ((q&7)==7)
			fprintf(outfile,"\n");
	}
}

//	return correct signed hex string
//	for example 0xa00 or -0xa00 this is how assemblers accept the values 

char hexstring[256];	
char *shex(uint16_t value)
{
bool isneg=value&0x8000;
	if (isneg==0)
		sprintf(hexstring,"0x%04X",value&0x7fff);
	else		
		sprintf(hexstring,"-0x%04X",(0-value)&0x7fff);
	return hexstring;
}

//	opcode type and structure
typedef union 
{
	//opcode	rs			rt			rd			shift 	funct
	//6 bits	5 bits	5 bits	5 bits	5 bits	6 bits
	//	R type instruction
	struct {
		uint32_t funct:6;
		uint32_t shift:5;
		uint32_t rd:5;
		uint32_t rt:5;
		uint32_t rs:5;
		uint32_t op:6;
	} R;

	//opcode	rs	rt	IMM
	//6 bits	5 bits	5 bits	16 bits
	//	I type instruction
	struct {
		uint32_t imm:16;
		uint32_t rt:5;
		uint32_t rs:5;
		uint32_t op:6;
	} I;

	//opcode	rs	rt	IMM
	//6 bits	5 bits	5 bits	16 bits
	//	J type instruction
	struct {
		uint32_t addr:26;
		uint32_t op:6;
	} J;

} MIPSI;


//	basically mips_instructionmame
//	R type opcodes

void mips_add(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ADD %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->R.rd] = (int32_t)mcpu.Registers[m->R.rs] + (int32_t)mcpu.Registers[m->R.rt];
#endif

}
void mips_addu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ADDU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->R.rd] = (uint32_t)mcpu.Registers[m->R.rs] + (uint32_t)mcpu.Registers[m->R.rt];
#endif
}

void mips_sub(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SUB %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->R.rd] = (int32_t)mcpu.Registers[m->R.rs] - (int32_t)mcpu.Registers[m->R.rt];
#endif	
}

void mips_subu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SUBU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif	
#ifdef EMULATOR
	mcpu.Registers[m->R.rd] = (uint32_t)mcpu.Registers[m->R.rs] - (uint32_t)mcpu.Registers[m->R.rt];
#endif	
}

void mips_and(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"AND %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->R.rd] = (int32_t)mcpu.Registers[m->R.rs] & (int32_t)mcpu.Registers[m->R.rt];
#endif
}

void mips_div(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"DIV %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.lo = (int32_t)mcpu.Registers[m->R.rs] / (int32_t)mcpu.Registers[m->R.rt];
	mcpu.hi = (int32_t)mcpu.Registers[m->R.rs] % (int32_t)mcpu.Registers[m->R.rt];
#endif

}
void mips_divu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"DIVU %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	mcpu.lo = (uint32_t)mcpu.Registers[m->R.rs] / (uint32_t)mcpu.Registers[m->R.rt];
	mcpu.hi = (uint32_t)mcpu.Registers[m->R.rs] % (uint32_t)mcpu.Registers[m->R.rt];
#endif
}

void mips_jr(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"JR %s\n",regnames[m->R.rs]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_mfhi(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MFHI %s\n",regnames[m->R.rd]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_mthi(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MTHI %s\n",regnames[m->R.rs]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_mflo(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MFLO %s\n",regnames[m->R.rd]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_mtlo(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MTLO %s\n",regnames[m->R.rd]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_mult(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MULT %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	uint64_t result = (int32_t)mcpu.Registers[m->R.rs] * (int32_t)mcpu.Registers[m->R.rt];
	mcpu.lo = (int32_t)result;
	mcpu.hi = (int32_t)(result>>32);
#endif
}

void mips_multu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"MULTU %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	uint64_t result = (uint32_t)mcpu.Registers[m->R.rs] * (uint32_t)mcpu.Registers[m->R.rt];
	mcpu.lo = (uint32_t)result;
	mcpu.hi = (uint32_t)(result>>32);
#endif
}

void mips_nor(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"NOR %s,%s,%s\n",regnames[m->R.rt],regnames[m->R.rs],regnames[m->R.rd]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_xor(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"XOR %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_or(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"OR %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_slt(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SLT %s,%s,%x\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_sltu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SLTU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_sll(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SLL %s,%s,0x%04X\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_srl(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SRL %s,%s,0x%04X\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_sra(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SRA %s,%s,%d\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}


/*
Service	System Call Code	Arguments	Result
print integer	1	$a0 = value	(none)
print float	2	$f12 = float value	(none)
print double	3	$f12 = double value	(none)
print string	4	$a0 = address of string	(none)
read integer	5	(none)	$v0 = value read
read float	6	(none)	$f0 = value read
read double	7	(none)	$f0 = value read
read string	8	$a0 = address where string to be stored
$a1 = number of characters to read + 1	(none)
memory allocation	9	$a0 = number of bytes of storage desired	$v0 = address of block
exit (end of program)	10	(none)	(none)
print character	11	$a0 = integer	(none)
read character	12	(none)	char in $v0

*/
void mips_syscall(MIPSI *m)
{

//addi $a0, $t0, 0
//addi $v0, $zero, 51
//syscall
#ifdef DISASM
	fprintf(outfile,"SYSCALL\n");
#endif	
#ifdef EMULATOR
	switch(mcpu.Registers[MIPS32_V0])
	{
		case 1:
		{
			fprintf(outfile,"%d",mcpu.Registers[MIPS32_A0]);
			break;
		}
		case 4:
		{
			//	syscall print string
			uint32_t stack_address = mcpu.Registers[MIPS32_A0]&0xffff;	
			fprintf(outfile,"%s",&mcpu_ram[stack_address]);
			break;
		}

		case 50:
		{
			int ret = 1;
			//	syscall confirm dialog 
			//	print message 
			
			uint32_t stack_address = mcpu.Registers[MIPS32_A0]&0xffff;	
			fprintf(outfile,"CONFIRM:%s (y/n)",&mcpu_ram[stack_address]);
			uint8_t a = getch();
			if (a=='y') ret = 0;
			mcpu.Registers[MIPS32_A0]=ret;
			break;
		}

		case 51:
		{
			//	syscall print string
			//	print message 
			uint32_t stack_address = mcpu.Registers[MIPS32_A0]&0xffff;	
			fprintf(outfile,"INPUT:%s",&mcpu_ram[stack_address]);
			char z[100];			
			gets(z);
			mcpu.Registers[MIPS32_A0]=atol(z);
			mcpu.Registers[MIPS32_A1]=0;
			break;
		}

		case 56:
		{
			//	syscall print string
			//	print message 
			uint32_t stack_address = mcpu.Registers[MIPS32_A0]&0xffff;	
			fprintf(outfile,"ALERT:%s %d",&mcpu_ram[stack_address],mcpu.Registers[MIPS32_A1]);
			break;
		}
		default:
		{
			printf("Unsupported %d",mcpu.Registers[MIPS32_V0]);
			break;
		}
	}
	fprintf(outfile,"\n");
#endif	
}

void mips_break(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"BREAK 0x%x\n",m->I.rt<<10);
#endif
#ifdef EMULATOR
	exit(0);
#endif
}

//	jump table for R type instructions

void (*mips_cpu_R_instructions[])() = 
{
// 0				1					2					3						4					5					6					7					8						9						A					B						C							D						E			F
	mips_sll,	NULL,			mips_srl,	mips_sra,		NULL,			NULL,			NULL,			NULL,			mips_jr,		NULL,				NULL,			NULL,				mips_syscall,	mips_break,	NULL,	NULL,	//	00
	mips_mfhi,mips_mthi,mips_mflo,mips_mfhi,	NULL,			NULL,			NULL,			NULL,			mips_mult,	mips_multu,	mips_div,	mips_divu,	NULL,					NULL,				NULL,	NULL,	//	10
	mips_add,	mips_addu,mips_sub,	mips_subu,	mips_and,	mips_or,	mips_xor,	mips_nor,	NULL,				NULL,				mips_slt,	mips_sltu,	NULL,					NULL,				NULL,	NULL,	//	20
	NULL,			NULL,			NULL,			NULL,				NULL,			NULL,			NULL,			NULL,			NULL,				NULL,				NULL,			NULL,				NULL,					NULL,				NULL,	NULL,	//	30
};

//	function for R types 
void cpu_R_instructions(MIPSI *mi)
{
	if (mips_cpu_R_instructions[mi->R.funct]!=NULL)
	{
		(*mips_cpu_R_instructions[mi->R.funct])(mi);
	}
	else 
	{
		uint32_t *v = (uint32_t*)mi;
#ifdef DISASM	
		fprintf(outfile,"dw 0x%08x\n",*v);
#endif
	}
}

//	J types

void mips_ja(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"J 0x%X\n",mcpu.base_address|(m->J.addr<<2));
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

void mips_jal(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"JAL 0x%X\n",mcpu.base_address|(m->J.addr<<2));
#endif
#ifdef EMULATOR
	fprintf(outfile,"TODO %d\n",__LINE__);
#endif
}

//	I types

void mips_beq(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BEQ\t");
	fprintf(outfile,"%s=%d,%s=%d,%s\n",regnames[m->I.rs],mcpu.Registers[m->I.rs],regnames[m->I.rt],mcpu.Registers[m->I.rt],shex(v));
#endif	
#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]==mcpu.Registers[m->I.rt])
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif	
}

void mips_bne(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BNE\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],mcpu.PC+4+v);
#endif	

#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]!=mcpu.Registers[m->I.rt])
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif	

}

void mips_blez(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BLEZ\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],mcpu.PC+4+v);
#endif
#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]<=0)
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif
}

void mips_bgez(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BGTZ\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],mcpu.PC+4+v);
#endif
#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]>=0)
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif
}

void mips_bgtz(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BGTZ\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],mcpu.PC+4+v);
#endif
#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]>0)
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif
}

void mips_bltz(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
#ifdef DISASM	
	fprintf(outfile,"BLTZ\t");
	fprintf(outfile,"%s,0x%x\n",regnames[m->I.rs],mcpu.PC+4+v);
#endif
#ifdef EMULATOR
	if (mcpu.Registers[m->I.rs]<0)
	{
		mcpu.PC = mcpu.PC+v;
	}
#endif

}

//	add immediate
void mips_addi(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ADDI\t");
	fprintf(outfile,"%s,%s,%s\n",regnames[m->I.rt],regnames[m->I.rs],shex(m->I.imm));
#endif	
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = mcpu.Registers[m->I.rs] + (int16_t)m->I.imm;
#endif	
}

//	add unsigned immediate
void mips_addiu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ADDIU\t");
	fprintf(outfile,"%s,%s,0x%04X\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif	
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = (uint32_t)mcpu.Registers[m->I.rs] + (uint16_t)m->I.imm;
#endif
}

void mips_slti(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SLTI %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = (int32_t)mcpu.Registers[m->I.rs] < (int16_t)m->I.imm;
#endif
}

void mips_sltiu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"SLTIU %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = (uint32_t)mcpu.Registers[m->I.rs] < (uint16_t)m->I.imm;
#endif
}

void mips_andi(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ANDI %s,%s,0x%04X\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = mcpu.Registers[m->I.rs] & (int16_t)m->I.imm;
#endif
}

void mips_ori(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"ORI %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif
#ifdef EMULATOR
	mcpu.Registers[m->I.rt] = mcpu.Registers[m->I.rs] | m->I.imm;
#endif
}


void mips_lb(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LB %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read8(ra);
#endif
}

void mips_lh(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LH %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read16(ra);
#endif
}

void mips_lw(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LW %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read32(ra);
#endif
}

void mips_lbu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LBU %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read8(ra);
#endif
}

void mips_lhu(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LHU %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read16(ra);
#endif
}

void mips_lui(MIPSI *m)
{
#ifdef DISASM	
	fprintf(outfile,"LUI %s,0x%x\n",regnames[m->I.rt],m->I.imm);
#endif
#ifdef EMULATOR
	uint32_t ra = mcpu.Registers[m->I.rs] + mcpu.Registers[m->I.imm];
	mcpu.Registers[m->I.rt] = read16(ra)<<16;
#endif
}

//	store byte
void mips_sb(MIPSI *m)
{
uint32_t address;	
uint8_t value;
#ifdef DISASM	
	fprintf(outfile,"SB %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	value = mcpu.Registers[m->I.rt];
	address = mcpu.Registers[m->I.rs] + (int16_t)m->I.imm;
	write8(address,value);
#endif
}

//	store half word ( 16 bits )
void mips_sh(MIPSI *m)
{
uint32_t address;	
uint16_t value;
#ifdef DISASM	
	fprintf(outfile,"SH %s,%s,0x%04x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
#endif
#ifdef EMULATOR
	value = mcpu.Registers[m->I.rt];
	address = mcpu.Registers[m->I.rs] + (int16_t)m->I.imm;
	write16(address,value);
#endif
}

//	store word ( 32 bits )
void mips_sw(MIPSI *m)
{
uint32_t address;	
uint32_t value;
#ifdef DISASM	
	fprintf(outfile,"SW %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
#endif
#ifdef EMULATOR
	value = mcpu.Registers[m->I.rt];
	address = mcpu.Registers[m->I.rs] + (int16_t)m->I.imm;
	write32(address,value);
#endif		
}

//	jumptable for base type instructions
//	op zero calls R type function jumptable

void (*mips_cpu_base_instructions[])() = 
{
// 0									1					2				3					4					5					6					7					8						9						A					B						C						D					E			F
	cpu_R_instructions,	mips_bltz,mips_ja,mips_jal,	mips_beq,	mips_bne,	mips_blez,mips_bgtz,mips_addi,	mips_addiu,	mips_slti,mips_sltiu,	mips_andi,	mips_ori,	NULL,	mips_lui,	//	00
	NULL,								NULL,			NULL,		NULL,			NULL,			NULL,			NULL,			NULL,			NULL,				NULL,				NULL,			NULL,				NULL,				NULL,			NULL,	NULL,	//	10
	mips_lb,						mips_lh,	NULL,		mips_lw,	mips_lbu,	mips_lhu,	NULL,			NULL,			mips_sb,		mips_sh,		NULL,			mips_sw,		NULL,				NULL,			NULL,	NULL,	//	20
	NULL,								NULL,			NULL,		NULL,			NULL,			NULL,			NULL,			NULL,			NULL,				NULL,				NULL,			NULL,				NULL,				NULL,			NULL,	NULL,	//	30
};

//	dumn commandline stuff
void main(int argc,char *argv[])
{
	int len = 0;
	int file_len = 0;
	bool rawbin = false;
	char *filenames[2];

	for (int q=0;q<2;q++)
		filenames[q]=NULL;
	if (argc==1)
	{
		printf("Usage: %s\topt infile [outfile]\n",argv[0]);
		printf("\t\t\t-b use raw binary ( default PSX.EXE ) \n");
		printf("\t\t\t[outfile] optional text file to output\n");
		exit(0);
	}
	//	default out is stdout
	outfile = stdout;
	buffer=NULL;
	//	handle args
	int filename=0;
	for (int q=1;q<argc;q++)
	{
		if (argv[q][0]!='-')
		{
			if (filename<2)
			{
				filenames[filename++]=argv[q];
			}
			else 
			{
				fprintf(outfile,"too many names\n");
				exit(0);
			}				
		}
		if (stricmp(argv[q],"-b")==0) rawbin=true;
	}

	// some checking

	if (filenames[0]==NULL)
	{
		fprintf(outfile,"no file to load\n");
		exit(0);
	}

	if (filenames[1]!=NULL)
	{
		outfile = fopen(filenames[1],"wt");
	}


	
	// read the file
	FILE *fp=fopen(filenames[0],"rb");

	if (fp==NULL)
	{
		printf("failed to open %s\n",filenames[0]);
		cleanup();
		exit(0);
	}

	fseek(fp,0,SEEK_END);
	file_len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	buffer = (uint8_t*)malloc(file_len);
	if (buffer==NULL)
	{
		printf("failed to malloc %x bytes\n",file_len);
		cleanup();
		exit(0);
	}

	//	if it's an EXE load the header and get start address
	if (rawbin==false)
	{
		fseek(fp,0x18,SEEK_SET);
		fread(&mcpu.base_address,1,4,fp);
		fseek(fp,0x800,SEEK_SET);
		printf("skipped exe header %x\n",mcpu.base_address);
	}		
	else 
	{
	//	raw is started at 0 
		mcpu.base_address = 0;
	}
	mcpu.length = fread(buffer,1,file_len,fp);
	fclose(fp);


	mips_reset();
	memcpy(mcpu_ram,buffer,mcpu.length);

	//	start the decode
	mcpu.PC = mcpu.base_address;
#ifdef EMULATOR	
	while(mcpu.PC<(mcpu.base_address+mcpu.length))
#else 
	for (int q=0;q<(mcpu.length>>2);q++)
#endif
	{
		uint32_t *ptr = (uint32_t*)&buffer[mcpu.PC-mcpu.base_address];
		MIPSI *mi = (MIPSI*)ptr;
		if (*ptr==0)
			fprintf(outfile,"NOP\n");
		else			
		{
			if (mips_cpu_base_instructions[mi->I.op]!=NULL)
			{
				(*mips_cpu_base_instructions[mi->I.op])(mi);
				mips_display(&mcpu);
			}
			else 
			{
				//	this handles unsupported cases by spitting out a dword 
				fprintf(outfile,"DW 0x%08x\n",*ptr);
			}
		}			
		mcpu.PC+=4;
	}
}

static void cleanup()
{

	//	close up shop
	if (outfile!=stdout)
		fclose(outfile);
	if (buffer!=NULL)
		free(buffer);

	exit(0);
}

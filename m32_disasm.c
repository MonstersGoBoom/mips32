
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>
#include <inttypes.h>

FILE	*outfile;
uint8_t buffer[65536*4];
uint32_t PC = 0;
uint32_t start_address = 0;

//	return correct signed hex string
//	for example 0xa00 or -0xa00 this is how assembers accept the values 

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
	//opcode	rs			rt			rd			shift (shamt)	funct
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


//	basically mips_instructionmame
//	R type opcodes

void mips_nop(MIPSI *m) { 	fprintf(outfile,"NOP\n"); }
void mips_add(MIPSI *m)
{
	fprintf(outfile,"ADD %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_addu(MIPSI *m)
{
	fprintf(outfile,"ADDU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_sub(MIPSI *m)
{
	fprintf(outfile,"SUB %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_subu(MIPSI *m)
{
	fprintf(outfile,"SUBU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_and(MIPSI *m)
{
	fprintf(outfile,"AND %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_div(MIPSI *m)
{
	fprintf(outfile,"DIV %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);

}
void mips_divu(MIPSI *m)
{
	fprintf(outfile,"DIVU %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_jr(MIPSI *m)
{
	fprintf(outfile,"JR %s\n",regnames[m->R.rs]);
}

void mips_mfhi(MIPSI *m)
{
	fprintf(outfile,"MFHI %s\n",regnames[m->R.rd]);
}

void mips_mthi(MIPSI *m)
{
	fprintf(outfile,"MTHI %s\n",regnames[m->R.rs]);
}

void mips_mflo(MIPSI *m)
{
	fprintf(outfile,"MFLO %s\n",regnames[m->R.rd]);
}

void mips_mtlo(MIPSI *m)
{
	fprintf(outfile,"MTLO %s\n",regnames[m->R.rd]);
}

void mips_mult(MIPSI *m)
{
	fprintf(outfile,"MULT %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_multu(MIPSI *m)
{
	fprintf(outfile,"MULTU %s,%s\n",regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_nor(MIPSI *m)
{
	fprintf(outfile,"NOR %s,%s,%s\n",regnames[m->R.rt],regnames[m->R.rs],regnames[m->R.rd]);
}

void mips_xor(MIPSI *m)
{
	fprintf(outfile,"XOR %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_or(MIPSI *m)
{
	fprintf(outfile,"OR %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_slt(MIPSI *m)
{
	fprintf(outfile,"SLT %s,%s,%x\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
}

void mips_sltu(MIPSI *m)
{
	fprintf(outfile,"SLTU %s,%s,%s\n",regnames[m->R.rd],regnames[m->R.rs],regnames[m->R.rt]);
}

void mips_sll(MIPSI *m)
{
	fprintf(outfile,"SLL %s,%s,0x%04X\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
}

void mips_srl(MIPSI *m)
{
	fprintf(outfile,"SRL %s,%s,0x%04X\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
}

void mips_sra(MIPSI *m)
{
	fprintf(outfile,"SRA %s,%s,%d\n",regnames[m->R.rd],regnames[m->R.rt],m->R.shift);
}

void mips_syscall(MIPSI *m)
{
	fprintf(outfile,"SYSCALL\n");
}

void mips_break(MIPSI *m)
{
	fprintf(outfile,"BREAK 0x%x\n",m->I.rt<<10);
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
		fprintf(outfile,"dw 0x%08x\n",*v);
	}
}

//	J types

void mips_ja(MIPSI *m)
{
	fprintf(outfile,"J 0x%X\n",start_address|(m->J.addr<<2));
}

void mips_jal(MIPSI *m)
{
	fprintf(outfile,"JAL 0x%X\n",start_address|(m->J.addr<<2));
}

//	I types

void mips_beq(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
	fprintf(outfile,"BEQ\t");
	fprintf(outfile,"%s,%s,0x%X\n",regnames[m->I.rs],regnames[m->I.rt],PC+4+v);
}

void mips_bne(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
	fprintf(outfile,"BNE\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],PC+4+v);
}

void mips_blez(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
	fprintf(outfile,"BLEZ\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],PC+4+v);
}

void mips_bgtz(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
	fprintf(outfile,"BGTZ\t");
	fprintf(outfile,"%s,%s,0x%x\n",regnames[m->I.rs],regnames[m->I.rt],PC+4+v);
}

void mips_bltz(MIPSI *m)
{
int16_t v=(m->I.imm)*4;	
	fprintf(outfile,"BLTZ\t");
	fprintf(outfile,"%s,0x%x\n",regnames[m->I.rs],PC+4+v);
}

void mips_addi(MIPSI *m)
{
	fprintf(outfile,"ADDI\t");
	fprintf(outfile,"%s,%s,%s\n",regnames[m->I.rt],regnames[m->I.rs],shex(m->I.imm));
}

void mips_addiu(MIPSI *m)
{
	fprintf(outfile,"ADDIU\t");
	fprintf(outfile,"%s,%s,0x%04X\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_slti(MIPSI *m)
{
	fprintf(outfile,"SLTI %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_sltiu(MIPSI *m)
{
	fprintf(outfile,"SLTIU %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_andi(MIPSI *m)
{
	fprintf(outfile,"ANDI %s,%s,0x%04X\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_ori(MIPSI *m)
{
	fprintf(outfile,"ORI %s,%s,0x%x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_mfc0(MIPSI *m)
{
	fprintf(outfile,"MFCO %s,%s\n",regnames[m->R.rt],regnames[m->R.rd]);
}

void mips_lb(MIPSI *m)
{
	fprintf(outfile,"LB %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
}

void mips_lw(MIPSI *m)
{
	fprintf(outfile,"LW %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
}

void mips_lh(MIPSI *m)
{
	fprintf(outfile,"LH %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
}

void mips_lbu(MIPSI *m)
{
	fprintf(outfile,"LBU %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
}

void mips_lhu(MIPSI *m)
{
	fprintf(outfile,"LHU %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
}

void mips_lui(MIPSI *m)
{
	fprintf(outfile,"LUI %s,0x%x\n",regnames[m->I.rt],m->I.imm);
}

void mips_sb(MIPSI *m)
{
	fprintf(outfile,"SB %s,0x%04x(%s)\n",regnames[m->I.rt],m->I.imm,regnames[m->I.rs]);
}

void mips_sh(MIPSI *m)
{
	fprintf(outfile,"SH %s,%s,0x%04x\n",regnames[m->I.rt],regnames[m->I.rs],m->I.imm);
}

void mips_sw(MIPSI *m)
{
	fprintf(outfile,"SW %s,%s(%s)\n",regnames[m->I.rt],shex(m->I.imm),regnames[m->I.rs]);
}

//	jumptable for base type instructions
//	op zero calls R type function jumptable

void (*mips_cpu_base_instructions[])() = 
{
// 0									1					2				3					4					5					6					7					8						9						A					B						C						D					E			F
	cpu_R_instructions,	mips_bltz,mips_ja,mips_jal,	mips_beq,	mips_bne,	mips_blez,mips_bgtz,mips_addi,	mips_addiu,	mips_slti,mips_sltiu,	mips_andi,	mips_ori,	NULL,	mips_lui,	//	00
	mips_mfc0,					NULL,			NULL,		NULL,			NULL,			NULL,			NULL,			NULL,			NULL,				NULL,				NULL,			NULL,				NULL,				NULL,			NULL,	NULL,	//	10
	mips_lb,						mips_lh,	NULL,		mips_lw,	mips_lbu,	mips_lhu,	NULL,			NULL,			mips_sb,		mips_sh,		NULL,			mips_sw,		NULL,				NULL,			NULL,	NULL,	//	20
	NULL,								NULL,			NULL,		NULL,			NULL,			NULL,			NULL,			NULL,			NULL,				NULL,				NULL,			NULL,				NULL,				NULL,			NULL,	NULL,	//	30
};

//	dumn commandline stuff
void main(int argc,char *argv[])
{
	int len = 0;
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

	//	if it's an EXE load the header and get start address
	if (rawbin==false)
	{
		fseek(fp,0x18,SEEK_SET);
		fread(&start_address,1,4,fp);
		fseek(fp,0x800,SEEK_SET);
	}		
	else 
	{
	//	raw is started at 0 
		start_address = 0;
	}
	len = fread(buffer,1,65536*4,fp);
	fclose(fp);

	//	start the decode
	uint32_t *ptr = (uint32_t*)&buffer[0];
	for (int q=0;q<(len>>2);q++)
	{
		MIPSI *mi = (MIPSI*)ptr;
		PC = start_address+((uint32_t)q*4);
		if (*ptr==0)
			fprintf(outfile,"NOP\n");
		else			
		{
			if (mips_cpu_base_instructions[mi->I.op]!=NULL)
			{
				(*mips_cpu_base_instructions[mi->I.op])(mi);
			}
			else 
			{
				//	this handles unsupported cases by spitting out a dword 
				fprintf(outfile,"dw 0x%08x\n",*ptr);
			}
		}			
		ptr++;		
	}

	//	close up shop
	if (outfile!=stdout)
		fclose(outfile);
}


typedef struct 
{
	uint32_t PC;
	uint32_t SP;
	uint32_t base_address;
	uint32_t length;
	uint32_t hi,lo;
	uint32_t Registers[32];
	unsigned char (*read8) (uint32_t address);
	void (*write8) (uint32_t address, unsigned char data);
} MIPS32;




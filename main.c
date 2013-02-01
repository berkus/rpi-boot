#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "atag.h"
#include "fb.h"
#include "console.h"

#define UNUSED(x) (void)(x)

void atag_cb(struct atag *tag)
{
	switch(tag->hdr.tag)
	{
		case ATAG_CORE:
			puts("ATAG_CORE");
			if(tag->hdr.size == 5)
			{
				puts("flags");
				puthex(tag->u.core.flags);
				puts("");

				puts("pagesize");
				puthex(tag->u.core.pagesize);
				puts("");

				puts("rootdev");
				puthex(tag->u.core.rootdev);
				puts("");
			}
			break;

		case ATAG_MEM:
			puts("ATAG_MEM");
			
			puts("start");
			puthex(tag->u.mem.start);
			puts("");
			
			puts("size");
			puthex(tag->u.mem.size);
			puts("");
			
			break;

		case ATAG_NONE:
			break;

		default:
			puts("Unknown ATAG");
			puthex(tag->hdr.tag);
			break;
	};

	puts("");
}

void console_test();
int sd_card_init();
int sd_read_block(uint32_t block_no, uint8_t *buf);

extern int (*stdout_putc)(int);
extern int (*stderr_putc)(int);
extern int (*stream_putc)(int, FILE*);
extern int console_putc(int);
extern int def_stream_putc(int, FILE*);

void kernel_main(uint32_t boot_dev, uint32_t arm_m_type, uint32_t atags)
{
	UNUSED(atags);

	// First use the serial console
	stdout_putc = uart_putc;
	stderr_putc = uart_putc;
	stream_putc = def_stream_putc;	

	puts("Hello World!");
	puthex(0xdeadbeef);
	puts("");

	puts("Boot device:");
	puthex(boot_dev);
	puts("");

	puts("Machine type:");
	puthex(arm_m_type);
	puts("");

	puts("ATAGS:");
	puthex(atags);
	puts("");

	puts("");

	// Dump ATAGS
	parse_atags(atags, atag_cb);

	int result = fb_init();
	if(result == 0)
		puts("Successfully set up frame buffer");
	else
	{
		puts("Error setting up framebuffer:");
		puthex(result);
	}

	// Switch to the framebuffer for output
	stdout_putc = console_putc;

	printf("Welcome to Rpi bootloader\n");
	printf("ARM system type is %x\n", arm_m_type);

	if(sd_card_init() == 0)
	{
		uint8_t *buf = (uint8_t *)malloc(512);
		sd_read_block(0, buf);

		printf("Block 0:\n");
		int j = 0;
		for(int i = 0; i < 512; i += 4)
		{
			printf("%08x ", *(uint32_t *)&buf[i]);
			j++;
			if(j == 8)
			{
				j = 0;
				printf("\n");
			}
		}
	}
}

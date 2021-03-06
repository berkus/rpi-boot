/* Copyright (C) 2013 by John Cronin <jncronin@tysos.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include "block.h"

int block_read(struct block_device *dev, uint8_t *buf, size_t buf_size, uint32_t starting_block)
{
	// Read the required number of blocks to satisfy the request
	int buf_offset = 0;
	uint32_t block_offset = 0;

	do
	{
		size_t to_read = buf_size;
		if(to_read > dev->block_size)
			to_read = dev->block_size;

#ifdef BLOCK_DEBUG
		printf("block_read: reading %i bytes from block %i on %s\n", to_read,
				starting_block + block_offset, dev->device_name);
#endif

		int ret = dev->read(dev, &buf[buf_offset], to_read, starting_block + block_offset);
		if(ret < 0)
			return ret;

		buf_offset += (int)to_read;
		block_offset++;

		if(buf_size < dev->block_size)
			buf_size = 0;
		else
			buf_size -= dev->block_size;
	} while(buf_size > 0);

	return buf_offset;
}


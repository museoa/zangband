/* File: compress.c */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

/*
 * Purpose: Compression and decompression of block streams.
 * 
 * This is used to decrease the size of savefiles, and to
 * decrease the bandwidth requirements of the multiplayer
 * version when it is finally made.
 * 
 * Original code by Steven Fuerst
 */

#include "angband.h"

/* 4k for each block_type */
#define BLOCK_DATA_SIZE ((int)(4096 - sizeof(u16b) - sizeof(void *)))

#define HI_BIT_16		0x8000
#define NEXT_BIT_16		0x4000
#define ALL_BITS		0xFFFF

/*
 * Maximum that a probability counter can obtain.
 */
#define PROB_TABLE_MAX	0x3FFF
/*
 * Increment to add per symbol
 * The larger this is, the faster we adapt to changes, but the
 * less accurate symbol frequencies become.
 */
#define PROB_CHANCE_INC	0x0020

typedef struct block_type block_type;

struct block_type
{
	/* Pointer to next block in list */
	block_type *b_next;
	
	/* Size of used region in this block */
	u16b size;
	
	/* Data in this block */
	byte block_data[BLOCK_DATA_SIZE];
};


typedef struct block_handle block_handle;

struct block_handle
{
	/* Pointer to a block */
	block_type *b_ptr;
	
	/* Pointer to the first block */
	block_type *bf_ptr;
	
	/* Position of read/write head */
	u32b counter;
};

static block_type *free_list = NULL;
static block_type *used_list = NULL;

static block_type* new_block(void)
{
	block_type *temp_ptr;
	
	/* Is the free list empty? */
	if (!free_list)
	{
		/* Ok - make a new block */
		MAKE(temp_ptr, block_type);
	
		/* Return a pointer to it */
		return (temp_ptr);
	}
	
	/* Pick the first thing on the free list */
	temp_ptr = free_list;
	
	/* Update the free list */
	free_list = free_list->b_next;
	
	/* Terminate the block */
	temp_ptr->b_next = NULL;

	/* Return a pointer to the block */
	return (temp_ptr);
}

static block_type *del_block(block_type *b_ptr)
{
	/* Save what this block is attached to */
	block_type *temp_ptr;
	
	/* Paranoia */
	if (!b_ptr) return (NULL);
	
	/* Save the block after *b_ptr */
	temp_ptr = b_ptr->b_next;
	
	/* Add to front of free list */
	b_ptr->b_next = free_list;
	free_list = b_ptr;
	
	/* Return the block after *b_ptr */
	return (temp_ptr);
}

/* Read a byte from the stream of blocks */
static int read_block_byte(block_handle *h_ptr)
{
	/* End of block? */
	if (h_ptr->counter >= h_ptr->b_ptr->size)
	{
		/* Change block */
		h_ptr->b_ptr = h_ptr->b_ptr->b_next;
		
		/* Start of new block */
		h_ptr->counter = 0;

		/* End of the stream */
		if (!h_ptr->b_ptr) return (-1);
	}
	
	return (h_ptr->b_ptr->block_data[h_ptr->counter++]);
}

/* Read a byte from the stream of blocks - erasing as we go*/
static int rerase_block_byte(block_handle *h_ptr)
{
	/* End of the stream */
	if (!h_ptr->b_ptr) return (-1);
	
	/* End of block? */
	if (h_ptr->counter >= h_ptr->b_ptr->size)
	{
		/* Change block, deleting old one */
		h_ptr->b_ptr = del_block(h_ptr->b_ptr);
		
		/* Start of new block */
		h_ptr->counter = 0;

		/* End of the stream */
		if (!h_ptr->b_ptr) return (-1);
	}
	
	return (h_ptr->b_ptr->block_data[h_ptr->counter++]);
}

static void write_block_byte(block_handle *h_ptr, byte output)
{
	/* End of block? */
	if (h_ptr->counter >= BLOCK_DATA_SIZE)
	{
		if (!h_ptr->b_ptr->b_next)
		{
			/* Do we need to make a new block? */
			h_ptr->b_ptr->b_next = new_block();
		}
		
		/* Change block */
		h_ptr->b_ptr = h_ptr->b_ptr->b_next;
		
		/* Start of new block */
		h_ptr->counter = 0;
	}
	
	h_ptr->b_ptr->block_data[h_ptr->counter++] = output;
	
	h_ptr->b_ptr->size = h_ptr->counter;
}

/* Current byte of information to read / write */
static byte current_byte;

/* Which bit are we up to */
static byte current_bit;

static void write_block_bit(block_handle *h_ptr, u16b output)
{
	/* Write the bit */
	current_byte *= 2;
	current_byte += output ? 1 : 0;
	
	/* Move to next bit */
	current_bit++;
	
	if (current_bit == 8)
	{
		/* We've done a whole byte */
		write_block_byte(h_ptr, current_byte);
		
		/* Reset to start of new byte */
		current_byte = 0;
		current_bit = 0;
	}
}

static byte rerase_block_bit(block_handle *h_ptr)
{
	int input_byte;
	byte result_bit;

	if (current_bit == 0)
	{
		input_byte = rerase_block_byte(h_ptr);
		
		/* At end of stream? */
		if (input_byte == -1) input_byte = 0;
		
		/* Read a new byte */
		current_byte = (byte) input_byte;
		
		current_bit = 8;
	}
	
	/* Move to next bit */
	current_bit--;
	
	result_bit = current_byte & 0x80;
	current_byte *= 2;
	
	/* Return that bit */
	return (result_bit ? 1 : 0);
}

/* Initialise static variables used to have a bitwise stream */
static void init_block_bit(void)
{
	current_byte = 0;
	current_bit = 0;
}

/* Write out the pending bits, if any exist */
static void flush_bits(block_handle *h_ptr)
{
	/* Anything to output? */
	while (current_bit)
	{
		write_block_bit(h_ptr, TRUE);
	}
}


/*
 * Clean the list of blocks.
 *
 * Compact everything until the least amount of memory is used.
 */
static void clean_blocks(block_type *b_ptr)
{
	int i = 0, j = 0;
	block_type* next_ptr = b_ptr;
	
	/* While there are blocks to do */
	while (next_ptr)
	{
		/* Note inline versions of read and write block_byte for speed */
		
		
		/* End of write block? */
		if (i >= BLOCK_DATA_SIZE)
		{
			i = 0;
			
			b_ptr->size = BLOCK_DATA_SIZE;
			
			b_ptr = b_ptr->b_next;
			
			continue;
		}
		
		/* End of read block? */
		if (j >= next_ptr->size)
		{
			j = 0;
			next_ptr = next_ptr->b_next;

			continue;
		}
		
		/*
		 * Copy from read to write.
		 *
		 * Note that read head is always further along the stream
		 * than the write head.
		 */
		b_ptr->block_data[i++] = next_ptr->block_data[j++];
	}
	
	if (b_ptr)
	{
		/* Save the size */
		b_ptr->size = i;
	
		/* We have finished copying the stuff - now delete the extra blocks */
		
		/* Save the start of the blocks to delete */
		b_ptr = b_ptr->b_next;
	
		while (b_ptr)
		{
			/* Delete the block - and point to its child */
			b_ptr = del_block(b_ptr);
		}
	
		b_ptr->b_next = NULL;
	}
	
	/* Delete the extra blocks in the free list */
	while (free_list)
	{
		/* Save next block */
		next_ptr = free_list->b_next;
		
		/* Destroy the current block */
		KILL(free_list, block_type);
		
		/* Point to next block */
		free_list = next_ptr;
	}
}

/*
 * Delete all the blocks in a particular stream
 */
static void delete_handle(block_handle *h_ptr)
{
	block_type *b_ptr = h_ptr->bf_ptr;
	
	while (b_ptr)
	{
		/* Delete the block - and point to its child */
		b_ptr = del_block(b_ptr);
	}

	/* Reset counter */
	h_ptr->counter = 0;
	
	/* Reset the pointers */
	h_ptr->bf_ptr = NULL;
	h_ptr->b_ptr = NULL;
}


/*
 * RLE encode this list of blocks - inserting new blocks as needed
 */
static void rle_blocks_encode(block_handle *h1_ptr)
{
	block_handle handle, *h2_ptr = &handle;
	block_type *b_ptr;
	int count = 0;
	byte symbol;
	int new_symbol;

	/* Swap the block streams the two handles refer to */
	b_ptr = h1_ptr->bf_ptr;
	h1_ptr->bf_ptr = new_block();
	h2_ptr->bf_ptr = b_ptr;
	
	/* Move the read/write head to the start of the stream */
	h1_ptr->counter = 0;
	h2_ptr->counter = 0;
	h1_ptr->b_ptr = h1_ptr->bf_ptr;
	h2_ptr->b_ptr = b_ptr;
	
	/* Paranoia */
	if (!h2_ptr->b_ptr) return;
	
	/* Get first symbol */
	symbol = rerase_block_byte(h2_ptr);
	
	/* While we have blocks to do */
	while (TRUE)
	{
		/* Output the old symbol */
		write_block_byte(h1_ptr, symbol);
		
		/* Get a new symbol */
		new_symbol = rerase_block_byte(h2_ptr);
		
		/* At the end? */
		if (new_symbol == -1) break;
		
		/* No match - keep looping */
		if (new_symbol != symbol)
		{
			symbol = (byte) new_symbol;
			continue;
		}
		
		/*
		 * There is a match - write out the symbol again.
		 * A duplicated symbol marks a run.
		 */
		write_block_byte(h1_ptr, symbol);

		/* Reset counter */
		count = 0;
	
		while (count < 255)
		{
			/* Get a new symbol */
			new_symbol = rerase_block_byte(h2_ptr);
			
			if (new_symbol != symbol) break;
		
			count++;
		}
		
		/* Output the count */
		write_block_byte(h1_ptr, count);
		
		/* At the end? */
		if (new_symbol == -1) break;
		
		/* Go back to looping */
		symbol = (byte) new_symbol;
	}
}


/*
 * The inverse of the above function.
 *
 * Scan for duplicated symbols, and then fill in the count
 * with copies of them.
 */
static void rle_blocks_decode(block_handle *h1_ptr)
{
	block_handle handle, *h2_ptr = &handle;
	block_type *b_ptr;
	
	int count = 0;
	int symbol, new_symbol;

	/* Swap the block streams the two handles refer to */
	b_ptr = h1_ptr->bf_ptr;
	h1_ptr->bf_ptr = new_block();
	h2_ptr->bf_ptr = b_ptr;
	
	/* Move the read/write head to the start of the stream */
	h1_ptr->counter = 0;
	h2_ptr->counter = 0;
	h1_ptr->b_ptr = h1_ptr->bf_ptr;
	h2_ptr->b_ptr = b_ptr;
		
	/* Paranoia */
	if (!h2_ptr->b_ptr) return;
	
	/* Get first symbol */
	symbol = rerase_block_byte(h2_ptr);
	
	/* While we have blocks to do */
	while (TRUE)
	{
		/* Paranoia - stop at end of file */
		if (symbol == -1) break;
		
		/* Output the old symbol */
		write_block_byte(h1_ptr, symbol);
		
		/* Get a new symbol */
		new_symbol = rerase_block_byte(h2_ptr);
		
		/* No match - keep looping */
		if (new_symbol != symbol)
		{
			symbol = new_symbol;
			continue;
		}
		
		/*
		 * There is a match - write out the symbol again.
		 * A duplicated symbol marks a run.
		 */
		write_block_byte(h1_ptr, symbol);

		/* Get the count */
		count = rerase_block_byte(h2_ptr);
		
		/* Paranoia */
		if (count == -1) break;
		
		/* Write out the run */
		for (;count > 0; count--)
		{
			write_block_byte(h1_ptr, symbol);
		}
	
		/* Get new start symbol */
		symbol = rerase_block_byte(h2_ptr);
	
		/* Paranoia */
		if (symbol == -1) break;
	}
}


/*
 * Function pointers used to
 * change the compression model.
 */
static void (*init_compress_hook)(u16b *prob_table);
static void (*calc_prob_hook)(byte symbol, u16b *prob_table);


/* Bounds of arithmetic code value */
static u16b bound1;
static u16b bound2;

/* 
 * Bits in an all-one or all-zero 'run' next to high-bit.
 * (This gives us extra precision and avoids overflows.)
 */
static u32b run_bits;


/* Encode a symbol into the block stream */
static void arth_symbol_encode(block_handle *h_ptr, u16b *prob_table, byte symbol)
{
	/*
	 * How large is the current range of possibilities?
	 */
	u32b range = (u32b)(bound2 - bound1) + 1;
		
	/*
	 * Rescale the bounds
	 */
	bound2 = (u16b)((range * prob_table[symbol + 1]) / prob_table[256]
		 - 1 + bound1);
	bound1 += (u16b)((range * prob_table[symbol]) / prob_table[256]);
	
	/* Prune as many bits as possible */
	while (TRUE)
	{
		/* Do the high bits match? */
		if ((bound1 & HI_BIT_16) == (bound2 & HI_BIT_16))
		{
			/* Output the bit */
			write_block_bit(h_ptr, bound1 & HI_BIT_16);
			
			/* Output the overflow bits in a run */
			for (;run_bits > 0; run_bits--)
			{
				write_block_bit(h_ptr, !(bound1 & HI_BIT_16));
			}
		}	
	
		/*
		 * Check the overflow bits - and store runs
		 * (Overflow if bit in bound1 is set, and unset in bound2)
		 */
		else if (bound1 & (~bound2) & NEXT_BIT_16)
		{
			/* Count the run bits */
			run_bits++;
			
			/*
			 * Make sure the bits differ so we
			 * shift them into the correct location.
			 *
			 * Note: we know that bound2 has the bit set,
			 * and bound1 has it cleared, because
			 * bound2 > bound1.
			 *
			 * (Basically we are just deleting the bits -
			 * which means setting them to the right thing,
			 * and shifting them into the high bit.)
			 */
			bound1 &= ~(NEXT_BIT_16);
			bound2 |= NEXT_BIT_16;
		}
		else
		{
			/* We have removed as many bits as possible */
			break;
		}
	
		/* Shift out the high bits */
		
		/* Shift in a zero */
		bound1 = bound1 << 1;
		
		/* Shift in a one */
		bound2 = (bound2 << 1) + 1;
	}
}


/*
 * Remove the current symbol from the code, and insert more
 * bits as needed.
 */
static byte remove_symbol(u16b *code, u16b *prob_table, block_handle *h_ptr)
{
	/* How large is the current range of possibilities? */
	u32b range = (u32b)(bound2 - bound1) + 1;
	
	byte symbol = 0;
		
	u32b temp = (u32b)(*code - bound1) + 1;
	u16b count = (u16b)((temp * prob_table[256] - 1) / range);
	
	int i;

	/* Find the symbol to use */
	for (i = 255; i >= 0; i--)
	{
		if (prob_table[i] <= count)
		{
			symbol = i;
			break;
		}
	}
			
	/*
	 * Rescale the bounds
	 */
	bound2 = (u16b)((range * prob_table[symbol + 1]) / prob_table[256]
		 - 1 + bound1);
	bound1 += (u16b)((range * (prob_table[symbol])) / prob_table[256]);

	/* Try to remove as many bits as possible */
	while (TRUE)
	{
		/* If the high bits match - remove them */
		if ((bound1 & HI_BIT_16) == (bound2 & HI_BIT_16))
		{
			/* Shift out the bits below */
		}
		
		/*
		 * Are we near an underflow?
		 * Want the bit in bound1 to be set, and unset in bound2.
		 */
		else if (bound1 & (~bound2) & NEXT_BIT_16)
		{
			/* Delete these bits by copying in the state of the high bits */
			
			/* Make these bits match the high bit. */
			bound1 &= ~(NEXT_BIT_16);
			bound2 |= NEXT_BIT_16;
			
			/* Flip the second highest bit in the code */
			*code ^= NEXT_BIT_16;
		}
		else
		{
			/*
			 * We can't shift out anything
			 * so exit with the symbol we found earlier.
			 */
			return (symbol);
		}

		/* Swap out the high bits */
		bound1 = bound1 << 1;
		bound2 = (bound2 << 1) + 1;
		
		/* Add in a new bit from the stream */
		*code = (*code << 1) + rerase_block_bit(h_ptr);
	}
}

/* Flush the final bits when done */
static void flush_arith(block_handle *h_ptr)
{
	/* Output the second highest bit */
	write_block_bit(h_ptr, bound1 & NEXT_BIT_16);
	
	/*
	 * Increment the number of overflow bits,
	 * so we always output at least two.
	 *
	 * Then we output all the pending ones.
	 */
	for(run_bits++; run_bits > 0; run_bits--)
	{
		write_block_bit(h_ptr, !(bound1 & NEXT_BIT_16));
	}
	
	/* Write out the final byte, if required */
	flush_bits(h_ptr);
}

static void arth_blocks_encode(block_handle *h1_ptr)
{
	block_handle handle, *h2_ptr = &handle;
	block_type *b_ptr;

	int symbol;
	u32b size = 0;
	u16b prob_table[257];
	
	/* Swap the block streams the two handles refer to */
	b_ptr = h1_ptr->bf_ptr;
	h1_ptr->bf_ptr = new_block();
	h2_ptr->bf_ptr = b_ptr;
	
	/* Move the read/write head to the start of the stream */
	h1_ptr->counter = 0;
	h2_ptr->counter = 0;
	h1_ptr->b_ptr = h1_ptr->bf_ptr;
	h2_ptr->b_ptr = b_ptr;
		
	/* Get size of the encoded stream */
	while (b_ptr)
	{
		/* Add up the size */
		size += b_ptr->size;
		
		/* Point to the next block */
		b_ptr = b_ptr->b_next;
	}

	/* Write the size to the file */
	write_block_byte(h1_ptr, size & 0xFF);
	write_block_byte(h1_ptr, (size >> 8) & 0xFF);
	write_block_byte(h1_ptr, (size >> 16) & 0xFF);
	write_block_byte(h1_ptr, (size >> 24) & 0xFF);
	
	/*
	 * Init the encoder.
	 *
	 * The boundary is 0->1 in fixed point.
	 */
	bound1 = 0;
	bound2 = ALL_BITS;
	
	/* No overflow yet */
	run_bits = 0;
	
	/* Init the compression model */
	init_compress_hook(prob_table);
	
	/* Init the bitwise stream */
	init_block_bit();
	
	/* Compress the blocks */
	while (TRUE)
	{
		/* Get symbols */
		symbol = rerase_block_byte(h2_ptr);
		
		if (symbol == -1)
		{
			/* Done - flush the data */
			flush_arith(h1_ptr);
			
			break;
		}
	
		/* Encode it */
		arth_symbol_encode(h1_ptr, prob_table, symbol);
		
		/* Update probability table */
		calc_prob_hook(symbol, prob_table);
	}
}

static void arth_blocks_decode(block_handle *h1_ptr)
{
	block_handle handle, *h2_ptr = &handle;
	block_type *b_ptr;

	int i;
	u32b size;
	u16b code = 0, prob_table[257];

	byte symbol;
	
	/* Swap the block streams the two handles refer to */
	b_ptr = h1_ptr->bf_ptr;
	h1_ptr->bf_ptr = new_block();
	h2_ptr->bf_ptr = b_ptr;
	
	/* Move the read/write head to the start of the stream */
	h1_ptr->counter = 0;
	h2_ptr->counter = 0;
	h1_ptr->b_ptr = h1_ptr->bf_ptr;
	h2_ptr->b_ptr = b_ptr;
	
	/*
	 * Init the decoder.
	 *
	 * The boundary is 0->1 in fixed point.
	 */
	bound1 = 0;
	bound2 = ALL_BITS;

	run_bits = 0;
	
	/* Paranoia */
	if (b_ptr->size <= 6)
	{
		msg_format("Stream too small to decode %d", b_ptr->size);
		
		return;
	}
	
	/* Get size of the encoded stream */
	size = rerase_block_byte(h2_ptr);
	size |= (rerase_block_byte(h2_ptr) << 8);
	size |= (rerase_block_byte(h2_ptr) << 16);
	size |= (rerase_block_byte(h2_ptr) << 24);

	/* Init the compression model */
	init_compress_hook(prob_table);
	
	/* Init the bitwise stream */
	init_block_bit();
	
	/* Initialise the decoder */
	for (i = 0; i < 16; i++)
	{
		code *= 2;
		code += rerase_block_bit(h2_ptr);
	}
	
	/* Decompress the blocks */
	for (;size > 0; size--)
	{
		/* Decode it */
		symbol = remove_symbol(&code, prob_table, h2_ptr);
		
		/* Output the symbol to the stream */
		write_block_byte(h1_ptr, symbol);
			
		/* Update probability table */
		calc_prob_hook(symbol, prob_table);
	}
	
	/* Paranoia - Get rid of the rest of the input stream */
	b_ptr = h2_ptr->b_ptr;
	
	while (b_ptr)
	{
		/* Delete the block - and point to its child */
		b_ptr = del_block(b_ptr);
	}
}

/* Macro for finding character in string at specified location */
#define GET_LOCATE(X)	((((X)) < string_len) ? ((X)) : ((X) - string_len))

static int string_len = 0;

/*
 * Sorting hook -- comp function -- by "string"
 *
 * We use "u" to point to an array of indexes into a
 * string.
 * "v" is the location of the string.
 */
static bool ang_sort_comp_string(vptr u, vptr v, int a, int b)
{
	int *x = (int*)(u);
	byte *s = (byte*) v;

	int size, p1 = x[a], p2 = x[b];
	bool result;
	
	/* Work out how far we can scan before going off of the end */
	size = string_len - MAX(p1, p2);
	
	/* Compare the first length of string */
	result = memcmp(&s[p1], &s[p2], size);

	if (result) return (result);

	p1 = GET_LOCATE(p1 + size);
	p2 = GET_LOCATE(p2 + size);
	
	/* Work out how far we can scan before going off of the end */
	size = string_len - MAX(p1, p2);
	
	/* Compare the first length of string */
	result = memcmp(&s[p1], &s[p2], size);

	if (result) return (result);
	
	p1 = GET_LOCATE(p1 + size);
	p2 = GET_LOCATE(p2 + size);
	
	/* Scan the remaining region */
	size = x[a] - p1;
	
	/* Compare the first length of string */
	return (memcmp(&s[p1], &s[p2], size));
}


/*
 * Sorting hook -- swap function -- by "wilderness height"
 *
 * We use "u" to point to an array of indexes into a
 * string
 */
static void ang_sort_swap_string(vptr u, vptr v, int a, int b)
{
	int *x = (int*)(u);

	int temp;

	/* Ignore v */
	(void) v;
	
	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;
}


/*
 * Sort a string, using the simple angband sorting algorithm
 *
 * This has a worst-case time of O(n^2log(n))
 * We avoid that by doing a RLE before calling this routine.
 */
static void sort_string(int *s_ptr, byte *string, int len)
{
	byte symbol;
	int i;
	
	int counts[256];
	int positions[256];

	string_len = len;
	
	/* Do one level of radix sort to speed everything up */
	
	C_WIPE(counts, 256, int);
	
	/* Get the sub-totals of the buckets */
	for (i = 0; i < len; i++)
	{
		counts[(int) string[i]]++;
	}
	
	/* Work out the partial regions */
	positions[0] = 0;
	for (i = 0; i < 255; i++)
	{
		positions[i + 1] = positions[i] + counts[i];
	}
	
	/* Output the semi-sorted pointers */
	for (i = 0; i < len; i++)
	{
		symbol = string[i];
		
		s_ptr[positions[symbol]++] = i;
	}
	

	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_string;
	ang_sort_swap = ang_sort_swap_string;

	/* Sort positions by height of wilderness */
	ang_sort(s_ptr, string, len);
}

/*
 * Do the Block Wheeler transform on the data.
 *
 * Reading h1_ptr, writing to h2_ptr;
 */
static void bw_block_trans(block_handle *h1_ptr, block_handle *h2_ptr)
{
	int *offsets;
	block_type *b_ptr = h1_ptr->b_ptr;
	byte *data = b_ptr->block_data;
	
	int size = h1_ptr->b_ptr->size, transform = 0;
	int i, j;
	
	
	/* Create the offset array */
	C_MAKE(offsets, BLOCK_DATA_SIZE, int);

	/* Sort the block, filling the offset array */
	sort_string(offsets, data, size);

	/* Record location of original string in sorted list */
	for (i = 0; i < size; i++)
	{
		if (!offsets[i])
		{
			transform = i;
			break;
		}
	}
	
	/* Write the size to the file */
	write_block_byte(h2_ptr, size & 0xFF);
	write_block_byte(h2_ptr, (size >> 8) & 0xFF);
	write_block_byte(h2_ptr, (size >> 16) & 0xFF);
	write_block_byte(h2_ptr, (size >> 24) & 0xFF);
	
	/* Write the transformation number to the file */
	write_block_byte(h2_ptr, transform & 0xFF);
	write_block_byte(h2_ptr, (transform >> 8) & 0xFF);
	write_block_byte(h2_ptr, (transform >> 16) & 0xFF);
	write_block_byte(h2_ptr, (transform >> 24) & 0xFF);
	
	/* Write the transformed block */
	for (i = 0; i < size; i++)
	{
		j = offsets[i] - 1;
		if (j == -1) j = size - 1;
		
		write_block_byte(h2_ptr, data[j]);
	}

	/* Cleanup */
	C_KILL(offsets, BLOCK_DATA_SIZE, int);
}

/*
 * Do the Inverse Block Wheeler transform on the data.
 *
 * Reading h1_ptr, writing to h2_ptr
 */
static void ibw_block_trans(block_handle *h1_ptr, block_handle *h2_ptr)
{
	int size, transform;
	int i;
	int counts[256];
	int symbol, total;

	u32b *temp, t;
	
	/* Get the data size, and transformation number */
	size = rerase_block_byte(h2_ptr);
	size |= (rerase_block_byte(h2_ptr) << 8);
	size |= (rerase_block_byte(h2_ptr) << 16);
	size |= (rerase_block_byte(h2_ptr) << 24);
	
	transform = rerase_block_byte(h2_ptr);
	transform |= (rerase_block_byte(h2_ptr) << 8);
	transform |= (rerase_block_byte(h2_ptr) << 16);
	transform |= (rerase_block_byte(h2_ptr) << 24);

	C_MAKE(temp, size, u32b);
	C_WIPE(counts, 256, int);

	/*
	 * Get the transformation vector, and put it in temp
	 * Note that we store the symbol there as well.
	 * This works so long as the maximum block size is
	 * less than 2^24 bytes.
	 *
	 * Doing this saves on cache misses, and makes the
	 * inverse transformation much quicker with large
	 * block sizes.
	 */
	for (i = 0; i < size; i++)
	{
		symbol = rerase_block_byte(h2_ptr);
		
		/* Paranoia */
		if (symbol == -1) return;
	
		/* Store the symbol + relative offset in temp */
		temp[i] = symbol + counts[symbol] * 256;
		
		/* Increment the count */
		counts[symbol]++;
	}
	
	/* Get cumulative counts */
	total = 0;
	for (i = 0; i < 256; i++)
	{
		/* Replace with cumulative counts */
		symbol = counts[i];
		counts[i] = total;
		total += symbol;
	}
	
	for (i = 0; i < size; i++)
	{
		/* Get the information */
		t = temp[transform];
		
		/* Get the symbol */
		symbol = t & 0xFF;
		write_block_byte(h1_ptr, symbol);
		
		/* Get the new point in the temp array */
		transform  = t / 256 + counts[symbol];
	}

	/* Cleanup */
	C_KILL(temp, size, u32b);
}


/* Data modeling */

/* Variables for move-to-front probability model */
static byte mtf_table[256];
static u16b mtf_prob_table[257];

static void init_compress_mtf(u16b *prob_table)
{
	int i;
	u32b value;
	
	/* Reset the move-to-front table */
	for (i = 0; i < 255; i++)
	{
		mtf_table[i] = i;
	}

	/* Reset the probability table */
	for (i = 0; i < 255; i++)
	{
		/* Hack - we are going to assume fixed probabilities for now */
		if (i > 128) value = 1;
		else if (i > 64) value = 2;
		else if (i > 32) value = 4;
	
	
	}

}

static void calc_prob_mtf(byte symbol, u16b *prob_table)
{

}

static void init_mtf_model(void)
{
	/*
	 * Initialise the function hooks to point to
	 * the Move-to-Front model.
	 */
	init_compress_hook = init_compress_mtf;
	calc_prob_hook = calc_prob_mtf;
}

/* Variables for simple-probability model */
static u16b prob_count[256];

static void init_compress_simple(u16b *prob_table)
{
	int i;
	
	/* Everything has equal probability to start with */
	for (i = 0; i < 256; i++)
	{
		prob_count[i] = 1;
	}
	
	/* Init prob_table[] */
	for (i = 0; i <= 256; i++)
	{
		prob_table[i] = i;
	}
}

static void calc_prob_simple(byte symbol, u16b *prob_table)
{
	int i;
		
	/* If counter gets too high - we need to rescale everything */
	if (prob_table[256] >= PROB_TABLE_MAX)
	{
		for (i = 0; i < 256; i++)
		{
			/* Halve all the counts - rounding up so don't get zero */
			prob_count[i] = (prob_count[i] + 1) / 2;
		}
	
		/* Reset the probability table */
		prob_table[0] = 0;
		for (i = 0; i < 256; i++)
		{
			prob_table[i + 1] = prob_table[i] + prob_count[i];
		}
	}
	
	/* Increment the count for the symbol */
	prob_count[symbol] += PROB_CHANCE_INC;

	/* Need to redo the counters */
	for (i = symbol + 1; i < 257; i++)
	{
		/* Update the cumluative frequencies */
		prob_table[i] += PROB_CHANCE_INC;
	}
}

static void init_simple_model(void)
{
	/*
	 * Initialise the function hooks to point to
	 * the simple probability model.
	 */
	init_compress_hook = init_compress_simple;
	calc_prob_hook = calc_prob_simple;
}

/*
 * Read a file into an empty stream of blocks.
 */
static errr read_file(block_handle *h_ptr, cptr name)
{
	int fd;
	
	block_type *b_ptr;
	
	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);
	
	/* Open the file */
	fd = fd_open(name, O_RDONLY);
	
	/* Get a new block */
	h_ptr->bf_ptr = new_block();

	b_ptr = h_ptr->bf_ptr;
	h_ptr->b_ptr = b_ptr;

	/* Process the file */
	while (TRUE)
	{
		/* Read the data */
		b_ptr->size = read(fd, b_ptr->block_data, BLOCK_DATA_SIZE);
		
		/* Out of data */
		if (b_ptr->size != BLOCK_DATA_SIZE) break;
		
		/* Need a new block */
		b_ptr->b_next = new_block();
		b_ptr = b_ptr->b_next;
	}
	
	/* Close the file */
	fd_close(fd);

	/* Done */
	return (0);
}

/*
 * Write the stream of blocks to a file.
 */
static errr write_file(block_handle *h_ptr, cptr name)
{
	int fd;
	
	int mode = 0644;
	
	block_type *b_ptr;
	
	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);
	
	/* Drop priv's */
	safe_setuid_drop();
	
	/* Open the file */
	fd = fd_make(name, mode);
	
	/* Grab priv's */
	safe_setuid_grab();

	/* No such file */
	if (fd < 0) return (-1);

	b_ptr = h_ptr->bf_ptr;

	/* Process the file */
	while (b_ptr)
	{
		/* Write out the data */
		if (write(fd, b_ptr->block_data, b_ptr->size) != b_ptr->size)
		{
			return (1);
		}

		/* Next block */
		b_ptr = b_ptr->b_next;
	}
	
	/* Close the file */
	fd_close(fd);

	/* Done */
	return (0);
}


/*
 * Test this module
 */
void test_compress_module(void)
{
	block_handle handle, *h_ptr = &handle;
	cptr infile = "test"; 
	cptr outfile =  "arth";
	cptr outfile2 = "result";
	char buf[1024];
	
	/* Build the filename */
	(void) path_build(buf, 1024, ANGBAND_DIR, infile);
	
	(void) read_file(h_ptr, buf);
	
	/* Use the "simple" compression model */
	init_simple_model();
	
	/* compress the file with rle */
	arth_blocks_encode(h_ptr);
	
	/* Build the filename */
	(void) path_build(buf, 1024, ANGBAND_DIR, outfile);

	(void) write_file(h_ptr, buf);
	
	/* decompress the file with rle */
	arth_blocks_decode(h_ptr);
	
	/* Build the filename */
	(void) path_build(buf, 1024, ANGBAND_DIR, outfile2);

	(void) write_file(h_ptr, buf);
	
	/* Done */
	delete_handle(h_ptr);
	
	/* Clean up */
	clean_blocks(NULL);
}

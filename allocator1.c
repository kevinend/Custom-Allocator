// Started to change names from sub_block to block because I want to change the Block_Storage to be called a Memory_Pool and the suballocations to be referenced as blocks
// Also, just realized that this would be bad with small allocations, like single chars or shorts. At least in the case of a single char it would be allocated as 8 bytes within the block...YIKES!
//  (Though most likely this wouldn't, you would allocate several characters to make up a string, though short words would still have a pretty high penalty!!!)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct 
{
	uint8_t *start;
	uint8_t *current;

	int64_t bytes_left;

} Block_Storage;

void
initial_block_allocate( Block_Storage *block, size_t block_byte_size )
{
	block->start = (uint8_t *)malloc( block_byte_size )	;
	if ( block->start == NULL )
	{
		printf( "Allocation failure...terminating\n" );
		exit  ( EXIT_FAILURE );
	}

	block->current    = block->start;
	block->bytes_left = block_byte_size; 

	return;
}

// FORCES each `char` to be size 8, might not want that!!!
uint8_t *
block_allocate( Block_Storage *block, size_t block_byte_size )
{
	assert( block_byte_size != 0 );
	assert( block_byte_size < block->bytes_left ); 

	{
		#define ALIGNMENT_REQUIREMENT 8 

		int alignment_padding; 
		int remainder;         
		bool properly_aligned;

		remainder = block_byte_size % ALIGNMENT_REQUIREMENT;
		properly_aligned = remainder == 0;
		if ( !properly_aligned ) 
		{
			alignment_padding = (ALIGNMENT_REQUIREMENT - remainder);
			block_byte_size   += alignment_padding;
		}
	}

	uint8_t *block_start;
	block_start = block->current;	
	
	block->current    += block_byte_size;
	block->bytes_left -= block_byte_size;

	return block_start;
}

int main() 
{

	#define BLOCK_SIZE 512

	Block_Storage block1;
	initial_block_allocate( &block1, BLOCK_SIZE );

	int *values;
	values = (int *)block_allocate( &block1, 5 * sizeof *values );

	for ( int i = 0; i < 5; ++i ) 
	{
		values[i] = i;
	}

	for ( int i = 0; i < 5; ++i )
	{
		printf( "%d\n", values[i] );
	}

	char *characters;
	for ( int i = 0; i < 26; ++i ) 
	{
		characters = (char *)block_allocate( &block1, 1 );
		*characters = 'a' + i;
	}

	printf( "Check Pool Usage == %lld\n", block1.bytes_left ); // these are all in the same block

	return 0;
}


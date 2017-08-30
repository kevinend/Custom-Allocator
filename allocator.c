#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct 
{
	int *values;	        // could just move this pointer
	int allocated;          // 5 is kind of a useless number, 20 bytes would be more helpful 
	int unallocated;

} Block;

int *
initial_block_allocate( size_t block_byte_size )
{
	int *block = (void *)malloc( block_byte_size );
	if ( block == NULL ) 
	{
		fprintf( stdout, "Unable to allocate the requested block size\n" );
		exit( EXIT_FAILURE );
	}

	return block;
}

int *
block_allocate( Block *block, size_t count )
{
	// MISSING: checks for size 
	int *new_block;	
	new_block = block->values + block->allocated; // this is the start

	block->allocated += count; // accounts for the next time an allocation occurs
	block->unallocated -= count;

	return new_block;
}

void
block_free( Block *block )
{
	block->allocated = 0;
	return;
}

void
block_reset_size( Block *block, size_t new_size )
{
	// MISSING: if new size is smaller, may not be worth allocating a smaller segment
	free( block->values );
	block->values = malloc( new_size * sizeof *block->values );
	if ( block->values == NULL )
	{
		fprintf( stdout, "Unable to reset the block size\n" );
		exit( EXIT_FAILURE );
	}

	block->allocated = 0;
	block->unallocated = new_size;

	return;
}

int main() 
{
	Block block;
	
	block.values      = NULL;
	block.allocated   = 0;
	block.unallocated = 5;

	block.values = initial_block_allocate( 5 * sizeof *block.values );
	if ( block.values == NULL ) 
	{
		fprintf( stdout, "Unable to allocate space for block.values\n" );
		exit( EXIT_FAILURE );
	}

	
	for ( int i = 0; i < 3; ++i ) 
	{
		int *new_values;
		new_values  = block_allocate( &block, 1 );
		*new_values = i;
		printf( "%d\n", *new_values );
	}

	block_free( &block );

	for ( int i = 0; i < 4; ++i )
	{
		int *new_values;
		new_values  = block_allocate( &block, 1 );
		*new_values = i;
		printf( "%d\n", *new_values );
	}

	block_free( &block );

	free( block.values );

	return 0;

}


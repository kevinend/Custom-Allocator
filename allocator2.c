// Still poor memory usage with small allocations, like single chars or shorts. At least in the case of a single char it would be allocated as 8 bytes within the block...YIKES!
//  (Though most likely you would allocate several characters to make up a string. Short strings would still have a pretty high penalty!!!)

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

	int64_t allocated_bytes; 
	int64_t unallocated_bytes;
	
} Memory_Pool;

void
initial_pool_allocate( Memory_Pool *pool, size_t pool_size_in_bytes )
{
	pool->start = (uint8_t *)malloc( pool_size_in_bytes );
	if ( pool->start == NULL )
	{
		printf( "Allocation failure...terminating\n" );
		exit  ( EXIT_FAILURE );
	}

	pool->current	   		= pool->start;
	pool->allocated_bytes   = 0;
	pool->unallocated_bytes = pool_size_in_bytes; 

	return;
}

// Since the program crashes when requesting more bytes than are available, need to be certain that
// the total requested allocations out of a pool will NOT exceed the pool's allocation size

uint8_t *
pool_allocate( Memory_Pool *pool, size_t requested_bytes )
{
	assert( requested_bytes != 0 );
	assert( requested_bytes < pool->unallocated_bytes ); 

	int allocated_bytes;
	{
		#define ALIGNMENT_REQUIREMENT 8 

		int alignment_padding; 
		int remainder;         
		bool properly_aligned;

		remainder = requested_bytes % ALIGNMENT_REQUIREMENT;
		properly_aligned = (remainder == 0);
		if ( !properly_aligned ) 
		{
			alignment_padding = ALIGNMENT_REQUIREMENT - remainder;
			allocated_bytes   = requested_bytes + alignment_padding;
		}
		else 
		{
			allocated_bytes = requested_bytes;
		}
	}

	uint8_t *block_start;
	block_start = pool->current;	
	
	pool->current           += allocated_bytes;
	pool->allocated_bytes   += allocated_bytes;
	pool->unallocated_bytes -= allocated_bytes;

	return block_start;
}

// override the previous blocks belonging to the pool
void
pool_reset( Memory_Pool *pool )
{
	pool->current           = pool->start;
	pool->unallocated_bytes = pool->allocated_bytes + pool->unallocated_bytes;
	pool->allocated_bytes   = 0;

	return;
}

void
pool_free( Memory_Pool *pool )
{
	free( pool->start );
	return;
}

void
pool_stats( Memory_Pool *pool )
{
	fprintf( stdout, "Memory Pool Stats:\n" );
	fprintf( stdout, "\tpool->start   == %p\n", pool->start );
	fprintf( stdout, "\tpool->current == %p\n", pool->current );
	fprintf( stdout, "\tpool->allocated_bytes == %lld\n", pool->allocated_bytes );
	fprintf( stdout, "\tpool->unallocated_bytes == %lld\n", pool->unallocated_bytes );
	fprintf( stdout, "\ttotal allocation in bytes == %lld\n\n", pool->allocated_bytes + pool->unallocated_bytes );


	return;
}



int main() 
{
	#define POOL_SIZE 512

	Memory_Pool temporary_storage;
	initial_pool_allocate( &temporary_storage, POOL_SIZE );

	int *values;
	values = (int *)pool_allocate( &temporary_storage, 5 * sizeof *values );

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
		characters = (char *)pool_allocate( &temporary_storage, 1 );
		*characters = 'a' + i;
	}

	pool_stats( &temporary_storage );

	pool_reset( &temporary_storage );

	pool_stats( &temporary_storage );

	pool_free ( &temporary_storage );

	return 0;
}


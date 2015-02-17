#include "malloc.h"

struct s_malloc_regions malloc_regions;

t_block	*get_first_block(t_type type)
{
	// C'EST RIGOLO MAIS PAS CLAIR DU TOUT
	// return (*(malloc_regions + type * sizeof(t_block *)));
	
	if (type == TINY)
		return (malloc_regions.tiny_first_block);
	else if (type == SMALL)
		return (malloc_regions.small_first_block);
	else if (type == LARGE)
		return (malloc_regions.large_first_block);
}

void	set_real_size(size_t *size, t_type type)
{
	size_t	quantum_size;

	if (type == TINY)
		quantum_size = TINY_QUANTUM_SIZE;
	else if (type == SMALL)
		quantum_size = SMALL_QUANTUM_SIZE;
	else if (type == LARGE)
		quantum_size = LARGE_QUANTUM_SIZE;
	*size += sizeof(t_block);
	if (*size % quantum_size)
	{
		*size = (*size / quantum_size) * quantum_size + quantum_size;
	}
}

void	alloc_region(t_block *last_block, t_type type)
{
	last_block->next = mmap_tiny_region();

}

int		alloc_first_blocks(void)
{
	int tiny_region_size;
	int small_region_size;

	tiny_region_size = TINY_REGION_LEN * getpagesize();
	small_region_size = SMALL_REGION_LEN * getpagesize();

	malloc_regions.tiny_first_block = (t_block *)mmap(0, tiny_region_size,
			PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	malloc_regions.small_first_block = (t_block *)mmap(0, small_region_size,
			PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if (malloc_regions.tiny_first_block == NULL || malloc_regions.small_first_block == NULL)
		return (-1);

	malloc_regions.tiny_first_block->block_flags = FREE;	
	// malloc_regions.tiny_first_block->block_size = tiny_region_size;
	// malloc_regions.tiny_first_block->next = NULL;
	malloc_regions.tiny_first_block->next = malloc_regions.tiny_first_block - sizeof(t_block);
	malloc_regions.tiny_first_block->next->block_flags  = FREE | LAST;
	malloc_regions.tiny_first_block->next->next  = NULL;

	malloc_regions.small_first_block->block_flags = FREE;	
	// malloc_regions.small_first_block->block_size = small_region_size;
	// malloc_regions.small_first_block->next = NULL;
	malloc_regions.small_first_block->next = malloc_regions.small_first_block - sizeof(t_block);
	malloc_regions.small_first_block->next->block_flags = FREE | LAST;
	malloc_regions.small_first_block->next->next = NULL;

	return (0);
}

void	set_allocd_metadata(t_block *block, size_t size)
{
	t_block *next;

	next = block->next;
	block->block_flags &= ~FREE;
	block->next = block + size;
	block->next->block_flags = FREE;
	block->next->next = next;
}

void	*alloc_block(size_t size, t_type type)
{
	t_block *prev_block;
	t_block *curr_block;

	prev_block = NULL;
	curr_block = get_first_block(type);
	while (!(curr_block->block_flags & FREE && curr_block->next - curr_block > size) && curr_block->next)
	{
		prev_block = curr_block;
		curr_block = curr_block->next;
	}
	if (curr_block->block_flags & FREE)
	{
		set_allocd_metadata(curr_block, size);
		return (curr_block);
	}
	else if (curr_block->block_flags & LAST || !curr_block->next)
	{
		alloc_region(curr_block, type);
		return (alloc_tiny_block(size, type));
	}
}

void	*malloc(size_t size)
{
	void *mallocd_block;
	t_type type;

	if (malloc_regions.tiny_first_block == NULL && alloc_first_blocks() == -1)
		return (NULL);
	if (size <= SMALL_THRESHOLD)
		type = TINY;
	else if (size <= LARGE_THRESHOLD)
		type = SMALL;
	else
		type = LARGE;
	set_real_size(&size, type);
	mallocd_block = alloc_block(size, type);

		
}

void  free(void *ptr)
{
}

void *realloc(void *ptr, size_t size)
{
}

void  show_alloc_mem()
{
}

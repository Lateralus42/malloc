#include "malloc.h"

struct s_malloc_regions malloc_regions;

t_block		*get_first_block(t_type type)
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

size_t		get_region_size(t_type type, size_t size)
{
	if (type == TINY)
		return (TINY_REGION_LEN * getpagesize());
	else if (type == SMALL)
		return (SMALL_REGION_LEN * getpagesize());
	else if (type == LARGE)
		return (size);
}

void		*alloc_region(t_block *ptr_to_update, t_type type, size_t size)
{
	size_t	region_size;

	region_size = get_region_size(type, size);
	ptr_to_update = (t_block *)mmap(0, region_size,
			PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (ptr_to_update == NULL)
		return (NULL);
	ptr_to_update->block_flags = FREE;
	ptr_to_update->block_size = region_size;
	ptr_to_update->next = NULL;
	return (ptr_to_update);
}

int			alloc_first_blocks(void)
{

	alloc_region(malloc_regions.tiny_first_block, TINY, 0);
	alloc_region(malloc_regions.small_first_block, SMALL, 0);
	if (malloc_regions.tiny_first_block == NULL || malloc_regions.small_first_block == NULL)
		return (-1);
	return (0);
}

void		*set_allocd_metadata(t_block *block, size_t size)
{
	t_block *next;

	next = block->next;
	block->block_flags &= ~FREE;
	block->block_size = size;
	if (next)
	{
		block->next = block + size;
		block->next->block_flags = FREE;
		block->next->next = next;
	}
	return (block + sizeof(t_block));
}

void		*alloc_block(size_t size, t_type type)
{
	t_block *curr_block;

	curr_block = get_first_block(type);
	while (!(type != LARGE && curr_block->block_flags & FREE && curr_block->block_size >= size) && curr_block->next)
	{
		curr_block = curr_block->next;
	}
	if (curr_block->block_flags & FREE)
	{
		return (set_allocd_metadata(curr_block, size));
	}
	else if (curr_block->block_flags & LAST || !curr_block->next)
	{
		if (!alloc_region(curr_block->next, type, size))
			return (NULL);
		return (set_allocd_metadata(curr_block->next, size));
		// alloc_block(size, type);
	}
}

void		set_real_size(size_t *size, t_type type)
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

void		*malloc(size_t size)
{
	t_type	type;

	if (malloc_regions.tiny_first_block == NULL && alloc_first_blocks() == -1)
		return (NULL);
	if (size <= SMALL_THRESHOLD)
		type = TINY;
	else if (size <= LARGE_THRESHOLD)
		type = SMALL;
	else
		type = LARGE;
	set_real_size(&size, type);
	return (alloc_block(size, type));
}

void  		free(void *ptr)
{
}

void 		*realloc(void *ptr, size_t size)
{
	return (NULL);
}

void  		show_alloc_mem()
{
}

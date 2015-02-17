#ifndef MALLOC_H
# define MALLOC_H

# include <stdlib.h>
# include <sys/mman.h>
# include <unistd.h>

# define TINY_REGION_LEN (512)
# define SMALL_REGION_LEN (4096)

# define SMALL_THRESHOLD (992)
# define LARGE_THRESHOLD (127 * 1024)

# define TINY_QUANTUM_SIZE (16)
# define SMALL_QUANTUM_SIZE (512)
# define LARGE_QUANTUM_SIZE (4 * 1024)

enum BLOCK_FLAGS
{
	FREE = 1,
	LAST = 2
};

enum MALLOC_TYPE
{
	TINY = 0,
	SMALL = 1,
	LARGE = 2
};

typedef enum MALLOC_TYPE t_type;

void  free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void  show_alloc_mem(void);

static int alloc_first_blocks(void);

struct s_malloc_block {
	// int		free_block;
	char	block_flags;
	// size_t	block_size;
	struct s_malloc_block *next;
};

typedef struct s_malloc_block t_block;

struct s_malloc_regions {
	t_block *tiny_first_block;
	t_block *small_first_block;
	t_block *large_first_block;
};

#endif /* MALLOC_H */

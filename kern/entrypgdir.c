#include <inc/mmu.h>
#include <inc/memlayout.h>

pte_t entry_pgtable[4][NPTENTRIES];

// The entry.S page directory maps the first 4MB of physical memory
// starting at virtual address KERNBASE (that is, it maps virtual
// addresses [KERNBASE, KERNBASE+4MB) to physical addresses [0, 4MB)).
// We choose 4MB because that's how much we can map with one page
// table and it's enough to get us through early boot.  We also map
// virtual addresses [0, 4MB) to physical addresses [0, 4MB); this
// region is critical for a few instructions in entry.S and then we
// never use it again.
//
// Page directories (and page tables), must start on a page boundary,
// hence the "__aligned__" attribute.  Also, because of restrictions
// related to linking and static initializers, we use "x + PTE_P"
// here, rather than the more standard "x | PTE_P".  Everywhere else
// you should use "|" to combine flags.
__attribute__((__aligned__(PGSIZE)))
pde_t entry_pgdir[NPDENTRIES] = {
	// Map VA's [0, 8MB) to PA's [0, 8MB)
	[0]                   = ((uintptr_t)entry_pgtable[0] - KERNBASE) + PTE_P,
	[1]                   = ((uintptr_t)entry_pgtable[1] - KERNBASE) + PTE_P,
	// Map VA's [KERNBASE, KERNBASE+8MB) to PA's [0, 8MB)
	[(KERNBASE>>PDXSHIFT)+0]  = ((uintptr_t)entry_pgtable[0] - KERNBASE) + PTE_P + PTE_W,
	[(KERNBASE>>PDXSHIFT)+1]  = ((uintptr_t)entry_pgtable[1] - KERNBASE) + PTE_P + PTE_W,
};

#define PTE_zzz000(z) z##000 | PTE_P | PTE_W
#define PTE_zzx000(z)  \
	PTE_zzz000(z##0), PTE_zzz000(z##1), PTE_zzz000(z##2), PTE_zzz000(z##3), PTE_zzz000(z##4), PTE_zzz000(z##5), PTE_zzz000(z##6), PTE_zzz000(z##7),  \
	PTE_zzz000(z##8), PTE_zzz000(z##9), PTE_zzz000(z##a), PTE_zzz000(z##b), PTE_zzz000(z##c), PTE_zzz000(z##d), PTE_zzz000(z##e), PTE_zzz000(z##f)
#define PTE_zxx000(z)   \
	PTE_zzx000(z##0), PTE_zzx000(z##1), PTE_zzx000(z##2), PTE_zzx000(z##3), PTE_zzx000(z##4), PTE_zzx000(z##5), PTE_zzx000(z##6), PTE_zzx000(z##7),  \
	PTE_zzx000(z##8), PTE_zzx000(z##9), PTE_zzx000(z##b), PTE_zzx000(z##b), PTE_zzx000(z##c), PTE_zzx000(z##d), PTE_zzx000(z##e), PTE_zzx000(z##f)

// Entry 0 of the page table maps to physical page 0, entry 1 to
// physical page 1, etc.
__attribute__((__aligned__(PGSIZE)))
pte_t entry_pgtable[4][NPTENTRIES] = {
	{ PTE_zxx000(0x0), PTE_zxx000(0x1), PTE_zxx000(0x2), PTE_zxx000(0x3) },
	{ PTE_zxx000(0x4), PTE_zxx000(0x5), PTE_zxx000(0x6), PTE_zxx000(0x7) },
};
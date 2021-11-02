// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

#define call(statement) do{int r; if((r=(statement)) < 0) return r;} while(0)
#define chkpte(pte, perm) (((pte) & (perm)) == (perm))
#define get_pde(idx)  (*(pde_t*)(PGADDR(PDX(UVPT), PDX(UVPT), (idx)*sizeof(pde_t))))
#define get_pte(pde_idx, pte_idx)  (*(pte_t*)(PGADDR(PDX(UVPT), pde_idx, (pte_idx)*sizeof(pte_t))))

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	assert_panic(err & FEC_PR, "page fault: access non-present page: %p", addr);
	assert_panic(err & FEC_U,  "page fault: no permission to access: %p", addr);
	assert_panic(err & FEC_WR, "page fault: not readable: %p",            addr);

	pte_t pte = get_pte(PDX(addr), PTX(addr));
	assert_panic(chkpte(pte, PTE_P | PTE_U | PTE_COW), "page fault: not writable %p", addr);

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

	void * page_addr = ROUNDDOWN(addr, PGSIZE);
	assert(sys_page_alloc(0, UTEMP, PTE_P | PTE_U | PTE_W) == 0);
	memmove(UTEMP, page_addr, PGSIZE);
	assert(sys_page_map(0, UTEMP, 0, page_addr, PTE_P | PTE_U | PTE_W) == 0);
	assert(sys_page_unmap(0, UTEMP) == 0);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, void * pageaddr)
{
	// LAB 4: Your code here.
	call(sys_page_map(0, pageaddr, envid, pageaddr, PTE_P | PTE_U | PTE_COW));
	call(sys_page_map(0, pageaddr, 0,     pageaddr, PTE_P | PTE_U | PTE_COW));
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault haenvidndler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// panic("fork not implemented");
	set_pgfault_handler(pgfault);

	envid_t envid = sys_exofork();
	if(!envid) return envid;

	for(size_t i = 0; i < PDX(UTOP); i++) {
		pde_t pde = get_pde(i);
		if(!chkpte(pde, PTE_P)) continue;
		for(size_t j = 0; j < NPTENTRIES; j++) {
			pte_t pte = get_pte(i, j);
			void * pgaddr = PGADDR(i, j, 0);
			if(pgaddr == (void*)(UXSTACKTOP - PGSIZE)) // ignore UXSTACK
				continue;
			if(!chkpte(pte, PTE_P | PTE_U))            // user not accessable
				continue;
			if(chkpte(pte, PTE_W) || chkpte(pte, PTE_COW)) {  // COW page
				call(duppage(envid, pgaddr));
			}
			else{
				call(sys_page_map(0, pgaddr, envid, pgaddr, PTE_FLAGS(pte)));
			}
		}
	}

	call(sys_page_alloc(envid, (void*)(UXSTACKTOP - PGSIZE), PTE_P | PTE_U | PTE_W));

	extern void _pgfault_upcall(void);
	call(sys_env_set_pgfault_upcall(envid, _pgfault_upcall));

	call(sys_env_set_status(envid, ENV_RUNNABLE));
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}

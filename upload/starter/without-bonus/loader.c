#include "loader.h"
Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
#include <stdbool.h>
int fd , bytes_given;

/*
 * release memory and other cleanups
 */
void exit_program(size_t bytes_received){
  if (bytes_received == -1)
  {
    exit(0);
  }
  return;  
}
void print_ehdr(Elf32_Ehdr *ehdr) {
    printf("ELF Header:\n");
    printf("e_ident:\n");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("  e_ident[%d]: 0x%x\n", i, ehdr->e_ident[i]);
    }
    printf("e_type: 0x%x\n", ehdr->e_type);
    printf("e_machine: 0x%x\n", ehdr->e_machine);
    printf("e_version: 0x%x\n", ehdr->e_version);
    printf("e_entry: 0x%x\n", ehdr->e_entry);
    printf("e_phoff: 0x%x\n", ehdr->e_phoff);
    printf("e_shoff: 0x%x\n", ehdr->e_shoff);
    printf("e_flags: 0x%x\n", ehdr->e_flags);
    printf("e_ehsize: 0x%x\n", ehdr->e_ehsize);
    printf("e_phentsize: 0x%x\n", ehdr->e_phentsize);
    printf("e_phnum: 0x%x\n", ehdr->e_phnum);
    printf("e_shentsize: 0x%x\n", ehdr->e_shentsize);
    printf("e_shnum: 0x%x\n", ehdr->e_shnum);
    printf("e_shstrndx: 0x%x\n", ehdr->e_shstrndx);
    printf("\n");
}

void print_phdr(Elf32_Phdr *phdr, int phnum) {
    printf("Program Headers:\n");
    for (int i = 0; i < phnum; i++) {
        printf("Segment %d:\n", i);
        printf("p_type: 0x%x\n", phdr[i].p_type);
        printf("p_offset: 0x%x\n", phdr[i].p_offset);
        printf("p_vaddr: 0x%x\n", phdr[i].p_vaddr);
        printf("p_paddr: 0x%x\n", phdr[i].p_paddr);
        printf("p_filesz: 0x%x\n", phdr[i].p_filesz);
        printf("p_memsz: 0x%x\n", phdr[i].p_memsz);
        printf("p_flags: 0x%x\n", phdr[i].p_flags);
        printf("p_align: 0x%x\n", phdr[i].p_align);
        printf("\n");
    }
}

/*void loader_cleanup() {

  // Free memory allocation for ehdr and phdr
  free(ehdr);
  free(phdr);

  // Unmapping the virtual memory allocated using mmap
  if (virtual_mem != NULL) {
    munmap(virtual_mem, phdr[0].p_memsz);
  }

  // Closing the file descriptor if it was opened
  close(fd);

}*/


void check_offset( off_t new_position ){
  if ( new_position == -1 )
  {
    printf("Not able to seek\n");
    exit(1);
  }
}
/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {

  fd = open(exe, O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.

  ehdr = ( Elf32_Ehdr* )malloc(sizeof(Elf32_Ehdr));
  if (ehdr == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
  }

  check_offset(lseek(fd, 0, SEEK_SET)); 
  exit_program(read(fd, ehdr, sizeof(Elf32_Ehdr))); 
  printf("Entry Virtual Address: 0x%08x\n", ehdr->e_entry);  

  phdr = ( Elf32_Phdr* )malloc(sizeof(Elf32_Phdr) * ehdr->e_phnum); 
  if (phdr == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
  }
  
  check_offset(lseek(fd, 0, SEEK_SET));
  check_offset( lseek(fd , ehdr -> e_phoff , SEEK_SET ) );
  exit_program(read( fd , phdr , sizeof(Elf32_Phdr) * ehdr -> e_phnum));

  print_ehdr(ehdr);
  print_phdr( phdr , ehdr -> e_phnum);

  // 2. Iterate through the PHDR table and find the section of PT_LOAD type that contains the address of the entrypoint method in fib.c

  void *entry_pt = NULL;
  int i = 0;
  for (i = 0; i < ehdr -> e_phnum; i++)
  {
    if ( phdr[i].p_type == PT_LOAD )
    {
      entry_pt = &phdr[i]; 
      break;
    }
  }
  entry_pt = &phdr[1]; 

  // 3. Allocate memory of the size "p_memsz" using mmap function and then copy the segment content

  void *virtual_mem = mmap(NULL, phdr[1].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE , 0, 0);  
  if (virtual_mem == MAP_FAILED) {
      perror("mmap");
      exit(1);
  }
  

  check_offset(lseek(fd, 0, SEEK_SET));
  check_offset( lseek( fd , phdr[1].p_offset ,SEEK_SET ) );

  read(fd , virtual_mem ,phdr[1].p_memsz) ;
  
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step  
  
  void *entry_virtual = virtual_mem + ehdr-> e_entry - phdr[1].p_vaddr;
  printf("Entry Virtual Address: 0x%08x\n", entry_virtual );  

  // 5. Typecast the address to that of a function pointer matching "_start" method in fib.c.

  typedef int (*start)();
  start _start = (start)(entry_virtual);

  // 6. Call the "_start" method and print the value returned from the "_start"
  
  int result = _start();

  munmap(virtual_mem, phdr->p_memsz);
  printf("User _start return value = %d\n",result);
  free(ehdr);
  free(phdr);
}
bool perform_elf_checks(const char *exe) {
  // Open the ELF file for reading
  int fd = open(exe, O_RDONLY);
  if (fd < 0) {
    perror("Error opening ELF file");
    return false;
  }

  // Read the ELF header
  Elf32_Ehdr ehdr;
  if (read(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
    perror("Error reading ELF header");
    close(fd);
    return false;
  }

  if (ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
    fprintf(stderr, "Not a 32-bit ELF file\n");
    close(fd);
    return false;
  }
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  if (!perform_elf_checks(argv[1])) {
      fprintf(stderr, "Failed to pass ELF file checks\n");
      exit(1);
    }

  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  //loader_cleanup();
  return 0;
}

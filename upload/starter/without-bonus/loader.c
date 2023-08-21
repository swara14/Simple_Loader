#include "loader.h"
Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd , bytes_given;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {

  // Free memory allocation for ehdr and phdr
  free(ehdr);
  free(phdr);

  // Unmapping the virtual memory allocated using mmap
  if (virtual_mem != NULL) {
    munmap(virtual_mem, phdr[0].p_memsz);
  }

  // Closing the file descriptor if it was opened
  close(fd);

}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
  fd = open(exe, O_RDONLY);
  // 1. Load entire binary content into the memory from the ELF file.

  char *file_data =  ( char* )malloc( 200 * sizeof( char ) );
  if (fd < 0 )
  {
    exit(1);
  }

  ehdr = ( Elf32_Ehdr* )malloc(sizeof(Elf32_Ehdr));
  phdr = ( Elf32_Phdr*)malloc(sizeof( ehdr -> e_phentsize * ehdr-> e_phnum));
  
  // 2. Iterate through the PHDR table and find the section of PT_LOAD type that contains the address of the entrypoint method in fib.c
  lseek(fd , ehdr -> e_phoff , SEEK_SET );
  read( fd , phdr , ehdr -> e_phentsize * ehdr -> e_phnum);
  Elf32_Addr entry_pt = 0;
  for (int i = 0; i < ehdr -> e_phnum; i++)
  {
    if ( phdr[i].p_type == PT_LOAD )
    {
      entry_pt = phdr[i].p_vaddr;
      break;
    }
  }
  
  // 3. Allocate memory of the size "p_memsz" using mmap function and then copy the segment content

  void *virtual_mem = mmap(NULL, phdr[0].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS| MAP_PRIVATE, 0, 0);

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step

  lseek( fd , phdr[0].p_offset ,SEEK_SET );
  read( fd , virtual_mem , phdr[0].p_filesz );

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  typedef int ( *start )();
  start _start = ( start )( entry_pt + (int)virtual_mem );
  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
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
  loader_cleanup();
  return 0;
}

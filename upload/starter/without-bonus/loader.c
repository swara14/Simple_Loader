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

// Loader cleanup 
// Free memory allocation for ehdr and phdr
void free_space(){
    free(ehdr);
    free(phdr);
}

void unmapping_virtual_memory(){
    if (virtual_mem != NULL) {
        munmap(virtual_mem, phdr[0].p_memsz);
    }

  // Closing the file descriptor if it was opened
    close(fd);
}

void checking_offset_1( off_t new_position ){
  if ( new_position == -1 )
  {
    printf("Not able to seek\n");
    exit(1);
  }
}

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
  free_space();
}

bool check_file_read(const char* exe){
// Open the ELF file for reading
  int fd = open(exe, O_RDONLY);
  if (fd < 0) {
    perror("Error opening ELF file");
    return false;
  }
  return true;
}

bool check_read_elf(const char* exe){
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
  return true;
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  if (!check_file_read(argv[1]) && !check_file_read(argv[1])) {
      fprintf(stderr, "Failed to pass the ELF file checks\n");
      exit(1);
    }

  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  free_space();
  unmapping_virtual_memory();
  return 0;
}

#include "loader.h"
Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd , i ,min_entrypoint;
Elf32_Addr entry_pt = 0 ;
void *virtual_mem = NULL;

/*
 * release memory and other cleanups
 */
void free_space(){
    free(ehdr);
    free(phdr);
}

void check_file_read(const char* exe){
  int fd = open(exe, O_RDONLY);
  if (fd < 0) {
    printf("Error opening ELF file\n");
    exit(1);
  }
}

void unmapping_virtual_memory(){
    if (virtual_mem != NULL) {
        munmap(virtual_mem, phdr[i].p_memsz);
    }
    close(fd);
}

void check_offset( off_t new_position ){
  if ( new_position == -1 )
  {
    printf("Not able to seek\n");
    exit(1);
  }
}

void load_phdr( size_t size_of_phdr ){
  phdr = ( Elf32_Phdr* )malloc( size_of_phdr * ehdr->e_phnum); 
  
  if (phdr == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
  }
  
  check_offset(lseek(fd, 0, SEEK_SET));
  check_offset( lseek(fd , ehdr -> e_phoff , SEEK_SET ) );
  
  if ( read( fd , phdr , size_of_phdr * ehdr -> e_phnum) !=  size_of_phdr * ehdr -> e_phnum)
  {
    printf("memory not allocated properly to phdr\n");
    exit(1);
  }
  return;
}

void load_ehdr( size_t size_of_ehdr ){
  ehdr = ( Elf32_Ehdr* )malloc(size_of_ehdr);
  
  if (ehdr == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
  }

  check_offset( lseek(fd, 0, SEEK_SET) ); 
  if (read(fd, ehdr, size_of_ehdr) != size_of_ehdr)
  {
    printf("Memory not allocated properly to ehdr\n");
    exit(1);
  }
  if (ehdr -> e_ident[EI_CLASS] != ELFCLASS32) {
    printf("Not a 32-bit ELF file\n");
    exit(1);
  }
  return;
}
/*
 * Load and run the ELF executable file
 */

void find_entry_pt(){
  i = 0  ;
  min_entrypoint = 0;
  int min = 0xFFFFFFFF;
  for ( i = 0; i < ehdr -> e_phnum ; i++)
  {
    if ( phdr[i].p_flags == 0x5 || phdr[i].p_flags == 0x6 )
    {
      if (min > ehdr->e_entry - phdr[i].p_vaddr )
      {
        min = ehdr->e_entry - phdr[i].p_vaddr;
        min_entrypoint = i;
      }
    }
  }
  i = min_entrypoint;
  entry_pt = phdr[i].p_vaddr;
}

void Load_memory(){
  virtual_mem = mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE , 0, 0);  
  
  if (virtual_mem == MAP_FAILED) {
      printf("Error allocating virtual memory\n");
      exit(1);
  }
  
  check_offset(lseek(fd, 0, SEEK_SET));
  check_offset( lseek( fd , phdr[i].p_offset ,SEEK_SET ) );

  read(fd , virtual_mem ,phdr[i].p_memsz) ;
}

void open_elf( char* exe ){
  fd = open(exe, O_RDONLY);
  
  if (fd < 0) {
    printf("Bad file Descriptor\n");
    exit(1);
  }
}

void load_and_run_elf(char* exe) {

  size_t size_of_phdr = sizeof( Elf32_Phdr ) ,size_of_ehdr = sizeof( Elf32_Ehdr); // of one segment

  // 1. Load entire binary content into the memory from the ELF file.

  load_ehdr( size_of_ehdr );
  load_phdr( size_of_phdr );

  // 2. Iterate through the PHDR table and find the section of PT_LOAD type that contains the address of the entrypoint method in fib.c

  find_entry_pt();

  // 3. Allocate memory of the size "p_memsz" using mmap function and then copy the segment content

  Load_memory();

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step  
  Elf32_Addr offset = ehdr->e_entry - entry_pt;   // offset between entry point address and starting address of the PT_LOAD segment.

  void *entry_virtual = virtual_mem + offset;  // actual memory address of the _start function in loaded memory.

  // 5. Typecast the address to that of a function pointer matching "_start" method in fib.c.
  
  int (*_start)() = (int(*)())entry_virtual;

  // 6. Call the "_start" method and print the value returned from the "_start"
  
  int result = _start();

  munmap(virtual_mem, phdr->p_memsz);
  printf("User _start return value = %d\n",result);

}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  open_elf( argv[1] );

  // 2. passing it to the loader for carrying out the loading/execution

  load_and_run_elf(argv[1]);

  // 3. invoke the cleanup routine inside the loader  

  free_space();
  unmapping_virtual_memory();

  return 0;
}

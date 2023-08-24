ASSIGNMENT -1 

SIMPLE LOADER: AN ELF LOADER IN C FROM SCRATCH


Link to Github:    https://github.com/swara14/Simple_Loader_OS_1

IMPLEMENTATION:
A simple loader is an operating system or software system component responsible for loading executable programs or binary files into memory so that the computer's CPU can execute them. 

Header includes Global variables:
The code includes the “loader.h” header and the standard library’s <stdbool.h> header.
Global variables include ehdr (pointer to an ELF32_Ehdr Structure), phdr (pointer to an array of ELF32_Phdr structure), fd (file descriptor for the opened ELF file), i(loop counter), min_entrypoint (index of the segment with the closest entry point), entry_point (pointer to a Elf32_Phdr structure representing the chosen entrypoint segment), and virtual_mem (pointer to allocated memory for the loaded ELF segment). 

Loader Cleanup functions:
‘free_spcae();’ is used to release memory allocated for ehdr and phdr.
‘unmapping _virtual_memory()’ maps the virtual memory if its not NULL and closes the file descriptor fd.

Find Entry Point function:
find_entry_pt() iterates through the program headers and finds the segment with type PT_LOAD. It choses the segment whose entry point is closest to the ELF header’s e_entry. The chosen segment’s pointer is stored in the entry_pt.

Load Memory Function:
Load_memory() uses mmap to allocate virtual memory of the size specified in the chosen segment’s p_memsz.

Load and Run ELF function:
It is the main function for loading and executing the ELF file.
It reads the given ELF header into ehdr using memory allocation and read. 
It allocates memory for the program headers and reads them into phdr.
It finds the entry point segment using the find_entry_pt function.
It allocates memory using Load_memory for the chosen segment.
It calculates the virtual memory address of the entry point and casts it to a function pointer _start. It calls _start and prints the result.


Note: We have modified the definition of load_and_run_elf( char **exe ) to load_and_run_elf( char *exe ) as there was an error in the starter files

CONTRIBUTIONS:

Shrutya Chawla (2022487): 
Construction of the load and run elf function, handling the core logic of ELF loading and execution.
Implemented the find_entry_pt function, iterating through program headers to identify the segment closest to the entry point.
Devised calculating the entry point virtual address and typecasting it to _start function pointer.
Crafted the check_offset functions to verify offset values’ validity during the file operations.

Swara Parekh (2022524): 
Implemented the open_elf function to open and validate the ELF file for reading, providing a boolean result.
Managed the cleanup phase of the loader by designing free_space and unmapping_virtual_memory functions.
Defined the Load_memory function to allocate memory using mmap and load segment content from the ELF file.
Contributed to the comprehensive main function that validates command-line arguments, loads and runs the elf file, and manages cleanup.



______________________________________________________________________________________


	


<h1>Simple Loader: <h3>An ELF Loader in C from Scratch</h3></h1>

<h2>Overview: </h2>
A simple loader is an operating system or software system component responsible for loading executable programs or binary files into memory so that the computer's CPU can execute them. 

<h2>Implementation: </h2>

1. Header includes Global variables:
The code includes the “loader.h” header and the standard library’s <stdbool.h> header.
Global variables include ehdr (pointer to an ELF32_Ehdr Structure), phdr (pointer to an array of ELF32_Phdr structure), fd (file descriptor for the opened ELF file), i(loop counter), min_entrypoint (index of the segment with the closest entry point), entry_point (pointer to a Elf32_Phdr structure representing the chosen entrypoint segment), and virtual_mem (pointer to allocated memory for the loaded ELF segment). 

2. Loader Cleanup functions:
‘free_spcae();’ is used to release memory allocated for ehdr and phdr.
‘unmapping _virtual_memory()’ maps the virtual memory if its not NULL and closes the file descriptor fd.

3. Find Entry Point function:
find_entry_pt() iterates through the program headers and finds the segment with type PT_LOAD. It choses the segment whose entry point is closest to the ELF header’s e_entry. The chosen segment’s pointer is stored in the entry_pt.

4. Load Memory Function:
Load_memory() uses mmap to allocate virtual memory of the size specified in the chosen segment’s p_memsz.

5. Load and Run ELF function:
It is the main function for loading and executing the ELF file.
It reads the given ELF header into ehdr using memory allocation and read. 
It allocates memory for the program headers and reads them into phdr.
It finds the entry point segment using the find_entry_pt function.
It allocates memory using Load_memory for the chosen segment.
It calculates the virtual memory address of the entry point and casts it to a function pointer _start. It calls _start and prints the result.

Note: We have modified the definition of load_and_run_elf( char **exe ) to load_and_run_elf( char *exe ) as there was an error in the starter files.

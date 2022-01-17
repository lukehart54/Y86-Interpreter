# Y86-Interpreter
This program reads the binary data from an ELF Header to check if it is valid. 

If all information is found to be valid it loads the program headers and segments of the file into a large virtual memory array.

Once all data has been read the program deconstructs it into machine code.

After the machine code is executed and the registers are filled with the correct data.

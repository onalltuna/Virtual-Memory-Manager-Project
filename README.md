# Virtual Memory Manager Project

This project, explores the concepts of virtual memory management, which includes translating virtual addresses to physical addresses and handling situations where physical memory is smaller than virtual memory.

## Part I - Virtual Memory Manager with Equal Sizes (Address Space)

In Part I, we implemented a virtual memory manager based on the assumption that the physical address space is the same size as the virtual address space. This means we will not need to implement any page-replacement policies. We have been provided with a base source code in the "virtmem.c" file, and our task is to complete its implementation by addressing the TODO comments.

### Components
- The virtual memory manager will use a TLB (Translation Lookaside Buffer) and a page table.
- A FIFO (First-In-First-Out) replacement policy will be used for the TLB.
- Instead of 16-bit addresses, the project uses 20-bit addresses, divided into a 10-bit page number and a 10-bit page offset.

## Part II - Virtual Memory Manager with Smaller Physical Memory

Part II extends the project to handle scenarios where physical memory (PM) is smaller than virtual memory (VM).

### Changes in Implementation
- In Part II, PM consists of 256 page frames rather than the 1024 frames in Part I.
- We will need to modify the program to manage free page frames.
- Implementation of a page-replacement policy is required, with options for both FIFO and LRU policies.
- A command-line argument (-p) allows us to select the page-replacement policy (e.g., -p 0 for FIFO and -p 1 for LRU).

## Getting Started

- To get started with the project, review the provided source code and address the TODO comments.
- Compile and run the program to see how virtual memory management works.

## Note

- Part II introduces the challenges of handling limited physical memory in the virtual memory manager.
- Implementing different page-replacement policies (FIFO and LRU) allows us to compare their performance.

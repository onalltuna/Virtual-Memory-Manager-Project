/**
 * virtmem.c
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 1047552 /* TODO */

#define PAGE_SIZE 1024
#define OFFSET_BITS 10
#define OFFSET_MASK 1023 /* TODO */

#define MEMORY_SIZE PAGES *PAGE_SIZE

// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

struct tlbentry
{
  unsigned char logical;
  unsigned char physical;
};

struct pageTableEntry
{
  unsigned char logical;
  unsigned char physical;
  int useCount;
};

struct pageTableEntry pagetable[PAGES];

// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];
// number of inserts into TLB that have been completed. Use as tlbindex % TLB_SIZE for the index of the next TLB line to use.
int tlbindex = 0;
int memoryIndex = 0;

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
// int pagetable[PAGES];

signed char main_memory[MEMORY_SIZE];

// Pointer to memory mapped backing file
signed char *backing;

int count[PAGE_SIZE] = {0};

int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

int getLRU(struct pageTableEntry pagetable[])
{
    int index = 0; 
    int min = pagetable[0].useCount;

    for(int i = 1; i < PAGES;i++)
    {
      if(pagetable[i].useCount < min)
      {
        index = i;
        min = pagetable[i].useCount;
      }
    }
    return index;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(unsigned char logical_page)
{
  /* TODO */
  for (int i = 0; i < TLB_SIZE; i++)
  {
    if ((tlb[i].logical == logical_page) && tlbindex > 0)
    {
      return tlb[i].physical;
    }
  }
  return -1;
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping (FIFO replacement). */
void add_to_tlb(unsigned char logical, unsigned char physical)
{
  /* TODO */
  tlb[tlbindex % TLB_SIZE].logical = logical;
  tlb[tlbindex % TLB_SIZE].physical = physical;
  tlbindex++;
}

int main(int argc, const char *argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "Usage ./virtmem backingstore input\n");
    exit(1);
  }

  const char *backing_filename = argv[1];
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0);

  const char *input_filename = argv[2];
  FILE *input_fp = fopen(input_filename, "r");

  const char *p = argv[3];

  int x = 0;
  

  // Fill page table entries with -1 for initially empty table.
  int i;
  for (i = 0; i < PAGES; i++)
  {
    pagetable[i].physical = -1;
    pagetable[i].logical = -1;
  }

  // Character buffer for reading lines of input file.
  char buffer[BUFFER_SIZE];

  // Data we need to keep track of to compute stats at end.
  int total_addresses = 0;
  int tlb_hits = 0;
  int page_faults = 0;

  // Number of the next unallocated physical page in main memory
  unsigned char free_page = 0;

  while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL)
  {
    total_addresses++;
    int logical_address = atoi(buffer);

    /* TODO
    / Calculate the page offset and logical page number from logical_address */
    int offset = (logical_address & OFFSET_MASK);
    int logical_page = (logical_address) >> 10;
    ///////

    int physical_page = search_tlb(logical_page);
    // TLB hit
    if (physical_page != -1)
    {
      tlb_hits++;

      for (int i = 0; i < PAGES; i++)
      {
        if (logical_page == pagetable[i].logical)
        {
          pagetable[i].useCount++;
          count[logical_page]++;
        }
        printf("count: %d\n", count[logical_page]);
      }
      // TLB miss
    }
    else
    {
      for (int i = 0; i < PAGES; i++)
      {
        if (logical_page == pagetable[i].logical)
        {
          physical_page = pagetable[i].physical;
          pagetable[i].useCount++;
          count[logical_page]++;
        }
      }

      // physical_page = pagetable[logical_page];

      // Page fault
      if (physical_page == -1)
      {
        /* TODO */
        page_faults++;
        if(strcmp(p,"0") == 0)
        {
    
          int temp = pagetable[memoryIndex%PAGES].logical;
          pagetable[memoryIndex%PAGES].logical = logical_page;
          pagetable[memoryIndex%PAGES].physical = memoryIndex%PAGES;
          pagetable[memoryIndex%PAGES].useCount = 0;
          

          physical_page =  memoryIndex%PAGES;

          FILE *fp;
          fp = fopen ("BACKING_STORE.bin","r");
          if(fp == NULL)
          {
            printf("Failed to open the file \n");
          }
          fseek(fp, logical_page*PAGE_SIZE, SEEK_SET);
          fread(&main_memory[(memoryIndex%PAGES)*PAGE_SIZE],1, PAGE_SIZE,fp);
          fclose(fp);

          for(int i = 0; i < TLB_SIZE; i++)
          {
            if(tlb[i].logical == temp)
            {
              tlb[i].logical = logical_page;
              tlb[i].physical = physical_page;
              x = 1;
            }
          }

          memoryIndex++;
        }
        else if(strcmp(p,"1") == 0)
        {

          int indexToDelete = memoryIndex%PAGES;
          if (memoryIndex >= PAGES)
          {
            indexToDelete = getLRU(pagetable);
          }
          
          int temp = pagetable[indexToDelete].logical;
          pagetable[indexToDelete].logical = logical_page;
          pagetable[indexToDelete].physical = indexToDelete;
          pagetable[indexToDelete].useCount = count[logical_page] + 1;
          

          physical_page =  indexToDelete;
          printf("page fault handling with LRU, indexToDelete = %d\n", indexToDelete);

          FILE *fp;
          fp = fopen ("BACKING_STORE.bin","r");
          if(fp == NULL)
          {
            printf("Failed to open the file \n");
          }
          fseek(fp, logical_page*PAGE_SIZE, SEEK_SET);
          fread(&main_memory[indexToDelete*PAGE_SIZE],1, PAGE_SIZE,fp);
          printf("page fault handling with LRU\n");
          fclose(fp);

          for(int i = 0; i < TLB_SIZE; i++)
          {
            if(tlb[i].logical == temp)
            {
              tlb[i].logical = logical_page;
              tlb[i].physical = physical_page;
              x = 1;
            }
          }

          memoryIndex++;

        }
      }
      if(x == 0)
      {
        add_to_tlb(logical_page, physical_page);
      }
      x = 0;
    }

    int physical_address = (physical_page << OFFSET_BITS) | offset;
    signed char value = main_memory[physical_page * PAGE_SIZE + offset];

    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
  }

  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", page_faults);
  printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
  printf("TLB Hits = %d\n", tlb_hits);
  printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));

  return 0;
}

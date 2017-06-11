#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "devices/disk.h"
#include "filesys/cache.h"

/* The disk that contains the file system. */
struct disk *filesys_disk;

static void do_format (void);

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void
filesys_init (bool format) 
{
  //printf("filesys init\n");
  filesys_disk = disk_get (0, 1);
  if (filesys_disk == NULL)
    PANIC ("hd0:1 (hdb) not present, file system initialization failed");


  cache_init();
  inode_init();
  free_map_init ();

  if (format) 
    do_format ();

  free_map_open ();
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void
filesys_done (void) 
{
  
  cache_close();
  free_map_close ();

}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create (const char *name, off_t initial_size) 
{
  printf("filesys create\n");
  disk_sector_t inode_sector = 0;
  struct dir *dir = dir_open_root ();
  printf("name:%s\n",name);
  // char * new_name;
  // strlcpy(new_name,name,15);
  hex_dump(0,name,15,true);


  
  free_map_allocate (1, &inode_sector);
  printf("flag1111111111111111111111name:%s\n",name);

  inode_create (inode_sector, initial_size);
  
    printf("flag2name3333333333333:%s\n",name);

  bool success=dir_add (dir, name, inode_sector);
  printf("flag3444444444444444444444444name:%s\n",name);
  
  
  
  // bool success = (dir != NULL
                  // && free_map_allocate (1, &inode_sector)
                  // && inode_create (inode_sector, initial_size)
                  // && dir_add (dir, name, inode_sector));
  

  hex_dump(0,name,15,true);


  printf("sucesssssssssssssssssssssssssssssss\n");

  if (!success && inode_sector != 0) 
    free_map_release (inode_sector, 1);
  dir_close (dir);

  return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *
filesys_open (const char *name)
{
  // printf("filesys open\n");

  struct dir *dir = dir_open_root ();
  struct inode *inode = NULL;

  printf("filesys open filename:%s\n",name);
  if (dir != NULL)
  {
    dir_lookup (dir, name, &inode);
  }
  dir_close (dir);


  return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool
filesys_remove (const char *name) 
{
    //printf("filesys remove\n");

  struct dir *dir = dir_open_root ();
  bool success = dir != NULL && dir_remove (dir, name);
  dir_close (dir); 

  return success;
}

/* Formats the file system. */
static void
do_format (void)
{
   // printf("do format\n");

  printf ("Formatting file system...");
  free_map_create ();

  if (!dir_create (ROOT_DIR_SECTOR, 16))
    PANIC ("root directory creation failed");
  free_map_close ();
  printf ("done.\n");
}

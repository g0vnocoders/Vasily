
#pragma once 
#include "fs/vfs.h"


decl_open(devfs_open);
decl_close(devfs_close);
decl_read(devfs_read);
decl_write(devfs_write);
decl_readdir(devfs_readdir);
decl_finddir(devfs_finddir);

void devfs_init();
struct vfs_node* devfs_int_creat(decl_read((*driver_read)),decl_write((*driver_write)));
extern struct vfs_node* devfs_root;

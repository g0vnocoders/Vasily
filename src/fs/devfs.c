#include "fs/devfs.h"
#include "heap.h"
#include "posix/errno.h"
#include "fs/vfs.h"
#include "multitask.h"

unsigned int inodecounter=0;


struct vfs_node* devfs_root;


decl_readdir(devfs_readdir){
    
}



decl_finddir(devfs_finddir){
    
}
/*
struct vfs_node{
    decl_open((*open));
    decl_close((*close));
    decl_read((*read));
    decl_write((*write));
    decl_readdir((*readdir));
    decl_finddir((*finddir));
    struct vfs_node* mountpoint; //mountpoint is null if nothing is mounted
    char name[255];
    unsigned short perms;
    unsigned int uid,gid;
    unsigned long long sz;
    unsigned int inode;
    struct vfs_node* next,*child;
    char type;
};
#define decl_open(name) int name(const char* path, int flags) 
#define decl_close(name) int name(int fd) 
#define decl_read(name) unsigned long name(int fd, void* buf,unsigned long count)
#define decl_write(name) unsigned long name(int fd,void* buf,unsigned long count)
#define decl_finddir(name) struct vfs_node* name(struct vfs_node* dir,char* n)
*/
struct vfs_node* devfs_int_creat(decl_read((*driver_read)),decl_write((*driver_write))){
    struct vfs_node* it=devfs_root;
    if(it->child!=0){
        it=it->child;
        while(it->next!=0){
            it=it->next;
        }
        it->next=khmalloc(sizeof(struct vfs_node));
        it=it->next;
    }
    else{
        it->child=khmalloc(sizeof(struct vfs_node));
        it=it->child;
    }


    it->next=0;
    it->read=driver_read;
    it->write=driver_write;
    it->readdir=devfs_readdir;
    it->finddir=devfs_finddir;
    it->perms=0644;
    it->uid=0;
    it->gid=0;
    it->sz=0;
    it->inode=inodecounter++;
    it->type=vfsblk;
    return it;
}

void devfs_init(){
        devfs_root=khmalloc(sizeof(struct vfs_node));
        devfs_root->perms=0b111101101;
        devfs_root->readdir=devfs_readdir;
        devfs_root->name[0]='d';
        devfs_root->name[1]='e';
        devfs_root->name[2]='v';
        devfs_root->name[3]=0;
        devfs_root->type=vfsdir;
        devfs_root->child=0;
        memroot->child=devfs_root;
        
}

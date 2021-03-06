#include "dev/pci.h"
#include "common.h"
#include "heap.h"
#include "io.h"
#include "vgatext.h"
struct pcidev *pciroot, *it;

void chkbus(unsigned char b);

unsigned short readconfword(unsigned char bus, unsigned char dev,
                            unsigned char func, unsigned char offset) {
  unsigned int address;
  unsigned int lbus = (unsigned int)bus;
  unsigned int lslot = (unsigned int)dev;
  unsigned int lfunc = (unsigned int)func;
  unsigned short tmp = 0;

  // Create configuration address as per Figure 1
  address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                           (offset & 0xFC) | ((unsigned int)0x80000000));

  // Write out the address
  outl(0xCF8, address);
  // Read in the data
  // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
  tmp = (unsigned short)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}


unsigned int readconfword32(unsigned char bus, unsigned char dev,
                            unsigned char func, unsigned char offset) {
  unsigned int address;
  unsigned int lbus = (unsigned int)bus;
  unsigned int lslot = (unsigned int)dev;
  unsigned int lfunc = (unsigned int)func;

  // Create configuration address as per Figure 1
  address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                           (offset & 0xFC) | ((unsigned int)0x80000000));

  // Write out the address
  outl(0xCF8, address);
  // Read in the data
  // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
  return inl(0xCFC);
}






void chkfunc(unsigned char bus, unsigned char device, unsigned char function) {
  unsigned char baseClass;
  unsigned char subClass;
  unsigned char secondaryBus;

  baseClass = readconfword(bus, device, function, 0xa) >> 8;
  subClass = readconfword(bus, device, function, 0xa);

  it->bus = bus;
  it->dev = device;
  it->func = function;
  it->classcode = baseClass;
  it->subclass = subClass;
  unsigned short vendorID = readconfword(bus, device, function, 0);
  it->vendorid = vendorID;
  it->devid = readconfword(bus, device, function, 2);
  it->irq=readconfword(bus,device,function,0x3c);
  it->next = khmalloc(sizeof(struct pcidev));
  it->next->prev = it;
  
  it = it->next;
  if ((baseClass == 0x6) && (subClass == 0x4)) {
    secondaryBus = readconfword(bus, device, function, 0x18)>>8;
    chkbus(secondaryBus);
  }
}
void chkdev(unsigned char bus, unsigned char device) {
  unsigned char function = 0;

  unsigned short vendorID = readconfword(bus, device, function, 0);
  if (vendorID == 0xFFFF)
    return; // Device doesn't exist
  chkfunc(bus, device, function);
  unsigned short headerType = readconfword(bus, device, function, 0xe);

  if ((headerType & 0x80) != 0) {
    // It's a multi-function device, so check remaining functions
    for (function = 1; function < 8; function++) {
      if (readconfword(bus, device, function, 0) != 0xFFFF) {
        chkfunc(bus, device, function);
      }
    }
  }
}

void chkbus(unsigned char b) {
  unsigned char d;
  for (d = 0; d < 32; ++d) {
    chkdev(b, d);
  }
}

/*

struct pcidev{
   unsigned char bus,dev,func;
   unsigned char classcode,subclass;
   unsigned short vendorid;
   unsigned char devid;
   struct pcidev* next;
};
*/

void init_pci() {
  putstring("init_pci()...");
  pciroot = it = khmalloc(sizeof(struct pcidev));
  unsigned char bus;
  unsigned int headertype = readconfword(0, 0, 0, 0xe)&0xff;
  if ((headertype & 0x80) == 0) {
    chkbus(0);
  } else {
    unsigned int function;
    for (function = 0; function < 8; function++) {
      if (readconfword(0, 0, function, 0) != 0xFFFF){
            bus = function;
            chkbus(bus);
      }
    }
  }
  it = it->prev;
  khfree(it->next);
  it->next = 0;
  it = pciroot;

  putstring(donemsg_str);
}

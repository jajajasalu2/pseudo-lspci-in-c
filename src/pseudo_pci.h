struct pci_slt_info {
	int vdrid;
	int devid;
	int sub_vdrid;
	int sub_devid;
	char *vdrname;
	char *devname;
	char *sub_vdrname;
	char *sub_devname;
} pci_slt_info;

void print_devices(const char* slot);

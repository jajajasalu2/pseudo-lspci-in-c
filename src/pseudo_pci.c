#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pci/pci.h>

#include "pseudo_pci.h"

#define SYS_PCI_DIR "/sys/bus/pci/devices"
#define VENDOR "vendor"
#define DEVICE "device"
#define SUBSYSTEM_VENDOR "subsystem_vendor"
#define SUBSYSTEM_DEVICE "subsystem_device"

static void info_init(struct pci_slt_info *info)
{
	info->vdrid = -1;
	info->devid = -1;
	info->sub_vdrid = -1;
	info->sub_devid = -1;
	info->vdrname = (char*) malloc(1024 * sizeof(char));
	info->devname = (char*) malloc(1024 * sizeof(char));
	info->sub_vdrname = (char*) malloc(1024 * sizeof(char));
	info->sub_devname = (char*) malloc(1024 * sizeof(char));
}

static int open_and_read(const char *path)
{
	FILE *file;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	if (path == NULL || ((file = fopen(path, "r")) == NULL)) {
		return -1;
	}
	read = getline(&line, &len, file);
	if (read != -1)
		return (int)strtol(line, NULL, 16);
	return -1;
}

static void get_names(struct pci_slt_info *info)
{
	struct pci_access *paccess = pci_alloc();
	pci_init(paccess);
	if (info->vdrid >= 0) {
		pci_lookup_name(paccess, info->vdrname,
				/*sizeof(info->vdrname),*/
				1024,
				PCI_LOOKUP_VENDOR,
				info->vdrid);
	}
	if (info->vdrid >= 0 && info->devid >= 0) {
		pci_lookup_name(paccess, info->devname,
				1024,
				PCI_LOOKUP_DEVICE,
				info->vdrid,
				info->devid);
	}
	if (info->sub_vdrid >= 0) {
		pci_lookup_name(paccess, info->sub_vdrname,
				1024,
				PCI_LOOKUP_VENDOR | PCI_LOOKUP_SUBSYSTEM,
				info->sub_vdrid);
	}
	if (info->vdrid >= 0 &&
	    info->devid >= 0 &&
	    info->sub_vdrid >= 0 &&
	    info->sub_devid >= 0) {
		pci_lookup_name(paccess, info->sub_devname,
				1024,
				PCI_LOOKUP_DEVICE | PCI_LOOKUP_SUBSYSTEM,
				info->vdrid,
				info->devid,
				info->sub_vdrid);
	}
}

static void get_ids(const char *slotdir, const char *fname,
		    struct pci_slt_info *info)
{
	char *path = malloc(50 * sizeof(char));
	sprintf(path, "%s/%s", slotdir, fname);
	if (strcmp(fname, VENDOR) == 0) {
		info->vdrid = open_and_read(path);
	} else if (strcmp(fname, DEVICE) == 0) {
		info->devid = open_and_read(path);
	} else if (strcmp(fname, SUBSYSTEM_VENDOR) == 0) {
		info->sub_vdrid = open_and_read(path);
	} else if (strcmp(fname, SUBSYSTEM_DEVICE) == 0) {
		info->sub_devid = open_and_read(path);
	}
	free(path);
}

static int get_slot_id_info(const char *slot_id, struct pci_slt_info *info)
{
	int err = 0;
	DIR *dir;
	struct dirent *ent;
	char *slotdir = malloc(50 * sizeof(char));
	sprintf(slotdir, "%s/%s", SYS_PCI_DIR, slot_id);
	if (access(slotdir, F_OK) == -1) {
		err = 1;
		goto exit;
	}
	dir = opendir(slotdir);
	if (dir == NULL) {
		err = 1;
		goto exit;
	}
	while ((ent = readdir(dir)) != NULL) {
		get_ids(slotdir, ent->d_name, info);
	}
	get_names(info);

exit:
	free(slotdir);
	return err;
}

static void print_dev_info(struct pci_slt_info *info)
{
	printf("Vendor Name: %s\n", info->vdrname);
	printf("Device Name: %s\n", info->devname);
	printf("Subsystem Vendor Name: %s\n", info->sub_vdrname);
	printf("Subsystem Device Name: %s\n", info->sub_devname);
}

void print_devices(const char *slot_id)
{
	int err;
	struct pci_slt_info *info = malloc(sizeof(struct pci_slt_info));
	info_init(info);
	err = get_slot_id_info(slot_id, info);
	if (err) {
		printf("Slot %s is not present in sysfs.\n", slot_id);
		goto exit;
	}
	print_dev_info(info);

exit:
	free(info);
}

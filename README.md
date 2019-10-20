# pseudo-lspci-in-c
Another pseudo implementation of lspci's -s option, in C.

## Configuration

Install libpci's development library first. On a Debian distribution, the command for this is:

```
sudo apt install libpci-dev
```

On a Fedora system using dnf, the command for this is:

```
sudo dnf install pciutils-devel
```

Then, cd into the folder and run make:

```
cd <cloned_repo_location>
make
```

## Running

Run the binary as follows:

```
./build/a.out <slot_id>
```

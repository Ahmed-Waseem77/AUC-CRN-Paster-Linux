# CRN Paster

### Usage

1. Have a CRN File each CRN on a new line
2. Go to your shell's `rc` file, eg. `.bashrc` if you are using bash and export your CRN file path for the variable `CRN_PASTER_PATH`

`.bashrc`

```bash
export CRN_PASTER_PATH="<your path here>"
```

#### Tips

- Bind the executable to a key combination this could be custom shortcuts on `gnome` for example, or hyprland.

### Installation

1. Clone the repo

```bash
git clone https://github.com/Ahmed-Waseem77/AUC-CRN-Paster-Linux.git
cd AUC-CRN-Paster-Linux
```

2. Compile the source

```bash
make
```

#### Manual Compilation

Just compile it like a cpp file

```bash
g++ CRN-paster.cpp -o crn-paster
```
and run it

### Issues

- Some Desktop Environments can't handle the very fast input of the script, I personally had issues with Hyprland

### How it works

This script interacts directly with kernel module `uinput` to make a virtual keyboard and simulate keypresses,
it should be portable on most linux distributions

### Troubleshooting

- Make sure your ``CRN_PASTER_PATH`` is corrent
- check if kernel module `uinput` is loaded with ``lsmod | grep uinput``
  - if not then enable it or modprobe it `modprobe uinput`
- It may need elevated privileges so use with `sudo`

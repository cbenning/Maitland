cmd_drivers/xen/process_stop/process_stop.ko := ld -r -m elf_x86_64  --build-id -o drivers/xen/process_stop/process_stop.ko drivers/xen/process_stop/process_stop.o drivers/xen/process_stop/process_stop.mod.o
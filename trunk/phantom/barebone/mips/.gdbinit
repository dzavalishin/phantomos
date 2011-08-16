set confirm off
symbol-file barebone.elf
dir .

target remote localhost:1234

set logging file gdb.log
set logging on


#break panic
#break pvm_exec_panic

break main

source -v .gdb-local

#break phantom_switch_context
#break do_test_physmem
#break driver_virtio_net_write


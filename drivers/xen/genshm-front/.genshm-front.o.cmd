cmd_/home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o := gcc -Wp,-MD,/home/chris/workspace/malware/drivers/xen/genshm-front/.genshm-front.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.4.1/include -Iinclude -Iinclude2 -I/home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include -I/home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include -include include/linux/autoconf.h   -I/home/chris/workspace/malware/drivers/xen/genshm-front -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m64 -mtune=generic -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=2048 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack  -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(genshm_front)"  -D"KBUILD_MODNAME=KBUILD_STR(genshm_front)"  -c -o /home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o /home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.c

deps_/home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o := \
  /home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.c \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/sysfs.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/stddef.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/compiler-gcc4.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/prefetch.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
    $(wildcard include/config/highmem64g.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/int-ll64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/bitsperlong.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitsperlong.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/posix_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/posix_types_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/processor.h \
    $(wildcard include/config/x86/vsmp.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/paravirt.h) \
    $(wildcard include/config/m386.h) \
    $(wildcard include/config/m486.h) \
    $(wildcard include/config/x86/debugctlmsr.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/processor-flags.h \
    $(wildcard include/config/vm86.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/vm86.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ptrace.h \
    $(wildcard include/config/x86/ptrace/bts.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ptrace-abi.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/segment.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/linkage.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/stringify.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/math_emu.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/sigcontext.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/current.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/numa.h) \
  /usr/lib/gcc/x86_64-linux-gnu/4.4.1/include/stdarg.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/alternative.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/asm.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/x86/invlpg.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/p6/nop.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/sched.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/hweight.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/fls64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/ext2-non-atomic.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/le.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/byteorder.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/byteorder/little_endian.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/swab.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/swab.h \
    $(wildcard include/config/x86/bswap.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/byteorder/generic.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bitops/minix.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/typecheck.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ratelimit.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/param.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/param.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dynamic_debug.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/percpu.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/system.h \
    $(wildcard include/config/ia32/emulation.h) \
    $(wildcard include/config/x86/32/lazy/gs.h) \
    $(wildcard include/config/x86/ppro/fence.h) \
    $(wildcard include/config/x86/oostore.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cmpxchg.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cmpxchg_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/nops.h \
    $(wildcard include/config/mk7.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/irqflags.h \
    $(wildcard include/config/debug/lock/alloc.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/highpte.h) \
    $(wildcard include/config/paravirt/spinlocks.h) \
    $(wildcard include/config/frame/pointer.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/compat/vdso.h) \
    $(wildcard include/config/proc/fs.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/const.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/page_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/page_64_types.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/flatmem.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pgtable_64_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/paravirt/debug.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/desc_defs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/kmap_types.h \
    $(wildcard include/config/debug/highmem.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/kmap_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/bitmap.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/string.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/string_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/page.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/page_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/getorder.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/msr.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/msr-index.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ioctl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ioctl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/ioctl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/errno.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/errno.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/errno-base.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cpumask.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ds.h \
    $(wildcard include/config/x86/ds.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/err.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/personality.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/math64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/div64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/div64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/stat.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/stat.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/seqlock.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/preempt.h \
    $(wildcard include/config/preempt/notifiers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ftrace.h \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/atomic.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/atomic_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/atomic-long.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/bottom_half.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/spinlock_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/spinlock_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/spinlock.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/rwlock.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/spinlock_api_smp.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kmod.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/gfp.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/debug/vm.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/wait.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/nodemask.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/bounds.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/notifier.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/errno.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/rwsem.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/srcu.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/sparsemem.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/smp.h \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/x86/32/smp.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/x86/numaq.h) \
    $(wildcard include/config/mca.h) \
    $(wildcard include/config/eisa.h) \
    $(wildcard include/config/x86/mpparse.h) \
    $(wildcard include/config/acpi.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/mpspec_def.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/x86_init.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/bootparam.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/screen_info.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/apm_bios.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/edd.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/e820.h \
    $(wildcard include/config/efi.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/memtest.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ioport.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ist.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/video/edid.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/apic.h \
    $(wildcard include/config/x86/x2apic.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/delay.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/delay.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/pm.h \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/pm/runtime.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/workqueue.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/jiffies.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/timex.h \
    $(wildcard include/config/no/hz.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/timex.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/tsc.h \
    $(wildcard include/config/x86/tsc.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/apicdef.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/provide/ohci1394/dma/init.h) \
    $(wildcard include/config/x86/visws/apic.h) \
    $(wildcard include/config/x86/f00f/bug.h) \
    $(wildcard include/config/x86/cyclone/timer.h) \
    $(wildcard include/config/pci/mmconfig.h) \
    $(wildcard include/config/intel/txt.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/acpi.h \
    $(wildcard include/config/acpi/numa.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/acpi/pdc_intel.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/numa.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/numa_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/mmu.h \
    $(wildcard include/config/xen.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/vsyscall.h \
    $(wildcard include/config/generic/time.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/io_apic.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/sparse/irq.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/topology.h \
    $(wildcard include/config/x86/ht.h) \
    $(wildcard include/config/x86/64/acpi/numa.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/topology.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/mmdebug.h \
    $(wildcard include/config/debug/virtual.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/elf.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/elf-em.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/elf.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/user.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/user_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/auxvec.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/vdso.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kobject.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/sysfs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kref.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/tracepoint.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rcupdate.h \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/completion.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rcutree.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/local.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/percpu.h \
    $(wildcard include/config/have/legacy/per/cpu/area.h) \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/debug/kmemleak.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/slub_def.h \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/slub/debug.h) \
    $(wildcard include/config/kmemtrace.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kmemtrace.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/trace/events/kmem.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/trace/define_trace.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kmemleak.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/pfn.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/module.h \
    $(wildcard include/config/m586.h) \
    $(wildcard include/config/m586tsc.h) \
    $(wildcard include/config/m586mmx.h) \
    $(wildcard include/config/mcore2.h) \
    $(wildcard include/config/matom.h) \
    $(wildcard include/config/m686.h) \
    $(wildcard include/config/mpentiumii.h) \
    $(wildcard include/config/mpentiumiii.h) \
    $(wildcard include/config/mpentiumm.h) \
    $(wildcard include/config/mpentium4.h) \
    $(wildcard include/config/mk6.h) \
    $(wildcard include/config/mk8.h) \
    $(wildcard include/config/x86/elan.h) \
    $(wildcard include/config/mcrusoe.h) \
    $(wildcard include/config/mefficeon.h) \
    $(wildcard include/config/mwinchipc6.h) \
    $(wildcard include/config/mwinchip3d.h) \
    $(wildcard include/config/mcyrixiii.h) \
    $(wildcard include/config/mviac3/2.h) \
    $(wildcard include/config/mviac7.h) \
    $(wildcard include/config/mgeodegx1.h) \
    $(wildcard include/config/mgeode/lx.h) \
    $(wildcard include/config/4kstacks.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/module.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/trace/events/module.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/cdev.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kdev_t.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/limits.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dcache.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rculist.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/path.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/radix-tree.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/prio_tree.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/pid.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/capability.h \
    $(wildcard include/config/security/file/capabilities.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/semaphore.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/fiemap.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/quota.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dqblk_xfs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dqblk_v1.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dqblk_v2.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/dqblk_qtree.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/nfs_fs_i.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/nfs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/sunrpc/msg_prot.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/inet.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/fcntl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/fcntl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/fcntl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/vmalloc.h \
    $(wildcard include/config/mmu.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/debug/pagealloc.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rbtree.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/auxvec.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pgtable.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pgtable_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pgtable_64_types.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/pgtable.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/have/mlocked/page/bit.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/s390.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/unistd.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/unistd.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/unistd_64.h \
  include/asm/asm-offsets.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/detect/softlockup.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/user/sched.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/group/sched.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/cputime.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/cputime.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/sem.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ipc.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ipcbuf.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/ipcbuf.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/sembuf.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/signal.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/signal.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/signal-defs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/siginfo.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/siginfo.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/proportions.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/percpu_counter.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/seccomp.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/seccomp_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/ia32_unistd.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/resource.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/resource.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/resource.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/latencytop.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/key.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/sysctl.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/aio.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/aio_abi.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/uio.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
    $(wildcard include/config/proc/kcore.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/magic.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/syscalls.h \
    $(wildcard include/config/event/profile.h) \
    $(wildcard include/config/ftrace/syscalls.h) \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/have/syscall/wrappers.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/trace/syscall.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ftrace_event.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ring_buffer.h \
    $(wildcard include/config/ring/buffer/allow/swap.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/kmemcheck.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/seq_file.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/trace_seq.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/hardirq.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/x86/mce.h) \
    $(wildcard include/config/x86/mce/threshold.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/irq.h \
    $(wildcard include/config/irq/per/cpu.h) \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/intr/remap.h) \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/numa/irq/desc.h) \
    $(wildcard include/config/generic/hardirqs/no//do/irq.h) \
    $(wildcard include/config/cpumasks/offstack.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/irqreturn.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/irqnr.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/irq.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/irq_regs.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/hw_irq.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/profile.h \
    $(wildcard include/config/profiling.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/sections.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/sections.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/io.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/xen.h \
    $(wildcard include/config/xen/dom0.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/xen.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/interface.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/interface_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/pvclock-abi.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/hypervisor.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/io_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/asm-generic/iomap.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/x86/wp/works/ok.h) \
    $(wildcard include/config/x86/intel/usercopy.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/uaccess_64.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/sync_bitops.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/xenbus.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/klist.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/device.h \
    $(wildcard include/config/dmar.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/pm_wakeup.h \
    $(wildcard include/config/pm.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/grant_table.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/io/xenbus.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/io/xs_wire.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/platform.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/xen.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/io/ring.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/grant_table.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/grant_table.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/features.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/features.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/page.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/page.h \
    $(wildcard include/config/xen/max/domain/memory.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/callback.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/events.h \
    $(wildcard include/config/xen/dom0/pci.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/linux/interrupt.h \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/debug/shirq.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/event_channel.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/hypercall.h \
    $(wildcard include/config/x86/.h) \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/sched.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/event_channel.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/physdev.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/include/xen/interface/xen-mca.h \
  /home/chris/workspace/dev_lib/modules/linux-2.6-pvops.git/arch/x86/include/asm/xen/events.h \
  /home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.h \

/home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o: $(deps_/home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o)

$(deps_/home/chris/workspace/malware/drivers/xen/genshm-front/genshm-front.o):

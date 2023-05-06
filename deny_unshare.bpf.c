#include <linux/bpf.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/types.h>

#include <bpf/bpf_tracing.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>

#define X86_64_UNSHARE_SYSCALL 272
#define UNSHARE_SYSCALL X86_64_UNSHARE_SYSCALL

typedef unsigned int gfp_t;

struct pt_regs {
	long unsigned int di;
	long unsigned int orig_ax;
} __attribute__((preserve_access_index));

typedef struct kernel_cap_struct {
	__u32 cap[_LINUX_CAPABILITY_U32S_3];
} __attribute__((preserve_access_index)) kernel_cap_t;

struct cred {
	kernel_cap_t cap_effective;
} __attribute__((preserve_access_index));

struct task_struct {
    unsigned int flags;
    const struct cred *cred;
} __attribute__((preserve_access_index));

char LICENSE[] SEC("license") = "GPL";
SEC("lsm/cred_prepare")
int BPF_PROG(handle_cred_prepare, struct cred *new, const struct cred *old, gfp_t gfp, int ret)
{
    struct pt_regs *regs;
    struct task_struct *task;
    kernel_cap_t caps;
    int syscall;
    unsigned long flags;
    
    if (ret) {
        return ret;
    }

    task = bpf_get_current_task_btf();
    regs = (struct pt_regs *) bpf_task_pt_regs(task);
    
    syscall = regs -> orig_ax;
    caps = task -> cred-> cap_effective;

    if (syscall != UNSHARE_SYSCALL) {
        return 0;
    }

    flags = PT_REGS_PARM1_CORE(regs);

    if (!(flags & CLONE_NEWUSER)) {
        return 0;
    }

    // if CAP_SYS_ADMIN is enabled then allow unshare (already root)
    if (caps.cap[CAP_TO_INDEX(CAP_SYS_ADMIN)] & CAP_TO_MASK(CAP_SYS_ADMIN)) {
        return 0;
    }

    return -EPERM;
}

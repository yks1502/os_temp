#ifndef PTREE_H_
#define PTREE_H_

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>
#include <linux/stddef.h> /* for true and false */
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/slab.h>

struct tasklist {
	struct task_struct *task;
	int depth;
	struct list_head list;
};

void get_tasklist_lock(void);

void release_tasklist_lock(void);

void process_node(int idx, struct prinfo *buf, struct task_struct *task);

int has_sibling(struct task_struct *task);

int has_no_child(struct task_struct *task);

int has_children(struct task_struct *task);

int get_process_dfs(struct prinfo *buf, int *nr);

int is_process(struct task_struct *task);

struct task_struct *get_init_task(void);

struct task_struct *get_next_node(struct task_struct *cur);

#endif
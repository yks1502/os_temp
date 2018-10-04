#include "ptree.h"

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int rv, rc, knr;
	struct prinfo *kbuf;

	if (buf == NULL || nr == NULL || *nr < 0) {
		return -EINVAL;
	}

	kbuf = kcalloc(*nr, sizeof(struct prinfo), GFP_KERNEL);
	if (kbuf == NULL) {
		return -ENOMEM;
	}

	rc = copy_from_user(&knr, nr, sizeof(int));
	if (rc != 0) {
		return -EFAULT;
	}

	rc = copy_from_user(kbuf, buf, sizeof(struct prinfo) * *nr);
	if (rc != 0) {
		return -EFAULT;
	}

	get_tasklist_lock();
	rv = get_process_dfs(kbuf, &knr);
	release_tasklist_lock();

	rc = copy_to_user(buf, kbuf, sizeof(struct prinfo) * *nr);
	if (rc != 0) {
		return EFAULT * (-1);
	}

	rc = copy_to_user(nr, &knr, sizeof(int));
	if (rc != 0) {
		return EFAULT * (-1);
	}

	kfree(kbuf);

	return rv;
}

void get_tasklist_lock(void)
{
	read_lock(&tasklist_lock);
}

void release_tasklist_lock(void)
{
	read_unlock(&tasklist_lock);
}

void process_node(int idx, struct prinfo *buf, struct task_struct *task)
{
	struct prinfo to_add;
	struct task_struct *next_sibling, *first_child;

	to_add.pid = task -> pid;
	to_add.state = task -> state;
	to_add.parent_pid = task -> parent -> pid;

	if (has_children(task)) {
		first_child = list_entry(task -> children.next, struct task_struct, sibling);
		to_add.first_child_pid = first_child -> pid;
	} else {
		to_add.first_child_pid = 0;
	}

	if (has_sibling(task)) {
		next_sibling = list_entry(task -> sibling.next, struct task_struct, sibling);
		to_add.next_sibling_pid = next_sibling -> pid;
	} else {
		to_add.next_sibling_pid = 0;
	}

	to_add.uid = task_uid(task);
	strncpy(to_add.comm, task -> comm, 64);
	buf[idx] = to_add;
}

int has_sibling(struct task_struct *task)
{
	struct list_head *head = &task -> parent -> children;

	if (list_is_last(&task -> sibling, head)) {
		return false;
	} else {
		return true;
	}
}

int get_process_dfs(struct prinfo *buf, int *nr)
{
	int total_count = 0;
	int buf_idx = 0;
	struct task_struct *cur = get_init_task();

	while (cur != NULL) {
		if (!is_process(cur) || cur -> pid == 0) {
			cur = get_next_node(cur);
			continue;
		}
		total_count++;
		if (buf_idx < *nr) {
			process_node(buf_idx, buf, cur);
			buf_idx++;
		}
		cur = get_next_node(cur);
	}
	*nr = buf_idx;

	return total_count;
}

int has_no_child(struct task_struct *task)
{
	struct list_head *children;
	if (task == NULL) {
		return false;
	}
	children = &task -> children;
	if (list_empty(children)) {
		return true;
	} else {
		return false;
	}
}

int has_children(struct task_struct *task)
{
	if (task == NULL) {
		return false;
	}

	return !has_no_child(task);
}

int is_process(struct task_struct *task)
{
	if (thread_group_empty(task)) {
		return true;
	} else {
		if (thread_group_leader(task)) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

struct task_struct *get_init_task(void)
{
	return &init_task;
}

struct task_struct *get_next_node(struct task_struct *_cur)
{
	struct task_struct *cur = _cur;

	if (has_children(cur)) {
		return list_entry(cur -> children.next, struct task_struct, sibling);
	}

	while (!has_sibling(cur)) {
		cur = cur -> parent;
		if (cur -> pid == 0) {
			return NULL;
		}
	}

	return list_entry(cur -> sibling.next, struct task_struct, sibling);
}

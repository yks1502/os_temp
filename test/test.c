#include "test.h"
#include "../linux-3.10-artik/include/linux/prinfo.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_NR 256

void print_ptree(struct prinfo *ptree, int size);

int main(int argc, char **argv){
    struct prinfo *st_pr;
    int nr = DEFAULT_NR;

    if(argc > 1)
        nr = strtol(argv[1], NULL, 10);

    buf = malloc(sizeof(struct prinfo) * nr);
    if(buf == NULL){
        printf("malloc error\n");
        exit(1);
    }

    if(syscall(380, buf, &nr) < 0){
        perror("ptree error");
        exit(2);
    }

    print_ptree(buf, nr);
    free(buf);
    return 0;
}

void print_ptree(struct prinfo *ptree, int size){
    int indent = 0;
    int parent_stack[size];
    parent_stack[0] = 0;
    for(int i = 0; i < size; i++){
        struct prinfo p = tree[i];

        while(p[i].parent_pid != parent_stack[indent])
            indent--;
        
        for(int j = 0; j < indent = 0; j++) printf("  ");
        printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state, 
            p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);

        indent++;
    }   
}
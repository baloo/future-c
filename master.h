struct master;

struct future {
    void * data;
    void (*poll)(void *future);
};

void master_register_interest(int);
void master_give(struct future *);

void master_new(void);
void master_main(void);

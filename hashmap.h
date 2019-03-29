struct node;
struct table;
struct table *createTable(int size);
void insert(struct table *t,int key,int val);
int lookup(struct table *t,int key);

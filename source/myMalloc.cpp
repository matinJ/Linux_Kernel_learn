//malloc的实现
size_t align8(size_t s) //按8字节对齐
{
    if(s & 0x7 == 0) return s;
    return ((s>>3) +1) <<3;
}
typedef struct s_block *t_block;
struct s_block {
    size_t size;  /* 数据区大小 */
    t_block next; /* 指向下个块的指针 */
    int free;     /* 是否是空闲块 */
    int padding;  /* 填充4字节，保证meta块长度为8的倍数 */
    char data[1]  /* 这是一个虚拟字段，表示数据块的第一个字节，长度不应计入meta */
};
/* First fit  初次适应算法找到块*/
t_block find_block(t_block *last, size_t size) {
    t_block b = first_block;
    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }
    return b;
}
#define BLOCK_SIZE 24 /* 由于存在虚拟的data字段，sizeof不能正确计算meta长度，这里手工设置 */
t_block extend_heap(t_block last, size_t s) //开辟新的块
{
    t_block b;
    b = sbrk(0);
    if(sbrk(BLOCK_SIZE + s) == (void *)-1)
        return NULL;
    b->size = s;
    b->next = NULL;
    if(last)
        last->next = b;
    b->free = 0;
    return b;
}
void *first_block =NULL;
void split_block(t_block b, size_t s)  //块太大，分割块
{
    t_block new;
    new = b->data + s;
    new->size = b->size -s - BLOCK_SIZE;
    new->next = b->next;
    new->free =1;
    b->size = s;
    b->next = new;
}

void *malloc(size_t size)
{
    t_block b, last;
    size_t s;
    s= align8(size);
    if(first_block)
    {
        last = first_block;
        b = find_block(&last, s);
        if(b)
        {
            if(b->size - s)>=(BLOCK_SIZE + 8)
                split_block(b,s);
            b->free = 0;
        }
        else
        {
            b = extend_heap(last, s);
            if(!b) return NULL;
        }
    }
    else
    {
        b = extend_headp(NULL, s);
        if(!b) return NULL;
        first_block = b;
    }
    return b->data;
}

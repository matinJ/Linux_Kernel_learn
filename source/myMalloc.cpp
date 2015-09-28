//malloc��ʵ��
size_t align8(size_t s) //��8�ֽڶ���
{
    if(s & 0x7 == 0) return s;
    return ((s>>3) +1) <<3;
}
typedef struct s_block *t_block;
struct s_block {
    size_t size;  /* ��������С */
    t_block next; /* ָ���¸����ָ�� */
    int free;     /* �Ƿ��ǿ��п� */
    int padding;  /* ���4�ֽڣ���֤meta�鳤��Ϊ8�ı��� */
    char data[1]  /* ����һ�������ֶΣ���ʾ���ݿ�ĵ�һ���ֽڣ����Ȳ�Ӧ����meta */
};
/* First fit  ������Ӧ�㷨�ҵ���*/
t_block find_block(t_block *last, size_t size) {
    t_block b = first_block;
    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }
    return b;
}
#define BLOCK_SIZE 24 /* ���ڴ��������data�ֶΣ�sizeof������ȷ����meta���ȣ������ֹ����� */
t_block extend_heap(t_block last, size_t s) //�����µĿ�
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
void split_block(t_block b, size_t s)  //��̫�󣬷ָ��
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

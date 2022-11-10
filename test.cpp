#include <stdio.h>
#include <malloc.h>
typedef int Status;
typedef struct BiTNode
{
    char data;
    struct BiTNode *Ichild, *rchild;
} BiTNoed, *BiTree;
int CreateBT(BiTree &T)
{
    char ch;
    scanf("%c", &ch);
    if (ch == '#')
    {
        T = NULL;
    }
    else
    {
        T = (BiTNode *)malloc(sizeof(BiTNode));
        T->data = ch;
        CreateBT(T->Ichild);
        CreateBT(T->rchild);
    }
}
void ExchangeBT(BiTree T)
{
    BiTree temp;
    if (T)
    {
        temp = T->Ichild;
        T->Ichild = T->rchild;
        T->rchild = temp;
        ExchangeBT(T->Ichild);
        ExchangeBT(T->rchild);
    }
}
BiTree SearchTree(BiTree T, char X)
{
    BiTree bt;
    if (T)
    {
        if (T->data == X)
            return T;
        bt = SearchTree(T->Ichild, X);
        if (bt == NULL)
            bt = SearchTree(T->rchild, X);
        return bt;
    }
    return NULL;
}
void LeafCount(BiTree T, int &count)
{
    if (T)
    {
        if ((T->Ichild == NULL) && (T->rchild == NULL))
            count++;
        LeafCount(T->Ichild, count);
        LeafCount(T->rchild, count);
    }
}
void DispBiTree(BiTree T, int level)
{
    int i;
    if (T)
    {
        DispBiTree(T->rchild, level + 1);
        for (i = 0; i < level; i++)
        {
            printf("#");
        }
        printf("%c\n", T->data);
        DispBiTree(T->Ichild, level + 1);
    }
}
void PreOrderTraverse(BiTree T)
{
    if (T)
    {
        printf("%c", T->data);
        PreOrderTraverse(T->Ichild);
        PreOrderTraverse(T->rchild);
    }
}
int main()
{
    BiTree T, SubT;
    char Subch;
    int count = 0;
    CreateBT(T);         //建立二叉链表
    PreOrderTraverse(T); //遍历二叉链表
    DispBiTree(T, 0);    //按树状打印
    ExchangeBT(T);       //交换左右孩子
    DispBiTree(T, 0);    //按树状打印
    fflush(stdin);
    SubT = SearchTree(T, Subch);
    LeafCount(SubT, count);
    printf("叶子节点数为：%d\n", count);
    return 0;
}
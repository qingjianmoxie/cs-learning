#include "Huffman.h"

int main() {
    Huffman<int> huff;
    int a[] = {10, 20, 30, 40};
    huff.creat(a, 4);  //构建一棵哈夫曼树
    huff.print();      //打印节点间关系
    return 0;
}

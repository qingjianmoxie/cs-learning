对二叉树节点的遍历一般来说有中序，后序，和前序三种遍历方法，如果二叉树的高用h来表示，那三种遍历方法所需要的空间复杂度为O(h). 例如对于中序遍历来说，如果我们使用递归来实现的话，代码如下：

```c++
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    vector<int> res;
    vector<int> inorderTraversal(TreeNode* root) {
        if (NULL == root)
            return res;
        inorderTraversal(root->left);
        res.push_back(root->val);
        inorderTraversal(root->right);
        return res;
    }
};
```

上面的实现中，有函数的递归调用，递归的深度等于二叉树的高度，也就是说递归导致的调用堆栈的高度等于二叉树的高度，这样的话，程序虽然没有显示的通过new 来分配内存，但实际上消耗的内存大小也是 O(h).

如果是用迭代法的话, 遍历到某个节点之后并不能回到上层的结点，这是由二叉树本身的结构所限制的，每个结点并没有指向父节点的指针，因此需要使用栈来完成回到上层结点的步骤。

本节要讲的Morris遍历法，能以O(1)的空间复杂度实现二叉树的遍历, 该方法利用了二叉树节点中大量指向null的指针。

举个具体的例子, 给定下面二叉树：

              6
           /     \
          4       9
         / \     / \ 
        2   5   7   10
       / \       \
      1   3       8

采用中序遍历的话，二叉树节点的访问情况如下：

1，2，3，4，5，6，7，8，9，10

给定某个节点，在中序遍历中，直接排在它前面的节点，我们称之为该节点的**前序节点**，例如节点5的前序节点就是4，同理，节点10的前序节点就是9.

在二叉树中如何查找一个节点的前序节点呢？

+ 如果当前节点有左孩子，那么从左孩子开始，沿着右孩子指针一直向右走到底，得到的节点就是它的前序节点.

+ 如果当前节点有左孩子，左孩子的右节点指针是空，那么左孩子就是当前节点的前序节点。

+ 如果当前节点没有左孩子，并且它是其父节点的右孩子，那么它的前序节点就是它的父节点.

+ 如果当前节点没有左孩子，并且它是父节点的左孩子，那么它没有前序节点，并且它自己就是首节点.

Morris遍历算法的步骤如下：

1. 根据当前节点，找到其前序节点，如果前序节点的右孩子是空，那么把前序节点的右孩子指向当前节点，然后进入当前节点的左孩子。

2. 如果当前节点的左孩子为空，打印当前节点，然后进入右孩子。

3. 如果当前节点的前序节点其右孩子指向了当前节点，那么把前序节点的右孩子设置为空，打印当前节点，然后进入右孩子。

我们以上面的例子走一遍。首先访问的是根节点6，得到它的前序节点是5，此时节点5的右孩子是空，所以把节点5的右指针指向节点6：

![](https://upload-images.jianshu.io/upload_images/2849961-dd6c7f2dacbb4053?imageMogr2/auto-orient/strip|imageView2/2/w/623/format/webp)

进入左孩子，也就到了节点4，此时节点4的前序节点为3，前序节点的右孩子指针是空，于是节点3的右孩子指针指向节点4，然后进入左孩子，也就是节点2

![](https://upload-images.jianshu.io/upload_images/2849961-d56d1537ef3ac3ec?imageMogr2/auto-orient/strip|imageView2/2/w/622/format/webp)

此时节点2的左孩子1没有右孩子，因此1就是2的前序节点，并且节点1的右孩子指针为空，于是把1的右孩子指针指向节点2，然后从节点2进入节点1：

![](https://upload-images.jianshu.io/upload_images/2849961-4672d4cd7e694559?imageMogr2/auto-orient/strip|imageView2/2/w/630/format/webp)

此时节点1没有左孩子，因此打印它自己的值，然后进入右孩子，于是回到节点2.根据算法步骤，节点2再次找到它的前序节点1，发现前序节点1的右指针已经指向它自己了，所以打印它自己的值，同时把前序节点的右孩子指针设置为空，同时进入右孩子，也就是节点3.于是图形变为：

![](https://upload-images.jianshu.io/upload_images/2849961-086c3f48a827227b?imageMogr2/auto-orient/strip|imageView2/2/w/622/format/webp)

此时节点3没有左孩子，因此打印它自己的值，然后进入它的右孩子，也就是节点4. 到了节点4后，根据算法步骤，节点4先获得它的前序节点，也就是节点3，发现节点3的右孩子节点已经指向自己了，所以打印它自己的值，也就是4，然后把前序节点的右指针设置为空，于是图形变成：

![](https://upload-images.jianshu.io/upload_images/2849961-dd6c7f2dacbb4053?imageMogr2/auto-orient/strip|imageView2/2/w/623/format/webp)

接着从节点4进入右孩子，也就是节点5，此时节点5没有左孩子，所以直接打印它本身的值，然后进入右孩子，也就是节点6，根据算法步骤，节点6获得它的前序节点5，发现前序节点的右指针已经指向了自己，于是就打印自己的值，把前序节点的右指针设置为空，然后进入右孩子。

接下来的流程跟上面一样，就不再重复了。

Morris遍历，由于要把前缀节点的右指针指向自己，所以暂时会改变二叉树的结构，但在从前缀节点返回到自身时，算法会把前缀节点的右指针重新设置为空，所以二叉树在结构改变后，又会更改回来。

在遍历过程中, 对于没有左子树的节点只到达一次, 对于有左子树的节点会到达两次, 一次是从父节点到当前节点，第二次是从前序节点的右孩子指针返回当前节点，所以Morris遍历算法的复杂度是O(n)。在遍历过程中，没有申请新内存，因此算法的空间复杂度是O(1).

```c++
#include <iostream>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right)
        : val(x), left(left), right(right) {}
};

// Morris中序遍历 (左 -> 根 -> 右)
void MorrisInOrderTraverse(TreeNode *head) {
    if (head == NULL) {
        return;
    }

    TreeNode *p1 = head;
    TreeNode *p2 = NULL;

    while (p1 != NULL) {
        p2 = p1->left;
        if (p2 != NULL) {
            while (p2->right != NULL && p2->right != p1) {
                p2 = p2->right;
            }
            if (p2->right == NULL) {
                p2->right = p1;  // 空闲指针
                p1 = p1->left;
                continue;
            } else {
                p2->right = NULL;
            }
        }
        cout << p1->val << " ";
        p1 = p1->right;
    }
}

// Morris前序遍历 (根 -> 左 -> 右)
void MorrisPreOrderTraverse(TreeNode *head) {
    if (head == NULL) {
        return;
    }

    TreeNode *p1 = head;
    TreeNode *p2 = NULL;

    while (p1 != NULL) {
        p2 = p1->left;
        if (p2 != NULL) {
            while (p2->right != NULL && p2->right != p1) {
                p2 = p2->right;
            }
            if (p2->right == NULL) {
                p2->right = p1;          // 空闲指针
                cout << p1->val << " ";  // 打印结点值的顺序稍微调整
                p1 = p1->left;
                continue;
            } else {
                p2->right = NULL;
            }
        } else {
            cout << p1->val << " ";
        }
        p1 = p1->right;
    }
}

// 逆序右边界
TreeNode *reverseEdge(TreeNode *head) {
    TreeNode *pre = NULL;
    TreeNode *next = NULL;

    while (head != NULL) {
        next = head->right;
        head->right = pre;
        pre = head;
        head = next;
    }

    return pre;
}

// 逆序打印左子树右边界
void printEdge(TreeNode *head) {
    TreeNode *lastNode = reverseEdge(head);
    TreeNode *cur = lastNode;

    while (cur != NULL) {
        cout << cur->val << " ";
        cur = cur->right;
    }
    reverseEdge(lastNode);
}

// Morris后序遍历 (左 -> 右 -> 根)
void MorrisPostOrderTraverse(TreeNode *head) {
    if (head == NULL) {
        return;
    }

    TreeNode *p1 = head;
    TreeNode *p2 = NULL;

    while (p1 != NULL) {
        p2 = p1->left;
        if (p2 != NULL) {
            while (p2->right != NULL && p2->right != p1) {
                p2 = p2->right;
            }
            if (p2->right == NULL) {
                p2->right = p1;  // 空闲指针
                p1 = p1->left;
                continue;
            } else {
                p2->right = NULL;
                printEdge(p1->left);
            }
        }
        p1 = p1->right;
    }
    printEdge(head);
}

void buildBinTree(TreeNode **head) {
    int _val;
    cin >> _val;

    if (_val == -1) {
        *head = NULL;
    } else {
        *head = (TreeNode *)malloc(sizeof(TreeNode));
        (*head)->val = _val;
        buildBinTree(&(*head)->left);
        buildBinTree(&(*head)->right);
    }
}

int main(void) {
    TreeNode *head;
    buildBinTree(&head);
    cout << "前序遍历序列为:";
    MorrisPreOrderTraverse(head);
    cout << endl;

    cout << "中序遍历序列为:";
    MorrisInOrderTraverse(head);
    cout << endl;

    cout << "后序遍历序列为:";
    MorrisPostOrderTraverse(head);
    cout << endl;

    return 0;
}
```

## Morris遍历的缺点

不具有多线程安全.
1. 切换分枝
`git checkout branchname`

1. 创建并切换分枝
`git checkout -b branchname`

1. 撤销所有本地没有提交的修改
`git checkout .`

1. 删除所有本地没有提交的新增文件
`git clean -xdf`
git clean参数说明
-f 删除新增的文件
-df 删除新增的文件和目录

1. 只拉取最近n次提交
`git clone --depth=n git_仓库_url`

1. 拉取单一分枝
`git clone -b git_仓库_分支 --single-branch git_仓库_url`

1. commit之后，不删除工作空间改动代码，撤销commit，并且撤销git add . 操作
`git reset HEAD^`
`git reset --mixed HEAD^`
两者等价，--mixed为默认参数

1. commit之后，不删除工作空间改动代码，撤销commit，不撤销git add . 操作
`git reset --soft HEAD^`

1. commit之后，删除工作空间改动代码，撤销commit，撤销git add .
`git reset --hard HEAD^`

1. commit之后，只想修改commit注释
`git commit --amend`
此时会进入默认vim编辑器，修改注释完毕后保存就好了。

## git stash

### 应用场景
1. 当正在dev分支上开发某个项目，这时项目中出现一个bug，需要紧急修复，但是正在开发的内容只是完成一半，还不想提交，这时可以用git stash命令将修改的内容保存至堆栈区，然后顺利切换到hotfix分支进行bug修复，修复完成后，再次切回到dev分支，从堆栈中恢复刚刚保存的内容。
2. 由于疏忽，本应该在dev分支开发的内容，却在master上进行了开发，需要重新切回到dev分支上进行开发，可以用git stash将内容保存至堆栈中，切回到dev分支后，再次恢复内容即可。
总的来说，git stash命令的作用就是将目前还不想提交的但是已经修改的内容进行保存至堆栈中，后续可以在某个分支上恢复出堆栈中的内容。这也就是说，stash中的内容不仅仅可以恢复到原先开发的分支，也可以恢复到其他任意指定的分支上。git stash作用的范围包括工作区和暂存区中的内容，也就是说没有提交的内容都会保存至堆栈中。

### git stash
能够将所有未提交的修改（工作区和暂存区）保存至堆栈中，用于后续恢复当前工作目录。
可以用下面的命令加上注释:
git stash save "comment"

### git stash list
查看当前stash中的内容

### git stash pop
将当前stash中的内容弹出，并应用到当前分支对应的工作目录上。
注：该命令将堆栈中最近保存的内容删除（栈是先进后出）
如果从stash中恢复的内容和当前目录中的内容发生了冲突，也就是说，恢复的内容和当前目录修改了同一行的数据，那么会提示报错，需要解决冲突，可以通过创建新的分支来解决冲突。

### git stash apply
将堆栈中的内容应用到当前目录，不同于git stash pop，该命令不会将内容从堆栈中删除，也就说该命令能够将堆栈的内容多次应用到工作目录中，适应于多个分支的情况。
可以使用git stash apply + stash名字（如stash@{0}）指定恢复哪个stash到当前的工作目录。

### git stash clear
清除堆栈中的所有内容

### git stash show
查看堆栈中最新保存的stash和当前目录的差异。
git stash show stash@{1}查看指定的stash和当前目录差异。
通过 git stash show -p 查看详细的不同;
同样，通过git stash show stash@{1} -p查看指定的stash的差异内容。

## git reset/git rebase简化提交的代码

在平时的代码开发中，需要加新的feature，或者fix bug以及optimize等操作时，一般都会从master上拉一个分支出来，然后自己在上面随便折腾，这也就导致在同一分支上，会有多次commit，最后在把这些commit都提交到主干，会导致主干上比较乱，这时候git reset命令就有用了：

`git reset –soft HEAD^^`: 把最近的两次提交的变动合并，结果以提交到暂存区的形式存在，即git add之后的文件状态，这个时候，你只需要再git commit一下，就能把多次提交合并。

`git reset –mixed HEAD^^`: 跟上面的类似，只不过文件回退到未加入暂存区之前的状态，也就是说，你还需要执行一把git add，然后才能执行git commit。

`git reset –hard HEAD^^`: O这个操作直接把最近两次的提交都给删除掉，代码没有了，慎用。

合并自己的commit后，也不能直接就提交，最好把master上的变更给同步过来，因为在你开发分支的时候master上可能有新的提交。这个时候git rebase命令就上场了。笔者常用的方法是，首先切换到master分支，git pull，然后切换回之前的分支并执行git rebase master，这样就会把master上的变动给同步过来，master上的变动在前，你自己的变动在后，如果两者有冲突，git rebase会停下来，你自己把冲突的文件给处理好后，然后git add，再执行git rebase –continue。最后再把分支提交，发起code review过程，如果通过的话，就可以直接merge到master，不会有冲突。使用git rebase还有一个好处是，能保证master上的提交是一条线，不像使用git merge提交的，会导致master上有很多分支，当然也有一个不好的地方，那就是会导致提交的时间发生变动，提交的时间不会保证是递增的顺序。
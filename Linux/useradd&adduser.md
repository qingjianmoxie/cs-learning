# useradd和adduser的区别

1. 在root权限下，useradd只是创建了一个用户名，如`useradd 用户名`，它并没有在/home目录下创建同名文件夹，也没有创建密码，因此利用这个用户登录系统，是登录不了的，为了避免这样的情况出现，可以用`useradd -m 用户名`的方式创建，它会在/home目录下创建同名文件夹，然后利用`passwd 用户名`为指定的用户名设置密码。

2. 可以直接利用adduser创建新用户(`adduser 用户名`), 这样在/home目录下会自动创建同名文件夹

3. 删除用户，只需使用一个简单的命令`userdel 用户名`即可。不过最好将它留在系统上的文件也删除掉，你可以使用`userdel -r 用户名`来实现这一目的。 

Linux系统如何添加用户这个问题到网上问一下或者搜一下，很多人可能会说useradd，实际这是不对的。useradd只会添加一个用户，没有创建它的主目录，除了添加一个新用户之外什么都没有。这个用户甚至不能登录，因为没有密码。正确的做法是man page里说的，adduser，这个命令实际是一个perl脚本，是useradd等类似底层命令的更友好的前端，它会用交互性的方式建立新用户，使用它可以指定新用户的家目录，登录密码，是否加密主目录等等.
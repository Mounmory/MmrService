# 1、在虚拟机上编译好三方库

​		在路径\3rdParty\thirdSourceCode\linux中的源码，编译，然后安装到/usr/local/lib目录下

# 2、制作镜像

## 2.1、运行基础镜像

```shell
docker run -it -v /usr/local/lib:/home/3rdInstall ubuntu:16.04 /bin/bash
```



## 2.2安装软件

### 	方法1：逐步安装

```shell
apt update
apt install tzdata -y
```

​		设置时区

```shell
ln -snf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
```

​		切换到库目录

```shell
cd /home/3rdInstall
```

​		把动态库拷过去

```shell
cp -f libprotobuf.so* libsqlite3.so* /usr/local/lib
```

### 	方法2：一行代码安装

```shell
apt update && \
apt install tzdata -y && \
ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime &&\
cd /home/3rdInstall && \
cp -f libprotobuf.so* libsqlite3.so* /usr/local/lib
```

# 3、保存镜像

​		新启动一个终端
​		查看当前正在运行的镜像	

```shell
docker ps
```

​		将正在运行的镜像保存

```shell
docker commit <container_id> mmr_service
```

# 4、新建容器 并启动

​		将程序安装到/mmr目录下，程序路径为/mmr/bin，运行如下docker命令

```shell
docker run -v /mmr:/mmr -p 30010:30010 -it --workdir=/mmr/bin --name mmrService mmr_service ./serviceApp 
```

​		这个命令使用镜像新建一个名字为mmrService的容器并执行，只执行一次即可。

# 5、启动容器

​		后续再启动容器，执行

```shell
docker start -ia mmrService
```

# 6、其他docker操作

​		拉取镜像

```shell
docker pull ubuntu:16.04
```

​		查看所有镜像

```shell
docker images
```

​		可以使用`docker ps`命令查看所有正在运行的容器。如果您想查看所有容器，无论它们的状态如何（运行中、已停止等），您可以添加`-a`或`--all`选项。

```shell
docker ps

docker ps -a
```

​		列出所有标签为“none”的镜像

```shell
docker images --filter "dangling=true"
```

​		删除镜像

```shell
#-f 或 --force：强制删除镜像，即使它被容器使用。
docker rmi [OPTIONS] IMAGE [IMAGE...]
```

​		删除标签为<none>的镜像

```shell
docker rmi $(docker images -f "dangling=true" -q)

#若提示删除失败，是因为有容器正在使用镜像运行，先关闭相关容器
docker rm -f <container_id_or_name>

#或者删除所有容器（慎重）
docker rm -f $(docker ps -aq)
```

​		将容器保存为tar压缩包

```shell
docker save -o <输出文件名.tar> <镜像名>:<标签>
```

​		加载tar压缩镜像文件

```shell
docker load -i <镜像文件.tar>
```

​		进入已运行容器控制台

```shell
docker exec -it <cotainerid> /bin/bash
```

 		如果容器控制台窗口关闭，重新进入控制台

```shell
docker attach <container_id>
```

#### 1 拉取docker镜像

这里使用ubuntu18.04（ubuntu20.04操作相同）

```shell
docker pull ubuntu:18.04
```

#### 2 运行镜像

```shell
docker run -it -v /media/sf_VMs:/code -v /mmr:/mmr -v /home:/home -p 30020:30020 --name ub18Work ubuntu:18.04  /bin/bash
```



#### 	3 安装工具

##### 3.1 安装开发工具和vim

```shell
apt update
apt install build-essential -y
apt install vim -y
```



```shell
#设置时区
apt install tzdata -y
#可能不需要
ln -snf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
```



##### 3.2 安装cmake	

切换到cmake安装包目录

拷贝到系统库目录

```shell
#拷贝安装
cp cmake-3.18.6-Linux-x86_64.tar.gz /usr/local

cd /usr/local
#解压
tar -zxvf cmake-3.18.6-Linux-x86_64.tar.gz
#删除包
rm -f cmake-3.18.6-Linux-x86_64.tar.gz
#改名
mv cmake-3.18.6-Linux-x86_64/ cmake
#设置环境变量
vim ~/.bashrc
#添加内容
export PATH="$PATH:/usr/local/cmake/bin"
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
#使生效
source ~/.bashrc
#查看版本
cmake --version
```



#### 4 拷贝依赖库

```shell
#切换到源码依赖库目录
cd /code/mmrCode/3rdParty/lib/linux/
#拷贝
cp ubuntu18_lib.tar.gz /usr/local/lib/

cd /usr/local/lib/

tar -zxvf ubuntu18_lib.tar.gz

rm -f ubuntu18_lib.tar.gz
```

#### 5 编译源码



```shell
cd /home/build18

cmake /code/mmrCode/code/

make -j2

make install

```

#### 6 运行程序

```shell
cd /mmr/bin

./serviceApp
```


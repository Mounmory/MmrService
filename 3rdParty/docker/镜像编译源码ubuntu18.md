#### 1 拉取docker镜像

这里使用centos stream9

```
docker pull ubuntu:18.04
```

#### 2 运行镜像

```
docker run -it -v /media/sf_VMs:/home/code --name ub18Work ubuntu:18.04  /bin/bash
```



###### 	方法1：逐步安装

```shell
apt update
apt install tzdata -y
```

​		设置时区

```shell
ln -snf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
```

​		切换到库目录




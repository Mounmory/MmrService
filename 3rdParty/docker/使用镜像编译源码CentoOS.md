由于Centos镜像源配置难搞，最终没有使用

#### 1 拉取docker镜像

这里使用centos stream9

```
docker pull quay.io/centos/centos:stream9
```

#### 2 运行镜像

```
docker run -it -v /media/sf_VMs:/home/code --name cent9Work quay.io/centos/centos:stream9  /bin/bash
```





更改镜像源

cp centos-addons.repo /etc/yum.repos.d/centos-addons.repo.bak



cat centos-addons.repo



vi /etc/yum.repos.d/centos-stream.repo

```
[base]
name=CentOS-$releasever - Base - mirrors.aliyun.com
#failovermethod=priority
baseurl=https://mirrors.aliyun.com/centos-stream/$stream/BaseOS/$basearch/os/
        http://mirrors.aliyuncs.com/centos-stream/$stream/BaseOS/$basearch/os/
        http://mirrors.cloud.aliyuncs.com/centos-stream/$stream/BaseOS/$basearch/os/
gpgcheck=1
gpgkey=https://mirrors.aliyun.com/centos-stream/RPM-GPG-KEY-CentOS-Official
 
#additional packages that may be useful
[extras]
name=CentOS-$releasever - Extras - mirrors.aliyun.com
failovermethod=priority
baseurl=https://mirrors.aliyun.com/centos-stream/$stream/extras/$basearch/os/
        http://mirrors.aliyuncs.com/centos-stream/$stream/extras/$basearch/os/
        http://mirrors.cloud.aliyuncs.com/centos-stream/$stream/extras/$basearch/os/
gpgcheck=1
gpgkey=https://mirrors.aliyun.com/centos-stream/RPM-GPG-KEY-CentOS-Official
 
#additional packages that extend functionality of existing packages
[centosplus]
    name=CentOS-$releasever - Plus - mirrors.aliyun.com
    #failovermethod=priority
    baseurl=https://mirrors.aliyun.com/centos-stream/$stream/centosplus/$basearch/os/
            http://mirrors.aliyuncs.com/centos-stream/$stream/centosplus/$basearch/os/
            http://mirrors.cloud.aliyuncs.com/centos-stream/$stream/centosplus/$basearch/os/
    gpgcheck=1
    enabled=0
    gpgkey=https://mirrors.aliyun.com/centos-stream/RPM-GPG-KEY-CentOS-Official
     
[PowerTools]
name=CentOS-$releasever - PowerTools - mirrors.aliyun.com
#failovermethod=priority
baseurl=https://mirrors.aliyun.com/centos-stream/$stream/PowerTools/$basearch/os/
        http://mirrors.aliyuncs.com/centos-stream/$stream/PowerTools/$basearch/os/
        http://mirrors.cloud.aliyuncs.com/centos-stream/$stream/PowerTools/$basearch/os/
gpgcheck=1
enabled=0
gpgkey=https://mirrors.aliyun.com/centos-stream/RPM-GPG-KEY-CentOS-Official
     
     
[AppStream]
name=CentOS-$releasever - AppStream - mirrors.aliyun.com
#failovermethod=priority
baseurl=https://mirrors.aliyun.com/centos-stream/$stream/AppStream/$basearch/os/
        http://mirrors.aliyuncs.com/centos-stream/$stream/AppStream/$basearch/os/
        http://mirrors.cloud.aliyuncs.com/centos-stream/$stream/AppStream/$basearch/os/
gpgcheck=1
gpgkey=https://mirrors.aliyun.com/centos-stream/RPM-GPG-KEY-CentOS-Official
```

dnf clean all && dnf makecache





#### 3 更新包和安装工具

```
yum update
yum groupinstall "Development Tools"
yum install gcc-c++
yum install libstdc++-static
yum install make cmake
```





若更新失败，需使用国内镜像源

```
vi /etc/yum.repos.d/CentOS-Base.repo
```

改为如下

```
[base]
name=CentOS-$releasever - Base - Aliyun
baseurl=http://mirrors.aliyun.com/centos/$releasever/os/$basearch/
gpgcheck=1
gpgkey=http://mirrors.aliyun.com/centos/RPM-GPG-KEY-CentOS-7

#released updates 
[updates]
name=CentOS-$releasever - Updates - Aliyun
baseurl=http://mirrors.aliyun.com/centos/$releasever/updates/$basearch/
gpgcheck=1
gpgkey=http://mirrors.aliyun.com/centos/RPM-GPG-KEY-CentOS-7

#additional packages that may be useful
[extras]
name=CentOS-$releasever - Extras - Aliyun
baseurl=http://mirrors.aliyun.com/centos/$releasever/extras/$basearch/
gpgcheck=1
gpgkey=http://mirrors.aliyun.com/centos/RPM-GPG-KEY-CentOS-7

#additional packages that extend functionality of existing packages
[centosplus]
name=CentOS-$releasever - Plus - Aliyun
baseurl=http://mirrors.aliyun.com/centos/$releasever/centosplus/$basearch/
gpgcheck=1
enabled=0
gpgkey=http://mirrors.aliyun.com/centos/RPM-GPG-KEY-CentOS-7
```

清除缓存并生成新的缓存

```
yum clean all
yum makecache
```

再使用命令`yum update`更新



升级GCC

```
yum install centos-release-scl scl-utils-build
```

```console
yum search devtoolset *# 搜索 GCC* 
yum install devtoolset-7-gcc.x86_64 *# 安装 GCC7* 
scl enable devtoolset-7 *# 启用 GCC7*
```

第二个方法

yum install -y centos-release-scl
# 安装linux依赖库

## 方法1：

### 安装sqlite

https://blog.csdn.net/lx7820336/article/details/123060921

```
# 解压缩
$ tar -zxvf sqlite-autoconf-3450000.tar.gz 
$切换到解压目录
$ cd sqlite-autoconf-3450000
# 执行
$ ./configure
# 执行
$ make -j4
# 执行
$ make install
```



### 安装protobuf

	# 自行下载源码包, 解压缩
	$ tar zxvf protobuf-cpp-3.21.12.tar.gz 
	# 进入到解压目录
	$ cd protobuf-3.21.12/
	# 构建并安装
	$ ./configure         # 检查安装环境, 生成 makefile
	$ make                # 编译
	$ sudo make install   # 安装

## 方法2：

```
# 运行脚本install_3rdParty.sh自动安装依赖库
$ ./install_3rdParty.sh
```



## 方法3：

编译较小教快

### sqlite编译

​	1）、编译libsqlite3.so
​		解压后在终端执行以下2行命令：
​		gcc -g -shared -fPIC -c sqlite3.c
​		gcc -g -shared -fPIC -o libsqlite3.so sqlite3.o
​		此时文件目录下就出现了libsqlite.so动态库了。
​	2）、编译命令行管理工具：
​		gcc shell.c sqlite3.c -lpthread -ldl -o sqlite3
​	

### protobuf编译

​	https://www.cnblogs.com/xupeidong/p/9376506.html

#### 	方法1

​		1、cd protobuf_xxx
​		2、mkdir build
​		3、cd build
​		4、ccmake ../cmake这里会弹出一个界面，
​		5、Press c进行配置，配置完成弹出一个配置界面，大家根据自己的需要进行配置即可

```
		 CMAKE_BUILD_TYPE                *Release
		 CMAKE_INSTALL_PREFIX            */usr/local
		 protobuf_BUILD_CONFORMANCE      *OFF
		 protobuf_BUILD_EXAMPLES         *OFF
		 protobuf_BUILD_LIBPROTOC        *OFF
		 protobuf_BUILD_PROTOC_BINARIES  *OFF
		 protobuf_BUILD_SHARED_LIBS      *ON
		 protobuf_BUILD_TESTS            *OFF
		 protobuf_DISABLE_RTTI           *OFF
		 protobuf_INSTALL                *ON
		 protobuf_INSTALL_EXAMPLES       *OFF
		 protobuf_MSVC_STATIC_RUNTIME    *OFF
		 protobuf_TEST_XML_OUTDIR        *OFF
		 protobuf_USE_EXTERNAL_GTEST     *OFF
		 protobuf_WITH_ZLIB              *ON
```

​	确认后，generate
​	6、make
​	7、make install

#### 方法2

​	1、cd  protobuf-3.5.0
​	2、cd cmake
​	3、vim CMakeList.txt
​	4、在Options选项的最下方添加

```
		set(CMAKE_BUILD_TYPE RELEASE)
		set(CMAKE_INSTALL_PREFIX "/usr/local/lib")
		set(protobuf_BUILD_EXAMPLES OFF)
		set(protobuf_BUILD_SHARED_LIBS ON)
		set(protobuf_BUILD_TESTS OFF)
		set(protobuf_INSTALL_EXAMPLES OFF)
		set(protobuf_MSVC_STATIC_RUNTIME OFF)
		set(protobuf_WITH_ZLIB ON)
```

​	5、cd ../回到protobuf-3.5.0创建build文件夹
​	6、cd build
​	7、cmake ../cmake   ////这里就是使用cmake编译cmake目录下的CMakeList.txt
​	8、make -j2
​	9、完成





# 设置库搜索目录

```
#若提示运行时找不到库，将安装的库目录设置到库搜索路径
$ vim ~/.bashrc
#添加如下内容
$ export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
#执行
$ source ~/.bashrc
```


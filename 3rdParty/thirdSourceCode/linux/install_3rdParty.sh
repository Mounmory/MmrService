#!/bin/bash

# 检查 /home/3rdInstall 目录是否存在，如果存在则返回  
if [ -d "/home/3rdInstall" ]; then  
  echo "error, Deleting existing /home/3rdInstall directory..."  
  echo "exit ..."  
  exit 1
fi  

CurDir=$(pwd)

InstallDir="/home/3rdInstall"

echo "Creating $InstallDir directory..."  
mkdir -p $InstallDir  

echo "--------install sqlite---------"  
cd $CurDir
cp sqlite-autoconf-3450000.tar.gz $InstallDir
cd $InstallDir
tar -zxvf sqlite-autoconf-3450000.tar.gz 
cd sqlite-autoconf-3450000
./configure
make -j4
make install

echo "--------install protobuf---------"  
cd $CurDir
cp protobuf-cpp-3.21.12.tar.gz /home/3rdInstall/
cd $InstallDir
tar zxvf protobuf-cpp-3.21.12.tar.gz 
cd protobuf-3.21.12/
./configure
make -j4
make install

#删除文件夹
rm -rf /home/3rdInstall 
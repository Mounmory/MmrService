FROM mmr_serv_base:latest

LABEL maintainer="237628106@qq.com"

LABEL version="0.1"

LABEL description="docker image whith mmr service."

ARG DEBIAN_FRONTEND=noninteractive

#将依赖库放到镜像中
COPY . /usr/local/lib

RUN apt update && \
	apt install tzdata -y && \
	ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime &&\

VOLUME ["/mmr"]

WORKDIR /mmr

EXPOSE 30010

CMD ["./serviceApp"]

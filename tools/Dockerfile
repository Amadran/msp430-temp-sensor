FROM ubuntu:22.04

# install necessary packages
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get install -y wget bzip2 make unzip cppcheck

# create a non-root user named "ubuntu", but put in root group
# since GitHub Actions needs permissions to create tmp files
RUN useradd -rm -rd /home/ubuntu -s /bin/bash -g root -G sudo \
    -u 1001 ubuntu
USER ubuntu
WORKDIR /home/ubuntu

# NOTE: the following is for convenience, but may not work in the future
# if the links break

# download toolchain
RUN wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-9.3.1.11_linux64.tar.bz2
RUN wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-LlCjWuAbzH/9.3.1.2/msp430-gcc-support-files-1.212.zip

# unpack files
RUN mkdir -p dev/tools/
RUN tar xjvf msp430-gcc-9.3.1.11_linux64.tar.bz2
RUN unzip msp430-gcc-support-files-1.212.zip
RUN mv msp430-gcc-support-files/include/* msp430-gcc-9.3.1.11_linux64/include/
RUN mv msp430-gcc-9.3.1.11_linux64 dev/tools/msp430-gcc
RUN rm -r msp430*

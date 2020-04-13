FROM ubuntu:18.04

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install g++ --std=c++11 valgrind cmake git -y

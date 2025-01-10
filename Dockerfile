FROM arm64v8/ubuntu:24.04

RUN apt-get update \
 && export DEBIAN_FRONTEND=noninteractive \
 && apt-get -y install --no-install-recommends \
        cmake \
        build-essential \
	neovim \
	gdb \
	libtbb-dev \
	git \
 && apt-get clean autoclean \
 && apt-get autoremove --yes 
# && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN echo "alias vi=nvim" >> ~/.bash_aliases \
 && echo "alias vim=nvim" >> ~/.bash_aliases

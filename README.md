# IOTA Pollen CLI Wallet

A CLI wallet to interact with Pollen network on POSIX and embedded systems.

# System requirements

* libcurl
* cmake 3.15 above


# Support Commands (WIP)

* node_info: shows status of the connected node.
* balance: display current wallet balance

# Disclaimer

This application is based on [pollen_qianbao](https://github.com/oopsmonk/pollen_qianbao), it is under developing and tested with [GoShimmer-0.3.0](https://github.com/iotaledger/goshimmer/releases/tag/v0.3.0) on Ubuntu 18.04 and Raspberry Pi.  

It's not guarantee to work with other GoShimmer release or systems, please use with caution!  

# Building

```
git clone https://github.com/oopsmonk/pollen_qianbao_cli.git
cd pollen_qianbao_cli && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$PWD ..
make -j8
```

# Running

![Imgur Image](https://i.imgur.com/x0eFvit.png)

# IOTA Pollen CLI Wallet

A CLI wallet to interact with Pollen network on POSIX and embedded systems.

# System requirements

* libcurl
* cmake 3.15 above


# Support Commands (WIP)

* `node_info`: shows status of the connected node.
* `balance`: display current wallet balance
* `addresses`: list wallet addresses
* `wallet_info`: display wallet status
* `req_funds`: request funds from faucet
* `new_address`: generate a new address
* `seed`: display the seed of this wallet
* `send_funds`: issue a payment, ex: send_funds [base58 encoded address] -v 100
* `exit`: exit and save settings to ./wallet.bin

# Disclaimer

This application is based on [goshimmer-client-c](https://github.com/oopsmonk/goshimmer-client-c), it is under developing and tested with [GoShimmer-0.3.0](https://github.com/iotaledger/goshimmer/releases/tag/v0.3.0) on Ubuntu 18.04 and Raspberry Pi.  

It's not guarantee to work with other GoShimmer release or systems, please use with caution!  

# Building

```
git clone https://github.com/oopsmonk/pollen_qianbao_cli.git
cd pollen_qianbao_cli && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$PWD ..
make -j8
```

# Running

Create wallet with a random seed  

```
$ ./pollen_cli 
== Pollen CLI starting....
Enter the node endpoint: http://116.203.236.178:8080/
Enter port number (empty for default): 
Enter seed (Enter for a random seed): 
Wallet will genrate a seed for you
Pollen> wallet_info
========= Wallet Status =========
bitmask capacity = 1
0 0 0 0 0 0 0 0 | 
last address index: 0
first unspent index: 0
last unspent index: 0
========= outputs =========
unspent_outputs: [
===
address: QBRG7GsMvMTv66oTAy2MHo8kpe43ZquXEKT5t6wQgJT2 
===
]
Pollen>
```

Create wallet with an existing seed

```
$ ./pollen_cli
== Pollen CLI starting....
Enter the node endpoint: http://goshimmer:8080/
Enter port number (empty for default): 
Enter seed (Enter for a random seed): 332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK
Enter last address index: 7
Enter first unspent index: 4
Enter last unspent index: 7
Pollen>
```

Create wallet from wallet.bin file

```
$ ./pollen_cli ./wallet.bin 
== Pollen CLI starting....
Enter the node endpoint: http://goshimmer:8080/
Enter port number (empty for default): 
Pollen> wallet_info
========= Wallet Status =========
bitmask capacity = 1
1 1 0 0 0 0 0 0 | 
last address index: 3
first unspent index: 2
last unspent index: 3
========= outputs =========
unspent_outputs: [
===
address: YJ7jSrJTuGmugHo9Z8FESR1aepHvESVJsNtBvTRrEA9Z [spent]
===
address: VYneBQWHB9fj6zBpw6AKpZB4ajd7uAXnFNmWk6VF7HGV [spent]
===
address: WX1n2Mk18JhCEqp8ksHzigE5kKTmGxm6K3YTXdhsd8SC 
id: 9maJdFQpuDXjBvAKvkJCL6xrVdCuJujXc2aeh22y1n1DXebo1KViqFo7tqmGoFct1c7VWb9pEyqrm3pzhCKpAPNK
inclusion_state: [ confirmed: true, finalized: true, liked: true,  ]
balances: [
IOTA, 1237
]
id: 9maJdFQpuDXjBvAKvkJCL6xrVdCuJujXc2aeh22y1n1DRabm2KMxwhh3dwZRByYgAaxdvpGVDtcALDYXJpmSywRC
inclusion_state: [ confirmed: true, finalized: true, liked: true,  ]
balances: [
IOTA, 1337
]
===
address: WdXHkr1vMGGyCQUD8j3VXa6ACS7mgt5Jut7aZvczX8ZE 
===
]
Pollen>
```

![Imgur Image](https://i.imgur.com/x0eFvit.png)

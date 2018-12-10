eosio.token
-----------

This eosio contract allows users to create, issue, and manage tokens on
eosio based blockchains.



钱包创建
```
localhost:~ yekai$ cleos wallet create --to-console
"/usr/local/Cellar/eosio/1.5.0/opt/eosio/bin/keosd" launched
Creating wallet: default
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
"PW5HxknpoC5kGEkRg2zMG6m6wsf6a3YH2VCueQNRtH4ejLT1LDqo8"
```

创建key

```
localhost:~ yekai$ cleos wallet create_key
Created new private key with a public key of: "EOS76reFPVt2RH8f3sS1cQqeM3q88wZsiJ6BbwUCrvaERTCM9gtUa"
```

创建key值
```
localhost:~ yekai$ cleos create key --to-console
Private key: 5JU21mzt4phX1hZA3o49b1ipJP5azwGpVXw11nXbQWqoDQ2cEV6
Public key: EOS6kxJkaJpBhcixJ92ah7zS7KAsJSHCFNFyZer1ZuSYT8etNgyuk
```


查看已经导入的key
```
localhost:~ yekai$ cleos wallet list keys
Wallets:
[
  "default *"
]
[
  "EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq",
  "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
  "EOS6kxJkaJpBhcixJ92ah7zS7KAsJSHCFNFyZer1ZuSYT8etNgyuk",
  "EOS76reFPVt2RH8f3sS1cQqeM3q88wZsiJ6BbwUCrvaERTCM9gtUa"
]

```

创建账户
```
cleos create account eosio alice EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq

cleos create account eosio bob EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq
```







创建账户

```
bogon:pdjtoken yekai$ cleos create account eosio  pdjtoken EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq 
executed transaction: e39421c121b1564d679b3ebbfd5e3ab8304aaeb02355ca84ae031cca3e240e4a  200 bytes  517 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"pdjtoken","owner":{"threshold":1,"keys":[{"key":"EOS5J9qgnyDGnoiGrxddMKud...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
```

部署合约


```
bogon:pdjtoken yekai$ cleos set contract pdjtoken ../pdjtoken/ -p  pdjtoken
Reading WASM from /Users/yekai/eoshome/mywork/tokenpai/contracts/pdjtoken/pdjtoken.wasm...
Publishing contract...
executed transaction: 21ae9c499bc9af1d09cc634782ff30113d82580002a8a2f95f62d6630ba5d8e6  7656 bytes  2886 us
#         eosio <= eosio::setcode               {"account":"pdjtoken","vmtype":0,"vmversion":0,"code":"0061736d0100000001a4011b60037f7e7f0060047f7e7...
#         eosio <= eosio::setabi                {"account":"pdjtoken","abi":"0e656f73696f3a3a6162692f312e310005076163636f756e7400010762616c616e63650...
warning: transaction executed locally, but may not be confirmed by the network yet
```

创建ptk，注意小数点


```
bogon:pdjtoken yekai$ cleos push action pdjtoken create '["pdj","1000000.00 PTK"]' -p pdjtoken
executed transaction: 521a26580ab250c13d9c252223d4ac177d4318602ad37c0c7560931780692315  120 bytes  1208 us
#      pdjtoken <= pdjtoken::create             {"issuer":"pdj","maximum_supply":"1000000.00 PTK"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
```

发行 

```
bogon:pdjtoken yekai$ cleos push action pdjtoken issue '["pdj","100.00 PTK","tuhao"]' -p pdj
executed transaction: 979f4dc35efe2a54cb3fb9bee5c812acd0099161d123d1c73e31042d4b6c25c0  128 bytes  307 us
#      pdjtoken <= pdjtoken::issue              {"to":"pdj","quantity":"100.00 PTK","memo":"tuhao"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

转账


```
bogon:pdjtoken yekai$ cleos push action pdjtoken transfer '["pdj","alice","25.00 PTK","meizi"]' -p pdj
executed transaction: e2b7f7276a75b26a6e983ba82146830974940e845527ea0b41690e8e2253e90d  136 bytes  340 us
#      pdjtoken <= pdjtoken::transfer           {"from":"pdj","to":"alice","quantity":"25.00 PTK","memo":"meizi"}
#           pdj <= pdjtoken::transfer           {"from":"pdj","to":"alice","quantity":"25.00 PTK","memo":"meizi"}
#         alice <= pdjtoken::transfer           {"from":"pdj","to":"alice","quantity":"25.00 PTK","memo":"meizi"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
```

查询ptk余额

```
bogon:contracts yekai$ cleos get currency balance pdjtoken alice PTK
125.00 PTK
```



创建任务

```
bogon:contracts yekai$ cleos push action pdjtask createtk '["alice","bob","10.00 PTK","first"]' -p alice bob
executed transaction: b95ae36174fd29d364b9020f7a0ad773636f40fe7e7b08a17405b3605bbf28da  152 bytes  431 us
#       pdjtask <= pdjtask::createtk            {"creator":"alice","worker":"bob","taskBonus":"10.00 PTK","memo":"first"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
```


查看任务列表

```
bogon:contracts yekai$ cleos get table pdjtask pdjtask  tasks
{
  "rows": [{
      "taskID": 0,
      "creator": "alice",
      "worker": "bob",
      "bonus": "10.00 PTK",
      "status": 0,
      "remark": "first",
      "comment": ""
    }
  ],
  "more": false
}

```

调用commit
```
bogon:contracts yekai$ cleos push action pdjtask commit '[0,"bob", "ok!"]' -p bob
executed transaction: 0142bade14c90f813c80589ebb6e04d010ecb77e95be8555bcc850a58dac2f7a  112 bytes  391 us
#       pdjtask <= pdjtask::commit              {"taskID":0,"worker":"bob","memo":"ok!"}
>> hi,pdjtask
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
bogon:contracts yekai$ cleos get table pdjtask pdjtask  tasks
{
  "rows": [{
      "taskID": 0,
      "creator": "alice",
      "worker": "bob",
      "bonus": "10.00 PTK",
      "status": 1,
      "remark": "first",
      "comment": "ok!"
    },{
      "taskID": 1,
      "creator": "alice",
      "worker": "pdj",
      "bonus": "10.00 PTK",
      "status": 0,
      "remark": "second",
      "comment": ""
    }
  ],
  "more": false
}

```


需要赋权操作，alice调用pdjtoken和pdjtask才不会有大问题
```
cleos set account permission alice active '{"threshold": 1,"keys": [{"key": "EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq","weight": 1}], "accounts": [ {"permission":{"actor":"pdjtoken","permission":"eosio.code"},"weight":1}] }' owner -p alice


cleos set account permission alice active '{"threshold": 1,"keys": [{"key": "EOS5J9qgnyDGnoiGrxddMKud1ox8DH3LMXuQvKLvhYKiz5DBqrfyq","weight": 1}], "accounts": [ {"permission":{"actor":"pdjtask","permission":"eosio.code"},"weight":1}] }' owner -p alice
```


调用confirm和结果查询
```
bogon:pdjtask yekai$ cleos push action pdjtask confirm '[0,1]' -p alice@active
executed transaction: 9f7b0d2dbb8a93631dfdbbc9b7a27a3fc3adf05b48c2757b560c237b5d5ba9ac  104 bytes  569 us
#       pdjtask <= pdjtask::confirm             {"taskID":0,"ok":1}
>> confirm---alice
#      pdjtoken <= pdjtoken::transfer           {"from":"alice","to":"bob","quantity":"10.00 PTK","memo":"very good!"}
#         alice <= pdjtoken::transfer           {"from":"alice","to":"bob","quantity":"10.00 PTK","memo":"very good!"}
#           bob <= pdjtoken::transfer           {"from":"alice","to":"bob","quantity":"10.00 PTK","memo":"very good!"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
bogon:pdjtask yekai$ cleos get currency balance pdjtoken bob
10.00 PTK

```



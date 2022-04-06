# 🌎 [cXc.world](https://cxc.world) Beta Pseudo-code 👨‍💻
Architecture for contracts related to cXc.world 🗺🎶 beta. 

## To Contribute to the pseudo-code
Simply open an issue with your ideas, or send pull requests. If you don't have a Github account, let us know what you see to be improved in the [Ideas + Feedback section](https://discord.gg/7MWFcYFjHz) of cXc's discord. 

**Find the contracts** 
- [ups.cxc](pseudo-contracts/ups.cxc) - Receives BLUX / Sol Ups / Big Ups and pays artists BLUX
- [soldisk.cxc](pseudo-contracts/soldisk.cxc) - Manages Solar Disk registration and records. 


## To contribute to smart contract development
Contact [Douglas Butner](mailto:douglas@cxc.world) if you'd like to be added to the private contract-development repo. Contributors to contract code will be paid in PURPLE based on their contribution manually on contract launch date. You can also elect to be paid a reduced rate in EOS, TLOS or WAX, or request a different compensation.


## Pseudo-code conventions used
```js

ACTION action_name(parameters, [needed environment_variable])
- One piece of actionable code

internal ACTION - Unexposed action AKA private function

TABLE [
  column_name
]

|table_name|
|table_name => column_name|
```
wordstogether and words_together both used (sorry not sorry)


## Cross-contract constants 

```js

TU = Internal Time Unit calculated from timestamp (All contracts)
AUTH_ACCOUNT = "currentxchng", "cxc", "[soldisk, ups,].cxc"

```

# [Learn more about cXc](https://linktr.ee/cxc.world)

> P.S. We're also working on our [geotribal system](https://github.com/dougbutner/web-4#geosocial-systems-geotribes--geodomains) contracts tribe.cxc, council.cxc, geo.cxc, stay tuned! 

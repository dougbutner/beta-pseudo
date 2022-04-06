# 🌎 [cXc.world](https://cxc.world) Beta Pseudo-code 👨‍💻
Architecture for contracts related to cXc.world 🗺🎶 beta. 

## To Contribute to the pseudo-code
Simply open an issue with your ideas, or send pull requests. If you don't have a Github account, let us know what you see to be improved in the [Ideas + Feedback section](https://discord.gg/7MWFcYFjHz) of cXc's discord. 

**Find the contracts** 
- [ups.cxc](pseudo-contracts/ups.cxc.js) - Receives BLUX / Sol Ups / Big Ups and pays artists BLUX
- [soldisk.cxc](pseudo-contracts/soldisk.cxc.js) - Manages Solar Disk registration and recharges


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

## Differences from Purple Explainer
We have had to make some changes for performance. Here's how it will work now.

> For more info, read the [Soldisk Documentation](https://github.com/currentxchange/purple-explainer/blob/master/Soldisk.md)

- Users must have a Soldisk (renamed from Solar Disk) with at least 1 PURPLE attached. 
- Levels now exist on Soldisks
- SOL will now be a regular eosio.token contract, so users can vote without the app. 
- This means users can trade SOL (unplanned) so we've added a requirement that 20% of SOL received must be used for Sol Ups or the user won't be able to claim more SOL 
- There is no 12-hour wait to claim, users can claim up to every Time Unit (5 minutes), and up to the maximum allowed by their level
- Low-level Solar Disks are allowed to claim 1 SOL per time unit passed (not based on  max charge). Next-claim reward amount will not increase after daily limit is reached. 
- Listeners will be be paid BLUX
- BLUX will be included in default top charts, and Blu Ups considered in reward algorithm, with limits
- Users will be able to trade Soldisk NFTs, but only register one at a time themselves

# [Learn more about cXc](https://linktr.ee/cxc.world)

> P.S. We're also working on our [geotribal system](https://github.com/dougbutner/web-4#geosocial-systems-geotribes--geodomains) contracts tribe.cxc, council.cxc, geo.cxc, stay tuned! 

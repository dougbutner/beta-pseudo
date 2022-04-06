// === Solar Disk Contract === \\
// Handles Sol Disk registration, charging

soldisk.cxc {
  LISTEN => transfer (SOL, BLUX)
  LISTEN => simpleassets:transfer (Soldisk)
  LISTEN => ups.cxc:updatetotal()

  ACTION chargedisk(simpleassetsID, [sender]) // Sends user SOL tokens based on time passed
  ACTION registerdisk(simpleassetsID, [sender]) // Associate max 1 Soldisk per account
  ACTION unregisterdisk(simpleassetsID, [caller]) // Remove account / Soldisk association
  ACTION update_blacklist(account, remove_true) // Add or remove accounts from blacklist, prevents charging Soldisk NFTs

  
  TABLE diskreg [
    account 
    disk_id - Simple Assets ID // Set to 0 when user calls unregisterdisk() to prevent cheating the 20% rule 
    level
    initiated - TU // Time unit the disk was registered
  ]
  
  TABLE rechargelog [
    disk_id 
    last_recharge - TU
    amount_recharged 
  ]
  
  TABLE blacklist [
    account 
    banner // What account banned this account
    reason // Optional text field explaining reason for ban
  ]
  
  TABLE ledger [
    account 
    solx_earned 
    solx_spent 
    updated - timestamp
    spent_tx_count // used to spot patterns indicative of bots
  ]

}//END soldisk.cxc 



// --- Actions --- \\
ACTION chargedisk (user)
- CHECK user has Solar Disk registered in TABLE soldiskreg
- CHECK user is not in 
- CHECK user's |ledger => solx_spent| / |ledger => solx_earned| ratio is over .2
- calculate the amount of TU passed since last recharge (find MAX by solar disk level)
- UPDATE |ledger => solx_earned| 
- TRANSFER SOLX

ACTION registerdisk(simpleassetsID, [caller])
// Activates a Solar Disk's functions by registering in the directory
- CHECK the diskid is a valid ID and of category solardisk
- if exists |diskreg => account| and diskid != diskid
  - UPDATE |diskreg => diskid| // Calling unregister_disk() is not needed
- if exists |diskreg => account| and diskid == diskid and level != level
  - UPDATE |diskreg => diskid|
- if exists |diskreg => diskid| // New level
  - UPDATE |diskreg => level|
- else 
- INSERT record |diskreg| 

LISTEN => simpleassets:transfer (Soldisk)
- call unregister_disk(simpleassetsID, [sender])

ACTION unregister_disk(simpleassetsID, [caller]) // Separate action easier for user
- CHECK (caller = |diskreg => account| OR caller = AUTH_ACCOUNT)
- UPDATE record |diskreg => disk_id| to 0 // This keeps the record in the table, so user can't avoid the 20% rule 

LISTEN => ups.cxc:updatetotal()
- if (record exists in |ledger|)
  - UPDATE record from |ledger|
  - else 
  - INSERT record into |ledger|  
  
ACTION update_blacklist(account, remove_true)
  - CHECK sender = AUTH_ACCOUNTs
  - if (remove = 1) DELETE record from |blacklist|
  - else
  - INSERT record into |blacklist|
  
  

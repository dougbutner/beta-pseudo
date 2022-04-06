// === Ups Contract === \\
// Takes all Ups, Pays artists, Maintains song and artist association + Band member lists


ups.cxc { 
  LISTEN => transfer (SOL, BLUX)
  ACTION payup(account) // Send payments listed in |ious|  
  ACTION updateartist(account, googleid) // Register artist, or change artist information
  ACTION updateartistgroup({members},{permissions}) // Register artist group, or change artist information
  ACTION updatesong(songID, {info}) // Who gets paid for each cXc post, by songid (internal cXc.world id)

  internal ACTION - updateup(ups_count, ups_type, [caller]) // Checks + calls logup() updateiou() and updatetotal()
  internal ACTION - logup(ups_count, ups_type, method, [caller]) // Store persistent record of UP in |ups| WARNING: consider performance
  internal ACTION - removeup(account [caller]) // Will remove blacklisted user's ups retroactively
  internal ACTION - updatetotal(songid) // keep single-row record of ups for each song
  internal ACTION - updateiou(sending_account, receiving_account) // Makes sure people get paid
  internal ACTION - removeiou([caller]) // All IOUS are removed from table, for performance
  
  
// --- Tables --- \\
  TABLE ups [ // Records each and every up (All types)
    account
    account_type
    song (songid)
    ups_type (0,1,2,3)(Solx Up, Blux Up, Sol with Big Ups)
    ups_count
    method // Set if this UP is from cXc.world or another MAY NEED NONCE
    tuid 
  ]
  
  TABLE totals [
    song
    ups_type
    ups_count
    updated
  ]
  
  TABLE ious [
    sending_account // Sent Ups
    receiving_account // Owed tokens 
    receiving_account_type // group or artist OR not needed if we check if account is valid + reg
    ups_type // SOL or BLUX
    ups_count 
    initiated // When the IOU was registeredtimestamp (internal, no need for TU)
  ]
  
  // TABLE ious_log [] REMOVED because it will get bloated. 
  
  TABLE songs [
    songID // From 
    recipient // Linked account or group, probably will be a name_n
    recipient_type - group or solo
    song_info{}
  ]
  
  TABLE artists [
    account
    googleid
    name
    artist_info {}
  ]

  TABLE artistgroups [
    groupname
    artists {}
    weights {} // Weights are from 1-12 and determine how much each person gets every time around the wheel
    artist_info {}
    pay_position // 
  ]
  
  TABLE internal_records [
    last_pay
    remaining payment 
  ]
}

// --- Sol.md actions --- \\
// For arguments, see above 

LISTEN => transfer ([SOLX / BLUX])
- IF (no memo / incorrect format) THEN return payment
- Call action updateup()
- IF 

logup()
- IF (record exists for TU)
  - UPDATE |ups| where TU = TU & account = account
  - else
  - INSERT record of Up into |ups|


updateup()
- IF (method == ui (0)) check (sender == AUTH_ACCOUNTs) ELSE method = contract (1)
- call ACTION updatetotal()
- call ACTION logup()
- call ACTION updateiou()


removeup()
- IF caller == account
  - DELETE record from |ups| where (account == account )
  - UPDATE record from |totals|
  - call updatetotal()


updatetotal()
- if (record exists in |totals|)
  - UPDATE record from |totals|
  - else 
  - INSERT record into |totals|  
- CONTRACT soldisk.cxc is notified and calls LISTEN => ups.cxc:updatetotal()


updateiou(sender, reciever, type, amount)
- IF token=purple CHECK (contract == approved_contracts)
- if (record exists in |ious|)
  - UPDATE record from |ious|
  - else 
  - INSERT record into |ious|


removeiou(sender, reciever, type, amount)
- DELETE record from |ious| 


payup(account, type = personal) // type can also be 'group'
- if (account = undefined) account = all_accounts
- if (account = all_accounts) 
    - CHECK (account = AUTH_ACCOUNT)

- READ the |ups.cxc => ious| table for account
  - Make [] with each record of owed to depth of 12 rows, starting with oldest

- if (account is group)
  - READ |artistgroups => pay_position|
  - Compile [] of the members owed by counting off each position and weight until all reward is given out
  
- TRANSFER the total sum owed to account[s]
- UPDATE / DELETE |ups.cxc => ious| table to reflect changes 


updateartist(caller)
- IF (exists |artists => account|)
- UPDATE |artists => artist_info|
- else 
- INSERT record into |artists|


updateartistgroup()
- CHECK (artists.length = weights.length OR 0 weights) // 0 = all the same
- IF (exists |artists => account|)
- UPDATE |artistgroups => artist_info|)
- else 
- INSERT |artistgroups|


updatesong(songid, info)
- CHECK (sender = artist || sender = AUTH_ACCOUNT)
  - if (group) CHECK (sender is in |artistgroups|)
- IF (exists |so => account|)
- UPDATE |songs = songid|)
- else 
- INSERT |songs|

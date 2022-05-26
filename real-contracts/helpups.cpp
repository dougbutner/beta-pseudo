/*/ -- In case you need an inline action -- \\
action(
  permission_level{get_self(), name("active")}, // CHECK why active? Is this bad?
  get_self(),
  name("lognewtempl"),
  make_tuple(
      aatemplateid,
      authorized_creator,
      collection_name,
      schema_name,
      transferable,
      burnable,
      max_supply,
      immutable_data
  )
).send();
SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
/*/


/*/
ACTION updateup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); 
ACTION logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); 
ACTION removeups(name user); 
ACTION updatetotal(uint32_t upscount, uint8_t upstype, uint8_t method_sent, name upsender); 
ACTION updateiou(name sender, name receiver, uint32_t amount, uint32_t songid, bool subtract); 
ACTION removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
ACTION updatelisten(uint32_t upscount, uint8_t upstype, uint8_t method_sent, name upsender);
ACTION removelisten(name upsender);
ACTION removesong(uint32_t songid); //TODO add to pseudo-code, removes all IOUs for song, song
/*/

// === Helper functions === \\
// --- Upsert _listeners and _totals --- \\
void upsert_total(uint32_t &upscount, uint8_t upstype, name &upsender, uint32_t &songid) {
  require_auth( upsender );
  _totals(get_self(), songid;
  auto total_iterator = _totals.find(upsender.value);
  if( iterator == _totals.end() )
  {
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = songid;
      row.upstype = upstype;
      row.upscount = upscount;
      row.updated = eosio::time_point_sec::sec_since_epoch() ;
    });
  } 
  else 
  {
    _totals.modify(iterator, upsender, [&]( auto& row ) {
      row.key = songid;
      row.upstype = upstype;
      row.upscount += upscount;
      row.updated = eosio::time_point_sec::sec_since_epoch() ;
    });
  }//END if(results)
}

// --- Store persistent record of UP in |ups| --- \\
ACTION ups::logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid) {
  // IF (record exists for TU)
    // UPDATE |ups| where TU = TU & account = account
    // --- else
    // INSERT record of Up into |ups|
    
}

// --- Will remove blacklisted user's ups retroactively --- \\
ACTION ups::removeups(name user) {
  // IF caller == account
    // DELETE record from |ups| where (account == account )
    // UPDATE record from |totals|
    // call updatetotal()
    
}

// --- Single-row record of ups for each song --- \\
ACTION ups::updatetotal(uint32_t &upscount, uint8_t upstype, name &upsender, uint32_t &songid) {
  
  uint32_t songid_upped = songid;
  
  
  // --- Check if record in TOTAL exists --- \\
  // --- Instantiate Table --- \\
  _totals(_self, _self.value);
  
  // --- Upsert record in TOTALS --- \\ 

  
  
  // if (record exists in |totals|)
    // UPDATE record from |totals|
    // else 
    // INSERT record into |totals|  
  // CONTRACT soldisk.cxc is notified and calls LISTEN => ups.cxc:updatetotal()
  
}

// --- Makes sure people get paid --- \\
ACTION ups::updateiou(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid, bool subtract) {
  //NOTE all PURPLE IOUs are now stored on a Charts contract
  // if (record exists in |ious|)
    // UPDATE record from |ious|
    // else 
    // INSERT record into |ious|
    
}

// --- All IOUS are removed from table once paid --- \\
ACTION ups::removeiou(name sender, name receiver) {
  // if (receiver = dummy_value && sender = dummy_value)
    // return (failed) 
  // if (receiver = dummy_value)
  // DELETE all records from |ious| where sender = sender
  // else if (sender = dummy_value)
  // DELETE all record from |ious| where reciever = reciever
  // else 
  // DELETE records from |ious| where reciever = reciever && sender = sender 
  
}

// --- Keep track of total account amounts for ALL users --- \\
ACTION ups::updatelisten(uint32_t upscount, uint8_t upstype, uint8_t method_sent, name upsender) {
  // CHECK (caller = AUTH_ACCOUNT)
  // UPDATE record from |listeners|  
  
}

// --- Remove record of user in event of blacklisting --- \\
ACTION ups::removelisten(name upsender) {
  // CHECK (caller = AUTH_ACCOUNT)
  // DELETE record from |listeners|  
  
}

// --- DIPATCHER ACTION Checks + calls logup() updateiou() and updatetotal() --- \\
ACTION ups::updateup(uint32_t &upscount, uint8_t &upstype, name &upsender, uint32_t songid) {  
  // --- Calls action to update the TOTALS table -- \\
  ups::updatetotal(upscount, upstype, upsender, songid);  //WARN CHECK may be better to just to the upsert function
  
  
  // --- Log the ups in UPSLOG table --- \\ 
  ups::logup(upscount, upstype, upsender, songid);
  
  // --- Record the Up to be paid via IOUS table --- \\
  ups::updateiou(upscount, upstype, upsender, songid, 0);
}

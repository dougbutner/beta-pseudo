/*/ -- In case you need an inline action -- \\
action(
  permission_level{get_self(), name("active")}, // CHECK why active? Is this bad?
  get_self(),
  name("lognewtempl"),
  make_tuple(
      template_id,
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
ACTION updateup(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid); 
ACTION logup(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid); 
ACTION removeups(name user); 
ACTION updatetotal(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name up_sender); 
ACTION updateiou(name sender, name receiver, uint32_t amount, bool subtract); 
ACTION removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
ACTION updatelisten(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name up_sender);
ACTION removelisten(name up_sender);
ACTION removesong(uint32_t songid); //TODO add to pseudo-code, removes all IOUs for song, song
/*/


// --- Store persistent record of UP in |ups| --- \\
ACTION ups::logup(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid) {
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
ACTION ups::updatetotal(uint32_t &ups_count, uint8_t ups_type, name &up_sender, uint32_t &songid) {
  
  // --- Check if record in TOTAL exists --- \\
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for record in TOTALS --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song " + to_string(songid_upped) + " was not found." ).c_str() );

  
  
  // if (record exists in |totals|)
    // UPDATE record from |totals|
    // else 
    // INSERT record into |totals|  
  // CONTRACT soldisk.cxc is notified and calls LISTEN => ups.cxc:updatetotal()
  
}

// --- Makes sure people get paid --- \\
ACTION ups::updateiou(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name up_sender, bool subtract) {
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
ACTION ups::updatelisten(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name up_sender) {
  // CHECK (caller = AUTH_ACCOUNT)
  // UPDATE record from |listeners|  
  
}

// --- Remove record of user in event of blacklisting --- \\
ACTION ups::removelisten(name up_sender) {
  // CHECK (caller = AUTH_ACCOUNT)
  // DELETE record from |listeners|  
  
}

// --- DIPATCHER ACTION Checks + calls logup() updateiou() and updatetotal() --- \\
ACTION ups::updateup(uint32_t &ups_count, uint8_t &ups_type, name &up_sender, uint32_t songid) {  
  // --- Calls action to update the TOTALS table -- \\
  ups::updatetotal(ups_count, ups_type, up_sender, songid);  
  
  // --- Log the ups in UPSLOG table --- \\ 
  ups::logup(ups_count, ups_type, up_sender, songid);
  
  // --- Record the Up to be paid via IOUS table --- \\
  ups::updateiou(ups_count, ups_type, up_sender, songid, 0);
}

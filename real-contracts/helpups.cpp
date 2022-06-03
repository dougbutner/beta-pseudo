/*/
WORKING ISSUES 

We may need to remove all the extranious updaters to move to a standard function. 
- Avoids spinning up a new environment to execute 

/*/


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

// --- I always forget this shit --- \\
multi_index_example( name receiver, name code, datastream<const char*> ds )
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

// --- Returns the current Time Unit --- \\
uint32_t find_tu(uint32_t &momentuin){
  // 1561139700 is the first Time Unit
  uint32_t time_unit = floor((momentuin - 1561139700) / 300);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- \\
uint32_t find_tu(void){
  uint32_t momentuin = eosio::time_point_sec::sec_since_epoch();
  uint32_t time_unit = floor((momentuin - 1561139700) / 300);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// === Upserterterses === \\
// --- Update running log of ups --- \\
void upsert_logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid){
  require_auth( upsender );
  uint32_t momentu = find_tu();
  
  // --- Sift Ups by Type (Requires upstype, Defines newxxxups)--- \\ 
#include "upsifter.cpp"
  
  // --- Add record to _upslog --- \\
  _upslog(get_self(), songid); //WARN CHECK - is songid right here? URGENT I really think this is wrong for all upserts
  auto upslog_iterator = _upslog.find(upid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( upslog_iterator == _upslog.end() )
  { // -- Make New Record
    _upslog.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.key = songid;
      row.totalsolups = newsolups;
      row.totalbluups = newbluups;
      row.totalbigups = newbigups;
      row.tuid = momentu;
    });
  } 
  else 
  { // -- Update Record
    _upslog.modify(upslog_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.key = songid;
      row.totalsolups += newsolups;
      row.totalbluups += newbluups;
      row.totalbigups += newbigups;
      //row.tuid = momentu; // This should be the same, we shouldn't be updating old TUs. May consider leaving in for mid-sec tx??
    });
  }//END if(results _upslog) 
}

// --- Upsert _listeners and _totals --- \\
void upsert_total(uint32_t &upscount, uint8_t upstype, name &upsender, uint32_t &songid) {
  require_auth( upsender );

  // --- Sift Ups by Type (Requires upstype, Defines newxxxups)--- \\
#include "upsifter.cpp"
  
  // --- Update _totals record of cumulative song Ups --- \\
  _totals(get_self(), songid);
  auto total_iterator = _totals.find(songid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( total_iterator == _totals.end() )
  { // -- Make New Record
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = songid;
      row.totalsolups = newsolups;
      row.totalbluups = newbluups;
      row.totalbigups = newbigups;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record
    _totals.modify(total_iterator, upsender, [&]( auto& row ) {
      row.key = songid;
      row.totalsolups += newsolups;
      row.totalbluups += newbluups;
      row.totalbigups += newbigups;
      row.updated = time_of_up;
    });
  }//END if(results _totals)

  // --- Update _listeners record --- \\
  _listeners(get_self(), songid);
  auto listener_iterator = _listeners.find(songid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( listener_iterator == _listeners.end() )
  {
    _listeners.emplace(upsender, [&]( auto& row ) {
      row.key = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.totalsolups = newsolups;
      row.totalbluups = newbluups;
      row.totalbigups = newbigups;
    });
  } 
  else 
  {
    _listeners.modify(listener_iterator, upsender, [&]( auto& row ) {
      row.key = upsender;
      row.firstup = upstype;
      row.lastup = time_of_up;
      row.totalsolups += newsolups;
      row.totalbluups += newbluups;
      row.totalbigups += newbigups;
    });
  }//END if(results _listeners)
}//END upsert_total()

// --- Upsert IOUs --- \\
void upsert_ious(uint32_t upscount, uint8_t upstype, name &upsender, uint32_t songid, bool subtract){
  require_auth( upsender );
  
  // --- Determine Artist + Artist Type --- \\
  _songs(get_self(), _self); //WARN CHECK - is songid right here? URGENT I really think this is wrong for all upserts
  auto songs_iterator = _songs.find(songid);
  check(songs_iterator != _songs.end(), "No song exists with this ID"); 
  name artistacc = songs_iterator->artistacc;
  name artisttype = songs_iterator->artisttype;

  // --- Sift Ups by Type (Requires upstype, Defines newxxxups)--- \\ WARN may not be needed, we know typr in IOUs
#include "upsifter.cpp"
  
  // --- Add record to _upslog --- \\
  _ious(get_self(), upsender);
  
  /*/!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Where does the iouid come from?? 
  iouid should be compsed of Songid and Tuid values
  Remember conversation in TG where was told we can simply combine these 
  Bitshift two 32ts into a 64t or else use 128t
  /*/
  
  
  auto ious_iterator = _ious.find(iouid); 
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  //TODO - May need to deal with the type of up, as it could be Big, and we update.. so...
  if( ious_iterator == _upslog.end())
  { // -- Make New Record
    _ious.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.key = _upslog.end(); //URGENT check this CHANGE to bit shifted combnation of values 
      row.upsender = upsender;
      row.upcatcher = artistacc;
      row.artisttype = artisttype;
      row.upscount = newsolups;
      row.upstype = SOL;
      row.initialized = time_of_up;
      row.updated = time_of_up;
    });
    // --- Insert Big Up IOUs --- \\ This is new concept needed to reward Big Ups in a special way, intended to not get convoluted
    if(newbigups > 0){
      _ious.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = _upslog.end(); //URGENT check this
        row.upsender = upsender;
        row.upcatcher = artistacc;
        row.artisttype = artisttype;
        row.upscount = newbigups;
        row.upstype = BIG;
        row.initialized = time_of_up;
        row.updated = time_of_up;
      });
    }//END if(newbigups)
  } 
  else 
  { // -- Update Record
    _ious.modify(ious_iterator, upsender, [&]( auto& row ) {
      row.upscount += upscount;
      //row.upstype = upstype;//Should not change, now we use different type
      row.updated = time_of_up;
      //row.tuid = momentu; // This should be the same, we shouldn't be updating old TUs. May consider leaving in for mid-sec tx??
    });
    // --- Update Big Up IOUs --- \\ 
    if(newbigups > 0){
      _ious.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = _upslog.end(); //URGENT check this
        row.upsender = upsender;
        row.upcatcher = artistacc;
        row.artisttype = artisttype;
        row.upscount += newbigups;
        row.upstype = BIG;
        row.updated = time_of_up;
      });
    }//END if(newbigups)
  }//END if(results _upslog)
}//END upsert_ious()

// --- Will remove blacklisted user's ups retroactively --- \\
ACTION ups::removeups(name user) {
  // IF caller == account
    // DELETE record from |ups| where (account == account )
    // UPDATE record from |totals|
    // call updatetotal()
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
  //ups::updatetotal(upscount, upstype, upsender, songid);  //WARN CHECK may be better to just to the upsert function
  upsert_total(upscount, upstype, upsender, songid);
  
  // --- Log the ups in UPSLOG table --- \\ 
  //ups::logup(upscount, upstype, upsender, songid);
  upsert_logup(upscount, upstype, upsender, songid);
  
  // --- Record the Up to be paid via IOUS table --- \\
  //ups::updateiou(upscount, upstype, upsender, songid, 0);
  upsert_iou(upscount, upstype, upsender, songid, 0);
}

#include "bluxbluxblux.cpp"
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

// --- Get Songid from iouif (concat of tuid + songid) --- \\
uint32_t iouid_to_songid(uint32_t iouid){
  uint32_t songid = (uint32_t) iouid;
  return songid;
}

// --- Get Tuid from iouif (concat of tuid + songid) --- \\
uint32_t iouid_to_tuid(uint32_t iouid){
  uint32_t tuid = (uint32_t iouid>>32);
  return tuid;
}

//uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) songid;

// === Pay + Mint NFTs === \\ 
// --- Pay BLUX to the SOL recipients --- \\

// --- Overload to accept Integer types --- \\
void send_blux( const name&    from,
                const name&    to,
                const uint32_t  quantity, 
                const string&  memo)
{
  
  string blux_string = "BLUX"
  symbol blux_symbol = eosio::symbol_code::symbol_code(blux_string);
  asset blux_asset = eosio::asset::asset( int64_t quantity, class symbol blux_symbol );
  
  send_blux(from, to, blux_asset, memo);
}//END unit-accepting overloaded send_blux()
//CHECK set permission eosio.code on the BLUX contract
void send_blux( const name&    from,
                const name&    to,
                const asset&   quantity, 
                const string&  memo)
{
  // --- Check that this contract is the caller  --- \\
  require_auth(get_self());
  //require_recipient(to);
  if (to != get_self() || from == get_self() || quantity::amount < 0)//CHECK last quantity::amount is correct
  {
      return;
  }
  
  action(
      permission_level{ get_self(),"active"_n },
      get_self,
      "transfer"_n,
      std::make_tuple(get_self(), from, to, quantity, memo)
  ).send();
}//END send_blux()

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
  require_auth( upsender ); //CHECK if this breaks call from payup for groups
  
  // --- Determine Artist + Artist Type --- \\
  _songs(get_self(), get_self().value); //WARN CHECK - is songid right here? URGENT I really think this is wrong for all upserts
  auto songs_iterator = _songs.find(songid);
  check(songs_iterator != _songs.end(), "No song exists with this ID"); 
  name artistacc = songs_iterator->artistacc;//CHECK does this need to be .value()?
  name artisttype = songs_iterator->artisttype;
  
  // --- Create the shifted iouid value --- \\
  uint32_t momentu = find_tu();
  uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) songid;

  // --- Sift Ups by Type (Requires upstype, Defines newxxxups)--- \\ WARN may not be needed, we know type in IOUs
#include "upsifter.cpp"
  
  /*/ --- iouid explanation -- \\
    Where does the iouid come from?? 
    iouid is a bit-combo of Songid and Tuid 
    uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) songid;
  /*/
  
  // --- Add record to _upslog --- \\
  _ious(get_self(), upsender); //WARN this should probably be a contract scope
  auto ious_itr = _ious.find(iouid); 
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  //TODO - May need to deal with the type of up, as it could be Big, and we update.. so...
  if( ious_itr == _upslog.end())
  { // -- Make New Record
    _ious.emplace(upsender, [&]( auto& row ) {
      row.key = iouid; 
      row.upsender = upsender;
      row.upcatcher = artistacc;
      row.artisttype = artisttype;
      row.upscount = newsolups;
      row.upstype = upstype; // This should be SOL (1 or BIGSOL)
      row.initialized = time_of_up;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record
    _ious.modify(ious_itr, upsender, [&]( auto& row ) {
      row.upscount += upscount;
      row.upstype = upstype;// Can be from SOL -> BIGSOL
      row.updated = time_of_up;
      //row.tuid = momentu; // This should be the same, we shouldn't be updating old TUs. May consider leaving in for mid-sec tx??
    });
  }//END if(results _upslog)
}//END upsert_ious()



// --- Upsert Groups --- \\
void upsert_groups(string &groupname, name &intgroupname, vector<name> artists, vector<int8_t> weights, uint8_t payposition){ //NOTE payposition may be filled with dummy value of 9999 or greater
  //require_auth( upsender ); //CHECK if this breaks call from payup for groups
  
  // --- Weights and Artists must be same length --- \\
  check(artists.length == weights.length , "Artists and Weights lists must be the same length.");
  check(artists.length < 13, "Groups can have a maximum of 12 artist accounts.");
  
  // --- Artists Must be Real Accounts --- \\
  bool has_authorized_caller = false;
  name authorized_caller;
  for(itr_artists = 0, itr_artists < artists.length, itr_artists++ ){
    check(is_account(artists[itr_artists]), string(to_string(artists[itr_artists])) + " is not a registered account." );
    // --- Creator must be one of group members --- \\
    if (has_auth(artists[itr_artists])){
      has_authorized_caller = true;
      authorized_caller = artists[itr_artists];
    }
  }//END for(itr_artists)
  
  // --- Verify the groupname is unique --- \\
  if(payposition > 9998){ // Flag for OG registration of the group
    //NOTE I don't think we need to do this anymore, as the TX will fail because it is primary key
  }
  
  //TODO --- Allow for contract-initiated calls --- \\
  // Set authorized_caller, truthify has_authorized_caller
  
  // --- Weights must follow cXc limits --- \\
  for(itr_weights = 0, itr_weights < weights.length, itr_weights++ ){
    check( weights[itr_weights] < 13 , string("All weights must be whole numbers, 1-12"));
  }
  
  // --- Deal with payposition --- \\
  if(payposition > 9998){ // Failsafe for int overflow, also dummy value for user-initiated calls
    payposition = 0;
  } else { // Call is internal
    require_auth(get_self());
  }
  
  // --- Add record to _groups --- \\
  check(has_authorized_caller, string("Only members of the group may update group information"));
  _groups(get_self(), get_self().value);
  auto groups_itr = _groups.find(groupname.value); 
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  //TODO - May need to deal with the type of up, as it could be Big, and we update.. so...
  if( groups_itr == _groups.end())
  { // -- Make New Record
    _groups.emplace(authorized_caller, [&]( auto& row ) {
      row.key = intgroupname; 
      row.groupname = groupname;
      row.artists = artists;
      row.weights = weights;
      row.payposition = payposition;
    });
  } 
  else 
  { // -- Update Record
    _groups.modify(groups_itr, get_self(), [&]( auto& row ) {
      row.groupname = groupname;
      row.artists = artists;
      row.weights = weights;
      row.payposition = payposition;
    });
  }//END if(results _upslog)
}//END upsert_group()

// === Deleters === \\
// --- Update running log of ups --- \\

// --- Will remove blacklisted user's ups retroactively --- \\
void removeups(name user) {
  require_auth(get_self());
  // --- Instantiate the ups + totals tables --- \\ 
  
  // --- 
  
  
    // DELETE record from |ups| where (account == account )
    // UPDATE record from |totals|
    // call updatetotal()
} 

/*/ OLD VERSION OF ABOVE --- Will remove blacklisted user's ups retroactively --- \\
ACTION ups::removeups(name user) {
  // IF caller == account
    // DELETE record from |ups| where (account == account )
    // UPDATE record from |totals|
    // call updatetotal()
} /*/


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

// --- Keep track of total account amounts for ALL users --- \\ CHANGE TO UPSERT
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
  //DEP ups::updatetotal(upscount, upstype, upsender, songid);  //WARN CHECK may be better to just to the upsert function
  upsert_total(upscount, upstype, upsender, songid);
  
  // --- Log the ups in UPSLOG table --- \\ 
  //DEP ups::logup(upscount, upstype, upsender, songid);
  upsert_logup(upscount, upstype, upsender, songid);
  
  // --- Record the Up to be paid via IOUS table --- \\
  //DEP ups::updateiou(upscount, upstype, upsender, songid, 0);
  upsert_iou(upscount, upstype, upsender, songid, 0);
}

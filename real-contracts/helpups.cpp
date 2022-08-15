#include "bluxbluxblux.cpp"
/*/
WORKING ISSUES 

We may need to remove all the updaters to move to a standard function. 
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

// --- Get Songid from iouid (concat of tuid + songid) --- \\
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
void upsert_logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)
  
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
    if (negative){
      _upslog.modify(upslog_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = songid;
        row.totalsolups -= newsolups;
        row.totalbluups -= newbluups;
        row.totalbigups -= newbigups;
        //row.tuid = momentu; // This should be the same, we shouldn't be updating old TUs. May consider leaving in for mid-sec tx??
      });
    } else {
      _upslog.modify(upslog_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = songid;
        row.totalsolups += newsolups;
        row.totalbluups += newbluups;
        row.totalbigups += newbigups;
        //row.tuid = momentu; // This should be the same, we shouldn't be updating old TUs. May consider leaving in for mid-sec tx??
      });
    }

    
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


// --- DIPATCHER Checks + calls logup() updateiou() and updatetotal() --- \\
void updateup(uint32_t &upscount, uint8_t &upstype, name &upsender, uint32_t songid, bool negative) {  
  if (!negative){
    // --- Log the ups in UPSLOG table --- \\ 
    upsert_logup(upscount, upstype, upsender, songid, 0);
    
    // --- Calls action to update the TOTALS table -- \\
    upsert_total(upscount, upstype, upsender, songid);
  }
  
  // --- Record the Up to be paid via IOUS table --- \\
  upsert_iou(upscount, upstype, upsender, songid, negative);
}

// --- Sends BLUX, clears IOUS. Called from overloaded payup() --- \\
void payupsender(name upsender){
     // --- Min Wait 5 seconds since last full pay payment --- \\
    uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
    _internallog(get_self(), get_self().value);
    check(_internallog.get().primary_key() < (time_of_up + 4), "Please wait 5 seconds between each payup. ");
    
    // --- Get oldest IOUs from _ious Table --- \\
    _ious(get_self(), upsender.value);//CHECK WARN scope
    auto ious_itr = _ious.get_index("initiated"_n)
    
    // --- Check if Groups exist in list to be paid --- \\ CHECK: Remove this from single-payer
    bool groupies = false;
    std::vector<name> foundgroups; //CHECK are we still using this 
    struct DemBoiz {
      name intgroupname,
      string groupname,
      vector<int8_t> artists,
      vector<int8_t> weights
    };
    
    // --- Check the next 12 entries for Groups --- \\
    for ( auto itr_g_check = ious_itr.rbegin(); itr_g_check >= ious_itr.rbegin() - 12; itr_g_check++ ) {//CHECK should ious_itr really be the table?
      if (uint8_t ious_itr->artisttype == 2){
        groupies = true;
        // Pass Groupname, Artists, weights upcatcher to 
      }
      //Pay Position is out of cumulative Artists*weight 
      //Weight is integer between 1-12
    }//END group check
    

    if (groupies) {// Instantiate the _groups table
      _groups(get_self(), get_self().value);
    }
    
    
    for ( auto itr_12 = ious_itr.rbegin(); itr_12 >= ious_itr.rbegin() - 12; itr_12++ ) {//CHECK (optimize/test) Goes 12 rows deep to avoid failed TX 
     // --- Build Memo --- \\
     uint32_t songid = iouid_to_songid(ious_itr->iouid);
     string memo = string("BLUX pay for cxc.world/" + to_string(songid) + " ");
     
     
     if(ious_itr->artisttype == 1) // 1=solo, 2=group
     {
       // --- Send Solo Artist BLUX --- \\ 
       send_blux(to, ious_itr->upcatcher, quantity, memo);//EXPLAIN To = old from (this contract) WORKING (Add to FRESH)
     } else {
       // === Pay Group of Artists === \\
      
       // --- Add group's Readable name to the memo --- \\ TODO refactor using string(memo + memo2)
       memo = string(memo + to_string(ious_itr->intgroupname); //CHECK to_string is used correctly
       
       // --- Get Artists and Weights --- \\
       auto groups_itr = _groups.require_find(ious_itr->upcatcher); //CHECK that the _require won't cause transactions to fail
       
       auto remaining_ups = ious_itr->upscount;
       auto last_position = groups_itr->payposition;
       auto current_position = 0;
       auto total_positions = std::accumulate(groups_itr->weights.begin(), groups_itr->weights.end(), 0);
       bool pay_started = false;

       // === Make payments + update table === \\
       // --- Figure out what # in groupmembers vector to pay by determining position --- \\
       for(int itr_g_members = 0, groups_itr->weights.size(), itr_g_members++){
         if(remaining_ups > 0){
          // --- Figure out who to pay --- \\
          uint32_t artist_first_payable_pos = 0;
          if (last_position < 1 || pay_started == false){
            // If less, No need to check the position
            auto checked_pay_position = 0;
            for (int itr_artist_position = 0, itr_artist_position +1 >= groups_itr->weights.size(), itr_artist_position++){ // CHECK +1 is correct (0-based array)
              
              checked_pay_position += groups_itr->weights[itr_g_members];
              //artist_position = itr_artist_position;
              
              // --- Check Weight (BLUX owed per full-circle) --- \\
              if ((checked_pay_position + groups_itr->weights[itr_g_members + 1]) <= remaining_ups){// This is the person to be paid
                artist_first_payable_pos = itr_artist_position;
                pay_started = true;
                break;
              }//END position check <= remaining_ups
              
            }//END for(itr_artist_position)  
          }//END if (last_position > 1)
            
            // --- Loop starting at the artist_first_payable_pos --- \\
            check(pay_started, "Group payments failed");
            for(int itr_g_paying_pos = artist_first_payable_pos, remaining_ups < 1, itr_g_paying_pos++){ /*itr_g_paying_pos >= (groups_itr->weights.size() + artist_first_payable_pos) ||*/
              if (itr_g_paying_pos == groups_itr->weights.size()){
                itr_g_paying_pos = 0; // Loop back to first recipient
              } 
              
              if (remaining_ups > 0 && remaining_ups < 99999999999){              
                // --- Determine Max + Real Pay by Weight --- \\ 
                auto artist_paid = groups_itr->artists[artist_first_payable_pos];
                auto real_payment = (remaining_ups >= groups_itr->weights[itr]) ? groups_itr->weights[itr] : remaining_ups;
                
                // --- Send Group Member BLUX --- \\ 
                send_blux(to, artist_paid, real_payment, memo);
                remaining_ups -= real_payment;
              }
              
              if(remaining_ups < 1 || remaining_ups > 99999999){ // It's the last payment
                // --- Update the table with new Payposition --- \\ 
                
                // --- Check for song in table --- \\ 
                auto groups_iter = _groups.require_find( ious_itr->intgroupname, string( "Group " + to_string(ious_itr->intgroupname) + " was not found when we tried to pay." ) );
                
                // --- Update Pay Position --- \\
                _groups.modify(groups_iter, get_self(), [&]( auto& row ) {
                  row.payposition = payposition;
                });
                
              }//END table update if (remaining_ups < 1)        
            
            }//END for(itr_g_paying_pos)

         } else {// CHECK: Is this position correct? (things moved) Dammit Charles, why don't you have any money? 
           break;
         }
       }//END for(itr_g_members)
     }
    
     if (ious_itr->upstype ==  BIGSOL){
       // --- How many Big Ups --- \\
       auto big_ups_count = floor(ious_itr->upscount / 64);
       // --- Mint NFTs for Big Ups to the SENDER --- \\
     }//END if (ious_itr->upstype == BIGSOL)
   }//END for (12 IOUs)
}//END final payupsender()

#include "bluxbluxblux.cpp"
/*/
WORKING ISSUES 

We may need to remove all the updaters to move to a standard function. 
- Avoids spinning up a new environment to execute 

/*/

/*/ -- In case you need an inline action -- //
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

// --- I always forget this shit --- //
multi_index_example( name receiver, name code, datastream<const char*> ds )
/*/


/*/
  void updateup(uint32_t upscount, name upsender, uint32_t content_id); //DISPATCHER
  void logup(uint32_t upscount, name upsender, uint32_t content_id); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t upscount, name upsender);
  void removelisten(name upsender);
  void removesong(uint32_t content_id); // Removes all IOUs for song + song record (minimal)
  void deepremvsong(uint32_t content_id); // Removes all records of Ups for this sond
  void updateartist(name artist_wax, vector<string> artistinfo, string artistalias); 
upsert_total(upscount, upstype, upsender, content_id, negative);
/*/

// === Helper functions === //

// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t &momentuin){
  // 1561139700 is the first Time Unit
  uint32_t time_unit = floor((momentuin - 1561139700) / 300);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- //
uint32_t find_tu(void){
  uint32_t momentuin = eosio::time_point_sec::sec_since_epoch();
  uint32_t time_unit = floor((momentuin - 1561139700) / 300);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Get content_id from iouid (concat of tuid + content_id) --- //
uint32_t iouid_to_content_id(uint32_t iouid){
  uint32_t content_id = (uint32_t) iouid;
  return content_id;
}

// --- Get Tuid from iouif (concat of tuid + content_id) --- //
uint32_t iouid_to_tuid(uint32_t iouid){
  uint32_t tuid = (uint32_t iouid>>32);
  return tuid;
}


// --- Check authorization of an artist --- //
//void cxcheck_auth()

//uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) content_id;

// === Pay + Mint NFTs === //


// --- Overload to accept Integer types --- //
void send_blux( const name&    from,
                const name&    to,
                const uint32_t  quantity, 
                const string&  memo,
                const uint32_t& content_id
              )
{
  
  string blux_string = "BLUX";
  symbol blux_symbol = eosio::symbol_code::symbol_code(blux_string);
  asset blux_asset = eosio::asset::asset( int64_t quantity, class symbol blux_symbol );
  
  send_blux(from, to, blux_asset, memo, content_id);
}//END unit-accepting overloaded send_blux()

  void send_blux( //CHECK set permission eosio.code on the BLUX contract
      const name&    from,
      const name&    to,
      const asset&   quantity, 
      const string&  memo,
      const uint32_t& content_id
  ){
  // --- Check that this contract is the caller  --- //
  require_auth(get_self());
  //require_recipient(to);
  if (to != get_self() || from == get_self() || quantity.amount < 0 || quantity.amount > 99999999)//CHECK last quantity::amount is correct
  {
    return;
  }
  
  action(
      permission_level{ get_self(),"active"_n },
      get_self,
      "transfer"_n,
      std::make_tuple(get_self(), from, to, quantity, memo)
  ).send();
  
  // --- Pass on to updateup() to remove IOUs with 1 to flag removal --- //
  updateup(quantity.amount, BIGSOL, to, content_id, 1);
  
}//END Final send_blux() 

// === Upserterterses ===//

/*/ SONG 
uint32_t content_id;
name artistacc; 
uint8_t artisttype;
song song;

struct song {
  string title;
  vector<double> geoloc; //CHECK this should accomidate changes, be optional of possible, but that would defeat the geo point
  uint8_t genre;
  uint8_t mood;
  uint8_t format;
  string atomictempid;
};

/*/

// --- Add + Fix Songs in the Database --- // 
void upsert_song(song song, name artistacc, name adderacc, uint8_t artisttype, uint32_t content_id, bool deleteme){  
  if(deleteme){ // Delete
    
    // --- Find the song in the _songs table --- //
    auto songs_iterator = _songs.require_find(content_id, string("Song not found"));
    
    // --- Erase Song --- // 
    _songs.erase(songs_iterator);   
    
    // --- Verify the song was successfully deleted --- //
     check(_songs.find(content_id == _songs.end(), "There was a problem deleting the song");

  }//END if(deleteme)
  else {
    auto songs_iterator = _songs.find(content_id);
    
    if( songs_iterator == _songs.end() )
    { // -- Make New Record
      _songs.emplace(adderacc, [&]( auto& row ) {
        row.key = content_id;
        row.artistacc = artistacc;
        row.artisttype = artisttype;
        row.song = song;
      });
    } 
    else 
    { // -- Update Record
      _songs.modify(songs_iterator, adderacc, [&]( auto& row ) {
        row.artistacc = artistacc;
        row.artisttype = artisttype;
        row.song = song;
      });    
    }//END if(results _songs) 
  }//END if(deleteme) else
  
}//END upsert_song



// --- Update running log of ups --- //
void upsert_logup(uint32_t upscount, name upsender, uint32_t content_id, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)
  
  require_auth( upsender );
  uint32_t momentu = find_tu();
  

  
  // --- Add record to _ups --- //
  _ups(get_self(), content_id); //WARN CHECK - is content_id right here? URGENT I really think this is wrong for all upserts
  auto upslog_iterator = _ups.find(upid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( upslog_iterator == _ups.end() )
  { // -- Make New Record
    _ups.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.key = content_id;
      row.totalups = newups;
      row.tuid = momentu;
    });
  } 
  else 
  { // -- Update Record
    if (negative){
      _ups.modify(upslog_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = content_id;
      row.totalups -= newups;
      });
    } else {
      _ups.modify(upslog_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
        row.key = content_id;
      row.totalups += newups;
      });
    }

    
  }//END if(results _ups) 
}


// --- Upsert _uppers and _totals --- //
void upsert_total(uint32_t &upscount, name &upsender, uint32_t &content_id, bool negative) {
  require_auth( upsender );


  
  // --- Update / Insert _totals record of cumulative song Ups --- //
  _totals(get_self(), content_id);
  auto total_iterator = _totals.find(content_id);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( total_iterator == _totals.end() )
  { // -- Make New Record
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = content_id;
      row.totalups = newups;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record 
    if(!negative){
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
      row.totalups += newups;
        row.updated = time_of_up;
      });
    } else { // Subtract the value from totals
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
      row.totalups -= newups;
        row.updated = time_of_up;
      });
    }
    
    
  }//END if(results _totals)

  // --- Update / Insert _uppers record --- //
  _uppers(get_self(), content_id);
  auto listener_iterator = _uppers.find(content_id);
  if( listener_iterator == _uppers.end() )
  {
    _uppers.emplace(upsender, [&]( auto& row ) {
      row.upsender = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.totalups = newups;
    });
  } 
  else 
  {
    _uppers.modify(listener_iterator, upsender, [&]( auto& row ) {
      row.lastup = eosio::time_point_sec::sec_since_epoch();
      row.totalups += newups;
    });
  }//END if(results _uppers)
}//END upsert_total()

// --- Upsert IOUs --- //
void upsert_ious(uint32_t upscount, name &upsender, uint32_t content_id, bool subtract){
  require_auth( upsender ); //CHECK if this breaks call from payup for groups
  
  // --- Determine Artist + Artist Type --- //
  _songs(get_self(), get_self().value); //WARN CHECK - is content_id right here? URGENT I really think this is wrong for all upserts
  auto songs_iterator = _songs.find(content_id);
  check(songs_iterator != _songs.end(), "No song exists with this ID"); 
  name artistacc = songs_iterator->artistacc;//CHECK does this need to be .value()?
  name artisttype = songs_iterator->artisttype;
  
  // --- Create the shifted iouid value --- //
  uint32_t momentu = find_tu();
  uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) content_id;

  // --- Sift Ups by Type (Requires upstype, Defines newxxxups)--- // WARN may not be needed, we know type in IOUs

  
  /*/ --- iouid explanation --- //
    Where does the iouid come from?? 
    iouid is a bit-smash of content_id and Tuid 
    uint64_t iouid = (uint32_t) momentu << 32 | (uint32_t) content_id;
  /*/
  
  // --- Add record to _ups --- //
  _ious(get_self(), get_self().value); //CHECK this is the correct scope
  auto ious_itr = _ious.find(iouid); 
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  //TODO - May need to deal with the type of up, as it could be Big, and we update.. so...
  if( ious_itr == _ups.end())
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
  }//END if(results _ups)
}//END upsert_ious()





// --- All IOUS are removed from table once paid --- //
void removeiou(name sender, name receiver, uint32_t removeamount) {
  
  if (receiver.value = sender.value){
    // Return an error that the Sender and receiver values cannot be the same 
  }
  
  // DELETE records from |ious| where reciever = reciever && sender = sender 
  // removeamount tells how many records to remove from the table
  // when removing a record, we must deduct the amount of ups from removeamount 
  // Must keep track of the amount of ups removed with removeamount
  // If the amount of ups in the row is greater than removeamount, then we must update the record instead of remove it

  // Make the row remove a maximum of 12 rows at a time

  require_auth(get_self());

  // Check if sender and receiver values are the same
  check(sender != receiver, "Sender and receiver values cannot be the same");

  // Instantiate the ious table
  ious_table ious(get_self(), get_self().value);



  // --- Get oldest IOUs from _ious Table --- //
  _ious(get_self(), sender.value);//CHECK WARN scope
  auto ious_itr = _ious.get_index<"byupcatcher"_n>(); 

}//END removeiou


// --- Keep track of total account amounts for ALL users --- // CHANGE TO UPSERT
void updatelisten(uint32_t upscount, name upsender) {
  // CHECK (caller = AUTH_ACCOUNT)
  // UPDATE record from |listeners| 
  
}

void removelisten(name upsender) {
  require_auth(get_self());

  // Delete record from |listeners|
  listeners_table listeners(get_self(), get_self().value);
  auto listener_itr = listeners.find(upsender.value);
  if (listener_itr != listeners.end()) {
    listeners.erase(listener_itr);
  }

  // Delete records from |ious| where upsender is sending or receiving CHECK TODO abstract TO removeiou() if overloaded in future
  ious_table ious(get_self(), get_self().value);
  auto ious_by_upsender = ious.get_index<"byupsender"_n>();
  auto ious_itr = ious_by_upsender.find(upsender.value);
  while (ious_itr != ious_by_upsender.end() && ious_itr->upsender == upsender) {
    ious_itr = ious_by_upsender.erase(ious_itr);
  }

  auto ious_by_upcatcher = ious.get_index<"byupcatcher"_n>();
  ious_itr = ious_by_upcatcher.find(upsender.value);
  while (ious_itr != ious_by_upcatcher.end() && ious_itr->upcatcher == upsender) {
    ious_itr = ious_by_upcatcher.erase(ious_itr);
  }
}


// --- DISPATCHER Checks + calls logup() updateiou() and upsert_total() --- //
//CHECK currently the negative isn't doing anything
void updateup(uint32_t &upscount, uint8_t &upstype, name &upsender, uint32_t content_id, bool negative) {  
  if (!negative){
    // --- Log the ups in UPSLOG table --- // 
    upsert_logup(upscount, upstype, upsender, content_id, 0);
    
    // --- Calls action to update the TOTALS table -- //
    upsert_total(upscount, upstype, upsender, content_id, 0);
  }
  
  if (upstype != BLUX){
    // --- Record the Up to be paid via IOUS table --- //
    upsert_iou(upscount, upstype, upsender, content_id, negative);

  }  
}//END updateup()

// --- Sends BLUX, clears IOUS. Called from overloaded payup() --- //
void payupsender(name upsender){
     // --- Min Wait 5 seconds since last full pay payment --- //
    _internallog(get_self(), get_self().value);
    
    // --- Get oldest IOUs from _ious Table --- //
    _ious(get_self(), upsender.value);//CHECK WARN scope
    auto ious_itr = _ious.get_index("byinitiated"_n)
    
    // --- Check if Groups exist in list to be paid --- // CHECK: Remove this from single-payer
    bool groupies = false;
    std::vector<name> foundgroups; //CHECK are we still using this 
    struct DemBoiz {
      name intgroupname,
      string groupname,
      vector<int8_t> artists,
      vector<int8_t> weights
    };
    
    // --- Check the next 12 entries for Groups --- //
    for ( auto itr_g_check = ious_itr.rbegin(); itr_g_check >= ious_itr.rbegin() - 12; itr_g_check++ ) {//CHECK should ious_itr really be in the table?
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
     // --- Build Memo --- //
     uint32_t content_id = iouid_to_content_id(ious_itr->iouid);
     string memo = string("BLUX pay for cxc.world/" + to_string(content_id) + " ");
     
     
     if(ious_itr->artisttype == 1) // 1=solo, 2=group
     {
       // --- Send Solo Artist BLUX --- // 
       send_blux(to, ious_itr->upcatcher, quantity, memo, content_id);//EXPLAIN To = old from (this contract) WORKING (Add to FRESH)
     } else {
       // === Pay Group of Artists === //
      
       // --- Add group's Readable name to the memo --- // TODO refactor using string(memo + memo2)
       memo = string(memo + to_string(ious_itr->intgroupname); //CHECK to_string is used correctly
       
       // --- Get Artists and Weights --- //
       auto groups_itr = _groups.require_find(ious_itr->upcatcher, string("Group not found")); //CHECK that the _require won't cause transactions to fail
       
       auto remaining_ups = ious_itr->upscount;
       auto last_position = groups_itr->payposition;
       auto current_position = 0;
       auto total_positions = std::accumulate(groups_itr->weights.begin(), groups_itr->weights.end(), 0);
       bool pay_started = false;

       // === Make payments + update table === //
       // --- Figure out what # in groupmembers vector to pay by determining position --- //
       for(int itr_g_members = 0, groups_itr->weights.size(), itr_g_members++){
         if(remaining_ups > 0){
          // --- Figure out who to pay --- //
          uint32_t artist_first_payable_pos = 0;
          if (last_position < 1 || pay_started == false){
            // If less, No need to check the position
            auto checked_pay_position = 0;
            for (int itr_artist_position = 0, itr_artist_position +1 >= groups_itr->weights.size(), itr_artist_position++){ // CHECK +1 is correct (0-based array)
              
              checked_pay_position += groups_itr->weights[itr_g_members];
              //artist_position = itr_artist_position;
              
              // --- Check Weight (BLUX owed per full-circle) --- //
              if ((checked_pay_position + groups_itr->weights[itr_g_members + 1]) <= remaining_ups){// This is the person to be paid
                artist_first_payable_pos = itr_artist_position;
                pay_started = true;
                break;
              }//END position check <= remaining_ups
              
            }//END for(itr_artist_position)  
          }//END if (last_position > 1)
            
            // --- Loop starting at the artist_first_payable_pos --- //
            check(pay_started, "Group payments failed");
            for(int itr_g_paying_pos = artist_first_payable_pos, remaining_ups < 1, itr_g_paying_pos++){ /*itr_g_paying_pos >= (groups_itr->weights.size() + artist_first_payable_pos) ||*/
              if (itr_g_paying_pos == groups_itr->weights.size()){
                itr_g_paying_pos = 0; // Loop back to first recipient
              } 
              
              if (remaining_ups > 0 && remaining_ups < 999999999999){              
                // --- Determine Max + Real Pay by Weight --- // 
                auto artist_paid = groups_itr->artists[artist_first_payable_pos];
                auto real_payment = (remaining_ups >= groups_itr->weights[itr]) ? groups_itr->weights[itr] : remaining_ups;
                
                // --- Send Group Member BLUX --- // 
                send_blux(to, artist_paid, real_payment, memo, content_id);
                remaining_ups -= real_payment;
              }
              
              if(remaining_ups < 1 || remaining_ups > 999999999999){ // It's the last payment
                // --- Check for song in table --- // 
                auto groups_iter = _groups.require_find( ious_itr->intgroupname, string( "Group " + to_string(ious_itr->intgroupname) + " was not found when we tried to pay." ) );
                
                // --- Update Pay Position --- //
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
       // --- How many Big Ups --- //
       auto big_ups_count = floor(ious_itr->upscount / 64);
       // --- Mint NFTs for Big Ups to the SENDER --- //
     }//END if (ious_itr->upstype == BIGSOL)
   }//END for (12 IOUs)
}//END final payupsender()


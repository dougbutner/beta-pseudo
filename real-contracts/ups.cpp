#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

/*/ TODO
ACTION payup(name upsender); // User's call to pay themselves
ACTION updateartist(name artistacc, vector<string> artistinfo, string artistalias);
ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
ACTION updatesong(uint32_t songid, vector<string>);
ACTION removesong(uint32_t songid)
ACTION deepremvsong(uint32_t songid)
/*/

// --- Receive SOL sent to contract + make ups --- \\
//NOTE UPs memo is either an integer of the songid, or the songid with " BIG appended" or any other string
[[eosio::on_notify("sol.cxc::transfer")]] void ups::sol_catch( const name from, const name to, const asset quantity, const string memo )
{  
  uint32_t songid_upped;
  require_auth(get_self());
  
  // --- Check that we're the intended recipient --- \\ //CHECK Is this really needed
  if (to != _self) return; // internal function no need to check()

  // --- Token-symbol + Memo = Songid Check --- \\
  check(quantity.symbol == symbol("SOL", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for song in table --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);
  
  // --- Check if BIG --- \\
  //EXPLAIN - Passing a character at the end of the memo (instead of just songid) triggers BIG check (app does "2947 BIG")
  auto ups_type;
  if(!isdigit( memo.back() ) ){
    ups_type = BIGSOL;
  } else {
    ups_type = SOL;
  }
  
  // --- Pass on to updateup() --- \\
  updateup(quantity, ups_type, song_iter, upsender); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->SOL ups 


// --- Receive BLUX sent to contract + make ups --- \\
[[eosio::on_notify("bluxbluxblux::transfer")]] void ups::sol_catch( const name from, const name to, const asset quantity, const string memo )
{  
  uint32_t songid_upped;
  require_auth(get_self());
  
  // --- Check that we're the intended recipient --- \\ //CHECK Is this really needed
  if (to != _self) return; // internal function no need to check()

  // --- Token-symbol + Memo = Songid Check --- \\
  check(quantity.symbol == symbol("BLUX", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for song in table --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);

  // --- Pass on to updateup() --- \\
  updateup(quantity, 2, song_iter, upsender); // 2=BLUX Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->BLUX ups 


// --- Send all owed payments listed in |ious|  --- \\
ACTION ups::payup(void) {
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
  
}//END payup(void)

// --- Send owed payments listed in |ious| for one account --- \\
ACTION ups::payup(name upsender) {

}//END payup(void)

// --- Register artist, or change artist information --- \\
ACTION ups::updateartist(name artistacc, vector<string> artistinfo, string artistalias) {
  // IF (exists |artists => account|)
  // UPDATE |artists => artistinfo|
  // Check if there is a change to 
  // else 
  // INSERT record into |artists| return;
}

// --- Register artist group, or change group information --- \\
ACTION ups::updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo) {
  // --- Call the upsert helper function with payposition flag 9999 --- \\
  upsert_groups(intgroupname, group_alias, artists, weights, groupinfo, 9999);
}


// --- WARN NEEDS REVIEW Update song info or receiving account --- \\
ACTION ups::updatesong(uint32_t songid, name artistacc) { 
  // NOTE : Music 
  // NOTE: Remove the '-' from genres coming from cXc.world, use enum
  // CHECK (sender = artist || sender = AUTH_ACCOUNT)
    // if (group) CHECK (sender is in |artistgroups|)
  // IF (exists |so => account|)
  // UPDATE |songs = songid|)
  // else 
  // INSERT |songs|
}


// --- Remove the song from earning potential --- \\
ACTION ups::removesong(uint32_t songid) {

}

// --- Remove all record of song in RAM --- \\
ACTION ups::deepremvsong(uint32_t songid) {

}

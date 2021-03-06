#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

/*/
ACTION payup(void); // Default call 
ACTION payup(name upsender); // User's call to pay themselves
ACTION updateartist(name artistacc, vector<string> artistinfo, string artistalias);
ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
ACTION updatesong(uint32_t songid, vector<string>);
ACTION removesong(uint32_t songid)
ACTION deepremvsong(uint32_t songid)
/*/

// --- Receive tokens sent to contract + make ups --- \\
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
  auto song_iter = _songs.require_find( songid_upped, string( "Song " + to_string(songid_upped) + " was not found." ).c_str() );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);

  // --- Pass on to updateup() --- \\
  ups::updateup(quantity, 1, song_iter, upsender); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->SOL ups 

//NOTE need separate LISTENER for Bluups

// --- Send all owed payments listed in |ious|  --- \\
ACTION ups::payup(void) {
   // --- Check Time for min 5 minutes since last payment --- \\
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  _internallog(get_self(), get_self().value);
  check(nftToTokenTable.get().primary_key() < (time_of_up + 3), "Please wait 5 seconds between each payup. ");//CHECK this syntax is correct
  
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
   /*/ 
     itr->secondary
     uint64_t ious_itr->iouid;
     name ious_itr->upsender;
     name ious_itr->upcatcher;
     uint8_t ious_itr->artisttype;
     uint32_t ious_itr->upscount // Should be either BIGSOL or sol up or both
     uint8_t ious_itr->upstype
     uint32_t ious_itr->initiated;
     uint32_t ious_itr->updated; 
   /*/
   
   // --- Build Memo --- \\
   uint32_t songid = iouid_to_songid(ious_itr->iouid);
   string memo1("BLUX pay for cxc.world/");
   string memo2(songid);
   string memo = memo1 + memo2;

   if(ious_itr->artisttype == 1) // 1=solo, 2=group
   {
     send_blux(to, ious_itr->upcatcher, quantity, memo);//EXPLAIN To = old from (this contract) WORKING (Add to FRESH)
   } else {
     // --- Get the information from dagroup --- \\
     
     // --- Add group's Readable name to the memo --- \\
     string prememo(ious_itr->intgroupname);
     string prememo2(" ");
     prememo = prememo + prememo2;
     memo = prememo + memo;
     
     // --- Get Artists and Weights --- \\
     auto groups_itr = _groups.require_find(ious_itr->upcatcher); //CHECK that the _require won't cause transactions to fail
     
     auto remaining_ups = ious_itr->upscount;
     auto last_position = groups_itr->payposition;
     auto current_position = 0;
     auto total_positions = std::accumulate(groups_itr->weights.begin(), groups_itr->weights.end(), 0);
     bool pay_started = false;
     
     /*/ HERE IN CASE ABOVE FAILS --- Get the total amount of payments --- \\
     for(int itr = 0, groups_itr->weights.size(), itr++){
       total_positions += groups_itr->weights[itr];
     }/*/
     
     // === Make payments + update table === \\
     // --- Figure out who in group to pay first, then pay the rest --- \\
     
     for(int itr_g_members = 0, groups_itr->weights.size(), itr_g_members++){
       if(remaining_ups > 0){
        // --- Figure out who to pay --- \\
        uint32_t artist_first_payable_pos = 0;
         
        if (last_position > 1 || pay_started == false){
          // If less, No need to check the position
          auto checked_position = 0;
          auto to_assign = ious_itr->upscount;

          for (int itr_artist_position = 0, groups_itr->weights.size(), itr_artist_position++){
            
            checked_position += groups_itr->weights[itr_g_members];
            //artist_position = itr_artist_position;
            
            if ((checked_position + groups_itr->weights[itr_g_members + 1]) <= remaining_ups){// This is the person to be paid
              artist_first_payable_pos = itr_artist_position;
              pay_started = true;
              break;
            }//END position check <= remaining_ups
          }//END for(itr_artist_position)
          
          if (pay_started){
            // --- Loop starting at the artist_first_payable_pos --- \\
            for(int itr_g_paying = artist_first_payable_pos, (groups_itr->weights.size() + 1), itr_g_paying++){// +1 needed to 
              if (itr_g_paying == groups_itr->weights.size()) 
                itr_g_paying = 0; // Loop bac to first recipient
                
            }//END for(itr_g_paying)
          }//END if()paystarted
            
            // --- Determine Max + Real Pay by Weight --- \\ 
            auto artist_paid = groups_itr->artists[artist_first_payable_pos];
            auto real_payment = (remaining_ups >= groups_itr->weights[itr]) ? groups_itr->weights[itr] : remaining_ups;
            
            // --- Send Group Member BLUX --- \\ 
            send_blux(to, artist_paid, real_payment, memo);
            remaining_ups -= real_payment;
            

            if(remaining_ups < 1 || remaining_ups > 99999999){ // It's the last payment
              // --- Update the table with new Payposition --- \\ 
              //TODO - Make Upsert function for Group info 
              
            }//END table update if (remaining_ups < 1)        

        }//END if (last_position > 1)
       } else {// Dammit Charles, why don't you have any money?
         break;
       }

       // --- Send Solo Artist BLUX --- \\ 
       send_blux(to, ious_itr->upcatcher, quantity, memo);//To = old from (this contract) WORKING (Add to FRESH)
     }//END for(itr_g_members)

   }
  
   if (ious_itr->upstype ==  BIGSOL){
     // --- How many Big Ups --- \\
     auto big_ups_count = floor(ious_itr->upscount / 64);
     // --- Mint NFTs for Big Ups --- \\
   }
 }//END for (12 IOUs)
  
  //auto& user_iterator = _upslog.find(username.value); // WARN jumped to other thing, this is not good
  //auto& ur_ious = _ious.get(username.value, "User doesn't exist");
  // Make [] with each record of owed to depth of 12 rows, starting with oldest
  // if (account is group)
    // READ |artistgroups => payposition|
    // Compile [] of the members owed by counting off each position and weight until all reward is given out
  // TRANSFER the total sum owed to account[s]
  // UPDATE / DELETE |ups.cxc => ious| table to reflect changes 


}//END payup(void)

// --- Send owed payments listed in |ious| for one account --- \\
ACTION ups::payup(name upsender) {
// Same as above but with account
  //require_auth(username);

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
ACTION ups::updategroup(name intgroupname,  vector<string> groupinfo, string group_alias, vector<string> artists, vector<int8_t> weights) {
  // CHECK (artists.length = weights.length OR 0 weights && 0 members) // 0 = no update, both or none
  // IF (exists |artistgroup => intgroupname|) // Check member list 
    // CHECK (exists |artistgroup => artists => account|)
    // UPDATE |artistgroups => artistinfo|)
  // else 
  // INSERT |artistgroups|
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

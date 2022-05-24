#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

/*/
ACTION payup(void); // Default call 
ACTION payup(name up_sender); // User's call to pay themselves
ACTION updateartist(name artist_account, vector<string> artist_info, string artist_alias);
ACTION updategroup(name internal_name, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
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
  ups::updateup(quantity, 1, song_iter, up_sender); // 1=SOL Ups (uint32_t quantity, uint8_t ups_type, uint32_t songid, name up_sender)
  
}

// --- Send all owed payments listed in |ious|  --- \\
ACTION ups::payup(void) {
  // if (account = undefined) account = all_accounts
  // check the time to ensure it's been 5 minutes
  // READ the |ious.cxc => ious| table for account
  //auto& user_iterator = _upslog.find(username.value); // WARN jumped to other thing, this is not good
  
  //auto& ur_ious = _ious.get(username.value, "User doesn't exist");
  
    // Make [] with each record of owed to depth of 12 rows, starting with oldest
    
    

  // if (account is group)
    // READ |artistgroups => pay_position|
    // Compile [] of the members owed by counting off each position and weight until all reward is given out
    
  // TRANSFER the total sum owed to account[s]
  // UPDATE / DELETE |ups.cxc => ious| table to reflect changes 


}

// --- Send owed payments listed in |ious| for one account --- \\
ACTION ups::payup(name up_sender) {
// Same as above but with account
  //require_auth(username);


}

// --- Register artist, or change artist information --- \\
ACTION ups::updateartist(name artist_account, vector<string> artist_info, string artist_alias) {
  // IF (exists |artists => account|)
  // UPDATE |artists => artist_info|
  // Check if there is a change to 
  // else 
  // INSERT record into |artists| return;
}

// --- Register artist group, or change group information --- \\
ACTION ups::updategroup(name internal_name,  vector<string> group_info, string group_alias, vector<string> artists, vector<int8_t> weights) {
  // CHECK (artists.length = weights.length OR 0 weights && 0 members) // 0 = no update, both or none

  // IF (exists |artistgroup => internal_name|) // Check member list 
    // CHECK (exists |artistgroup => artists => account|)
    // UPDATE |artistgroups => artist_info|)
  // else 
  // INSERT |artistgroups|
}


// --- WARN NEEDS REVIEW Update song info or receiving account --- \\
ACTION ups::updatesong(uint32_t songid, name artist_account) { 
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

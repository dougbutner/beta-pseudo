#include "ups.hpp"
#include "songs.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

void payup(void); // Default call by AUTH_ACCOUNTS
void payup(name account); // User's call to pay themselves
void updateartist(name account, vector<string> artist_info, string artist_name);
void updategroup(name internal_name, string group_name, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
void updatesong(uint32_t songid, vector<string>);


// --- Receive tokens sent to contract + make ups --- \\

[[eosio::on_notify("sol.cxc")]] void ups::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
  // --- Check that we're the intended recipient --- \\ //CHECK Is this really needed
  if (to != _self) return;
  
  // --- Make sure it's the right symbol --- \\
  eosio::check(quantity.symbol == symbol("SOL", 0), "Now accepting SOL and BLUX");
  
  // --- Set up Variables --- \\
  uint32_t quantity = //CHECK 
  
  
  // --- Pass on to updateup() --- \\
  //updateup(uint32_t quantity, uint8_t ups_type, name account);
  updateup(quantity, 1, , name account); // 1=SOL Ups
  }
  
}

// --- Send all owed payments listed in |ious|  --- \\
void ups::payup(void) {
  // if (account = undefined) account = all_accounts
  
  // check the time to ensure it's been 5 minutes
  
  
  
  // if (account = all_accounts) 
      // CHECK (account = AUTH_ACCOUNT)

  // READ the |ups.cxc => ious| table for account
  auto user_iterator = _ups.find(username.value);
  
  auto& ur_ious = _ious.get(username.value, "User doesn't exist");
  
    // Make [] with each record of owed to depth of 12 rows, starting with oldest
    
    

  // if (account is group)
    // READ |artistgroups => pay_position|
    // Compile [] of the members owed by counting off each position and weight until all reward is given out
    
  // TRANSFER the total sum owed to account[s]
  // UPDATE / DELETE |ups.cxc => ious| table to reflect changes 
  
  _ious.modify(user, username, [&](auto& modified_user) {

    // Assign the newly created game to the player
    modified_user.game_data = game_data;
  });
  

}

// --- Send owed payments listed in |ious| for one account --- \\
void ups::payup(name account) {
// Same as above but with account
  require_auth(username);


}

// --- Register artist, or change artist information --- \\
void ups::updateartist(name account, vector<string> artist_info, string artist_name) {
  // IF (exists |artists => account|)
  // UPDATE |artists => artist_info|
  // else 
  // INSERT record into |artists| return;
}

// --- Register artist group, or change group information --- \\
void ups::updategroup(name internal_name, string group_name, vector<string> artists, vector<int8_t> weights, vector<string> group_info) {
  // CHECK (artists.length = weights.length OR 0 weights && 0 members) // 0 = no update, both or none

  // IF (exists |artistgroup => internal_name|) // Check member list 
    // CHECK (exists |artistgroup => artists => account|)
    // UPDATE |artistgroups => artist_info|)
  // else 
  // INSERT |artistgroups|
}



// --- WARN NEEDS REVIEW Update song info or receiving account --- \\
void ups::updatesong(uint32_t songid, vector<string>) {
  // NOTE : Music 
  // NOTE: MUST remove the '-' from genres coming from cXc.world
  // CHECK (sender = artist || sender = AUTH_ACCOUNT)
    // if (group) CHECK (sender is in |artistgroups|)
  // IF (exists |so => account|)
  // UPDATE |songs = songid|)
  // else 
  // INSERT |songs|
}


// --- Remove the song from earning potential --- \\
void ups::removesong(uint64_t songid) {

}

// --- Remove all record of song in state --- \\
void ups::deepremvsong(uint64_t songid) {

}

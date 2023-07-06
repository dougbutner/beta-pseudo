ups.hpp

// --- Will remove blacklisted user's ups retroactively --- //
/*/
Only the contract may remove ups. This is because of the financial aspect of the 

Reality Check
It's only feasible to update the totals and the IOUs table.
Individually removing from upslog_ table requires parsing the concatenated values of the TUid and the UserID
It will be possible to process and remove these later with an action that can be called from eosjs
/*/
void removeups(name user) {
  require_auth(get_self()); //CHECK do we need to call this with the eosio.code permission?
  // --- Instantiate the ups + totals tables --- //
  
    // DELETE record from |ups| where (account == account )
        // Call updateup with negative value
        updateup(upscount, upstype, upsender, upcatcher, songid, negative);
    
    // UPDATE record from |totals|
  
    // call upsert_total(upscount, upstype, upsender, songid, negative);
} 


  void updateup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); //DISPATCHER
  void logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t upscount, uint8_t upstype, name upsender);
  void removelisten(name upsender);
  void removesong(uint32_t songid); // Removes all IOUs for song + song record (minimal)
  void deepremvsong(uint32_t songid); // Removes all records of Ups for this sond
  void updateartist(name artist_wax, vector<string> artistinfo, string artistalias); 


#include "ups.hpp"
#include <eosio/system.hpp>


void updateup(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account); 
void logup(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account); 
void removeups(name user); 
void updatetotal(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account); 
void updateiou(name sender, name receiver, uint32_t amount, bool subtract); 
void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
void updatelisten(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account);
void removelisten(name account);
void removesong(uint64_t songid); //TODO add to pseudo-code, removes all IOUs for song, song


// --- Checks + calls logup() updateiou() and updatetotal() --- \\
void ups::updateup(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account) {
  // IF (method == ui (0)) check (sender == AUTH_ACCOUNTs) ELSE method = contract (1)
  // call ACTION updatetotal()
  // call ACTION logup()
  // call ACTION updateiou()

}

// --- Store persistent record of UP in |ups| --- \\
void ups::logup(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account) {
  // IF (record exists for TU)
    // UPDATE |ups| where TU = TU & account = account
    // else
    // INSERT record of Up into |ups|
    
}

// --- Will remove blacklisted user's ups retroactively --- \\
void ups::removeups(name user) {
  // IF caller == account
    // DELETE record from |ups| where (account == account )
    // UPDATE record from |totals|
    // call updatetotal()
    
}

// --- Single-row record of ups for each song --- \\
void ups::updatetotal(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account) {
  // if (record exists in |totals|)
    // UPDATE record from |totals|
    // else 
    // INSERT record into |totals|  
  // CONTRACT soldisk.cxc is notified and calls LISTEN => ups.cxc:updatetotal()
  
}

// --- Makes sure people get paid --- \\
void ups::updateiou(name sender, name receiver, uint32_t amount, bool subtract) {
  //NOTE all PURPLE IOUs are now stored on a Charts contract
  // if (record exists in |ious|)
    // UPDATE record from |ious|
    // else 
    // INSERT record into |ious|
    
}

// --- All IOUS are removed from table once paid --- \\
void ups::removeiou(name sender, name receiver) {
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
void ups::updatelisten(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name account) {
  // CHECK (caller = AUTH_ACCOUNT)
  // UPDATE record from |listeners|  
  
}

// --- Remove record of user in event of blacklisting --- \\
void ups::removelisten(name account) {
  // CHECK (caller = AUTH_ACCOUNT)
  // DELETE record from |listeners|  
  
}

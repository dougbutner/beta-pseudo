#include "eosio/eosio.hpp"
#include <eosio/asset.hpp>
#include "cxc.hpp"
#include <vector> //CHECK is needed? 

using namespace std;
using namespace eosio;
using std::string;


class [[eosio::contract]] ups : public contract {
  
using contract::contract;
public: 

#include "songs.hpp"

struct song {
  string title;
  vector<double> geoloc; //CHECK this should accomadate changes, be optional with 0.0, but that would defeat the geo point
  uint8_t genre;
  uint8_t mood;
  uint8_t format;
  string nationiso3;   //DEFAULT VALUE XXX + CXC
  string atomictempid;
};

// --- Bring in song, genres, moods, formats --- // 
uint32_t timeunit;

// Artist Types: { 1: solo, 2: group}

std::string AUTH_ACCOUNTS[6] = { "currentxchng", "cxc", "cron.cxc", "pay.cxc", "ups.cxc", "helpups.cxc" }; // CHECK these are right

enum up_type { //CHECK is this really just making vars across the system (needed)
  SOL = 1,
  BLUX = 2,
  BIG = 3,
  BIGSOL = 4 // -- This allows one call to send both Sol and Big Ups, greedy for all 64 Ups to make one Big Up (Won't work otherwise) 
};

private:  
  TABLE upslog { 
    uint64_t upid; 
    uint32_t songid;
    uint32_t totalsolups; 
    uint32_t totalbluups;
    uint32_t totalbigups;
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_songid() const { return (uint64_t) songid; }
    uint64_t by_solups() const { return (uint64_t) totalsolups; }
    uint64_t by_bluups() const { return (uint64_t) totalbluups; }
    uint64_t by_bigups() const { return (uint64_t) totalbigups; }
    uint64_t by_tuid() const { return (uint64_t) tuid; }
  };
  
  using upslog_table = multi_index<name("upslog"), upslog,
    eosio::indexed_by<"bysongid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_songid>>,
    eosio::indexed_by<"bysolups"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_solups>>,
    eosio::indexed_by<"bybluups"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_bluups>>,
    eosio::indexed_by<"bybigups"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_bigups>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_tuid>>
  >;
  
  TABLE totals {
    uint32_t songid;
    uint32_t totalsolups; 
    uint32_t totalbluups;
    uint32_t totalbigups;
    uint32_t updated;
    
    uint64_t primary_key() const { return songid; }
  };
  
  using totals_table = multi_index<name("totals"), totals>;
  
  // --- Connects cXc.world's DB to chain with songid, maintains NFT list for other dapps --- //
  TABLE songs { 
    uint32_t songid;
    name artistacc;
    uint8_t artisttype;
    song song;
        
    uint64_t primary_key() const { return (uint64_t) songid; }
  };
  
  using songs_table = multi_index<name("songs"), songs>;
  
  // --- Activity stats for Listeners (For future awards) --- //
  TABLE listeners {
    name upsender;
    uint32_t firstup;
    uint32_t lastup;
    uint32_t totalsolups;
    uint32_t totalbluups;
    uint32_t totalbigups;
    
    uint64_t primary_key() const { return upsender.value; }
  };
  
    using listeners_table = multi_index<name("listeners"), listeners>;
  
  // --- Store record of who to pay --- // 
  // CHECK (in .cpp) that we are paying both the upsender + upcatcher
  TABLE ious {
    uint64_t iouid;
    name upsender;
    name upcatcher;
    uint8_t artisttype;
    uint32_t upscount; // Should be either BIGSOL or sol up or both
    uint8_t upstype;
    uint32_t initiated;
    uint32_t updated; 
    uint64_t primary_key() const { return iouid; }
    uint64_t by_upcatcher() const { return upcatcher.value; }
    uint64_t by_upsender() const { return upsender.value; }
    uint64_t by_artisttype() const { return (uint64_t) artisttype; }
    uint64_t by_upscount() const { return (uint64_t) upscount; }
    uint64_t by_initiated() const { return (uint64_t) initiated; }
    uint64_t by_updated() const { return (uint64_t) updated; }
  };

  using ious_table = multi_index<name("ious"), ious,
    eosio::indexed_by<"byupcatcher"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upcatcher>>,
    eosio::indexed_by<"byupsender"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upsender>>,
    eosio::indexed_by<"byartisttype"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_artisttype>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upscount>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>,
    eosio::indexed_by<"byupdated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_updated>>
  >;
  
  // --- Keep record of Artists (recipient in _songs) to pay --- //
  TABLE artists {
    name artistacc;
    string artistalias;
    vector<string> artistinfo;
    uint64_t primary_key() const { return artistacc.value; }
  };
  using artists_table = multi_index<name("artists"), artists>;
  
  TABLE artistgroups {
    name intgroupname; // abcdef.cxc // CHECK is there benefit to a name (uint64_t) vs a simple uint32_t? 
    string groupname;
    vector<name> artists;
    vector<int8_t> weights;
    //vector<string> groupinfo; //DEPRECIATED //CHECK this will work, can pass empty strings, or is there an <auto> type, key value pairs
    uint8_t payposition;  
    uint64_t primary_key() const { return intgroupname.value; }
  };
  
    using groups_table = multi_index<name("artistgroups"), artistgroups>;
  
  
  
  TABLE internallog {
    uint32_t lastpay; // Last time the payment was called for all 
    uint32_t lastfullpay; // All accounts sent BLUX 
    bool remaining; // Did we reach the end of who is owed to pay? 
    
    uint64_t primary_key() const { return (uint64_t) lastpay; } //WARN CHECK if this is singleton (it isn't, fix it)
  };
  
  using cxclog_table = multi_index<name("internallog"), internallog>;
  
  
  void updateup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); //DISPATCHER
  void logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t upscount, uint8_t upstype, name upsender);
  void removelisten(name upsender);
  void removesong(uint32_t songid); // Removes all IOUs for song + song record (minimal)
  void deepremvsong(uint32_t songid); // Removes all records of Ups for this sond
  void updateartist(name artist_wax, vector<string> artistinfo, string artistalias); 
  //MOVED to upsert in helpups.cpp // void updateartistgroup(string groupname, name intgroupname, vector<string> artists, vector<int8_t> weights);
  
  
  // --- Declare the _tables for later use --- // 
  ious_table _ious;
  upslog_table _upslog;
  songs_table _songs;
  listeners_table _listeners;
  artists_table _artists;
  groups_table _groups;
  totals_table _totals;
  cxclog_table _internallog;
  
public:
  
  [[eosio::on_notify("sol.cxc::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void sol_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  ACTION payup(void); // Default call
  
  ACTION payup(name upsender); // User's call to pay themselves
  
  ACTION updateartist(name upsender, vector<string> artistinfo, string artistalias);
  
  ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
  
  ACTION updatesong(uint32_t songid, song dasong); 
};

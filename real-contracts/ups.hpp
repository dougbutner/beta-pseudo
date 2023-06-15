#include "eosio/eosio.hpp"
#include <eosio/asset.hpp>
#include "cxc.hpp"
#include <vector>
using namespace std;
using namespace eosio;
using std::string;

class [[eosio::contract]] ups : public contract {
    using contract::contract;

public:
    #include "songs.hpp"
    struct song {
        string title;
        vector<double> geoloc;
        uint8_t genre;
        uint8_t mood;
        uint8_t format;
        uint64_t atomictempid;
    };

    uint32_t timeunit;
    std::string AUTH_ACCOUNTS[6] = { "currentxchng", "cxc", "cron.cxc", "pay.cxc", "ups.cxc", "helpups.cxc" };
    enum up_type {
        SOL = 1,
        BLUX = 2,
        BIG = 3,
        BIGSOL = 4
    };

private:
    TABLE upslog {
        uint upid;
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
    
    TABLE songs {
    uint32_t songid;
    name artistacc;
    uint8_t artisttype;
    song song;
    uint64_t primary_key() const { return (uint64_t) songid; }
};

using songs_table = multi_index<name("songs"), songs>;

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

TABLE ious {
    uint64_t iouid;
    name upsender;
    name upcatcher;
    uint8_t artisttype;
    uint32_t upscount;
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

TABLE artists {
    name artistacc;
    string artistalias;
    vector<string> artistinfo;
    uint64_t primary_key() const { return artistacc.value; }
};

using artists_table = multi_index<name("artists"), artists>;

TABLE artistgroups {
    name intgroupname;
    string groupname;
    vector<name> artists;
    vector<int8_t> weights;
    uint8_t payposition;
    uint64_t primary_key() const { return intgroupname.value; }
};

using groups_table = multi_index<name("artistgroups"), artistgroups>;

TABLE internallog {
    uint32_t lastpay;
    uint32_t lastfullpay;
    bool remaining;
    uint64_t primary_key() const { return (uint64_t) lastpay; }
};

using cxclog_table = multi_index<name("internallog"), internallog>;

void updateup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid);
void logup(uint32_t upscount, uint8_t upstype, name upsender, uint32_t songid);
void removeups(name user);
void removeiou(name sender, name receiver);
void updatelisten(uint32_t upscount, uint8_t upstype, name upsender);
void removelisten(name upsender);
void removesong(uint32_t songid);
void deepremvsong(uint32_t songid);
void updateartist(name artist_wax, vector<string> artistinfo, string artistalias);

public:
[[eosio::on_notify("sol.cxc::transfer")]]
void sol_catch(const name from, const name to, const asset quantity, const std::string memo);

ACTION payup(void);
ACTION payup(name upsender);
ACTION updateartist(name upsender, vector<string> artistinfo, string artistalias);
ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
ACTION updatesong(uint32_t songid, song dasong);

};

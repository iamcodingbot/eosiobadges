#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

CONTRACT gotchabadge : public contract {
  public:
    using contract::contract;

    ACTION create (name org, name badge, time_point_sec starttime, uint64_t cycle_length, uint8_t max_cap, string ipfsimage, string details);

    ACTION give (name org, name badge, name from, name to, uint8_t amount, string memo );
  

  private:
   // scoped by org
    TABLE metadata {
      name badge;
      time_point_sec starttime; //0000
      uint64_t cycle_length; // 24 hrs
      time_point_sec last_known_cycle_start;
      time_point_sec last_known_cycle_end;
      uint8_t max_cap;
      string ipfsimage;
      string details;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

    // scoped by org
    TABLE stats {
      uint64_t id;
      name account;
      name badge;
      uint8_t balance;
      time_point last_claimed_time;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("stats"), stats,
    indexed_by<name("accountbadge"), const_mem_fun<stats, uint128_t, &stats::acc_badge_key>>
    > stats_table;

    struct achievement_args {
      name org;
      name badge;
      name account;
      uint8_t count;
    };

    struct initbadge_args {
      name org;
      name badge;
      string ipfs;
      string details;
    };

};

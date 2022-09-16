#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

#define CHECKS_CONTRACT ""
#define SIMPLEBADGE_CONTRACT "sbadge.gm"

using namespace std;
using namespace eosio;

CONTRACT org : public contract {
  public:
    using contract::contract;
    struct badge_count {
      name badge;
      uint16_t count;
    };

    ACTION recognize (name org_admin, name trusted_badge_contract);
    ACTION initcoll (name org_admin, name collection_name);
    
  
    ACTION createsimple (name creator, name badge, vector<name> parentbadge, string ipfsimage, string details, bool write_to_aa);
    ACTION creategotcha (name creator, name badge, time_point_sec starttime, uint64_t cycle_length, uint8_t max_cap, string ipfsimage, string details);
 //   ACTION createrollup (name creator, name badge, vector<badge_count> rollup_criteria, string ipfsimage, string details);
    ACTION givegotcha (name badge, name from, name to, uint8_t amount, string memo );
    ACTION givesimple (name from, name to, name badge, string memo );
 //   ACTION takerollup (name account, name badge);

  private:


    struct createsimple_args {
      name org;
      name badge;
      vector<name> parentbadge;
      string ipfsimage;
      string details;
      bool write_to_aa;
    };

    struct creategotcha_args {
      name org;
      name badge;
      time_point_sec starttime;
      uint64_t cycle_length;
      uint8_t max_cap;
      string ipfsimage;
      string details;
    };

    struct createrollup_args {
      name org;
      name badge;
      vector<badge_count> rollup_criteria;
      string ipfsimage;
      string details;
    };

    struct claimgotcha_args {
      name org;
      name account; 
    };

    struct givegotcha_args {
      name org;
      name badge;
      name from;
      name to;
      uint8_t amount;
      string memo;
    };

    struct givesimple_args {
      name org;
      name to;
      name badge;
      string memo;
    };

    struct rollup_args {
      name org;
      name account;
      name badge;
    };

};

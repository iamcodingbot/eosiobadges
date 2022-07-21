#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT simplebadge : public contract {
  public:
    using contract::contract;

    ACTION create (name org, name badge, vector<name> parentbadge, string ipfsimage, string details);

    ACTION give (name org, name to, name badge, string memo );


  private:
    TABLE metadata {
      name badge;
      vector<name> parentbadge;
      string ipfsimage;
      string details;
      auto primary_key() const {return badge.value; }
    };
    typedef multi_index<name("metadata"), metadata> metadata_table;

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

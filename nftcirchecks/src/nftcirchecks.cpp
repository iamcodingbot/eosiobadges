#include <nftcirchecks.hpp>


ACTION nftcirchecks::addmember (name member) {
  require_auth(get_self());
  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(member.value);
  check (itr == _members.end(), "<member> already exists") ;
  vector<name> roles ;
  roles.push_back(name("member"));
  _members.emplace (get_self(), [&]( auto& row) {
      row.member = member;
      row.roles = roles;
  });
}

ACTION nftcirchecks::addrole(name member, name role) {
  // admin, issuer, creater, member
  require_auth(get_self());

  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(member.value);
  check (itr != _members.end(), "<member> not found") ;

  vector<name> roles = itr->roles;

  for(auto i = 0 ; i < roles.size(); i++) {
    check (role != roles[i], "<role> already exists");
  }

  roles.push_back(role);

  _members.modify( itr, get_self(), [&]( auto& row ) {
    row.roles = roles;
  });

}
    
void nftcirchecks::sbcreatcheck (name creator, name badge, vector<name> parentbadge, string ipfsimage, string details) {
  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(creator.value);
  check(itr != _members.end(), "<creator> is not a member") ;
  vector<name> roles = itr->roles;
  bool rolefound = false;
  for(auto i = 0 ; i < roles.size(); i++) {
    if (roles[i] == name("sbcreator") || roles[i] == name("admin")) {
      rolefound = true;
    }
  }
  check (rolefound, "<creator> does not have permission to create a simple badge");

}

void nftcirchecks::sbgivecheck (name from, name to, name badge, string memo) {
  members_table _members (get_self(), get_self().value);
  auto itr = _members.find(from.value);
  check(itr != _members.end(), "<from> is not a member") ;
  vector<name> roles = itr->roles;
  bool rolefound = false;
  for(auto i = 0 ; i < roles.size(); i++) {
    if (roles[i] == name("sbissuer") || roles[i] == name("admin")) {
      rolefound = true;
    }
  }
  check (rolefound, "<from> does not have permission to issue simple badge");
}
#include <userprefs.hpp>

ACTION userprefs::allow(name account, name org) {
  require_auth(account);
  prefs_table _prefs (_self, account.value);
  auto prefs_itr = _prefs.find(org.value);

  check(prefs_itr == _prefs.end(), "<org> is already allowed to issue badges");
  _prefs.emplace(account, [&](auto& row) {
    row.org = org;
  });
}

ACTION userprefs::unallow(name account, name org, string memo) {
  require_auth(account);
  prefs_table _prefs (_self, account.value);
  auto prefs_itr = _prefs.find(org.value);

  check(prefs_itr != _prefs.end(), "<org> already not allowed to issue badges");
  _prefs.erase(prefs_itr);
}

ACTION userprefs::checkallow(name org, name account) {
  require_auth(name("profiles"));
  prefs_table _prefs (_self, account.value);
  auto prefs_itr = _prefs.find(org.value);
  check (prefs_itr != _prefs.end(), "<account> has not allowed <org> to issue badges");
}


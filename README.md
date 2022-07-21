# eosiobadges

design considerations - 
minimum code write for orgs using badges.
minimum need of redeploy of smart contracts.
ram to be payed by org whose members issue the badges.

## CONTRACT - profile 

### TABLES
#### authorized
#### badgedata
#### achievements
### ACTIONS
#### authcontract (name org, name trusted_contract)
##### requires authority of org.
#### initbadge(name org, name badge, string ipfs, string details)
##### requires authority of one of the trusted_contracts
#### achievement (name org, name badge, name account, uint8_t count)
##### requires authority of one of the trusted_contracts

## CONTRACT - org

### tables
### actions

## CONTRACT - orgbill

### tables
### actions

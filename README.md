# eosiobadges

## Intro
This suite of smart contracts implements a badge distribution system. Badge is a non-transferable token. 

Badges are best defined in badge specific smart contracts. Badge contracts must have interfaces defined for initializing a badge and issuing/claiming it. 

3 Badge contracts that are defined in this suite are -

Simple Badge - Simple Badge smart contract allows any account to initializr and issue a simple badge. When a simple badge is issued, all its parent badges are also issued to an account.

Gotcha Badge - Gotcha badges are defined with a max issuance supply and duration cycle. At the end of the duration cycle, issuance capacity for an account is restored to max.

Rollup Badge -  Rollup badge is defined by setting up one or more [badge, count] combinations. An account can claim a rollup badge, if it holds badges for all defined combinations.

Cumulative account balance of badges is maintained in profile smart contract and granular per badge information can be maintained in Atomic Assets. 

cleos  --url http://jungle2.cryptolions.io:80  get account eddytest1111 | grep liquid;
cleos  --url http://jungle2.cryptolions.io:80  get account eddytest1114 | grep liquid;
cleos  --url http://jungle2.cryptolions.io:80  get table eddytest1114 eddytest1114 bankbullsf;
cleos  --url http://jungle2.cryptolions.io:80 push action eosio.token  transfer '[ "eddytest1111", "eddytest1114", "0.8500 EOS", "rock" ]' -p eddytest1111@active;
cleos  --url http://jungle2.cryptolions.io:80  get account eddytest1111 | grep liquid;
cleos  --url http://jungle2.cryptolions.io:80  get account eddytest1114 | grep liquid;
cleos  --url http://jungle2.cryptolions.io:80  get table eddytest1114 eddytest1114 bankbullsf;


##清理table
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 tableclear '["all"]' -p eddytest1114@active;

##初始化设置项
##key 必须是"timestop","timegame","timeround","minupbank","mindownbank","limitplybank","limitsysbank","sizegames","sizerounds"
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "timestop","25"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "timegame","45"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "timeround","720"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "minupbank","500000"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "mindownbank","100000"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "limitplybank","1000000"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "limitsysbank","500000"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "sizegames","100"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "sizerounds","100"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "lastlogs2id","0"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 delstate '[ "sizerounds"]' -p eddytest1114@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ "sizerounds","100"]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 get  table eddytest1114  eddytest1114 globalstate





cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1111","eddytest1114","3.1200 EOS","resbank,1"]' -p eddytest1111@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1112","eddytest1114","1.1200 EOS","resbank,1"]' -p eddytest1112@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startround '[]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1114@active;

cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1113","eddytest1114","2.1200 EOS","resbank,1"]' -p eddytest1113@active;

cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1113","eddytest1114","0.3000 EOS","bet,1,eddytest1113,0.1000\ EOS,0.0400\ EOS,0.0300\ EOS,0.0200\ EOS,0.0100\ EOS,SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1113@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1115","eddytest1114","3.0000 EOS","bet,1,eddytest1115,1.0000\ EOS,0.4000\ EOS,0.3000\ EOS,0.2000\ EOS,0.1000\ EOS,SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1115@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 stopbet '[]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active;
cleos  --url http://jungle3.cryptolions.io:80 push action eddytest1114 sltgame '["1","1"]' -p eddytest1114@active;
#####cleos  --url http://jungle3.cryptolions.io:80 push action eddytest1114 sltround '["2","1"]' -p eddytest1114@active;

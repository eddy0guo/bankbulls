cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 setstate '[ max_amount, 10]' -p eddytest1111@active;
cleos  --url http://jungle3.cryptolions.io:80  get table eddytest1114 eddytest1114  globalstate
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 delstate '[ max_amount]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80  get table eddytest1114 eddytest1114  globalstate
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 testsign '[ "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7" ]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startround '[]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 currounda
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 resbank '[eddytest1111, 100.0000 EOS, 1]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 resbankers
##预约下庄，减仓，取消预约
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 resbank '["eddytest1111", "100.0000 EOS", "2"]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 resbank '["eddytest1111", "100.0000 EOS", "4"]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 resbank '["eddytest1111", "100.0000 EOS", "5"]' -p eddytest1111@active

##预约上庄,加仓
cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1111","eddytest1114","3.1200 EOS","resbank,1"]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1111","eddytest1114","3.1200 EOS","resbank,3"]' -p eddytest1111@active

cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 resbankers
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 game
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startbet '["1000", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1114@active
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 game
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 bets 
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 bet '["11","eddytest1111", "100.0000 EOS", "25.0000 EOS","25.0000 EOS","25.0000 EOS","25.0000 EOS","SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 bets

cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 game
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 stopbet '[]' -p eddytest1114@active
cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 game

cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","1","2" ]' -p eddytest1114@active


## 开轮-开局-下注-停局-开牌
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startround '[]' -p eddytest1111@active

cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 startbet '["1000", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1114@active

cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 game1


##cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 bet '["11","eddytest1111", "100.0000 EOS", "25.0000 EOS","25.0000 EOS","25.0000 EOS","25.0000 EOS","SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1111@active
##cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 bet '["11","eddytest1111", "100.0000 EOS", "25.0000 EOS","25.0000 EOS","25.0000 EOS","25.0000 EOS","SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1111@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eosio.token  transfer '["eddytest1111","eddytest1114","3.0000 EOS","bet,1,eddytest1111,1.0000\ EOS,0.2500\ EOS,0.2500\ EOS,0.2200\ EOS,0.2800\ EOS,SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1111@active


cleos  --url http://jungle3.cryptolions.io:80 get table eddytest1114 eddytest1114 bets1

cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 stopbet '[]' -p eddytest1114@active

cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KYXtZhyopFxrvYF8shpoAgJGCh4xgLSKkRmCon3z29n1o5uw9MDgxccus8RsAFst6GBfi29us2tGtCQuH3i38QZikzXRHP", "5b5f53bed32f36e093559f52accc454eaf27c9d3ca7cb4b081c63d6581f50733","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KgEv2azTirpMw36Aj4PfYVit5wURaHkzg97m9pzBSRsST5rhHFY8M8KH3xPGTXX3JKiYWQ3cv7DDDyQvTnxmmgcwWTmomi", "f47502ab2e09fe23b015011538c9a06f660a19bd68b903a24f4f8159389fdb77","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active

cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KB2pjqTbwDKcLNmrttaRE24ap9rvoeraW6zfjFoxq63vpvuZ3c3P1e6n6q81EAJzEJiYb5G5ZGA5E2SEi4vfeBWwQ3Pt1x", "45e248b69258369fa2e1687d443f5765065666e0402e007e84bb5136dcabb1f0","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active
##niuniu
cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 opencards '[ "SIG_K1_KdQ5UXg9QRbWmZkcKk52mQ61RFotTeRZrvDRWxscJh46scjwSupv3GZvrVabZSVCWAHessWtnkAdy59refAjs6jFiFfGA5", "c44f0971f10b145a22d02d904c98f75c2e77cc00bcc38cbaa98877c323a19cd1","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1114@active
## 第二个参数只是标记没用到
cleos  --url http://jungle3.cryptolions.io:80 push action eddytest1114 sltgame '["1","1"]' -p eddytest1114@active

##清数据
 cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 tableclear '["all"]' -p eddytest1114@active
 cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1114 tableclear '["game"]' -p eddytest1114@active

#!/bin/bash
##所有的测试都需要的前置

catch_failed() {
  if [ $? -eq 1 ]; then
    echo -e "increase banker failed and catch it successfully \n\n\n"
    return
  elif [ $? -eq 0 ]; then
    echo "should n't successful"
    exit
  else
    echo ""
    exit
  fi
}
init() {
  ##set -xve
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 tableclear '["all"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minupbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "lastlogs2id","0"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","0.6200 EOS","resbank|eddytest1121|3.12|1"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1122","eddytest1124","1.1200 EOS","resbank|eddytest1122|1.12|1"]' -p eddytest1122@active 1>/dev/null
  :'
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "timestop","26"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "timegame","46"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "timeround","721"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "minupbank","500001"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "mindownbank","100001"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "limitplybank","1000001"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "limitsysbank","500001"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "sizegames","101"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "sizerounds","101"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "lastlogs2id","1"]' -p eddytest1124@active;
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 delstate '[ "sizerounds"]' -p eddytest1124@active
  cleos  --url http://jungle3.cryptolions.io:80 push action  eddytest1124 setstate '[ "sizerounds","101"]' -p eddytest1124@active;
  '
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["1","1"]' -p eddytest1124@active 1>/dev/null

}

init2() {
  set -xv
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 tableclear '["all"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitsysbank","10000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minupbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "lastlogs2id","0"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","50.6200 EOS","resbank|eddytest1121|3.12|1"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1122","eddytest1124","100.1200 EOS","resbank|eddytest1122|1.12|1"]' -p eddytest1122@active 1>/dev/null
}
cancel_game() {
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1123","eddytest1124","0.3000 EOS","bet|1|eddytest1123|0.1000\ EOS|0.0400\ EOS|0.0300\ EOS|0.0200\ EOS|0.0100\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1123@active
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1125","eddytest1124","3.0000 EOS","bet|1|eddytest1125|1.0000\ EOS|0.4000\ EOS|0.3000\ EOS|0.2000\ EOS|0.1000\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1125@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 stopbet '[]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1124@active
  echo "records account info before test cancel game"
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1124 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1123 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1125 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 games
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 bets
  echo "start cancle game"
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 cancelgame '[]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["1","1"]' -p eddytest1124@active
  echo "start cancle game and sltgame"
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 games
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 bets
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1124 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1123 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1125 | grep liquid
}

cancel_round() {
  echo "cancel_round"
}
res_up_banker() {
  echo "res_up_banker"
}
res_down_banker() {
  echo "res_down_banker"
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1121 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs2
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["2","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1121 | grep liquid
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1122", "100.0000 EOS", "2"]' -p eddytest1122@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "100.0000 EOS", "2"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["2","1"]' -p eddytest1124@active

  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs2
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs
  cleos --url http://jungle3.cryptolions.io:80 get account eddytest1121 | grep liquid
  ##todo:最后1111的余额等于之前轮次结算前的余额 加上轮次的派彩
}
res_increase_banker() {
  echo "res_increase_banker"
}

res_reduce_banker() {
  echo "res_reduce_banker"
}

params_limit_test() {
  echo -e "params_limit_test"
  ##set -xv;
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 tableclear '["all"]' -p eddytest1124@active 1>/dev/null
  sleep 2
  ##默认值50eos这里会报错
  ##上庄金额需大于50 EOS。
  echo -e "\n\n\n                 should be failed "
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","3.1200 EOS","resbank|eddytest1121|3.12|1"]' -p eddytest1121@active 1>/dev/null
  catch_failed
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minupbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","3.1200 EOS","resbank|eddytest1121|3.12|1"]' -p eddytest1121@active 1>/dev/null

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["1","1"]' -p eddytest1124@active 1>/dev/null
  ##测试玩一项就清除，使用默认值
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 delstate '[ "minupbank"]' -p eddytest1124@active

  ##预约上调下调
  ##预约加仓金额不可低于50EOS。
  ##预约减仓金额不可高于上庄余额且不可低于5EOS，减仓生效后若余额低于50EOS将自动下庄。
  ##上调小于阀值
  echo -e "\n\n\n                 should be failed "
  up_result=$(cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","3.1200 EOS","resbank|eddytest1121|3.12|3"]' -p eddytest1121@active)
  catch_failed
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minplusbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","3.1200 EOS","resbank|eddytest1121|3.12|3"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "10.0000 EOS", "5"]' -p eddytest1121@active

  ##下调应小于上庄余额减去庄最小额度 2.62 = 3.12 + 0 - 0.5
  ##下调调整区间为 (0.1 2.62)
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitsysbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minminusbank","1000"]' -p eddytest1124@active
  echo -e "\n\n\n                 should be failed "
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "2.6300 EOS", "4"]' -p eddytest1121@active 1>/dev/null
  catch_failed
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "2.6200 EOS", "4"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "10.0000 EOS", "5"]' -p eddytest1121@active

  ##下调应大于阀值
  echo -e "\n\n\n                 should be failed "
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "0.0900 EOS", "4"]' -p eddytest1121@active 1>/dev/null
  catch_failed
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "0.1000 EOS", "4"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "10.0000 EOS", "5"]' -p eddytest1121@active

}

complete_game_once() {
  echo "complete_game_once"
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe","EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1123","eddytest1124","0.3000 EOS","bet|1|eddytest1123|0.1000\ EOS|0.0400\ EOS|0.0300\ EOS|0.0200\ EOS|0.0100\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1123@active 1>/dev/null
  sleep 2
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1125","eddytest1124","3.0000 EOS","bet|1|eddytest1125|1.0000\ EOS|0.4000\ EOS|0.3000\ EOS|0.2000\ EOS|0.1000\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1125@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 stopbet '[]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1124@active 1>/dev/null
}

complete_game_once2() {
  current_id=$1
  echo "  complete_game_once current_id=  "$current_id
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe","EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7"]' -p eddytest1124@active 1>/dev/null
  sleep 20 ##fixme规避created_at一样的情况
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1123","eddytest1124","0.3000 EOS","bet|'$current_id'|eddytest1123|0.1000\ EOS|0.0400\ EOS|0.0300\ EOS|0.0200\ EOS|0.0100\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1123@active 1>/dev/null
  sleep 2 ##fixme规避created_at一样的情况
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1125","eddytest1124","3.0000 EOS","bet|'$current_id'|eddytest1125|1.0000\ EOS|0.4000\ EOS|0.3000\ EOS|0.2000\ EOS|0.1000\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1125@active 1>/dev/null
  ## cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 stopbet '[]' -p eddytest1124@active
  sleep 21 ## opencard 等stopbet完成后进行也就是等25s
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1124@active
  ## cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["2","1"]' -p eddytest1124@active 1>/dev/null
  sleep 5
}

: <<!
在只有用户庄的时候更新成limit_system_bank的设置值
##或者有用户参与的时候更新为新设置的limit_player_bank设置值
!
set_game_limit() {
  echo "res_reduce_banker"
  complete_game_once
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitplybank","100000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 globalstate
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["2","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1122", "100.0000 EOS", "2"]' -p eddytest1122@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 resbank '["eddytest1121", "100.0000 EOS", "2"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["2","1"]' -p eddytest1124@active

  ##等入块
  sleep 1
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitsysbank","99999"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 globalstate
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 logs
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 curround

}

case $1 in
##测试流局
cancel_game)
  echo "-b----$1"
  init
  cancel_game
  ;;
##测试下庄之后，轮次结算
res_down_banker)
  init
  complete_game_once
  res_down_banker
  ;;
##测试下庄之后，轮次结算
res_reduce_banker)
  init
  complete_game_once
  res_down_banker
  ;;
complete_game_once)
  init
  complete_game_once
  ;;
set_game_limit)
  init
  set_game_limit
  ;;
##各种阀值报错设置
params_limit_test)
  params_limit_test
  ;;
loop_round)
  init2
  id=0
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  while true; do
    ((id++))
    complete_game_once2 $id
  done
  :'
  while true; do
    round_start_time=$(date +%s)
    declare -i round_stop_time=$round_start_time+120
    echo "round_start_time"$round_start_time
    echo "round_stop_time"$round_stop_time
    ##cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
    ##cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["2","1"]' -p eddytest1124@active
    while true; do
      ((id++))
      complete_game_once2 $id
      current_time=$(date +%s)
      if [ $current_time -ge $round_stop_time ]; then
        echo "round stop at"$current_time
        echo "round_stop_time"$round_stop_time
        break
      fi
    done
  done
'
  ;;
test)
  init
  ;;
#todo:如果系统没坐庄，若某局投注总额＞玩家庄池*1.5%，为不影响大额玩家体验，系统将参与补庄，并与玩家庄按份额共担盈亏。
system_join_bankers)
  set -xv
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 tableclear '["all"]' -p eddytest1124@active 1>/dev/null

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "lastlogs2id","0"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minupbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitsysbank","10000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1121","eddytest1124","15.0000 EOS","resbank|eddytest1121|3.12|1"]' -p eddytest1121@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1122","eddytest1124","15.0000 EOS","resbank|eddytest1122|1.12|1"]' -p eddytest1122@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["1","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 curround

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe","EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1123","eddytest1124","3.0000 EOS","bet|1|eddytest1123|1.0000\ EOS|0.2500\ EOS|0.2500\ EOS|0.2500\ EOS|0.2500\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1123@active 1>/dev/null
  sleep 2 ##fixme规避created_at一样的情况
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1125","eddytest1124","0.3000 EOS","bet|1|eddytest1125|0.1000\ EOS|0.0250\ EOS|0.0250\ EOS|0.0250\ EOS|0.0250\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1125@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 stopbet '[]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["2","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 curround
  ;;
only_sys_banker)
  set -xv
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 tableclear '["all"]' -p eddytest1124@active 1>/dev/null

  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "lastlogs2id","0"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "minupbank","5000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 setstate '[ "limitsysbank","10000"]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startround '[]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltround '["1","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 curround
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 startbet '["1", "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe","EOS67sxYKu379Q9QLb3ERgza8oCN21Z3Rr3FDFii1gzj8iDGifxE7"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1123","eddytest1124","3.0000 EOS","bet|1|eddytest1123|1.0000\ EOS|0.2500\ EOS|0.2500\ EOS|0.2500\ EOS|0.2500\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1123@active 1>/dev/null
  sleep 2 ##fixme规避created_at一样的情况
  cleos --url http://jungle3.cryptolions.io:80 push action eosio.token transfer '["eddytest1125","eddytest1124","0.3000 EOS","bet|1|eddytest1125|0.1000\ EOS|0.0250\ EOS|0.0250\ EOS|0.0250\ EOS|0.0250\ EOS|SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe"]' -p eddytest1125@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 stopbet '[]' -p eddytest1124@active
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 opencards '[ "SIG_K1_KjwqmE98ZPvEjFPw2MnrTHqrbmeXgMgFsBcNcttPHwZWR1kPemafWKkXQc1yZecq6bHdTAWtvpBC3X5u6Ggh2x9i1PvrKe", "778fea5af7567ecfa776f6a379eeed5103e705588fd9dd3ed15e118899358479","10000","06494dc1b567337bcb8bce2aef584494e1f43cf0e83e69177c8c154133608281" ]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 push action eddytest1124 sltgame '["2","1"]' -p eddytest1124@active 1>/dev/null
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 upbankers
  cleos --url http://jungle3.cryptolions.io:80 get table eddytest1124 eddytest1124 curround
  ;;
--help)
  echo "./test.sh   cancel_game"
  echo "./test.sh   res_down_banker"
  echo "./test.sh   res_reduce_banker"
  echo "./test.sh   complete_game_once"
  echo "./test.sh   set_game_limit"
  echo "./test.sh   params_limit_test"
  echo "./test.sh   loop_round"
  echo "./test.sh   system_join_bankers"
  echo "./test.sh   only_sys_banker"
  echo "./test.sh   test"
  ;;
esac

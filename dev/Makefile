finger:
	eosio-cpp -I=../include ../src/bankbulls.cpp -o bankbulls/bankbulls.wasm

install:
	-cleos  --url http://jungle3.cryptolions.io:80 wallet unlock --password PW5KRs4fjCsBFL3YZev4beobmdsYEeNFqtxC1cK3Q5Eb3E6eiuMKe;
	cleos  --url http://jungle3.cryptolions.io:80 set contract eddytest1124 bankbulls -p eddytest1124@active;

clean:
	rm -rf bankbulls/*

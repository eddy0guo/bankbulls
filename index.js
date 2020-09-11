//let {PrivateKey, PublicKey, Signature, Aes, key_utils, config} = require('eosjs-ecc')
const ecc = require('eosjs-ecc')
const random = require('string-random')
const crypto = require('crypto');
const fmz = require("fmz")


async function test() {
    // const priv = await ecc.unsafeRandomKey()
    // console.log(priv)

    const seed = await ecc.seedPrivate('324eddy778guo111133')
    //const seed = await ecc.seedPrivate('mysecretseeddds')

    console.log("seed=", seed);

    console.log(random(64));

    var task = fmz.VCtx({
        start: '2019-09-06 00:00:00',
        end: '2019-10-05 00:00:00',
        period: '1h',
        exchanges: [{"eid":"Bitfinex","currency":"BTC_USD"}]
    });
    console.log(exchange.GetTicker());
    exchange.GetAccount()
    /**
     let obj=crypto.createHash('sha256');
     obj.update(seed);
     let  str=obj.digest('hex');//hex是十六进制
     console.log("ramdom sha256 ",str);
     **/

    let obj = crypto.createHash('sha512');
    obj.update(seed);
    let str = obj.digest('hex');//hex是十六进制
    console.log("ramdom sha512 ", str);


    let priv = '5KKsde3FM6dydSbrf742sQY3ukvWjPeM51zDK7JcPL8AHJqhAuB'
    console.log("before ", Date.now());
    let pub = ecc.privateToPublic(priv)
    console.log("after ", Date.now());
    console.log(pub)
    let signature = ecc.sign(seed, priv)
    console.log("signature=", signature)
    console.log("signature-%d", 3)
}

test();

#include <linux/crypto.h>
#include <linux/scatterlist.h>

void do_sha1(const unsigned char *data, u8 *hashval)
{
    struct scatterlist sg;
    struct hash_desc desc;
    sg_init_one(&sg, data, PAGE_SIZE);
    desc.tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);

    crypto_hash_init(&desc);
    crypto_hash_update(&desc, &sg, PAGE_SIZE);
    crypto_hash_final(&desc, hashval);

    crypto_free_hash(desc.tfm);
}

// struct sdesc {
//     struct shash_desc shash;
//     char ctx[];
// };

// static struct sdesc *init_sdesc(struct crypto_shash *alg)
// {
//     struct sdesc *sdesc;
//     int size;

//     size = sizeof(struct shash_desc) + crypto_shash_descsize(alg);
//     sdesc = kmalloc(size, GFP_KERNEL);
//     if (!sdesc)
//         return ERR_PTR(-ENOMEM);
//     sdesc->shash.tfm = alg;
//     return sdesc;
// }

// static int calc_hash(struct crypto_shash *alg,
//              const unsigned char *data, unsigned int datalen,
//              unsigned char *digest)
// {
//     struct sdesc *sdesc;
//     int ret;

//     sdesc = init_sdesc(alg);
//     if (IS_ERR(sdesc)) {
//         pr_info("can't alloc sdesc\n");
//         return PTR_ERR(sdesc);
//     }

//     ret = crypto_shash_digest(&sdesc->shash, data, datalen, digest);
//     kfree(sdesc);
//     return ret;
// }

// int do_sha256(const unsigned char *data, unsigned char *out_digest)
// {
//     struct crypto_shash *alg;
//     char *hash_alg_name = "sha256";
//     //unsigned int datalen = sizeof(data) - 1; // remove the null byte
//     unsigned int datalen = PAGE_SIZE;

//     alg = crypto_alloc_shash(hash_alg_name, 0, 0);
//     if(IS_ERR(alg)){
//         pr_info("can't alloc alg %s\n", hash_alg_name);
//         return PTR_ERR(alg);
//     }
//     calc_hash(alg, data, datalen, out_digest);

//     // Very dirty print of 8 first bytes for comparaison with sha256sum
//     printk(KERN_INFO "HASH(%s, %i): %02x%02x%02x%02x%02x%02x%02x%02x\n",
//           data, datalen, out_digest[0], out_digest[1], out_digest[2], out_digest[3], out_digest[4], 
//           out_digest[5], out_digest[6], out_digest[7]);

//     crypto_free_shash(alg);

//     out_digest[256] = 0;
//     return 0;
// }
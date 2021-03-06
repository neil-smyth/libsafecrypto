/*****************************************************************************
 * Copyright (C) Queen's University Belfast, ECIT, 2016                      *
 *                                                                           *
 * This file is part of libsafecrypto.                                       *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

#include <stdlib.h>
#include <check.h>
#include "safecrypto.h"
#include "safecrypto_private.h"
#include "safecrypto_version.h"
#include "utils/crypto/prng_types.h"

START_TEST(test_safecrypto_create_private)
{
	int32_t retcode;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_create_public)
{
    int32_t retcode;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_create_unknown)
{
    safecrypto_t *sc;
    UINT32 flags[1] = {0};

    sc = safecrypto_create(SC_SCHEME_NONE, 0, flags);
    ck_assert_ptr_eq(sc, NULL);
}
END_TEST

START_TEST(test_safecrypto_create_null_string)
{
    safecrypto_t *sc;
    UINT32 flags[1] = {0};

    sc = safecrypto_create(78, 0, flags);
    ck_assert_ptr_eq(sc, NULL);
}
END_TEST

START_TEST(test_safecrypto_initial_api)
{
    int32_t retcode;
    uint32_t version, errcode;
    const char *version_str;
    const char *invocation_str;
    char version_str_actual[32] = {0};
    char version_str_check[32];
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    sc_debug_level_e level;

    sprintf(version_str_actual, "%d.%d.%d.%d    [",
        MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION, PATCH_VERSION);

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    version = safecrypto_get_version();
    ck_assert_int_eq(version, ( MAJOR_VERSION << 24 ) |
                              ( MINOR_VERSION << 16 ) |
                              ( BUILD_VERSION <<  8 ) |
                              ( PATCH_VERSION       ));
    version_str = safecrypto_get_version_string();
    strncpy(version_str_check, version_str, strlen(version_str_actual));
    version_str_check[strlen(version_str_actual)] = 0;
    ck_assert_str_eq(version_str_check, version_str_actual);
    ck_assert_str_eq(version_str+strlen(version_str)-1, "]");

    invocation_str = safecrypto_get_configure_invocation();
    ck_assert_str_eq(invocation_str, CONFIGURE_INVOCATION);

    level = safecrypto_get_debug_level(sc);
#ifdef DEBUG
    ck_assert_int_eq(level, SC_LEVEL_DEBUG);
#else
    ck_assert_int_eq(level, SC_LEVEL_NONE);
#endif

    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_OK);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_get_signature_schemes)
{
    const sc_pkc_scheme_t* schemes = safecrypto_get_signature_schemes();
    while (NULL != schemes) {
#if !defined(DISABLE_SIGNATURES)
        int valid = 0;
#if !defined(DISABLE_SIG_BLISS_B)
        valid |= SC_SCHEME_SIG_BLISS == schemes->scheme;
#endif
#if !defined(DISABLE_SIG_DILITHIUM)
        valid |= SC_SCHEME_SIG_DILITHIUM == schemes->scheme;
#endif
#if !defined(DISABLE_SIG_DILITHIUM_G)
        valid |= SC_SCHEME_SIG_DILITHIUM_G == schemes->scheme;
#endif
#if !defined(DISABLE_SIG_RING_TESLA)
        valid |= SC_SCHEME_SIG_RING_TESLA == schemes->scheme;
#endif
#if !defined(DISABLE_SIG_ENS)
        valid |= SC_SCHEME_SIG_ENS == schemes->scheme || SC_SCHEME_SIG_ENS_WITH_RECOVERY == schemes->scheme;
#endif
#if !defined(DISABLE_SIG_DLP)
        valid |= SC_SCHEME_SIG_DLP == schemes->scheme || SC_SCHEME_SIG_DLP_WITH_RECOVERY == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);
#endif

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_encryption_schemes)
{
    const sc_pkc_scheme_t* schemes = safecrypto_get_encryption_schemes();
    while (NULL != schemes) {
#if !defined(DISABLE_ENCRYPTION)
        int valid = 0;
#if !defined(DISABLE_ENC_RLWE)
        valid |= SC_SCHEME_ENC_RLWE == schemes->scheme;
#endif
#if !defined(DISABLE_ENC_KYBER)
        valid |= SC_SCHEME_ENC_KYBER_CPA == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);
#endif

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_kem_schemes)
{
    const sc_pkc_scheme_t* schemes = safecrypto_get_kem_schemes();
    while (NULL != schemes) {
#if !defined(DISABLE_SIGNATURES)
        int valid = 0;
#if !defined(DISABLE_KEM_ENS)
        valid |= SC_SCHEME_KEM_ENS == schemes->scheme;
#endif
#if !defined(DISABLE_KEM_KYBER)
        valid |= SC_SCHEME_KEM_KYBER == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);
#endif

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_ibe_schemes)
{
    const sc_pkc_scheme_t* schemes = safecrypto_get_ibe_schemes();
    while (NULL != schemes) {
#if !defined(DISABLE_SIGNATURES)
        int valid = 0;
#if !defined(DISABLE_IBE_DLP)
        valid |= SC_SCHEME_IBE_DLP == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);
#endif

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_hash_schemes)
{
    const sc_hash_t* schemes = safecrypto_get_hash_schemes();
    while (NULL != schemes) {
        int valid = 0;
#if defined(ENABLE_SHA2)
        valid |= SC_HASH_SHA2_512 == schemes->scheme;
        valid |= SC_HASH_SHA2_384 == schemes->scheme;
        valid |= SC_HASH_SHA2_256 == schemes->scheme;
        valid |= SC_HASH_SHA2_224 == schemes->scheme;
#endif
#if defined(ENABLE_SHA3)
        valid |= SC_HASH_SHA3_512 == schemes->scheme;
        valid |= SC_HASH_SHA3_384 == schemes->scheme;
        valid |= SC_HASH_SHA3_256 == schemes->scheme;
        valid |= SC_HASH_SHA3_224 == schemes->scheme;
#endif
#if defined(ENABLE_BLAKE2)
        valid |= SC_HASH_BLAKE2_512 == schemes->scheme;
        valid |= SC_HASH_BLAKE2_384 == schemes->scheme;
        valid |= SC_HASH_BLAKE2_256 == schemes->scheme;
        valid |= SC_HASH_BLAKE2_224 == schemes->scheme;
#endif
#if defined(ENABLE_WHIRLPOOL)
        valid |= SC_HASH_WHIRLPOOL_512 == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_xof_schemes)
{
    const sc_xof_t* schemes = safecrypto_get_xof_schemes();
    while (NULL != schemes) {
        int valid = 0;
#if defined(ENABLE_SHA3)
        valid |= SC_XOF_SHAKE256 == schemes->scheme;
        valid |= SC_XOF_SHAKE128 == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);

        schemes = schemes->next;
    }
}
END_TEST

START_TEST(test_safecrypto_get_prng_schemes)
{
    const sc_prng_t* schemes = safecrypto_get_prng_schemes();
    while (NULL != schemes) {
        int valid = 0;
#if defined(SC_PRNG_SYSTEM)
        valid |= SC_PRNG_SYSTEM == schemes->scheme;
#endif
#if defined(SC_PRNG_AES_CTR_DRBG)
        valid |= SC_PRNG_AES_CTR_DRBG == schemes->scheme;
#endif
#if defined(SC_PRNG_AES_CTR)
        valid |= SC_PRNG_AES_CTR == schemes->scheme;
#endif
#if defined(SC_PRNG_CHACHA)
        valid |= SC_PRNG_CHACHA == schemes->scheme;
#endif
#if defined(SC_PRNG_SALSA)
        valid |= SC_PRNG_SALSA == schemes->scheme;
#endif
#if defined(SC_PRNG_ISAAC)
        valid |= SC_PRNG_ISAAC == schemes->scheme;
#endif
#if defined(SC_PRNG_KISS)
        valid |= SC_PRNG_KISS == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_SHA2_256)
        valid |= SC_PRNG_HASH_DRBG_SHA2_256 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_SHA2_512)
        valid |= SC_PRNG_HASH_DRBG_SHA2_512 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_SHA3_256)
        valid |= SC_PRNG_HASH_DRBG_SHA3_256 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_SHA3_512)
        valid |= SC_PRNG_HASH_DRBG_SHA3_512 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_BLAKE2_256)
        valid |= SC_PRNG_HASH_DRBG_BLAKE2_256 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_BLAKE2_512)
        valid |= SC_PRNG_HASH_DRBG_BLAKE2_512 == schemes->scheme;
#endif
#if defined(SC_PRNG_HASH_DRBG_WHIRLPOOL_512)
        valid |= SC_PRNG_HASH_DRBG_WHIRLPOOL_512 == schemes->scheme;
#endif
#if defined(SC_PRNG_FILE)
        valid |= SC_PRNG_FILE == schemes->scheme;
#endif
#if defined(SC_PRNG_HIGH_ENTROPY)
        valid |= SC_PRNG_HIGH_ENTROPY == schemes->scheme;
#endif
        ck_assert_int_ne(valid, 0);

        schemes = schemes->next;
    }
}
END_TEST

static void prng_cb_fn(size_t len, UINT8 *data)
{
    size_t i;
    for (i=len; i--;) {
        data[i] = i;
    }
}

START_TEST(test_safecrypto_prng_create)
{
    safecrypto_prng_t *ctx;
    ctx = safecrypto_prng_create(SC_PRNG_MAX, 0, prng_cb_fn);
    ck_assert_ptr_eq(ctx, NULL);
    ctx = safecrypto_prng_create(SC_PRNG_AES_CTR_DRBG, 0, prng_cb_fn);
    ck_assert_ptr_eq(ctx, NULL);
    ctx = safecrypto_prng_create(SC_PRNG_AES_CTR_DRBG, 0x10000000, prng_cb_fn);
    ck_assert_ptr_ne(ctx, NULL);
    safecrypto_prng_destroy(ctx);
}
END_TEST

START_TEST(test_safecrypto_prng_32)
{
    UINT32 data[4] = {};
    safecrypto_prng_t *ctx;
    ctx = safecrypto_prng_create(SC_PRNG_AES_CTR_DRBG, 0x10000000, prng_cb_fn);
    ck_assert_ptr_ne(ctx, NULL);
    data[0] = safecrypto_prng_32(ctx);
    data[1] = safecrypto_prng_32(ctx);
    data[2] = safecrypto_prng_32(ctx);
    data[3] = safecrypto_prng_32(ctx);
    ck_assert_int_ne(data[0], 0);
    ck_assert_int_ne(data[0] ^ data[1] ^ data[2] ^ data[3], 0);
    safecrypto_prng_destroy(ctx);
}
END_TEST

START_TEST(test_safecrypto_aes_create)
{
    SINT32 retval;
    UINT8 key[16];
    safecrypto_aes_t *ctx;
    ctx = safecrypto_aes_create(SC_AES_MAX, NULL);
    ck_assert_ptr_eq(ctx, NULL);
    ctx = safecrypto_aes_create(SC_AES_ENCRYPT_128, NULL);
    ck_assert_ptr_eq(ctx, NULL);
    retval = safecrypto_aes_destroy(NULL);
    ck_assert_int_eq(retval, SC_FUNC_FAILURE);

    ctx = safecrypto_aes_create(SC_AES_ENCRYPT_128, key);
    ck_assert_ptr_ne(ctx, NULL);
    retval = safecrypto_aes_destroy(ctx);
    ck_assert_int_eq(retval, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_initial_api_multiple)
{
    int32_t retcode;
    uint32_t errcode;
    safecrypto_t *sc1, *sc2;
    UINT32 flags[1] = {0};
    sc_debug_level_e level;

    sc1 = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc1, NULL);
    sc2 = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc2, NULL);

#ifdef DEBUG
    retcode = safecrypto_set_debug_level(sc1, SC_LEVEL_ERROR);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_set_debug_level(sc2, SC_LEVEL_WARNING);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);

    level = safecrypto_get_debug_level(sc1);
    ck_assert_int_eq(level, SC_LEVEL_ERROR);
    level = safecrypto_get_debug_level(sc2);
    ck_assert_int_eq(level, SC_LEVEL_WARNING);
#else
    retcode = safecrypto_set_debug_level(sc1, SC_LEVEL_ERROR);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    retcode = safecrypto_set_debug_level(sc2, SC_LEVEL_WARNING);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    level = safecrypto_get_debug_level(sc1);
    ck_assert_int_eq(level, SC_LEVEL_NONE);
    level = safecrypto_get_debug_level(sc2);
    ck_assert_int_eq(level, SC_LEVEL_NONE);
#endif

    errcode = safecrypto_err_get_error(sc1);
    ck_assert_uint_eq(errcode, SC_OK);

    retcode = safecrypto_destroy(sc1);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_destroy(sc2);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_initial_api_null)
{
    int32_t retcode;
    uint32_t errcode;
    safecrypto_t *sc1, *sc2;
    UINT32 flags[1] = {0};
    sc_debug_level_e level;

    sc1 = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc1, NULL);
    sc2 = NULL;

#ifdef DEBUG
    retcode = safecrypto_set_debug_level(sc1, SC_LEVEL_ERROR);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_set_debug_level(sc2, SC_LEVEL_WARNING);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    level = safecrypto_get_debug_level(sc1);
    ck_assert_int_eq(level, SC_LEVEL_ERROR);
    level = safecrypto_get_debug_level(sc2);
    ck_assert_int_eq(level, SC_LEVEL_NONE);
#else
    retcode = safecrypto_set_debug_level(sc1, SC_LEVEL_ERROR);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    retcode = safecrypto_set_debug_level(sc2, SC_LEVEL_WARNING);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    level = safecrypto_get_debug_level(sc1);
    ck_assert_int_eq(level, SC_LEVEL_NONE);
    level = safecrypto_get_debug_level(sc2);
    ck_assert_int_eq(level, SC_LEVEL_NONE);
#endif

    errcode = safecrypto_err_get_error(sc1);
    ck_assert_uint_eq(errcode, SC_OK);
    errcode = safecrypto_err_get_error(sc2);
    ck_assert_uint_eq(errcode, SC_GETERR_NULL_POINTER);

    retcode = safecrypto_destroy(sc1);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_destroy(sc2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
}
END_TEST

START_TEST(test_safecrypto_initial_api_temp_ram)
{
    int32_t retcode;
    uint32_t errcode;
    size_t scratch_len;
    uint8_t *m, *sigret, *scratch;
    size_t m_len, siglen;
    safecrypto_t *sc;
    UINT32 flags[3] = {SC_FLAG_MORE, SC_FLAG_MORE, SC_FLAG_2_MEMORY_TEMP_EXTERNAL};
    sc_debug_level_e level;

    m = malloc(1024);
    sigret = malloc(1024);
    scratch = malloc(1024);

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_scratch_size(sc, &scratch_len);
    ck_assert_uint_eq(retcode, SC_FUNC_SUCCESS);
    ck_assert_uint_eq(scratch_len, 0);

    // A call to any API function other than those involved in configuring the
    // scratch memory will result in failure
    retcode = safecrypto_sign(sc, m, m_len, &sigret, &siglen);
    ck_assert_ptr_ne(sigret, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    // Now if the external scratch memory is assigned the API allows functions to be called
    retcode = safecrypto_scratch_size(sc, &scratch_len);
    ck_assert_uint_eq(retcode, SC_FUNC_SUCCESS);
    ck_assert_uint_eq(scratch_len, 0);
    retcode = safecrypto_scratch_external(sc, scratch, 1024);
    ck_assert_uint_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_sign(sc, m, m_len, &sigret, &siglen);
    ck_assert_ptr_ne(sigret, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);

    free(m);
    free(sigret);
    free(scratch);
}
END_TEST

START_TEST(test_safecrypto_keys_load)
{
    SINT32 retcode;
    UINT32 errcode;
    size_t len;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    UINT8 *dummy = (UINT8*) flags;

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_public_key_load(sc, NULL, 0);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_private_key_load(sc, NULL, 0);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_public_key_encode(sc, NULL, &len);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_public_key_encode(sc, &dummy, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_private_key_encode(sc, NULL, &len);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_private_key_encode(sc, &dummy, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_private_key_encode(sc, &dummy, &len);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_encapsulation)
{
    SINT32 retcode;
    UINT32 errcode;
    size_t len_1, len_2;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    UINT8 *dummy_1 = (UINT8*) flags, *dummy_2 = (UINT8*) flags;

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_encapsulation(NULL, &dummy_1, &len_1, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    retcode = safecrypto_encapsulation(sc, NULL, &len_1, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_encapsulation(sc, &dummy_1, NULL, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_encapsulation(sc, &dummy_1, &len_1, NULL, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_encapsulation(sc, &dummy_1, &len_1, &dummy_2, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_encapsulation(sc, &dummy_1, &len_1, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_decapsulation)
{
    SINT32 retcode;
    UINT32 errcode;
    size_t len_1, len_2;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    UINT8 *dummy_1 = (UINT8*) flags, *dummy_2 = (UINT8*) flags;

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_decapsulation(NULL, dummy_1, len_1, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    retcode = safecrypto_decapsulation(sc, dummy_1, len_1, NULL, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_decapsulation(sc, dummy_1, len_1, &dummy_2, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_decapsulation(sc, dummy_1, len_1, &dummy_2, &len_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_ibe_extract)
{
    SINT32 retcode;
    UINT32 errcode;
    size_t len_1, len_2;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    UINT8 *dummy_1 = (UINT8*) flags, *dummy_2 = (UINT8*) flags;

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_ibe_extract(NULL, len_1, dummy_1, &len_2, &dummy_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    retcode = safecrypto_ibe_extract(sc, len_1, dummy_1, &len_2, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_ibe_extract(sc, len_1, dummy_1, NULL, &dummy_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_ibe_extract(sc, len_1, dummy_1, &len_2, &dummy_2);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

START_TEST(test_safecrypto_ibe_public_encrypt)
{
    SINT32 retcode;
    UINT32 errcode;
    size_t len;
    safecrypto_t *sc;
    UINT32 flags[1] = {0};
    UINT8 *dummy = (UINT8*) flags;

    sc = safecrypto_create(SC_SCHEME_SIG_HELLO_WORLD, 0, flags);
    ck_assert_ptr_ne(sc, NULL);

    retcode = safecrypto_ibe_public_encrypt(NULL, len, dummy, len, dummy, &len, &dummy);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);

    retcode = safecrypto_ibe_public_encrypt(sc, len, dummy, len, dummy, NULL, &dummy);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_ibe_public_encrypt(sc, len, dummy, len, dummy, &len, NULL);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_NULL_POINTER);

    retcode = safecrypto_ibe_public_encrypt(sc, len, dummy, len, dummy, &len, &dummy);
    ck_assert_int_eq(retcode, SC_FUNC_FAILURE);
    errcode = safecrypto_err_get_error(sc);
    ck_assert_uint_eq(errcode, SC_INVALID_FUNCTION_CALL);

    retcode = safecrypto_destroy(sc);
    ck_assert_int_eq(retcode, SC_FUNC_SUCCESS);
}
END_TEST

Suite *safecrypto_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_basic;
    TCase *tc_limits;
    TCase *tc_keys;

    s = suite_create("safecrypto");

    /* Test cases */
    tc_core = tcase_create("CORE");
    tcase_add_test(tc_core, test_safecrypto_create_private);
    tcase_add_test(tc_core, test_safecrypto_create_public);
    tcase_add_test(tc_core, test_safecrypto_create_unknown);
    tcase_add_test(tc_core, test_safecrypto_create_null_string);
    suite_add_tcase(s, tc_core);

    tc_basic = tcase_create("BASIC");
    tcase_add_test(tc_basic, test_safecrypto_initial_api);
    tcase_add_test(tc_basic, test_safecrypto_initial_api_multiple);
    tcase_add_test(tc_basic, test_safecrypto_initial_api_null);
    tcase_add_test(tc_basic, test_safecrypto_initial_api_temp_ram);
    tcase_add_test(tc_basic, test_safecrypto_get_signature_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_encryption_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_kem_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_ibe_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_hash_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_xof_schemes);
    tcase_add_test(tc_basic, test_safecrypto_get_prng_schemes);
    tcase_add_test(tc_basic, test_safecrypto_prng_create);
    tcase_add_test(tc_basic, test_safecrypto_prng_32);
    tcase_add_test(tc_basic, test_safecrypto_aes_create);
    suite_add_tcase(s, tc_basic);

    tc_limits = tcase_create("LIMITS");
    tcase_add_test(tc_limits, test_safecrypto_encapsulation);
    tcase_add_test(tc_limits, test_safecrypto_decapsulation);
    tcase_add_test(tc_limits, test_safecrypto_ibe_extract);
    tcase_add_test(tc_limits, test_safecrypto_ibe_public_encrypt);
    suite_add_tcase(s, tc_limits);

    tc_keys = tcase_create("KEYS");
    tcase_add_test(tc_basic, test_safecrypto_keys_load);
    suite_add_tcase(s, tc_keys);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = safecrypto_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


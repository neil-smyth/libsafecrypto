
#include <stdio.h>
#include <inttypes.h>
#include "roots_of_unity.h"


#if __WORDSIZE == 64
#define FMT_LIMB    "%zu"
#else
#define FMT_LIMB    "%d"
#endif

typedef struct _table_params_t {
	SINT32 type;
	size_t modulus;
	size_t n;
} table_params_t;

static const table_params_t params[] = {
	{16, 7681, 256},
	{16, 12289, 512},
	{16, 12289, 1024},
	{16, 18433, 512},
	{16, 18433, 1024},
	{32, 4206593, 512},
	{32, 4206593, 1024},
	{32, 5767169, 512},
	{32, 5767169, 1024},
	{32, 8380417, 256},
	{32, 8399873, 512},
	{32, 10223617, 512},
	{32, 10223617, 1024},
	{32, 16813057, 512},
	{32, 51750913, 512},
	{32, 51750913, 1024},
	{32, 134348801, 1024},
};


static void print_header(FILE *fp)
{
	fprintf(fp, "/*****************************************************************************\n");
	fprintf(fp, " * Copyright (C) Queen's University Belfast, ECIT, 2017                      *\n");
	fprintf(fp, " *                                                                           *\n");
	fprintf(fp, " * This file is part of libsafecrypto.                                       *\n");
	fprintf(fp, " *                                                                           *\n");
	fprintf(fp, " * This file is subject to the terms and conditions defined in the file      *\n");
	fprintf(fp, " * 'LICENSE', which is part of this source code package.                     *\n");
	fprintf(fp, " *****************************************************************************/\n");
	fprintf(fp, "\n");
	fprintf(fp, "/**\n");
	fprintf(fp, " * THIS FILE IS AUTOMATICALLY GENERATED\n");
	fprintf(fp, " */\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
}

void inverse_shuffle_16(SINT16 *v, size_t n)
{
    size_t i, j, k;

    // This is the fallback method that also increments the MSB's
    j = n >> 1;
    for (i = 1; i < n - 1; i++) {       // 00..0 and 11..1 remain same
        if (i < j) {
            SINT16 x = v[i];
            v[i] = v[j];
            v[j] = x;
        }
        k = n;
        do {
            k >>= 1;
            j ^= k;
        } while ((j & k) == 0);
    }
}

void inverse_shuffle_32(SINT32 *v, size_t n)
{
    size_t i, j, k;

    // This is the fallback method that also increments the MSB's
    j = n >> 1;
    for (i = 1; i < n - 1; i++) {       // 00..0 and 11..1 remain same
        if (i < j) {
            SINT32 x = v[i];
            v[i] = v[j];
            v[j] = x;
        }
        k = n;
        do {
            k >>= 1;
            j ^= k;
        } while ((j & k) == 0);
    }
}

int main(int argc, char *argv[])
{
	SINT16 fwd16[1024], inv16[1024];
	SINT32 fwd32[1024], inv32[1024];
	SINT32 retval;
	size_t i, j;
	sc_ulimb_t seed = 0;

	if (2 != argc) {
		fprintf(stderr, "ERROR! Please supply a base filename\n");
		return SC_FUNC_FAILURE;
	}

	char *basename = argv[1];
	fprintf(stderr, "basename = %s\n", basename);

	char c_file[256], h_file[256];
	sprintf(c_file, "%s.c", basename);
	sprintf(h_file, "%s.h", basename);

	FILE *fp;
	if (NULL == (fp = fopen(h_file, "w"))) {
		fprintf(stderr, "ERROR! Could not open C header file %s\n", h_file);
		return SC_FUNC_FAILURE;
	}
	print_header(fp);
	fprintf(fp, "#pragma once\n\n");
	fprintf(fp, "#include \"safecrypto_types.h\"\n\n");
	for (j=0; j<sizeof(params)/sizeof(table_params_t); j++) {
		fprintf(fp, "#ifdef NTT_NEEDS_%zu\n", params[j].modulus);
		if (16 == params[j].type) {
			fprintf(fp, "extern const SINT16 w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT16 r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT16 rev_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT16 inv_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n/2);
			fprintf(fp, "extern const SINT16 inv_r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
		}
		else {
			fprintf(fp, "extern const SINT32 w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT32 r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT32 rev_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
			fprintf(fp, "extern const SINT32 inv_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n/2);
			fprintf(fp, "extern const SINT32 inv_r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "];\n",
				params[j].modulus, params[j].n, params[j].n);
		}
		fprintf(fp, "#endif\n\n");
	}
	fclose(fp);

	if (NULL == (fp = fopen(c_file, "w"))) {
		fprintf(stderr, "ERROR! Could not open C file %s\n", c_file);
		return SC_FUNC_FAILURE;
	}
	print_header(fp);
	fprintf(fp, "#include \"safecrypto_types.h\"\n\n");

	fprintf(fp, "#ifndef DISABLE_ENC_KYBER\n");
	fprintf(fp, "#include \"schemes/enc/kyber/kyber_enc_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_ENC_RLWE\n");
	fprintf(fp, "#include \"schemes/enc/rlwe_enc/rlwe_enc_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_IBE_DLP\n");
	fprintf(fp, "#include \"schemes/ibe/dlp/dlp_ibe_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_KEM_ENS\n");
	fprintf(fp, "#include \"schemes/kem/ens/ens_kem_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_KEM_KYBER\n");
	fprintf(fp, "#include \"schemes/kem/kyber/kyber_kem_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_SIG_BLISS_B\n");
	fprintf(fp, "#include \"schemes/sig/bliss_b/bliss_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#if !defined(DISABLE_SIG_DILITHIUM) || !defined(DISABLE_SIG_DILITHIUM_G)\n");
	fprintf(fp, "#include \"schemes/sig/dilithium/dilithium_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#if !defined(DISABLE_SIG_DLP) || !defined(DISABLE_SIG_ENS)\n");
	fprintf(fp, "#include \"schemes/sig/ens_dlp/ens_dlp_sig_params.h\"\n");
	fprintf(fp, "#endif\n");

	fprintf(fp, "#ifndef DISABLE_SIG_RING_TESLA\n");
	fprintf(fp, "#include \"schemes/sig/ring_tesla/ring_tesla_params.h\"\n");
	fprintf(fp, "#endif\n");
	fprintf(fp, "\n\n");

	fprintf(fp, "#ifndef DISABLE_SIG_FALCON\n");
	fprintf(fp, "#include \"schemes/sig/falcon/falcon_params.h\"\n");
	fprintf(fp, "#endif\n");
	fprintf(fp, "\n\n");

	for (j=0; j<sizeof(params)/sizeof(table_params_t); j++) {

		fprintf(fp, "#ifdef NTT_NEEDS_%zu\n", params[j].modulus);

		if (16 == params[j].type) {
			retval = roots_of_unity_s16(fwd16, inv16, params[j].n, params[j].modulus, seed, 0);
			if (SC_FUNC_SUCCESS != retval) {
				fprintf(stderr, "ERROR! Could not generate roots of unity table for (p=" FMT_LIMB ",N=" FMT_LIMB ")\n",
					params[j].modulus, params[j].n);
				fclose(fp);
				return SC_FUNC_FAILURE;
			}

			fprintf(fp, "const SINT16 w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%8d, ", fwd16[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
    		fprintf(fp, "const SINT16 r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
    			params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%8d, ", inv16[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");

    		sc_mod_t mod;
    		sc_ulimb_t hi, lo;
		    limb_mod_init(&mod, params[j].modulus);
		    limb_mul_hi_lo(&hi, &lo, fwd16[1], inv16[params[j].n-1]);

    		inverse_shuffle_16(fwd16, params[j].n);
			fprintf(fp, "const SINT16 rev_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", fwd16[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");

    		inv_root_square_s16(fwd16, params[j].n, params[j].modulus, seed, 0);
			inverse_shuffle_16(fwd16, params[j].n/2);

			fprintf(fp, "const SINT16 inv_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n/2);
    		for (i=0; i<params[j].n/2; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%8d, ", fwd16[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
    		fprintf(fp, "const SINT16 inv_r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
    			params[j].modulus, params[j].n, params[j].n);

			fprintf(fp, "    ");
		    fprintf(fp, "%8d, ", (SINT16)limb_mod_reduction_norm(hi, lo, mod.m, mod.m_inv, mod.norm));
    		for (i=1; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%8d, ", inv16[params[j].n - i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
    	}
    	else {
			retval = roots_of_unity_s32(fwd32, inv32, params[j].n, params[j].modulus, seed, 0);
			if (SC_FUNC_SUCCESS != retval) {
				fprintf(stderr, "ERROR! Could not generate roots of unity table for (p=" FMT_LIMB ",N=" FMT_LIMB ")\n",
					params[j].modulus, params[j].n);
				fclose(fp);
				return SC_FUNC_FAILURE;
			}

			fprintf(fp, "const SINT32 w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", fwd32[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
			fprintf(fp, "const SINT32 r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
    			params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", inv32[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");

			sc_mod_t mod;
    		sc_ulimb_t hi, lo;
		    limb_mod_init(&mod, params[j].modulus);
		    limb_mul_hi_lo(&hi, &lo, fwd32[1], inv32[params[j].n-1]);

			inverse_shuffle_32(fwd32, params[j].n);
			fprintf(fp, "const SINT32 rev_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n);
    		for (i=0; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", fwd32[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");

    		inv_root_square_s32(fwd32, params[j].n, params[j].modulus, seed, 0);
			inverse_shuffle_32(fwd32, params[j].n);

			fprintf(fp, "const SINT32 inv_w" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
				params[j].modulus, params[j].n, params[j].n/2);
    		for (i=0; i<params[j].n/2; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", fwd32[i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
			fprintf(fp, "const SINT32 inv_r" FMT_LIMB "_n" FMT_LIMB "[" FMT_LIMB "] = {\n",
    			params[j].modulus, params[j].n, params[j].n);

			fprintf(fp, "    ");
		    fprintf(fp, "%9d, ", (SINT32)limb_mod_reduction_norm(hi, lo, mod.m, mod.m_inv, mod.norm));
    		for (i=1; i<params[j].n; i++) {
    			if ((i&7) == 0) fprintf(fp, "    ");
		        fprintf(fp, "%9d, ", inv32[params[j].n - i]);
        		if ((i&7) == 7) fprintf(fp, "\n");
    		}
    		fprintf(fp, "};\n\n");
    	}

    	fprintf(fp, "#endif\n\n");
    }

	fclose(fp);

	return SC_FUNC_SUCCESS;
}

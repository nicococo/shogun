/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2008-2009 Soeren Sonnenburg
 * Copyright (C) 2008-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _TANIMOTOKERNELNORMALIZER_H___
#define _TANIMOTOKERNELNORMALIZER_H___

#include <shogun/kernel/normalizer/KernelNormalizer.h>
#include <shogun/kernel/string/CommWordStringKernel.h>

namespace shogun
{
/** @brief TanimotoKernelNormalizer performs kernel normalization inspired by
 * the Tanimoto coefficient (see http://en.wikipedia.org/wiki/Jaccard_index )
 *
 * \f[
 * k'({\bf x},{\bf x'}) = \frac{k({\bf x},{\bf x'})}{k({\bf x},{\bf x})+k({\bf x'},{\bf x'})-k({\bf x},{\bf x'})}
 * \f]
 */
class CTanimotoKernelNormalizer : public CKernelNormalizer
{
	public:
		/** default constructor
		 * @param use_opt_diag - some kernels support faster diagonal compuation
		 * via compute_diag(idx), this flag enables this
		 */
		CTanimotoKernelNormalizer(bool use_opt_diag=false)
			: CKernelNormalizer(), diag_lhs(NULL), diag_rhs(NULL),
			use_optimized_diagonal_computation(use_opt_diag)
		{
		}

		/** default destructor */
		virtual ~CTanimotoKernelNormalizer()
		{
			SG_FREE(diag_lhs);
			SG_FREE(diag_rhs);
		}

		/** initialization of the normalizer
         * @param k kernel */
		virtual bool init(CKernel* k)
		{
			ASSERT(k);
			int32_t num_lhs=k->get_num_vec_lhs();
			int32_t num_rhs=k->get_num_vec_rhs();
			ASSERT(num_lhs>0);
			ASSERT(num_rhs>0);

			CFeatures* old_lhs=k->lhs;
			CFeatures* old_rhs=k->rhs;

			k->lhs=old_lhs;
			k->rhs=old_lhs;
			bool r1=alloc_and_compute_diag(k, diag_lhs, num_lhs);

			k->lhs=old_rhs;
			k->rhs=old_rhs;
			bool r2=alloc_and_compute_diag(k, diag_rhs, num_rhs);

			k->lhs=old_lhs;
			k->rhs=old_rhs;

			return r1 && r2;
		}

		/** normalize the kernel value
		 * @param value kernel value
		 * @param idx_lhs index of left hand side vector
		 * @param idx_rhs index of right hand side vector
		 */
		virtual float64_t normalize(
			float64_t value, int32_t idx_lhs, int32_t idx_rhs)
		{
			float64_t diag_sum=diag_lhs[idx_lhs]*diag_rhs[idx_rhs];
			return value/(diag_sum-value);
		}

		/** normalize only the left hand side vector
		 * @param value value of a component of the left hand side feature vector
		 * @param idx_lhs index of left hand side vector
		 */
		virtual float64_t normalize_lhs(float64_t value, int32_t idx_lhs)
		{
			SG_ERROR("linadd not supported with Tanimoto normalization.\n");
			return 0;
		}

		/** normalize only the right hand side vector
		 * @param value value of a component of the right hand side feature vector
		 * @param idx_rhs index of right hand side vector
		 */
		virtual float64_t normalize_rhs(float64_t value, int32_t idx_rhs)
		{
			SG_ERROR("linadd not supported with Tanimoto normalization.\n");
			return 0;
		}

		/** Returns the name of the SGSerializable instance.  It MUST BE
		 *  the CLASS NAME without the prefixed `C'.
		 *
		 * @return name of the SGSerializable
		 */
		virtual const char* get_name() const {
			return "TanimotoKernelNormalizer"; }

    public:
		/**
		 * alloc and compute the vector containing the square root of the
		 * diagonal elements of this kernel.
		 */
		bool alloc_and_compute_diag(CKernel* k, float64_t* &v, int32_t num)
		{
			SG_FREE(v);
			v=SG_MALLOC(float64_t, num);

			for (int32_t i=0; i<num; i++)
			{
				if (k->get_kernel_type() == K_COMMWORDSTRING)
				{
					if (use_optimized_diagonal_computation)
						v[i]=((CCommWordStringKernel*) k)->compute_diag(i);
					else
						v[i]=((CCommWordStringKernel*) k)->compute_helper(i,i, true);
				}
				else
					v[i]=k->compute(i,i);

				if (v[i]==0.0)
					v[i]=1e-16; /* avoid divide by zero exception */
			}

			return (v!=NULL);
		}

    protected:
		/** diagonal left-hand side */
		float64_t* diag_lhs;
		/** diagonal right-hand side */
		float64_t* diag_rhs;
		/** flat if optimized diagonal computation is used */
		bool use_optimized_diagonal_computation;
};
}
#endif

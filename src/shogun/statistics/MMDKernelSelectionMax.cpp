/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2012 Heiko Strathmann
 */

#include <shogun/statistics/MMDKernelSelectionMax.h>
#include <shogun/statistics/KernelTwoSampleTestStatistic.h>

using namespace shogun;

CMMDKernelSelectionMax::CMMDKernelSelectionMax() : CMMDKernelSelection()
{
}

CMMDKernelSelectionMax::CMMDKernelSelectionMax(
		CKernelTwoSampleTestStatistic* mmd) : CMMDKernelSelection(mmd)
{
}

CMMDKernelSelectionMax::~CMMDKernelSelectionMax()
{
}

float64_t CMMDKernelSelectionMax::compute_measure(CKernel* kernel)
{
	/* just return plain MMD */
	m_mmd->set_kernel(kernel);
	return m_mmd->compute_statistic();
}

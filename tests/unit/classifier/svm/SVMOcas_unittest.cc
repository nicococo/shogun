#include <shogun/classifier/svm/SVMOcas.h>
#include <shogun/features/DataGenerator.h>
#include <shogun/features/DenseFeatures.h>
#include <gtest/gtest.h>

using namespace shogun;

TEST(SVMOcasTest,train)
{
	index_t num_samples = 50;
	SGMatrix<float64_t> data =
		CDataGenerator::generate_gaussians(num_samples, 2, 2);
	CDenseFeatures<float64_t> features(data);

	SGVector<index_t> train_idx(num_samples), test_idx(num_samples);
	SGVector<float64_t> labels(num_samples);
	for (index_t i = 0, j = 0; i < data.num_cols; ++i)
	{
		if (i % 2 == 0)
			train_idx[j] = i;
		else
			test_idx[j++] = i;

		labels[i/2] = (i < data.num_cols/2) ? 1.0 : -1.0;
	}

	CDenseFeatures<float64_t>* train_feats = (CDenseFeatures<float64_t>*)features.copy_subset(train_idx);
	CDenseFeatures<float64_t>* test_feats =  (CDenseFeatures<float64_t>*)features.copy_subset(test_idx);

	CBinaryLabels* ground_truth = new CBinaryLabels(labels);

	CSVMOcas* ocas = new CSVMOcas(1.0, train_feats, ground_truth);
	ocas->train();
	
	CLabels* pred = ocas->apply(test_feats);
	for (int i = 0; i < num_samples; ++i)
		EXPECT_EQ(ground_truth->get_int_label(i), ((CBinaryLabels*)pred)->get_int_label(i));

	SG_UNREF(ocas);
	SG_UNREF(train_feats);
	SG_UNREF(test_feats);
	SG_UNREF(pred);
}


#include"Fisher_Matrix.hpp"
#include"custom_modules.hpp"
#include"utils/yaml-torch.hpp"

using namespace custom_models;


#include<yaml-cpp/yaml.h>
int main(int argc, char** argv)
{
	YAML::Node config = YAML::LoadFile(((argc>1)?argv[1]:"config.yaml"));
	const auto USE_GPU=config["USE_GPU"].as<bool>();
	torch::DeviceType device_type= torch::kCPU;
	if(USE_GPU)
	{
		if (torch::cuda::is_available()) {
			std::cout << "Device: GPU." << std::endl;
			device_type = torch::kCUDA;
		} else {
			std::cout << "Device: CPU." << std::endl;
		}
	}
	else
	{
		std::cout << "Device: CPU." << std::endl;
	}
	torch::Device device(device_type);

	at::set_default_dtype(caffe2::TypeMeta::Make<double>());

	const auto N_PARAM=config["N_PARAM"].as<int64_t>();
	const auto N_DATA=config["N_DATA"].as<int64_t>();
	const auto SIN=config["SIN"].as<int64_t>();
	
	auto models=std::vector<MODEL>(N_PARAM,nullptr);
	std::for_each(EXEC,models.begin(),models.end(),[&config](auto &item){
			item=MODEL(config["Model"]);
			});

	std::vector<torch::Tensor> xs(N_PARAM);
	std::for_each(EXEC,xs.begin(),xs.end(),[&N_DATA,&SIN](auto &item){
			item= torch::randn({N_DATA,SIN});
			});

	auto FM=statistical::get_Fisher_Matrix<MODEL>(models,xs);

	auto NFME=statistical::get_Normalized_Fisher_eig(FM);

	auto Ave_NFME=torch::sum(NFME,0)/NFME.size(0);

	PRINTT(Ave_NFME,"Tensor(D_PHI)")
		std::ofstream spec("Spectrum.txt");
	spec<<Ave_NFME;
	spec.close();

	const auto npoints_from=(config["Effective Dimension"])["Points from"].as<double>();
	const auto npoints_To=(config["Effective Dimension"])["Points to"].as<double>();
	const auto npoints_Step=(config["Effective Dimension"])["Points step"].as<double>();
	const auto Gamma=(config["Effective Dimension"])["Gamma"].as<double>();

	auto points=torch::arange(npoints_from,npoints_To,npoints_Step);
	auto Effec=statistical::get_Effective_Dimension(NFME,points,Gamma);

	std::ofstream Effect("Effect_dime.txt");
	std::ofstream npo("Effect_dime_x.txt");
	Effect<<Effec;
	npo<<points;
	Effect.close();
	npo.close();
	return 0;
}

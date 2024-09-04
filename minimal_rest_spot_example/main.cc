#include <smartspectra/container/foreground_container.hpp>
#include <physiology/interface/glog/logging.h>
#include <physiology/interface/nlohmann/json.hpp>
namespace spectra = presage::smartspectra;
namespace settings = presage::smartspectra::container::settings;
using DeviceType = presage::platform_independence::DeviceType;

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;

    settings::Settings<settings::OperationMode::Spot, settings::IntegrationMode::JsonRestApi> settings;
    settings.integration.physiology_key = "YOUR_API_KEY_HERE";
    settings.spot.spot_duration_s = 30;

    spectra::container::SpotRestForegroundContainer<DeviceType::Cpu> container(settings);
    container.OnMetricsOutput = [](const nlohmann::json& metrics) {
        LOG(INFO) << "Got metrics from Physiology REST API: " << metrics;
        return absl::OkStatus();
    };
    auto status = container.Initialize();
    if (status.ok()) { status = container.Run(); }

    if (!status.ok()) {
        LOG(ERROR) << "Run failed. " << status.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
}

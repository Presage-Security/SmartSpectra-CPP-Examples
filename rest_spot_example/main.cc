// stdlib includes
#include <string>

// third-party includes
#include <physiology/interface/absl/status/status.h>
#include <physiology/interface/absl/flags/flag.h>
#include <physiology/interface/absl/flags/parse.h>
#include <physiology/interface/absl/flags/usage.h>
#include <physiology/interface/glog/logging.h>
#include <physiology/modules/configuration.h>
#include <smartspectra/container/settings.hpp>
#include <smartspectra/video_source/camera/camera.hpp>
#include <smartspectra/container/foreground_container.hpp>
#include <smartspectra/formats/metrics.hpp>

namespace pcam = presage::camera;
namespace spectra = presage::smartspectra;
namespace settings = presage::smartspectra::container::settings;
namespace vs = presage::smartspectra::video_source;
// region ==================================== CAMERA PARAMETERS =======================================================
//TODO: implement ABSL_FLAG_GROUP(group_name, param1, param2, param3, ...) macro in Abseil,
// which prints visually-separated, named groups of parameters/flags in help message, and use it here
ABSL_FLAG(int, camera_device_index, 0, "The index of the camera device to use in streaming capture mode.");
ABSL_FLAG(
    vs::ResolutionSelectionMode, resolution_selection_mode, vs::ResolutionSelectionMode::Auto,
    "A flag to specify the resolution selection mode when both a range and exact resolution are specified."
    "Possible values: "
    + absl::StrJoin(vs::GetResolutionSelectionModeNames(), ", ")
);
ABSL_FLAG(int, capture_width_px, -1,
          "The capture width in pixels. Set to 1280 if resolution_selection_mode is set to 'auto' and no resolution range is specified.");
ABSL_FLAG(int, capture_height_px, -1,
          "The capture height in pixels. Set to 720 if resolution_selection_mode is set to 'auto' and no resolution range is specified.");
ABSL_FLAG(
    pcam::CameraResolutionRange,
    resolution_range,
    pcam::CameraResolutionRange::Unspecified_EnumEnd,
    absl::StrCat(
        "The resolution range to attempt to use. Possible values: ",
        pcam::kCommonCameraResolutionRangeNameList
    )
);
ABSL_FLAG(pcam::CaptureCodec, codec, pcam::CaptureCodec::MJPG,
          absl::StrCat("Video codec to use in streaming capture mode. Possible values: ",
                       pcam::kCaptureCodecNameList));
ABSL_FLAG(bool, auto_lock, true,
          "If true, will try to use auto-exposure before recording and lock exposure when recording starts. If false, doesn't do this automatically.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. Signifies prerecorded video mode will be used. When not provided, "
          "the app will attempt to use a webcam / stream.");
// endregion ===========================================================================================================

ABSL_FLAG(bool, headless, false, "If true, no GUI will be displayed. Only valid for prerecorded video mode.");
ABSL_FLAG(bool, also_log_to_stderr, false, "If true, log to stderr as well.");
ABSL_FLAG(int, interframe_delay, 20,
          "Delay, in milliseconds, before capturing the next frame: "
          "higher values may free more Cpu resources for the graph, giving it more time to process what it already has "
          "and drop fewer frames, resulting in more robust output metrics.");
ABSL_FLAG(bool, start_with_recording_on, false, "Attempt to switch data recording on at the start (even in streaming mode).");
ABSL_FLAG(int, start_time_offset_ms, 0,
          "Offset, in milliseconds, before capturing the first frame: "
          "0 starts from beginning.  30000 starts at 30s mark. "
          "Not functional for streaming mode, as start is disabled until this offset.");
ABSL_FLAG(bool, scale_input, true,
          "If true, uses input scaling in the ImageTransformationCalculator within the graph.");
ABSL_FLAG(bool, enable_phasic_bp, false, "If true, enable the phasic blood pressure computation.");
ABSL_FLAG(bool, print_graph_contents, false, "If true, print the graph contents.");
ABSL_FLAG(std::string,
          output_directory,
          "out",
          "Directory where to save preprocessed analysis data as JSON. "
          "If it does not exist, the app will attempt to make one.");
ABSL_FLAG(int, verbosity, 1, "Verbosity level -- raise to print more.");
ABSL_FLAG(std::string, physiology_key, "",
          "API key to use for the Physiology online service. "
          "If not provided, final features and/or metrics are not retrieved.");
// region ======================== SPOT-MODE SETTINGS ==================================================================
ABSL_FLAG(double, spot_duration, 30.0, "Spot duration in floating-point seconds.");
// endregion ===========================================================================================================
// region =========================== VIDEO OUTPUT SETTINGS ============================================================
ABSL_FLAG(std::string, output_video_destination, "",
          "Full path of video to save or gstreamer output configuration string (see mode documentation). "
          "Signifies video output mode will be used. When not provided, there will be no video output "
          "(WARNING: Developer feature only. Not currently supported using the standard Physiology SDK).");
ABSL_FLAG(settings::VideoSinkMode, video_sink_mode, settings::VideoSinkMode::Unknown_EnumEnd,
          "A flag to specify the video output mode. "
          "(WARNING: Developer feature only. Not currently supported using the standard Physiology SDK). "
          "Possible values: "
          + absl::StrJoin(vs::GetResolutionSelectionModeNames(), ", ") +
          " Note that, in the `gstreamer` mode, `destination`  ");
ABSL_FLAG(bool, passthrough_video, false,
          "If true, output video will just use the input video frames directly (see destination "
          "documentation), without passing through any processing "
          "(which might contain rendered visual content from the graph).");
// endregion ===========================================================================================================
// region ========================  CUSTOM SETTINGS (not for container) ================================================
ABSL_FLAG(bool, use_gpu, false, "If true, use the GPU for some operations.");
// endregion ===========================================================================================================


template<presage::platform_independence::DeviceType TDeviceType>
absl::Status RunRestSpotPreprocessing(
    settings::Settings<settings::OperationMode::Spot, settings::IntegrationMode::JsonRestApi>& settings
) {
    spectra::container::SpotRestForegroundContainer<TDeviceType> container(settings);
    container.OnMetricsOutput = [](const nlohmann::json& api_json_metrics) {
        auto metrics_or_status = spectra::formats::MetricsFromRestApiJson(api_json_metrics);
        if (!metrics_or_status.ok()) {
            return metrics_or_status.status();
        }
        spectra::formats::Metrics metrics = metrics_or_status.value();
        nlohmann::json metrics_nice_json{metrics};
        LOG(INFO) << "Got metrics from Physiology REST API: " << metrics_nice_json.dump(2);
        return absl::OkStatus();
    };
    MP_RETURN_IF_ERROR(container.Initialize());
    MP_RETURN_IF_ERROR(container.Run());

    return absl::OkStatus();
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);

    absl::SetProgramUsageMessage(
        "Run Presage Physiology Preprocessing C++ Rest Spot Example on either a video file or video input from camera.\n"
        "The application will use Presage Web API to retrieve metrics upon successful preprocessing of \n"
        "the input video if a valid Physiology Web API key is provided via the --api_key argument."
    );
    absl::ParseCommandLine(argc, argv);

    if (absl::GetFlag(FLAGS_also_log_to_stderr)) {
        FLAGS_alsologtostderr = true;
    }

    settings::Settings<settings::OperationMode::Spot, settings::IntegrationMode::JsonRestApi> settings{
        vs::VideoSourceSettings{
            absl::GetFlag(FLAGS_camera_device_index),
            absl::GetFlag(FLAGS_resolution_selection_mode),
            absl::GetFlag(FLAGS_capture_width_px),
            absl::GetFlag(FLAGS_capture_height_px),
            absl::GetFlag(FLAGS_resolution_range),
            absl::GetFlag(FLAGS_codec),
            absl::GetFlag(FLAGS_auto_lock),
            absl::GetFlag(FLAGS_input_video_path),
        },
        settings::VideoSinkSettings{
            absl::GetFlag(FLAGS_output_video_destination),
            absl::GetFlag(FLAGS_video_sink_mode),
            absl::GetFlag(FLAGS_passthrough_video)
        },
        absl::GetFlag(FLAGS_headless),
        absl::GetFlag(FLAGS_interframe_delay),
        absl::GetFlag(FLAGS_start_with_recording_on),
        absl::GetFlag(FLAGS_start_time_offset_ms),
        absl::GetFlag(FLAGS_scale_input),
        /*binary_graph=*/true,
        absl::GetFlag(FLAGS_enable_phasic_bp),
        absl::GetFlag(FLAGS_print_graph_contents),

        absl::GetFlag(FLAGS_verbosity),
        settings::SpotSettings {
            absl::GetFlag(FLAGS_spot_duration)
        },
        settings::JsonRestApiSettings{
            absl::GetFlag(FLAGS_physiology_key),
            absl::GetFlag(FLAGS_output_directory),
            /*save_to_disk=*/false,
        }
    };


    absl::Status status;

    if (settings.headless && settings.video_source.input_video_path.empty()) {
        LOG(ERROR) << "Cannot use headless mode without loading video. Run with --help=main to see usage.";
        exit(-1);
    }

#ifdef WITH_OPENGL
    if (absl::GetFlag(FLAGS_use_gpu)) {
        status = RunRestSpotPreprocessing<presage::platform_independence::DeviceType::OpenGl>(settings);
    } else {
        status = RunRestSpotPreprocessing<presage::platform_independence::DeviceType::Cpu>(settings);
    }
#else
    // no choice left here but to use Cpu-only version
    status = RunRestSpotPreprocessing<presage::platform_independence::DeviceType::Cpu>(settings);
#endif

    if (!status.ok()) {
        LOG(ERROR) << "Run failed. " << status.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }

    return 0;
}


# Physiology Preprocessing C++ File Example App

The file example app continuously writes chunks of preprocessed data to disk in JSON format to a specified directory. 

## Build Instructions
### x86_64 / amd64 (most Linux & MacOS machines, not tested on Windows)

1. Ensure you have read and followed the [dependency setup instructions](/docs/dependencies/README.md).
2. Launch the build (but consult notes below first!):
   - **Linux**
       ```bash
       bazel build --copt=-O3 //applications/cpp_file_example
       ```
   - **MacOS**
       ```bash
       bazel build --copt=-O3 --define MEDIAPIPE_DISABLE_GPU=1 //applications/cpp_file_example
       ```
   - Explanation: when building on Mac, MediaPipe doesn't support GLES, so the CPU-only version should be built.
   - **Developer Note**: pass in `--define='WITH_VIDEO_OUTPUT=1'` after `bazel build` if you need the video output for debugging.

## Usage Instructions

To run directly using built binary, from root of repository, invoke:
```bash
`bazel-bin/applications/cpp_file_example/cpp_file_example <args>`
```

**Note**: to understand the command line physiology_core_process_options, either consult the [Command Line Reference](/mediapipe/presage/docs/command_line_reference.md) or run the program with ` --help=main`.

By default, JSON outputs and status codes are written to the folder `out` in the application's working directory. This
can be changed by setting the `--output_path` and `--status_file_directory_path` arguments for JSON and status codes respectively.

An example run that attempts data preprocessing right away and stores JSON at 3-second intervals would look something like:

```bash
bazel-bin/applications/cpp_file_example/cpp_file_example \
--also_log_to_stderr --start_with_recording_on \
--file_stream_path=/home/presage/Data/test_data/file_stream/video_based_stream/frame_0000000000000000.png \
--erase_read_files=true --file_stream_rescan_delay=5 --buffer_duration=0.5 --status_file_directory_path=out/status --output_path=out/json
```

The example above will attempt to read frame files from the folder `/home/greg/Data/test_data/file_stream/video_based_stream/` in sequence, 
erasing them after it read each one. It will stop when a file named `end_of_stream` appears in the folder. It will also write status codes to the folder `out/status` and the preprocessed JSON data to the folder `out/json`, as dictated by the arguments. These output directories will be generated if they do not exist.

**Note:** the comprehensive mapping of various status code values to their respective meanings can be found in the [Status Code Reference](/mediapipe/presage/docs/status_code_reference.md).

Notably, following arguments are also available (please see the [Command Line Reference](/mediapipe/presage/docs/command_line_reference.md) or run the program with:
` --help=main` for an exhaustive reference):

- Use the `--headless` argument to run without a GUI. 
- Use `--erase_read_files=false` to leave the read frame files in place; otherwise, they will be removed.
- Use `--loop` mode to use the frames in a folder in a loop (requires an end-of-stream token file to appear in the folder 
to signal exiting).

**Note**: `--loop` and `--erase_read_files=true` are mutually exclusive, while the latter is the default.

During a GUI run (not with `--headless`), you can use the following keyboard shortcuts:
- `q` or `ESC`: exit
- `s`: start/stop recording data (**webcam input / streaming** mode only)
- `e`: lock/unlock exposure (**webcam input / streaming** mode only)
- `-` and `=`: decrease or increase exposure (**webcam input / streaming** mode only, and only when exposure is locked)

To emulate a file stream for input, you can use some form of the following terminal command from the root of this repostory:
```bash
python modules/video_source/file_stream/video_to_frame_image_files.py --input=/home/presage/Data/test_data/treadmill_demo.avi \
--output_mask=/home/presage/Data/test_data/file_stream/video_based_stream/frame_0000000000000000.png \
--delay=5
```

Modify the `--input` path and `--output_mask` to in the above example to suit your needs.

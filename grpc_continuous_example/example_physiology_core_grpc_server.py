#!/usr/bin/env python3
# standard Python library
import argparse
import sys
import time
from concurrent import futures
import logging
import random
from typing import Union

# third-party
import google.protobuf.empty_pb2 as empty
import google.protobuf.wrappers_pb2 as wrappers
import grpc
import numpy as np

# local
import graph.physiology_core_service_pb2_grpc as ps_grpc
import graph.physiology_core_service_pb2 as ps
import modules.messages.metrics_pb2 as metr


# Dummy/Example Physiology Core gRPC Server (procedures produce random outputs).

class PreprocessedHeartRateDataNumpy:
    def __init__(self,
                 face_landmarks: np.ndarray,
                 roi_bgr_averages: np.ndarray,
                 face_bgr_average: np.ndarray,
                 time_now: float) -> None:
        self.face_landmarks = face_landmarks
        self.roi_bgr_averages = roi_bgr_averages
        self.face_bgr_average = face_bgr_average
        self.time_now = time_now

    def __str__(self):
        return f"face landmarks: \n{self.face_landmarks}\n" \
               f"roi_bgr_averages: \n{self.roi_bgr_averages}\n" \
               f"face_bgr_average: \n{self.face_bgr_average}\n" \
               f"time_now: \n{self.time_now}"


class PreprocessedRespiratoryRateDataNumpy:
    def __init__(self,
                 tracked_points: np.ndarray,
                 tracked_point_labels: np.ndarray,
                 reset: bool,
                 time_now: float) -> None:
        self.tracked_points = tracked_points
        self.tracked_point_labels = tracked_point_labels
        self.reset = reset
        self.time_now = time_now

    def __str__(self):
        return f"tracked_points: \n{self.tracked_points}\n" \
               f"tracked_point_labels: \n{self.tracked_point_labels}\n" \
               f"reset: \n{self.reset}\n" \
               f"time_now: \n{self.time_now}"


def preprocessed_proto_hr_to_numpy(preprocessed_heart_rate_data: ps.HrPreprocessedFrameData) \
        -> PreprocessedHeartRateDataNumpy:
    face_landmarks = np.zeros((len(preprocessed_heart_rate_data.face_landmark), 2), np.int32)
    # there currently isn't a fast way to reinterpret a repeated proto field as a numpy array,
    # track this issue for progress: https://github.com/protocolbuffers/protobuf/issues/10930
    for i_landmark, landmark in enumerate(preprocessed_heart_rate_data.face_landmark):
        landmark = preprocessed_heart_rate_data.face_landmark[i_landmark]
        face_landmarks[i_landmark, 0] = landmark.x
        face_landmarks[i_landmark, 1] = landmark.y

    roi_bgr_averages = np.zeros((len(preprocessed_heart_rate_data.roi_bgr_average) - 1, 3), np.float32)
    face_bgr_average = np.zeros((1, 3), np.float32)
    for i_roi, roi in enumerate(preprocessed_heart_rate_data.roi_bgr_average):
        if i_roi == 0:
            face_bgr_average[0, 0] = roi.x
            face_bgr_average[0, 1] = roi.y
            face_bgr_average[0, 2] = roi.z
        else:
            roi_bgr_averages[i_roi - 1, 0] = roi.x
            roi_bgr_averages[i_roi - 1, 1] = roi.y
            roi_bgr_averages[i_roi - 1, 2] = roi.z

    return PreprocessedHeartRateDataNumpy(face_landmarks, roi_bgr_averages, face_bgr_average,
                                          preprocessed_heart_rate_data.time_now)


def preprocessed_proto_rr_to_numpy(preprocessed_respiratory_rate_data: ps.RrPreprocessedFrameData) \
        -> PreprocessedRespiratoryRateDataNumpy:
    tracked_points = np.zeros((len(preprocessed_respiratory_rate_data.tracked_point), 2), np.float32)
    for i_point, point in enumerate(preprocessed_respiratory_rate_data.tracked_point):
        tracked_points[i_point, 0] = point.x
        tracked_points[i_point, 1] = point.y
    tracked_point_labels = np.zeros(len(preprocessed_respiratory_rate_data.tracked_point_label), np.int32)
    for i_label, label in enumerate(preprocessed_respiratory_rate_data.tracked_point_label):
        tracked_point_labels[i_label] = label
    return PreprocessedRespiratoryRateDataNumpy(tracked_points, tracked_point_labels,
                                                preprocessed_respiratory_rate_data.reset,
                                                preprocessed_respiratory_rate_data.time_now)


VERBOSE = False


def randomize_measurement(lower_limit: float, upper_limit: float,
                          measurement: Union[metr.Measurement, metr.MeasurementWithConfidence]):
    value_range = upper_limit - lower_limit
    measurement.value = lower_limit + random.random() * value_range
    measurement.stable = True


def randomize_measurement_with_confidence(lower_limit: float, upper_limit: float,
                                          measurement: metr.MeasurementWithConfidence):
    randomize_measurement(lower_limit, upper_limit, measurement)
    measurement.confidence = random.random()


def generate_and_append_random_metrics(buffer: metr.MetricsBuffer, length_seconds=1):
    initial_time_second = 0
    if len(buffer.pulse.rate) > 0:
        initial_time_second = buffer.pulse.rate[len(buffer.pulse.rate) - 1] + 1
    for time_second in range(initial_time_second, initial_time_second + length_seconds):
        pr = metr.MeasurementWithConfidence(time=time_second)
        randomize_measurement_with_confidence(38.0, 105.0, pr)
        buffer.pulse.rate.append(pr)

        pt = metr.Measurement(time=time_second)
        randomize_measurement(0.0, 1.0, pt)
        buffer.pulse.trace.append(pt)

        br = metr.MeasurementWithConfidence(time=time_second)
        randomize_measurement_with_confidence(6., 12., br)
        buffer.breath.rate.append(br)

        ubt = metr.Measurement(time=time_second)
        randomize_measurement(0.0, 1.0, ubt)
        buffer.breath.upper_trace.append(ubt)

        lbt = metr.Measurement(time=time_second)
        randomize_measurement(0.0, 1.0, lbt)
        buffer.breath.lower_trace.append(lbt)

        phasic_pressure = metr.MeasurementWithConfidence(time=time_second)
        randomize_measurement_with_confidence(70.0, 105.0, phasic_pressure)
        buffer.pressure.phasic.append(phasic_pressure)


def generate_and_append_constant_metrics(buffer: metr.MetricsBuffer, length_seconds=1):
    initial_time_second = 0
    if len(buffer.pulse.rate) > 0:
        initial_time_second = buffer.pulse.rate[len(buffer.pulse.rate) - 1] + 1
    for time_second in range(initial_time_second, initial_time_second + length_seconds):
        buffer.pulse.rate.append(metr.MeasurementWithConfidence(time=time_second, value=38.0, confidence=1.0))
        buffer.pulse.trace.append(metr.Measurement(time=time_second, value=0.5))
        buffer.breath.rate.append(metr.MeasurementWithConfidence(time=time_second, value=12.0, confidence=1.0))
        buffer.breath.upper_trace.append(metr.Measurement(time=time_second, value=0.5))
        buffer.breath.lower_trace.append(metr.Measurement(time=time_second, value=0.5))
        buffer.pressure.phasic.append(metr.MeasurementWithConfidence(time=time_second, value=70.0, confidence=1.0))


class DummyPhysiologyServer(ps_grpc.PhysiologyServicer):

    def __init__(self, use_random_seed=False, random_seed=0, use_uniform=False):
        self.use_random_seed = use_random_seed
        self.random_seed = random_seed
        self.use_uniform = use_uniform
        super().__init__()

    def AddPreprocessedData(self, request: ps.PreprocessedDataBuffer, context):
        for hr_data in request.hr_data:
            if VERBOSE:
                print("Got preprocessed hr data: ")
                print(preprocessed_proto_hr_to_numpy(hr_data))

        for rr_data in request.rr_data:
            if VERBOSE:
                print("Got preprocessed rr data: ")
                print(preprocessed_proto_rr_to_numpy(rr_data))

        return empty.Empty()

    def GetMetrics(self, request, context):
        if not self.use_uniform and self.use_random_seed:
            random.seed(self.random_seed)
            np.random.seed(self.random_seed)
        buffer = metr.MetricsBuffer()
        if self.use_uniform:
            generate_and_append_constant_metrics(buffer, 4)
        else:
            generate_and_append_random_metrics(buffer, 4)
        return buffer

    def IssueBlueTooth(self, request: ps.BlueTooth, context):
        return empty.Empty()

    def SetBufferDuration(self, request: wrappers.DoubleValue, context):
        print(f"Set buffer duration to {request.value}")
        return empty.Empty()

    def ResetProcessing(self, request, context):
        print("Got command to clean the test buffer. Cleaning the imaginary test buffer.")
        return empty.Empty()


APPLICATION_EXIT_SUCCESS = 0


def main():
    parser = argparse.ArgumentParser("A basic sample Python application mimicking "
                                     "Physiology Core Server but producing random output.")
    parser.add_argument("--port", "-p", type=int, default=50051,
                        help="Port number to expect the Physiology Server to service from.")
    parser.add_argument("--seed", "-s", type=int, default=-1,
                        help="Seed to use to randomize the outputs")
    args = parser.parse_args()
    logging.basicConfig()

    print(f"Random seed set to {args.seed}")

    port = args.port
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=1))
    ps_grpc.add_PhysiologyServicer_to_server(
        DummyPhysiologyServer(args.seed != -1, args.seed, use_uniform=False), server
    )
    server.add_insecure_port(f'[::]:{port}')
    server.start()
    print(f"Example Physio Core gRPC Server started, listening on port {port}")
    try:
        server.wait_for_termination()
    except KeyboardInterrupt:
        print("Server stopped.")
        server.stop(1.0)

    return APPLICATION_EXIT_SUCCESS


if __name__ == '__main__':
    sys.exit(main())
